// $Id$ -*- C++ -*-
// Exit DDD

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char exit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "exit.h"

#include "AgentM.h"
#include "AppData.h"
#include "DestroyCB.h"
#include "ExitCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "MString.h"
#include "commandQ.h"
#include "ddd.h"
#include "exectty.h"
#include "findParent.h"
#include "history.h"
#include "host.h"
#include "options.h"
#include "post.h"
#include "question.h"
#include "sigName.h"
#include "status.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"

#include <signal.h>
#include <iostream.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#if defined(HAVE_RAISE)
#if !defined(HAVE_RAISE_DECL)
extern int raise(int sig);
#endif
#else // !HAVE_RAISE
#define raise(sig) kill(getpid(), sig)
#endif

static void ddd_signal(int sig...);
static void ddd_fatal(int sig...);

// True if DDD is about to exit
bool ddd_is_exiting = false;

// True if DDD is about to restart
bool ddd_is_restarting = false;

//-----------------------------------------------------------------------------
// Signal handling
//-----------------------------------------------------------------------------

// Setup signals: Cleanup on termination
void ddd_install_signal()
{
#ifdef SIGHUP
    signal(SIGHUP, SignalProc(ddd_signal));
#endif

#ifdef SIGTERM
    signal(SIGTERM, SignalProc(ddd_signal));
#endif
}

static char *ddd_invoke_name = ddd_NAME;

// Setup signals: Issue message on fatal errors
void ddd_install_fatal(char *program_name)
{
    // Make sure strsignal() is initialized properly
    (void)sigName(1);

    // Save program name
    if (program_name != 0 && program_name[0] != '\0')
	ddd_invoke_name = program_name;

    // Install signal handlers
#ifdef SIGINT
    signal(SIGINT, SignalProc(ddd_fatal));
#endif

#ifdef SIGFPE
    signal(SIGFPE, SignalProc(ddd_fatal));
#endif

#ifdef SIGILL
    signal(SIGILL, SignalProc(ddd_fatal));
#endif

#ifdef SIGSEGV
    signal(SIGSEGV, SignalProc(ddd_fatal));
#endif

#ifdef SIGBUS
    signal(SIGBUS, SignalProc(ddd_fatal));
#endif

#ifdef SIGIOT
    signal(SIGIOT, SignalProc(ddd_fatal));
#endif

#ifdef SIGTRAP
    signal(SIGTRAP, SignalProc(ddd_fatal));
#endif

#ifdef SIGEMT
    signal(SIGEMT, SignalProc(ddd_fatal));
#endif

#ifdef SIGSYS
    signal(SIGSYS, SignalProc(ddd_fatal));
#endif
}

