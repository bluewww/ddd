// $Id$ -*- C++ -*-
// DDD session management

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char session_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "session.h"

#include "config.h"

#if XtSpecificationRelease >= 6
#include <X11/SM/SM.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "AppData.h"
#include "DataDisp.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "ExitCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "SourceView.h"
#include "charsets.h"
#include "commandQ.h"
#include "ddd.h"
#include "cook.h"
#include "exit.h"
#include "file.h"
#include "filetype.h"
#include "options.h"
#include "post.h"
#include "question.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>		// mkdir()
#endif
}


// ---------------------------------------------------------------------------
// Session files
// ---------------------------------------------------------------------------

static string dddbase = ddd_NAME;

static string home_dir()
{
    char *home = getenv("HOME");
    if (home == 0)
    {
	static int warned = 0;
	if (warned++ == 0)
	    cerr << "Warning: environment variable HOME undefined\n";
	home = ".";
    }

    return home;
}

static string state_dir()
{
    char *ddd_state = getenv("DDDSTATE");
    if (ddd_state != 0)
	return ddd_state;
    else
	return home_dir() + "/." + dddbase;
}

static string session_base_dir()
{
    char *ddd_sessions = getenv("DDDSESSIONS");
    if (ddd_sessions != 0)
	return ddd_sessions;
    else
	return state_dir() + "/sessions";
}

static string session_dir(const string& session)
{
    if (session == DEFAULT_SESSION)
	return state_dir();
    else
	return session_base_dir() + "/" + session;
}

string session_file(const string& session, const string& base)
{
    return session_dir(session) + "/" + base;
}


// ---------------------------------------------------------------------------
// Session file creation
// ---------------------------------------------------------------------------

class StreamAction {
private:
    ostream& stream;
    string action;
    bool ok;

public:
    StreamAction(ostream& os, const string& c)
	: stream(os), action(c), ok(true)
    {
	stream << action << "...\n";
    }
    ~StreamAction()
    {
	if (ok)
	    stream << action << "...done.\n";
    }
    void failed(const string& msg)
    {
	stream << action << " failed: " << msg << "\n";
	ok = false;
    }
};

static int makedir(string name, ostream& msg)
{
    StreamAction action(msg, "Creating " + quote(name + "/"));

    mode_t mask = umask(0);
    umask(mask);
    mode_t mode = 0777 & ~mask;
    int ret = mkdir(name, mode);

    if (ret != 0)
	action.failed(strerror(errno));

    return ret;
}

static void copy(const string& from_name, const string& to_name, ostream& msg)
{
    FILE *from = fopen(from_name, "r");
    if (from == NULL)
	return;			// Don't care

    StreamAction action(msg, "Copying " + quote(from_name) 
			+ " to " + quote(to_name));

    FILE *to = fopen(to_name, "w");
    if (to == NULL)
    {
	action.failed(strerror(errno));
	return;
    }

    int c;
    while ((c = getc(from)) != EOF)
	putc(c, to);

    fclose(from);
    if (fclose(to) == EOF)
    {
	action.failed(strerror(errno));
	unlink(to_name);
    }
}

// Create DDD state directory
static void create_ddd_state_dir(ostream& msg)
{
    // Create or find state directory
    if (!is_directory(state_dir()) && makedir(state_dir(), msg) == 0)
    {
	// Check for DDD 2.1 `~/.dddinit' and `~/.ddd_history' files; 
	// copy them to new location if needed
	copy(home_dir() + "/.dddinit",
	     session_state_file(DEFAULT_SESSION), msg);
	copy(home_dir() + "/.ddd_history",
	     session_history_file(DEFAULT_SESSION), msg);
    }
}

// Create session directory
void create_ddd_session_dir(const string& session, ostream& msg)
{
    // Create state directory
    create_ddd_state_dir(msg);
    
    // Create session base directory
    if (session != DEFAULT_SESSION && !is_directory(session_base_dir()))
	makedir(session_base_dir(), msg);

    // Create session directory
    if (session != DEFAULT_SESSION 
	&& is_directory(session_base_dir())
	&& !is_directory(session_dir(session))
	&& makedir(session_dir(session), msg) == 0)
    {
	// Check for default session state and history files;
	// copy them to new location
	copy(session_state_file(DEFAULT_SESSION), 
	     session_state_file(session), msg);
	copy(session_history_file(DEFAULT_SESSION), 
	     session_history_file(session), msg);
    }
}



// ---------------------------------------------------------------------------
// X11R6 Session management
// ---------------------------------------------------------------------------

#if XtSpecificationRelease >= 6

// Realize X11R6 session management protocols.

static void AskSaveSessionCB    (Widget w, XtPointer, XtPointer call_data);
static void ConfirmSaveSessionCB(Widget w, XtPointer, XtPointer call_data);
static void CancelSaveSessionCB (Widget w, XtPointer, XtPointer call_data);

static void AskShutdownCB       (Widget w, XtPointer, XtPointer call_data);
static void ConfirmShutdownCB   (Widget w, XtPointer, XtPointer call_data);
static void CancelShutdownCB    (Widget w, XtPointer, XtPointer call_data);

static void ask(string text, String name,
		XtCheckpointToken token, Widget w,
		XtCallbackProc yes, XtCallbackProc no);

