// $Id$ -*- C++ -*-
// Exit DDD (including fatal exits)

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char exit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

// Debug the Debugger
// ------------------
// 
// I appreciate that the April 1997 _Communications_
// covers the issue of debugging
// and hope we programmers can be relieved
// from the pain of debugging
// with the help of advanced tools.
// However, all I'm looking for today
// is a well-debugged debugger.
// I have been working with C/C++ debuggers
// from several Unix vendors
// as well as a Microsoft Windows-based debugger.
// All of them more than occasionally fail
// to perform some very basic function,
// such as setting a break point,
// displaying a data structure,
// evaluating an expression,
// and looking up a symbol.
// Some of them simply crash too often.
//
// My observation is that the debugger
// is often buggier
// than the program being debugged.
// One debugging difficulty Eisenstadt points out
// is the inability of the debugging tool.
// This is certainly true.
// I should emphasize that the difficulty
// is often a debugger-reliability problem
// rather than a capability problem.
//
// W. QUOCK, San Mateo, California
// Communications of the ACM, August 1997/Vol.40, No. 8, p. 31

#include "exit.h"

#include "AgentM.h"
#include "AppData.h"
#include "DestroyCB.h"
#include "ExitCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "TimeOut.h"
#include "charsets.h"
#include "Command.h"
#include "cmdtty.h"
#include "converters.h"
#include "ddd.h"
#include "exectty.h"
#include "findParent.h"
#include "filetype.h"
#include "fonts.h"
#include "history.h"
#include "host.h"
#include "longName.h"
#include "mainloop.h"
#include "misc.h"
#include "options.h"
#include "post.h"
#include "question.h"
#include "session.h"
#include "shell.h"
#include "sigName.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"

#include <signal.h>
#include <iostream.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>

#if HAVE_RAISE
#if !HAVE_RAISE_DECL
extern int raise(int sig);
#endif
#else // !HAVE_RAISE
#define raise(sig) kill(getpid(), sig)
#endif

#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif

static void ddd_signal(int sig...);
static void ddd_fatal(int sig...);
static bool ddd_dump_core(int sig...);
static void debug_ddd(bool core_dumped = true);

// True if GDB is about to exit
bool gdb_is_exiting = false;

// True if DDD is about to exit
bool ddd_is_exiting = false;

// True if DDD is about to restart
bool ddd_is_restarting = false;

// True if DDD has crashed and needs restarting
bool ddd_has_crashed = false;

static void DDDDoneAnywayCB(Widget w, XtPointer client_data, 
			    XtPointer call_data);

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
    kill_command_tty();

    if (gdb)
    {
	gdb->shutdown();
	gdb->terminate(true);
    }

    if (gdb_initialized)
    {
	// If GDB created a history file, overwrite it such that it is not
	// cluttered with internal DDD commands.
	if (is_regular_file(gdb_history_file()))
	    save_history(gdb_history_file(), command_shell);

	// Now write the session-specific DDD history file.
	if (app_data.save_history_on_exit)
	    save_history(session_history_file(app_data.session));
    }

    // Famous last words
    string last_words = "Thanks for using " DDD_NAME " " DDD_VERSION "!";
    if (ddd_has_crashed)
	last_words += "  (We apologize for the inconvenience.)";
    set_status_mstring(rm(last_words));

    // Flush log file
    dddlog.flush();

    // Unlock `~/.ddd/'.
    unlock_session_dir(DEFAULT_SESSION);
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

#ifdef SIGQUIT
    signal(SIGQUIT, SignalProc(ddd_signal));
#endif
}

// Setup signals: Issue message on fatal errors
void ddd_install_fatal(const char * /* program_name */)
{
    // Make sure strsignal() is initialized properly
    (void)sigName(1);

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

#ifdef SIGABRT
    signal(SIGABRT, SignalProc(ddd_fatal));
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

#ifdef SIGUSR1
    signal(SIGUSR1, SignalProc(ddd_dump_core));
#endif
}