// Show the user that a signal has been raised
void ddd_show_signal(int sig)
{
    // Show diagnostic
    cerr << sigName(sig) << "\n";
    gdb_question_running = false;

    // Interrupt current GDB action
    gdb_keyboard_command = true;
    _gdb_command("\003", gdb_w);

    // Show the message in an error dialog,
    // allowing the user to clean up manually.
    if (sig != SIGINT)
    {
	static Widget fatal_dialog = 0;
	if (fatal_dialog)
	    DestroyWhenIdle(fatal_dialog);
	fatal_dialog = 
	    verify(XmCreateErrorDialog (find_shell(),
					"fatal_dialog", 0, 0));
	Delay::register_shell(fatal_dialog);

	XtAddCallback (fatal_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
	XtAddCallback (fatal_dialog, XmNokCallback,     DDDRestartCB,    0);
	XtAddCallback (fatal_dialog, XmNcancelCallback, DDDExitCB,       0);

	string msg = string("Internal error (") + sigName(sig) + ")";
	MString mtext(msg, "rm");
	XtVaSetValues (fatal_dialog,
		       XmNmessageString, mtext.xmstring(),
		       0);

	XtManageChild(fatal_dialog);

	// Wait until dialog is mapped, such that DDD will exit
	// if we get another signal during that time.
	wait_until_mapped(fatal_dialog);
    }
}


//-----------------------------------------------------------------------------
// Exiting
//-----------------------------------------------------------------------------

void ddd_cleanup()
{
    if (ddd_is_exiting)
	return;

    ddd_is_exiting = true;

    kill_exec_tty();
    if (command_shell && app_data.save_history_on_exit)
	save_history(command_shell);
    if (gdb)
	gdb->shutdown();
}

// Signal handler: clean up and re-raise signal
static void ddd_signal(int sig...)
{
    ddd_cleanup();
    signal(sig, SignalProc(SIG_DFL));
    raise(sig);
}

// Fatal signal handler: issue error message and re-raise signal
static void ddd_fatal(int sig...)
{
    // IF YOU GET HERE WHILE DEBUGGING DDD, READ THIS
    // ----------------------------------------------
    //
    // On some systems, especially HP-UX, the stack frame gets corrupted
    // when a program exits via a signal handler - one cannot determine
    // the place the signal handler was called from, which makes debugging
    // impossible.
    // 
    // You can circumvent this problem by invoking the debugged DDD
    // process with the environment variable DDD_NO_SIGNAL_HANDLERS set.
    // This disables installation of the `ddd_fatal' signal handler
    // and makes it possible for you to determine the problem cause.

    static int fatal_entered = 0;

    if (fatal_entered++ || !main_loop_entered || ddd_is_exiting)
    {
	static const char *msg =
	    "\nInternal error (%s).\n"
	    "\n"
	    "Oops!  You have found a bug in " DDD_NAME ".\n"
	    "\n"
	    "If you can reproduce this bug, please send a bug report\n"
	    "to `ddd-bugs@ips.cs.tu-bs.de', giving a subject like\n"
	    "\n"
	    "    " DDD_NAME " " DDD_VERSION 
	    " (" DDD_HOST ") gets `%s' signal\n"
	    "\n"
	    "To enable us to fix the bug, you should include "
	    "the following information:\n"
	    "  * What you were doing to get this message.  "
	    "Report all the facts.\n"
	    "  * Your " DDD_NAME " configuration.  "
	    "Run `" ddd_NAME " --configuration' to get it.\n"
	    "  * If a core file was generated in your directory, please run\n"
	    "    `gdb " ddd_NAME " core', "
	    "and type `where' at the `(gdb)' prompt.\n"
	    "    (Include this output only.  Do not include "
	    "the core file itself.)\n"
	    "  * Invoke " DDD_NAME " with the `--trace' option, "
	    "and if you can reproduce the bug,\n"
	    "    include the trace output in your bug report.\n"
	    "Please read also the section \"Reporting Bugs\" "
	    "in the " DDD_NAME " manual.\n"
	    "\n"
	    "We thank you for your support.\n\n";

	if (sig != SIGINT)
	    fprintf(stderr, msg, sigName(sig), sigName(sig));

	// Re-raise signal.  This should kill us as we return.
	ddd_signal(sig);
	return;
    }

    // Reinstall fatal error handlers
    ddd_install_fatal();

    // Return to main event loop
    fatal_entered--;
    longjmp(main_loop_env, sig);
}

static void SaveOptionsAndExitCB(Widget w, XtPointer client_data,
				 XtPointer call_data)
{
    ddd_cleanup();

    XtCallbackProc closure = 
	ddd_is_restarting ? XtCallbackProc(RestartCB) : XtCallbackProc(ExitCB);

    DDDSaveOptionsCB(w, client_data, call_data);
    closure(w, client_data, call_data);
}

// Exit callback
void _DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_cleanup();

    XtCallbackProc closure = ddd_is_restarting ? RestartCB : ExitCB;

    if (startup_preferences_changed())
    {
	// Startup options are still changed; request confirmation
	if (yn_dialog)
	    DestroyWhenIdle(yn_dialog);
	yn_dialog = 
	    verify(XmCreateQuestionDialog(find_shell(w), 
					  "save_options_dialog", 0, 0));
	Delay::register_shell(yn_dialog);
	XtAddCallback (yn_dialog, XmNokCallback,     SaveOptionsAndExitCB, 0);
	XtAddCallback (yn_dialog, XmNcancelCallback, closure, 0);
	XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
	XtManageChild(yn_dialog);
    }
    else
    {
	closure(w, client_data, call_data);
    }
}

// Exit after confirmation
void DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_is_restarting = false;

    if (gdb == 0 || !gdb->running())
    {
	_DDDExitCB(w, client_data, call_data);
	return;
    }

    if (gdb->isReadyWithPrompt())
    {
	gdb_command("quit");
	return;
    }

    // Debugger is still running; request confirmation
    if (yn_dialog)
	DestroyWhenIdle(yn_dialog);
    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "quit_dialog", 0, 0));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     _DDDExitCB, 0);
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    XtManageChild(yn_dialog);
}

// Restart after confirmation
void DDDRestartCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_is_restarting = true;

    if (gdb == 0 || !gdb->running())
    {
	_DDDExitCB(w, client_data, call_data);
	return;
    }

    if (gdb->isReadyWithPrompt())
    {
	gdb_command("quit");
	return;
    }

    // Debugger is still running; request confirmation
    if (yn_dialog)
	DestroyWhenIdle(yn_dialog);
    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "quit_dialog", 0, 0));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     _DDDExitCB, 0);
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    XtManageChild(yn_dialog);
}


// EOF on input/output detected
void gdb_eofHP(Agent *, void *, void *)
{
    // Kill and exit
    gdb->terminate();
}
