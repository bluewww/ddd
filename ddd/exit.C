// $Id$ -*- C++ -*-
// Exit DDD

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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
#include "charsets.h"
#include "commandQ.h"
#include "converters.h"
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

// True if DDD has crashed and needs restarting
bool ddd_has_crashed = false;


//-----------------------------------------------------------------------------
// General clean-up actions before exiting DDD
//-----------------------------------------------------------------------------

// Some clean-up actions before exiting
void ddd_cleanup()
{
    if (ddd_is_exiting)
	return;

    ddd_is_exiting = true;

    kill_exec_tty();
    if (gdb)
    {
	gdb->shutdown();
	gdb->terminate(true);
    }
    if (app_data.save_history_on_exit)
	save_history(command_shell);
}


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

// Post a dialog containing TITLE and CAUSE
static void post_fatal(string title, string cause)
{
    static Widget fatal_dialog = 0;
    if (fatal_dialog == 0)
    {
	fatal_dialog = verify(XmCreateErrorDialog (find_shell(),
						   "fatal_dialog", 0, 0));
	Delay::register_shell(fatal_dialog);

	XtAddCallback(fatal_dialog, XmNhelpCallback, ImmediateHelpCB, 0);
	XtAddCallback(fatal_dialog, XmNokCallback, 
		      DDDExitCB, XtPointer(EXIT_FAILURE));
    }

    defineConversionMacro("TITLE", title);
    defineConversionMacro("CAUSE", cause);

    string msg = string("Internal error (") + title + ")";
    MString mtext = rm(msg);
    XtVaSetValues (fatal_dialog,
		   XmNmessageString, mtext.xmstring(),
		   0);

    manage_and_raise(fatal_dialog);

    // Wait until dialog is mapped and synchronize, such that DDD will
    // exit if we get another signal or X error during that time.
    wait_until_mapped(fatal_dialog);
    XSync(XtDisplay(fatal_dialog), False);
}

// Show the user that a signal has been raised
void ddd_show_signal(int sig)
{
    // Show diagnostic
    if (sig > 0)
	cerr << sigName(sig) << "\n";
    gdb_question_running = false;

    // Interrupt current GDB action
    gdb_keyboard_command = true;
    _gdb_command("\003", gdb_w);

    // Show the message in an error dialog,
    // allowing the user to clean up manually.
    if (sig > 0 && sig != SIGINT)
    {
	string title = sigName(sig);
	string cause = "`" + title + "' signal";
	post_fatal(title, cause);
    }
}

// Signal handler: clean up and re-raise signal
static void ddd_signal(int sig...)
{
    ddd_cleanup();
    signal(sig, SignalProc(SIG_DFL));
    raise(sig);
}

// Issue fatal message on stderr
static void print_fatal_msg(char *title, char *cause)
{
    static const char *msg =
	"\nInternal error (%s).\n"
	"\n"
	"Oops!  You have found a bug in " DDD_NAME ".\n"
	"\n"
	"If you can reproduce this bug, please send a bug report\n"
	"to <ddd-bugs@ips.cs.tu-bs.de>, giving a subject like\n"
	"\n"
	"    " DDD_NAME " " DDD_VERSION 
	" (" DDD_HOST ") gets %s\n"
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

    fprintf(stderr, msg, title, cause);
}