// Post a dialog containing TITLE and CAUSE
static void post_fatal(string title, string cause, string cls, 
		       bool core_dumped = false)
{
    (void) core_dumped;		// Use it

    static Widget fatal_dialog = 0;

#if DEBUG_BUTTON
    static Widget debug = 0;
#endif

    if (fatal_dialog == 0)
    {
	fatal_dialog = verify(XmCreateErrorDialog (find_shell(),
						   (char *)"fatal_dialog", 
						   0, 0));
	Delay::register_shell(fatal_dialog);

	XtAddCallback(fatal_dialog, XmNhelpCallback, ImmediateHelpCB, 0);
	XtAddCallback(fatal_dialog, XmNokCallback, 
		      DDDRestartCB, XtPointer(EXIT_FAILURE));

#if XmVersion >= 1002
	Widget exit = verify(XmCreatePushButton(fatal_dialog, 
						(char *)"exit", 0, 0));
	XtManageChild(exit);
	XtAddCallback(exit, XmNactivateCallback,
		      DDDExitCB, XtPointer(EXIT_FAILURE));

#if DEBUG_BUTTON
	debug = verify(XmCreatePushButton(fatal_dialog, "debug", 0, 0));
	XtAddCallback(debug, XmNactivateCallback, DDDDebugCB, XtPointer(True));
#endif

#endif
    }

#if DEBUG_BUTTON
    if (core_dumped)
	XtManageChild(debug);
    else
	XtUnmanageChild(debug);
#endif

    defineConversionMacro("CLASS", cls);
    defineConversionMacro("TITLE", title);
    defineConversionMacro("CAUSE", cause);

    string msg = cls + ": " + title;
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
    bool core_dumped = false;
    if (sig < 0)
    {
	core_dumped = true;
	sig = -sig;
    }

    // Show diagnostic
    if (sig > 0 && (sig != SIGINT || !tty_running()))
    {
	string s = sigName(sig);
	if (core_dumped)
	    s += " (core dumped)";
	cerr << s << "\n";
    }
    gdb_question_running = false;

    // Interrupt current GDB action
    gdb_keyboard_command = true;
    gdb_command("\003", gdb_w);

    // Show the message in an error dialog,
    // allowing the user to clean up manually.
    if (sig > 0 && sig != SIGINT)
    {
	string title = sigName(sig);
	string cause = "`" + title + "' signal";

	if (core_dumped)
	    title += " (core dumped)";

	post_fatal(title, cause, "Internal error", core_dumped);
    }
}

// Signal handler: clean up and re-raise signal
static void ddd_signal(int sig...)
{
    ddd_cleanup();
    signal(sig, SignalProc(SIG_DFL));
    raise(sig);
}

// Show the user that a C++ exception has been raised
void ddd_show_exception(const char *c, const char *what)
{
    gdb_question_running = false;

    // Interrupt current GDB action
    gdb_keyboard_command = true;
    gdb_command("\003", gdb_w);

    // Show the message in an error dialog,
    // allowing the user to clean up manually.

    if (what == 0)
	what = "C++ exception";

    string title = what;
    string cause = what;
    string cls   = "Internal error";

    // EGCS 1.0 prepends the name length to type names; fix this
    while (isdigit(*c))
	c++;

    if (c != 0)
	cls = c;

    cls.gsub('_', ' ');
    cls[0] = toupper(cls[0]);

    if (c != 0)
	cause.prepend(cls + ": ");

    post_fatal(title, cause, cls);
}