// 1. The user initiates a checkpoint.  Have DDD save its options.
void SaveSessionCB(Widget w, XtPointer, XtPointer call_data)
{
    XtCheckpointToken token = XtCheckpointToken(call_data);

    // Save session
    const bool interact = (token->interact_style == SmInteractStyleAny);
    const bool shutdown = token->shutdown;

    unsigned long flags = SAVE_SESSION | SAVE_GEOMETRY;
    if (interact)
	flags |= MAY_INTERACT;
    if (shutdown)
	flags |= MAY_KILL;

    if (!shutdown && saving_options_kills_program(flags))
    {
	if (interact)
	    XtAddCallback(w, XtNinteractCallback, AskSaveSessionCB, 0);
	else
	    return;		// Cannot save state right now
    }
    else
    {
	token->save_success = save_options(flags);
    }

    if (shutdown)
    {
	// Prepare for shutdown
	if (interact)
	    XtAddCallback(w, XtNinteractCallback, AskShutdownCB, 0);
	else
	{
	    if (!gdb->isReadyWithPrompt())
	    {
		// GDB is still busy - cannot shutdown right now
		token->request_cancel = true;
	    }
	}
    }
}


// Save state

// 2. Request license to kill the program
static void AskSaveSessionCB(Widget w, XtPointer, XtPointer call_data)
{
    XtCheckpointToken token = XtCheckpointToken(call_data);

    ask("", "kill_to_save_dialog", token, w, 
	ConfirmSaveSessionCB, CancelSaveSessionCB);
}

// Ok, kill it
static void ConfirmSaveSessionCB(Widget, XtPointer client_data, XtPointer)
{
    XtCheckpointToken token = XtCheckpointToken(client_data);

    unsigned long flags = 
	SAVE_SESSION | SAVE_GEOMETRY | MAY_KILL | MAY_INTERACT;

    token->save_success = save_options(flags);
    XtSessionReturnToken(token);
}

// No, don't kill -- save everything else.
static void CancelSaveSessionCB(Widget, XtPointer client_data, XtPointer)
{
    XtCheckpointToken token = XtCheckpointToken(client_data);

    unsigned long flags = SAVE_SESSION | SAVE_GEOMETRY | MAY_INTERACT;

    token->save_success = save_options(flags);
    XtSessionReturnToken(token);
}


// Shutdown

// 2. If the checkpoint was part of a shutdown, make sure the
//     debugged program is killed properly.
static void AskShutdownCB(Widget w, XtPointer, XtPointer call_data)
{
    XtCheckpointToken token = XtCheckpointToken(call_data);

    if (token->cancel_shutdown || token->request_cancel)
    {
	XtSessionReturnToken(token);
	return;
    }

    if (gdb == 0 || !gdb->running())
    {
	XtSessionReturnToken(token);
	return;
    }

    if (!gdb->isReadyWithPrompt())
    {
	// Debugger is still running; request confirmation
	ask(gdb->title() + " is still busy.  Shutdown anyway (and kill it)?",
	    "quit_dialog", token, w, ConfirmShutdownCB, CancelShutdownCB);
	return;
    }

    if (program_running())
    {
	// Program is still running; request confirmation
	ask("The program is running.  Shutdown anyway (and kill it)?",
	    "shutdown_dialog", token, w, ConfirmShutdownCB, CancelShutdownCB);
	return;
    }

    ConfirmShutdownCB(w, XtPointer(token), call_data);
}

// 3. Confirm or cancel shutdown.
static void ConfirmShutdownCB(Widget, XtPointer client_data, XtPointer)
{
    XtCheckpointToken token = XtCheckpointToken(client_data);
    XtSessionReturnToken(token);
}

static void CancelShutdownCB(Widget, XtPointer client_data, XtPointer)
{
    XtCheckpointToken token = XtCheckpointToken(client_data);
    token->request_cancel = true;
    XtSessionReturnToken(token);
}

// 4. Let DDD die.
void DieSessionCB(Widget w, XtPointer, XtPointer call_data)
{
    if (gdb != 0 && gdb->isReadyWithPrompt())
    {
	if (gdb->type() == GDB)
	    gdb_question("set confirm off");

	DDDExitCB(w, XtPointer(EXIT_SUCCESS), call_data);
    }
    else
    {
	_DDDExitCB(w, XtPointer(EXIT_SUCCESS), call_data);
    }
}

// Create a confirmation dialog
static void ask(string text, String name, XtCheckpointToken token, Widget w,
		XtCallbackProc yes, XtCallbackProc no)
{
    // Program is still running; request confirmation
    Arg args[10];
    int arg;

    static Widget dialog = 0;
    if (dialog)
	DestroyWhenIdle(dialog);

    arg = 0;
    MString msg = rm(text);
    if (text != "")
    {
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    }
    dialog = verify(XmCreateQuestionDialog(find_shell(w), name,
					   args, arg));
    Delay::register_shell(dialog);
    XtAddCallback(dialog, XmNokCallback,     yes, XtPointer(token));
    XtAddCallback(dialog, XmNcancelCallback, no, XtPointer(token));
    XtAddCallback(dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    manage_and_raise(dialog);
}

#else // XtSpecificationRelease < 6

// X11R5 and earlier: Nothing yet...
void SaveSessionCB(Widget, XtPointer, XtPointer) {}
void DieSessionCB(Widget, XtPointer, XtPointer) {}

#endif // XtSpecificationRelease < 6