// Fatal signal handler: issue error message and re-raise signal
static void ddd_fatal(int sig...)
{
    // IF YOU GET HERE WHILE DEBUGGING DDD, READ THIS
    // ----------------------------------------------
    //
    // On some systems, especially HP-UX, the stack frame gets
    // corrupted when a program exits via a signal handler - one
    // cannot determine the place the signal handler was called from,
    // which makes debugging impossible.
    // 
    // You can circumvent this problem by invoking the debugged DDD
    // process with the environment variable DDD_NO_SIGNAL_HANDLERS
    // set.  This disables installation of the `ddd_fatal' signal
    // handler and makes it possible for you to determine the problem
    // cause.

    ddd_has_crashed = true;

    static int fatal_entered = 0;

    if (fatal_entered++ || !main_loop_entered || ddd_is_exiting)
    {
	if (sig != SIGINT)
	{
	    char *title = sigName(sig);
	    char cause[BUFSIZ];
	    sprintf(cause, "`%s' signal", title);
	    print_fatal_msg(title, cause);
	}

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


//-----------------------------------------------------------------------------
// X I/O error
//-----------------------------------------------------------------------------

static int (*old_x_fatal_handler)(Display *display) = 0;

// Fatal X I/O error handler: cleanup and issue error message
static int ddd_x_fatal(Display *display)
{
    ddd_cleanup();
    return old_x_fatal_handler(display);
}

// Cleanup on fatal X I/O errors
void ddd_install_x_fatal()
{
    old_x_fatal_handler = XSetIOErrorHandler(ddd_x_fatal);
}


//-----------------------------------------------------------------------------
// Other X errors
//-----------------------------------------------------------------------------

static string xtext(Display *display, char *code, char *def, int arg = 0)
{
    char *mtype = "XlibMessage";
    char format[BUFSIZ];
    char message[BUFSIZ];
    XGetErrorDatabaseText(display, mtype, code, def, format, sizeof format);
    sprintf(message, format, arg);
    return message;
}

// Give a diagnostic on EVENT on OS.  Patterned after
// _XPrintDefaultError(dpy, event, fp) in X11R6.3.
static void print_x_error(Display *display, XErrorEvent *event, ostream& os)
{
    char buffer[BUFSIZ];
    XGetErrorText(display, event->error_code, buffer, sizeof buffer);

    os << xtext(display, "XError", "X Error")
       << ":  " << buffer << "\n  "
       << xtext(display, "MajorCode", "Request Major code %d", 
		event->request_code);
    
    char number[32];
    sprintf(number, "%d", event->request_code);
    XGetErrorDatabaseText(display, "XRequest", 
			  number, "", buffer, sizeof buffer);
    os << " (" << buffer << ")\n";

    if (event->request_code >= 128)
	os << xtext(display, "MinorCode", "Request Minor code %d\n",
		    event->minor_code);

    switch (event->error_code)
    {
    case BadWindow:
    case BadPixmap:
    case BadCursor:
    case BadFont:
    case BadDrawable:
    case BadColor:
    case BadGC:
    case BadIDChoice:
	os << "  "
	   << xtext(display, "ResourceID", "ResourceID 0x%x", 
		    event->resourceid)
	   << "\n";
	break;

    case BadValue:
	os << "  "
	   << xtext(display, "Value", "Value 0x%x", event->resourceid)
	   << "\n";
	break;

    case BadAtom:
	os << "  "
	   << xtext(display, "AtomID", "AtomID 0x%x", event->resourceid)
	   << "\n";
	break;
    }

    os << "  " 
       << xtext(display, "ErrorSerial", "Error Serial #%d", event->serial)
       << "\n"
       << "  "
       << xtext(display, "CurrentSerial", "Current Serial #%d", 
		NextRequest(display) - 1) 
       << "\n";
}

static int (*old_x_error_handler)(Display *, XErrorEvent *) = 0;

static void PostXErrorCB(XtPointer client_data, XtIntervalId *)
{
    string *msg_ptr = (string *)client_data;
    string msg = *msg_ptr;
    delete msg_ptr;

    string title = msg.before('\v');
    string cause = msg.after('\v');
    post_fatal(title, cause);
}

static bool recovered_from_x_error = true;

static Boolean recovery_done(XtPointer)
{
    recovered_from_x_error = true;
    return True;		// Remove from the list of work procs
}

// X error handler: cleanup and issue error message
static int ddd_x_error(Display *display, XErrorEvent *event)
{
    if (event->error_code == BadImplementation)
    {
	// Taken care of by the standard X handler - just proceed
	return old_x_error_handler(display, event);
    }

    // Fetch precise diagnostics
    char buffer[BUFSIZ];
    XGetErrorText(display, event->error_code, buffer, sizeof buffer);
    string cause = buffer;
    if (cause.contains(" ("))
	cause = cause.before(" (");
    cause = "`" + cause + "' error";

    string title = buffer;
    if (title.contains('('))
	title = title.after('(');
    if (title.contains(')'))
	title = title.before(')');

    if (!recovered_from_x_error)
    {
	// Not recovered from last X error

	// Ignore being called while cleaning up
	static bool entered = false;
	if (entered)
	    return 0;
	entered = true;

	// Exit after diagnostics
	ddd_cleanup();
	print_x_error(display, event, cerr);
	print_fatal_msg(title, cause);
	exit(EXIT_FAILURE);
    }

    // Issue error on stderr
    print_x_error(display, event, cerr);

    // Prepare for issuing error in dialog
    string *msg_ptr = new string(title + '\v' + cause);
    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    XtAppAddTimeOut(app_context, 0, PostXErrorCB, XtPointer(msg_ptr));

    // Set RECOVERED_FROM_X_ERROR to FALSE until DDD is idle again
    recovered_from_x_error = false;
    XtAppAddWorkProc(app_context, recovery_done, XtPointer(0));

    return 0;			// Keep on acting
}

// Cleanup on fatal X I/O errors
void ddd_install_x_error()
{
    old_x_error_handler = XSetErrorHandler(ddd_x_error);
}


//-----------------------------------------------------------------------------
// Xt and Motif errors
//-----------------------------------------------------------------------------

static XtErrorHandler old_xt_error_handler;

static void ddd_xt_error(String message = 0)
{
    if (message == 0 || message[0] == '\0')
    {
	ddd_cleanup();
	exit(EXIT_FAILURE);
    }
	
    static int entered = 0;

    // Issue error on stderr
    cerr << "Error: " << message << "\n";

    string title = message;
    string cause = "Xt error";

    if (entered++ || !main_loop_entered || ddd_is_exiting)
    {
	print_fatal_msg(title, cause);
	ddd_xt_error();
    }

    // Issue error in dialog
    string *msg_ptr = new string(title + '\v' + cause);
    XtAppContext app_context = XtWidgetToApplicationContext(command_shell);
    XtAppAddTimeOut(app_context, 0, PostXErrorCB, XtPointer(msg_ptr));

    // Return to main event loop
    entered--;
    longjmp(main_loop_env, -1);
}

void ddd_install_xt_error(XtAppContext app_context)
{
    old_xt_error_handler = XtAppSetErrorHandler(app_context, ddd_xt_error);
}



//-----------------------------------------------------------------------------
// GDB I/O error
//-----------------------------------------------------------------------------

// EOF on input/output detected
void gdb_eofHP(Agent *, void *, void *)
{
    // Kill and exit
    gdb->terminate();
}


// GDB died
void gdb_diedHP(Agent *gdb, void *, void *call_data)
{
    char *reason = (char *)call_data;
    post_gdb_died(reason, gdb->lastStatus());
}


//-----------------------------------------------------------------------------
// Controlled exiting
//-----------------------------------------------------------------------------

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
	static Widget save_options_dialog = 0;
	if (save_options_dialog == 0)
	{
	    save_options_dialog = 
		verify(XmCreateQuestionDialog(find_shell(w), 
					      "save_options_dialog", 0, 0));
	    Delay::register_shell(save_options_dialog);
	    XtAddCallback(save_options_dialog, XmNokCallback,
			  SaveOptionsAndExitCB, client_data);
	    XtAddCallback(save_options_dialog, XmNcancelCallback,
			  closure, client_data);
	    XtAddCallback(save_options_dialog, XmNhelpCallback,
			  ImmediateHelpCB, 0);
	}
	manage_and_raise(save_options_dialog);
    }
    else
    {
	closure(w, client_data, call_data);
    }
}

// Exit/Restart after confirmation, depending on the setting of
// DDD_IS_RESTARTING
static void DDDDoneCB(Widget w, XtPointer client_data, XtPointer call_data)
{
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
    Arg args[10];
    int arg;

    static Widget quit_dialog = 0;
    if (quit_dialog)
	DestroyWhenIdle(quit_dialog);

    arg = 0;
    MString msg = rm(gdb->title() + " is still busy.  "
		     + (ddd_is_restarting ? "Restart" : "Exit")
		     + " anyway (and kill it)?");
    XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    quit_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "quit_dialog", args, arg));
    Delay::register_shell(quit_dialog);
    XtAddCallback(quit_dialog, XmNokCallback,   _DDDExitCB, client_data);
    XtAddCallback(quit_dialog, XmNcancelCallback, UnmanageThisCB, quit_dialog);
    XtAddCallback(quit_dialog, XmNhelpCallback, ImmediateHelpCB, 0);

    manage_and_raise(quit_dialog);
}

// Exit after confirmation
void DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_is_restarting = false;
    DDDDoneCB(w, client_data, call_data);
}

// Restart after confirmation
void DDDRestartCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_is_restarting = true;
    DDDDoneCB(w, client_data, call_data);
}