// Issue fatal message on stderr
static void print_fatal_msg(const char *title, const char *cause, 
			    const char *cls)
{
    static const char *msg =
	"\n%s (%s).\n"
	"\n"
	"Oops!  You have found a bug in " DDD_NAME ".\n"
	"\n"
	"If you can reproduce this bug, please send a bug report\n"
	"to <bug-ddd@gnu.org>, giving a subject like\n"
	"\n"
	"    " DDD_NAME " " DDD_VERSION 
	" (" DDD_HOST ") gets %s\n"
	"\n"
	"To enable us to fix the bug, you should include "
	"the following information:\n"
	"* What you were doing to get this message.  Report all the facts.\n"
	"* The contents of the `~/." ddd_NAME "/log' file "
	"as generated by this session.\n"
	"Please read also the section \"Reporting Bugs\" "
	"in the " DDD_NAME " manual.\n"
	"\n"
	"We thank you for your support.\n\n";

    fprintf(stderr, msg, cls, title, cause);
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

    if (sig != SIGINT)
	ddd_has_crashed = true;

    // Make sure we have a ~/.ddd/log file
    init_dddlog();

    dddlog << "!  " << sigName(sig) << "\n";
    dddlog.flush();

    // Reinstall fatal error handlers (for SVR4 and others)
    ddd_install_fatal();

    static int fatal_entered = 0;
    fatal_entered++;

    bool have_core_file = false;
    if (fatal_entered == 1 && sig != SIGINT && 
	(!main_loop_entered || app_data.dump_core))
    {
	// Create core file (without interrupting DDD)
	if (ddd_dump_core(sig))
	{
	    // We are the child: return to originating sequence such
	    // that we can dump core.
	    return;
	}

	have_core_file = is_core_file("core");
    }

    if (fatal_entered > 1 || !main_loop_entered || ddd_is_exiting)
    {
	// We're done.  Suicide is painless.
	if (have_core_file)
	    report_core(dddlog);

	if (sig != SIGINT)
	{
	    const char *title = sigName(sig);
	    char cause[BUFSIZ];
	    sprintf(cause, "`%s' signal", title);
	    print_fatal_msg(title, cause, "Internal error");
	}

	// Re-raise signal.  This should kill us as we return.
	ddd_signal(sig);
	return;
    }

    // Return to main event loop
    fatal_entered--;
    goto_main_loop(have_core_file ? -sig : sig);
}


//-----------------------------------------------------------------------------
// Dump core (for debugging)
//-----------------------------------------------------------------------------

// Dump core using SIG; return true iff child
static bool ddd_dump_core(int sig...)
{
    unlink("core");

    int core_pid;
    if ((core_pid = fork()) == 0)
    {
	// I am the child: re-raise signal
#if defined(SIGUSR1)
	if (sig == SIGUSR1)
	{
	    // SIGUSR1 does not cause a core dump; use abort() instead
#if defined(SIGABRT)
	    sig = SIGABRT;
#elif defined(SIGIOT)
	    sig = SIGIOT;
#endif // defined(SIGIOT)
	}
#endif // defined(SIGUSR1)

	// Re-raise signal.  This should kill us as we return.
	signal(sig, SignalProc(SIG_DFL));
	raise(sig);
	return true;
    }

    if (core_pid < 0)
    {
	perror(ddd_NAME);
    }
    else
    {
	// Wait for the child to finish
	int status;
	pid_t ret = waitpid(core_pid, &status, 0);
	if (ret < 0)
	    perror(ddd_NAME);
    }

#if defined(SIGUSR1)
    if (sig == SIGUSR1)
    {
	// Re-install handler (for SVR4 and others)
	signal(sig, SignalProc(ddd_dump_core));
    }
#endif // defined(SIGUSR1)

    return false;
}

void DDDDumpCoreCB(Widget, XtPointer, XtPointer)
{
    StatusDelay delay("Dumping core");

#if defined(SIGABRT)
    if (ddd_dump_core(SIGABRT))
	exit(EXIT_FAILURE);
#elif defined(SIGIOT)
    if (ddd_dump_core(SIGIOT))
	exit(EXIT_FAILURE);
#else
    abort();
#endif

    if (!is_core_file("core"))
	delay.outcome = "failed";
}

//-----------------------------------------------------------------------------
// X I/O error
//-----------------------------------------------------------------------------

static int (*old_x_fatal_handler)(Display *display) = 0;

// Fatal X I/O error handler: cleanup and issue error message
static int ddd_x_fatal(Display *display)
{
    int saved_errno = errno;

    if (errno != EPIPE)
    {
	ddd_has_crashed = true;
	dddlog << "!  X I/O error\n";
	dddlog.flush();
    }
    ddd_cleanup();

    errno = saved_errno;
    return old_x_fatal_handler(display);
}

