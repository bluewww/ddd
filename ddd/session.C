// $Id$ -*- C++ -*-
//

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

#if XtSpecificationRelease >= 6
#include <X11/SM/SM.h>
#endif

#include <stdlib.h>

#include "Delay.h"
#include "DestroyCB.h"
#include "ExitCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "WhatNextCB.h"
#include "charsets.h"
#include "commandQ.h"
#include "ddd.h"
#include "exit.h"
#include "options.h"
#include "question.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

// ---------------------------------------------------------------------------
// Save state
// ---------------------------------------------------------------------------






// ---------------------------------------------------------------------------
// Session management
// ---------------------------------------------------------------------------

#if XtSpecificationRelease >= 6

// Realize X11R6 session management protocols.

static void ShutdownCB(Widget w, XtPointer, XtPointer call_data);
static void ConfirmShutdownCB(Widget w, XtPointer, XtPointer call_data);
static void CancelShutdownCB(Widget w, XtPointer, XtPointer call_data);
static void post_shutdown(string text, String name, XtCheckpointToken token, 
			  Widget w);

// 1. The user initiates a checkpoint.  Have DDD saves its options.
void SaveSessionCB(Widget w, XtPointer, XtPointer call_data)
{
    XtCheckpointToken token = XtCheckpointToken(call_data);

    bool interact = (token->interact_style == SmInteractStyleAny);

    unsigned long flags = OPTIONS_SAVE_SESSION;
    if (interact)
	flags |= OPTIONS_INTERACT;

    // Save session
    
    token->save_success = save_options(options_file(), flags);

    if (token->shutdown)
    {
	// Prepare for shutdown
	if (interact)
	    XtAddCallback(w, XtNinteractCallback, ShutdownCB, 0);
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

// 2. If the checkpoint was part of a shutdown, make sure the
//    debugged program is killed properly.
static void ShutdownCB(Widget w, XtPointer, XtPointer call_data)
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
	post_shutdown(gdb->title() + " is still busy.  "
		      "Shutdown anyway (and kill it)?", 
		      "quit_dialog", token, w);
	return;
    }

    string state;
    if (program_running(state))
    {
	// Program is still running; request confirmation
	post_shutdown("The program is running.  "
		      "Shutdown anyway (and kill it)?",
		      "shutdown_dialog", token, w);
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

// Create a shutdown dialog
static void post_shutdown(string text, String name,
			  XtCheckpointToken token, Widget w)
{
    // Program is still running; request confirmation
    Arg args[10];
    int arg;

    static Widget dialog = 0;
    if (dialog)
	DestroyWhenIdle(dialog);

    arg = 0;
    MString msg = rm(text);
    XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    dialog = verify(XmCreateQuestionDialog(find_shell(w), name,
					   args, arg));
    Delay::register_shell(dialog);
    XtAddCallback(dialog, XmNokCallback, 
		  ConfirmShutdownCB, XtPointer(token));
    XtAddCallback(dialog, XmNcancelCallback,
		  CancelShutdownCB, XtPointer(token));
    XtAddCallback(dialog, XmNhelpCallback, 
		  ImmediateHelpCB, 0);

    manage_and_raise(dialog);
}

#else // XtSpecificationRelease < 6

// X11R5 and earlier: Nothing yet...
void SaveSessionCB(Widget, XtPointer, XtPointer) {}
void DieSessionCB(Widget, XtPointer, XtPointer) {}

#endif // XtSpecificationRelease < 6