// Cleanup on fatal X I/O errors
void ddd_install_x_fatal()
{
    old_x_fatal_handler = XSetIOErrorHandler(ddd_x_fatal);
}


//-----------------------------------------------------------------------------
// Xt and Motif errors
//-----------------------------------------------------------------------------

static void PostXtErrorCB(XtPointer client_data, XtIntervalId *)
{
    string *msg_ptr = (string *)client_data;
    string msg = *msg_ptr;
    delete msg_ptr;

    string title = msg.before('\v');
    string cause = msg.after('\v');
    post_fatal(title, cause, "X Toolkit Error");
}

static XtAppContext xt_error_app_context = 0;

static void ddd_xt_error(String message = 0)
{
    ddd_has_crashed = true;

    dddlog << "!  Xt error";
    if (message != 0)
	dddlog << ": " << message;
    dddlog << "\n";
    dddlog.flush();

    if (message == 0 || message[0] == '\0')
    {
	// Occurred twice in a row
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
	print_fatal_msg(title, cause, "Xt error");
	ddd_xt_error();
    }

    // Issue error in dialog
    string *msg_ptr = new string(title + '\v' + cause);
    XtAppAddTimeOut(xt_error_app_context, 0, PostXtErrorCB, 
		    XtPointer(msg_ptr));

    // Return to main event loop
    entered--;
    goto_main_loop(-1);
}

void ddd_install_xt_error(XtAppContext app_context)
{
    (void) XtAppSetErrorHandler(app_context, ddd_xt_error);
    xt_error_app_context = app_context;
}


//-----------------------------------------------------------------------------
// Other X errors
//-----------------------------------------------------------------------------

static void PostXErrorCB(XtPointer client_data, XtIntervalId *)
{
    string *msg_ptr = (string *)client_data;
    string msg = *msg_ptr;
    delete msg_ptr;

    string title = msg.before('\v');
    string cause = msg.after('\v');
    post_fatal(title, cause, "X Error");
}

static string xtext(Display *display, const char *code, const char *def, int arg = 0)
{
    const char *mtype = "XlibMessage";
    char format[BUFSIZ];
    char message[BUFSIZ];
    XGetErrorDatabaseText(display, mtype, code, def, format, sizeof format);
    sprintf(message, format, arg);
    return message;
}

// Give a diagnostic on EVENT on OS.  Patterned after
// _XPrintDefaultError(dpy, event, fp) in X11R6.3, but also issue the
// widget associated with the resource (as in Netscape).
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

    int resourceid = 0;
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
    case BadMatch:
	os << "  "
	   << xtext(display, "ResourceID", "ResourceID 0x%x", 
		    event->resourceid)
	   << "\n";
	resourceid = event->resourceid;
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

    case BadAccess:
    case BadAlloc:
    case BadImplementation:
    case BadLength:
    case BadName:
    case BadRequest:
	// Nothing special
	break;
    }

    os << "  " 
       << xtext(display, "ErrorSerial", "Error Serial #%d", event->serial)
       << "\n"
       << "  "
       << xtext(display, "CurrentSerial", "Current Serial #%d", 
		NextRequest(display) - 1) 
       << "\n";

    if (resourceid != 0)
    {
	os << "  Widget hierarchy of resource:  ";
	Widget w = XtWindowToWidget(display, resourceid);
	if (w == 0)
	    os << "unknown";
	else
	    os << longName(w);
	os << '\n';
    }

    os.flush();
}

static int (*old_x_error_handler)(Display *, XErrorEvent *) = 0;

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

    ddd_has_crashed = true;

    dddlog << "!  X error\n";
    dddlog.flush();

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
	print_x_error(display, event, dddlog);
	print_fatal_msg(title, cause, "X error");
	exit(EXIT_FAILURE);
    }

    // Issue error on stderr and DDD log
    print_x_error(display, event, cerr);
    print_x_error(display, event, dddlog);

    if (xt_error_app_context != 0)
    {
	// Prepare for issuing error in dialog
	string *msg_ptr = new string(title + '\v' + cause);
	XtAppAddTimeOut(xt_error_app_context, 0, 
			PostXErrorCB, XtPointer(msg_ptr));

	// Set RECOVERED_FROM_X_ERROR to FALSE until DDD is idle again
	recovered_from_x_error = false;
	XtAppAddWorkProc(xt_error_app_context, recovery_done, XtPointer(0));
    }

    return 0;			// Keep on acting
}

// Cleanup on fatal X I/O errors
void ddd_install_x_error()
{
    old_x_error_handler = XSetErrorHandler(ddd_x_error);
}




//-----------------------------------------------------------------------------
// GDB I/O error
//-----------------------------------------------------------------------------

// EOF on input/output detected
void gdb_eofHP(Agent *agent, void *, void *)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    if (gdb != 0)
    {
	set_status(gdb->title() + ": EOF detected");
    }

    if (app_data.terminate_on_eof)
    {
	// Kill GDB
	agent->terminate();
    }
}


static XtIntervalId post_exception_timer = 0;
static void PostExceptionCB(XtPointer, XtIntervalId *id)
{
    assert (*id == post_exception_timer);
    (void) id;			// Use it
    post_exception_timer = 0;

    post_gdb_died(gdb->title() + ": internal exception", -1);
}

// Internal exception (JDB)
void gdb_exceptionHP(Agent *agent, void *, void *call_data)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, agent);
    if (gdb == 0)
	return;

    bool exception_state = (int)(long)call_data;
    
    if (exception_state)
    {
	// Entered exception state
	assert (post_exception_timer == 0);

	// Try sending a simple nop command
#if 1
	gdb->send_user_ctrl_cmd(gdb->nop_command() + "\n");
#else
	Command c(gdb->nop_command());
	c.echo     = false;
	c.verbose  = false;
	c.priority = COMMAND_PRIORITY_AGAIN;
	gdb_command(c);
#endif

	// Wait 5 seconds before offering a restart
	post_exception_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 5000,
			    PostExceptionCB, 0);
    }
    else
    {
	// Left exception state (i.e. prompt appeared)

	if (post_exception_timer)
	    XtRemoveTimeOut(post_exception_timer);
	post_exception_timer = 0;
    }
}


// GDB died
void gdb_diedHP(Agent *gdb, void *, void *call_data)
{
    if (running_shells() > 0)
    {
	char *reason = (char *)call_data;
	post_gdb_died(reason, gdb->lastStatus());
    }
    else
    {
	// No shell open (yet).  If we get here, this is usually
	// because we could not invoke the inferior debugger.
	
	if (!tty_running())
	{
	    // Forward diagnostics from debugger console to stderr
	    String s = XmTextGetString(gdb_w);
	    string message = s + messagePosition;
	    XtFree(s);
	    cerr << message;
	}

	_DDDExitCB(gdb_w, XtPointer(EXIT_FAILURE), XtPointer(0));
    }

    // Don't care about any exceptions seen before the exit
    if (post_exception_timer)
	XtRemoveTimeOut(post_exception_timer);
    post_exception_timer = 0;
}


//-----------------------------------------------------------------------------
// Controlled exiting
//-----------------------------------------------------------------------------

// Exit callback
void _DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    ddd_cleanup();

    XtCallbackProc closure = ddd_is_restarting ? RestartCB : ExitCB;
    closure(w, client_data, call_data);
}

// `quit' has been canceled
static void DDDQuitCanceledCB(const string&, void *)
{
    gdb_is_exiting = false;
}

// Exit/Restart after confirmation, depending on the setting of
// DDD_IS_RESTARTING
static void DDDDoneCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    gdb_is_exiting = true;

    if (gdb == 0 || !gdb->running())
    {
	_DDDExitCB(w, client_data, call_data);
	return;
    }

    if (can_do_gdb_command())
    {
	Command c("quit", w);	// This works for all inferior debuggers
	c.callback = DDDQuitCanceledCB;
	gdb_command(c);
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
						(char *)"quit_dialog", 
						args, arg));
    Delay::register_shell(quit_dialog);
    XtAddCallback(quit_dialog, XmNokCallback,   DDDDoneAnywayCB, client_data);
    XtAddCallback(quit_dialog, XmNcancelCallback, UnmanageThisCB, quit_dialog);
    XtAddCallback(quit_dialog, XmNhelpCallback, ImmediateHelpCB, 0);

    manage_and_raise(quit_dialog);
}

// Exit immediately if DDD is not ready
static void DDDDoneAnywayCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    // If GDB has gotten ready in between, use controlled exit.
    if (gdb && gdb->isReadyWithPrompt())
	DDDDoneCB(w, client_data, call_data);
    else
	_DDDExitCB(w, client_data, call_data);
}

// Exit after confirmation
void DDDExitCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    // Delete temporary restart session, if any
    delete_session(restart_session(), true);
    set_restart_session();

    // Delete temporary session, if any
    if (is_temporary_session(app_data.session))
	delete_session(app_data.session);

    ddd_is_restarting = false;
    DDDDoneCB(w, client_data, call_data);
}


// Restart unconditionally
static void _DDDRestartCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    static string initial_session;
    initial_session = app_data.session;
    app_data.initial_session = initial_session;

    set_session(RESTART_SESSION);

    unsigned long flags = (unsigned long)client_data;
    DDDSaveOptionsCB(w, XtPointer(flags), call_data);

    set_restart_session(app_data.session);
    register_environ();

    ddd_is_restarting = true;
    DDDDoneCB(w, client_data, call_data);
}


// Restart after confirmation
void DDDRestartCB(Widget w, XtPointer, XtPointer call_data)
{
    unsigned long flags = 
	SAVE_SESSION | SAVE_GEOMETRY | DONT_RELOAD_CORE | DONT_COPY_CORE;
    if (gdb->running())
	flags |= SAVE_CORE;

    if (saving_options_kills_program(flags))
    {
	// Saving session would kill program; request confirmation
	static Widget dialog = 0;
	if (dialog)
	    DestroyWhenIdle(dialog);

	dialog = verify(
	    XmCreateQuestionDialog(find_shell(w),
				   (char *)"confirm_restart_dialog",
				   0, 0));
	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, _DDDRestartCB,
		      XtPointer(flags | MAY_KILL));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, 0);
    
	manage_and_raise(dialog);
    }
    else
	_DDDRestartCB(w, XtPointer(flags), call_data);
}

static void debug_ddd(bool core_dumped)
{
    StatusDelay delay("Invoking Debug Window");

    string term_command = app_data.term_command;
    term_command.gsub("Execution", "Debug");
    term_command.gsub("@FONT@", make_font(app_data, FixedWidthDDDFont));

    string gdb_command = string("gdb ") + saved_argv()[0] + " ";

    if (core_dumped)
	gdb_command += "core";
    else
	gdb_command += itostring(getpid());

#if 0
    gdb_command.prepend("echo \"Debugging " DDD_NAME ".  "
			"Enter \\`quit' to quit.\"; ");
#endif

    term_command += " " + sh_quote(gdb_command) + " &";
    system(sh_command(term_command, true));
}

// Insert `where' info into LOG
void report_core(ostream& log)
{
    if (!is_core_file("core"))
	return;

    string tempfile = tmpnam(0);
    ofstream os(tempfile);
    os << 
	"set verbose off\n"
	"set height 0\n"
	"where\n"
	"quit\n";
    os.close();

    string gdb_command = 
	sh_command("gdb -x " + tempfile + " " + saved_argv()[0] + " core", 
		   true);

    FILE *fp = popen(gdb_command, "r");
    int c;
    while ((c = getc(fp)) != EOF)
	log << (char)c;
    log.flush();

    pclose(fp);

    unlink(tempfile);
}

// Debug DDD
void DDDDebugCB(Widget, XtPointer client_data, XtPointer)
{
    bool core_dumped = (int)(long)client_data;
    debug_ddd(core_dumped);
}
