// $Id$ -*- C++ -*-
// Execution TTY

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char exectty_rcsid[] = 
    "$Id$";

#include "exectty.h"

#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "LiterateA.h"
#include "MString.h"
#include "SignalB.h"
#include "charsets.h"
#include "config.h"
#include "cook.h"
#include "ddd.h"
#include "disp-read.h"
#include "fonts.h"
#include "logo.h"
#include "post.h"
#include "question.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "windows.h"
#include "wm.h"

#include <fstream>
#include <signal.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>


extern "C" {
// The GNU termcap declarations should also work for non-GNU termcap
// implementations.  We do not include the system declaration files here
// (i.e. <curses.h> or <term.h>), since these include lots of other
// macros and declarations cluttering our namespace.
  int tgetent (char *buffer, const char *termtype);
  char *tgetstr (const char *name, char **area);
}

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif

//-----------------------------------------------------------------------------
// Separate tty
//-----------------------------------------------------------------------------

// TTY name
static string separate_tty_name = "/dev/tty";

// TTY pid (0: not initialized, -1: failed)
static pid_t separate_tty_pid   = 0;

// TTY terminal type
static string separate_tty_term  = "dumb";

// TTY window
static Window separate_tty_window = 0;

// GDB command redirection
static string gdb_redirection = "";

// GDB run command
static string gdb_run_command = "";

// GDB TTY
string gdb_tty = "";

// True if the next 'Starting' line is to be displayed in the separate tty
static bool show_starting_line_in_tty    = false;

pid_t exec_tty_pid()     { return separate_tty_pid; }
Window exec_tty_window() { return separate_tty_window; }

static void CancelTTYCB(Widget, XtPointer client_data, XtPointer)
{
    bool *flag = (bool *)client_data;
    *flag = true;
}

static void GotReplyHP(Agent *, void *client_data, void *call_data)
{
    DataLength *d = (DataLength *)call_data;
    string& reply = *(string *)client_data;
    reply += string(d->data, d->length);
}

// Create a separate tty window; return its name in TTYNAME, its
// process id in PID, its terminal type in TERM, and its window id in
// WINDOWID.
static void launch_separate_tty(string& ttyname, pid_t& pid, string& term,
				Window& windowid, Widget origin)
{
    // If we're already running, all is done.
    if (pid > 0 && (remote_gdb() || kill(pid, 0) == 0))
	return;

    string term_command = app_data.term_command;
    term_command.gsub("@FONT@", make_font(app_data, FixedWidthDDDFont));

    static bool canceled;
    canceled = false;

    static Widget dialog = 0;
    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNdialogStyle, 
		 XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	dialog = verify(XmCreateWorkingDialog(find_shell(origin), 
					      XMST("launch_tty_dialog"), 
					      args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, 
					     XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, 
					     XmDIALOG_HELP_BUTTON));
	XtAddCallback(dialog, XmNcancelCallback, CancelTTYCB,
		      XtPointer(&canceled));
    }

    string base = term_command;
    if (base.contains(' '))
	base = base.before(' ');
    MString msg = rm("Starting ") + tt(base) + rm("...");
    XtVaSetValues(dialog, XmNmessageString, msg.xmstring(), XtPointer(0));
    manage_and_raise(dialog);
    wait_until_mapped(dialog);

    StatusDelay delay("Starting execution window");

    // Fill in defaults
    ttyname = "";
    pid     = -1;

    string command = 
	
	// Set up a temporary file in TMP.
	"tmp=${TMPDIR-/tmp}/ddd$$; export tmp; "

	// Be sure to remove it when exiting...
	"trap \"rm -f $tmp\" 0; "

	// ... or being interrupted.
	"trap 'exit 1' 1 2 15; "

	// Now execute the xterm command
	+ term_command +

	// which saves TTY, PID, TERM, and WINDOWID in TMP and goes to
	// sleep forever.  Signal 2 (SIGINT) is blocked for two
	// reasons: first, we don't want ^C to kill the tty window;
	// second, later invocations will send us SIGINT to find out
	// whether we're still alive.
	" '"
	"echo `tty` $$ $TERM $WINDOWID >$tmp; "
	"trap \"\" 2; "
	"while true; do sleep 3600; done"
	"' "

	// The whole thing is redirected and in the background such
	// that rsh won't wait for us.
	">/dev/null </dev/null 2>&1 & "

	// The main file waits for TMP to be created...
	"while test ! -s $tmp; do sleep 1; done; "

	// ...and sends TMP's contents to stdout, where DDD is waiting.
	"cat $tmp";

    if (pid > 0 && remote_gdb())
    {
	// We're already running.  Don't start a new tty
	// if the old one is still running.
	std::ostringstream os;
	os << "kill -2 " << pid << " 2>/dev/null"
	   << " || ( " << command << " )";
	command = string(os);
    }

    command = sh_command(command);

    {
	XtAppContext app_context = XtWidgetToApplicationContext(dialog);
	LiterateAgent tty(app_context, command);

	string reply = "";
	tty.addHandler(Input, GotReplyHP, (void *)&reply);
	tty.start();

	while (!reply.contains('\n') && !canceled && tty.running())
	    XtAppProcessEvent(app_context, XtIMAll);

	if (reply.length() > 2)
	{
	    std::istringstream is(reply.chars());
	    is >> ttyname >> pid >> term >> windowid;
	}

	tty.terminate();
    }

    // Sanity check
    if (ttyname.empty() || ttyname[0] != '/')
	pid = -1;

    // Waiting is over
    XtUnmanageChild(dialog);

    if (pid < 0)
    {
	if (!canceled)
	{
	    post_error("The execution window could not be started.", 
		       "tty_exec_error", origin);
	}

	delay.outcome = (canceled ? "canceled" : "failed");
    }

    // Set icon and group leader
    if (windowid)
    {
	wm_set_icon(XtDisplay(command_shell), windowid,
		    iconlogo(command_shell), iconmask(command_shell));
    }

    // Be sure to be notified when the TTY window is deleted
    if (windowid)
    {
	XSelectInput(XtDisplay(gdb_w), windowid, StructureNotifyMask);
    }
}

static void get_args(const string& command, string& base, string& args)
{
    // Find (last) arguments to `run' command
    base = command;
    args = "";

    if (command.contains(rxwhite))
    {
	base = command.before(rxwhite);
	args = command.after(rxwhite);
    }

    if (args.empty() && gdb->type() == GDB)
    {
	args = gdb_question("show args");

	// GDB 4.16 issues `Arguments list', GDB 4.17 `Argument list'.  Shhh.
	if (!args.contains("Argument", 0))
	{
	    args = "";
	}
	else
	{
	    // Strip one pair of enclosing `"' characters
	    if (args.contains('"'))
	    {
		args = args.after('"');
		if (args.contains('"'))
		    args = args.before('"', -1);
	    }
	}
    }

    strip_leading_space(args);
}


// Set TERM environment variable, using CMD
static int set_term_env(const string& cmd, const string& term_type,
			Widget origin, bool silent)
{
    string reply = gdb_question(cmd);
    if (reply == NO_GDB_ANSWER)
    {
	if (!silent)
	{
	    post_warning(string("Cannot set terminal type to ") 
			 + quote(term_type), "tty_type_error", origin);
	}
    }
    else if (!reply.empty())
    {
	if (!silent)
	    post_gdb_message(reply, true, origin);
	return -1;
    }

    return 0;
}


static bool has_redirection(const string& args, const string& redirection)
{
    return args.contains(redirection, 0) || args.contains(" " + redirection);
}

// Set debugger TTY to TTY_NAME and terminal type to TERM_TYPE.
// Use without arguments to silently restore original TTY.
static int gdb_set_tty(string tty_name = "",
		       const string& term_type = "dumb",
		       Widget origin = 0)
{
    bool silent = false;
    if (tty_name.empty())
    {
	tty_name = gdb->slave_tty();
	silent = true;
    }

    switch (gdb->type())
    {
    case GDB:
    {
	if (app_data.use_tty_command && tty_name != gdb_tty)
	{
	    // Issue `tty' command to perform redirection
	    const string tty_cmd = "tty " + tty_name;
	    const string reply = gdb_question(tty_cmd);

	    if (reply == NO_GDB_ANSWER)
	    {
		if (!silent)
		    post_error("GDB I/O error: cannot send tty command", 
			       "tty_command_error", origin);
		return -1;
	    }
	    else if (!reply.empty())
	    {
		if (!silent)
		    post_gdb_message(reply, true, origin);
		return -1;
	    }
	    else
		gdb_tty = tty_name;
	}

	// Set remote terminal type
	int ok = set_term_env("set environment TERM " + term_type, term_type,
			      origin, silent);
	if (!ok)
	    return ok;
    }
    break;

    case DBX:
    {
	if (app_data.use_tty_command && gdb->has_run_io_command())
	{
	    if (tty_name != gdb_tty)
	    {
		// Issue `dbxenv run_io pty' and `dbxenv run_pty' commands
		// to perform redirection
		string command = string("dbxenv run_io pty");
		string reply = gdb_question(command);

		if (reply == NO_GDB_ANSWER)
		{
		    if (!silent)
			post_error("DBX I/O error: cannot send "
				   "dbxenv run_io command",
				   "tty_command_error", origin);
		    return -1;
		}
		else if (!reply.empty())
		{
		    if (!silent)
			post_gdb_message(reply, true, origin);
		    return -1;
		}
		else
		{
		    command = "dbxenv run_pty " + tty_name;
		    reply = gdb_question(command);

		    if (reply == NO_GDB_ANSWER)
		    {
			if (!silent)
			    post_error("DBX I/O error: cannot send "
				       "dbxenv run_pty command",
				       "tty_command_error", origin);
			return -1;
		    }
		    else if (!reply.empty())
		    {
			if (!silent)
			    post_gdb_message(reply, true, origin);
			return -1;
		    }
		    else
			gdb_tty = tty_name;
		}
	    }
	}

	// Set remote terminal type
	if (gdb->has_setenv_command())
	{
	    int ok = set_term_env("setenv TERM " + term_type, term_type,
				  origin, silent);
	    if (!ok)
		return ok;
	}
    }
    break;

    case PERL:
    {
	// Set remote terminal type
	string cmd = "$ENV{'TERM'} = " + quote(term_type, '\'');
	int ok = set_term_env(cmd, term_type, origin, silent);
	if (!ok)
	    return ok;

#if 0
	// Benoit Rochefort <benoitr@adopt.qc.ca> reports that
	// Perl-5.005_02 on HP-UX exits after executing 
	// open(STDIN, "</dev/tty").  
	// So, redirect to the original TTY name instead.
	if (tty_name == gdb->slave_tty())
	    tty_name = "/dev/tty";
#endif

	cmd = "";
	if (!has_redirection(gdb->path(), "<"))
	    cmd += "open(STDIN, \"<" + tty_name + "\"); ";
	if (!has_redirection(gdb->path(), ">"))
	    cmd += "open(STDOUT, \">" + tty_name + "\"); ";
	if (!has_redirection(gdb->path(), "2>"))
	    cmd += "open(STDERR, \">" + tty_name + "\"); ";

	gdb_question(cmd);
    }
    break;

    case BASH:
    case MAKE:
    case DBG:
    case JDB:
    case PYDB:	// for now
    case XDB:
	// No way to set environment variables
	break;
    }

    return 0;
}

// Add redirection commands for SH-like shell
static void add_sh_redirection(string& gdb_redirection,
			       const string& tty_name,
			       const string& args)
{
    // sh, bsh, ksh, bash, zsh, sh5, ...
    bool stdout_redirected = has_redirection(args, ">");
    bool stderr_redirected = has_redirection(args, "2>");

    if (!stdout_redirected && !stderr_redirected)
    {
	gdb_redirection += " > " + tty_name;
	gdb_redirection += " 2>&1";
    }
    else if (stderr_redirected)
    {
	gdb_redirection += " > " + tty_name;
    }
    else if (stdout_redirected)
    {
	gdb_redirection += " 2> " + tty_name;
    }
}

// Add redirection commands for RC shell
static void add_rc_redirection(string& gdb_redirection,
			       const string& tty_name,
			       const string& args)
{
    // rc (from tim@pipex.net)
    bool stdout_redirected = has_redirection(args, ">");
    bool stderr_redirected = 
	has_redirection(args, "2>") || has_redirection(args, ">[2");

    if (!stdout_redirected && !stderr_redirected)
    {
	gdb_redirection += " > " + tty_name;
	gdb_redirection += " >[2=1]";
    }
    else if (stderr_redirected)
    {
	gdb_redirection += " > " + tty_name;
    }
    else if (stdout_redirected)
    {
	gdb_redirection += " 2> " + tty_name;
    }
}

// Redirect COMMAND to TTY_NAME
static void redirect_process(string& command,
			     const string& tty_name,
			     Widget origin)
{
    // Try `tty' command to perform redirection
    int ret = gdb_set_tty(tty_name, app_data.term_type, origin);
    if ((gdb->type() == PERL || app_data.use_tty_command) && ret == 0)
	return;

    // Use redirection directives
    string base;
    string args;
    get_args(command, base, args);

    if (!gdb_redirection.empty())
    {
	static string empty;
	args.gsub(gdb_redirection, empty);
	strip_space(args);
    }

    gdb_redirection = "";
    if (!has_redirection(args, "<"))
	gdb_redirection = "< " + tty_name;

    switch (gdb->type())
    {
    case GDB:
    {
	// In GDB, COMMAND is interpreted by the user's shell.
	static string shell;

	if (shell.empty())
	{
	    // The shell is determined only once, as it cannot change.
	    if (remote_gdb())
	    {
		string sh = gdb_question(gdb->shell_command("echo $SHELL"));
		if (sh != NO_GDB_ANSWER)
		    shell = sh.before('\n');
	    }
	    else
	    {
		const char *shell_s = getenv("SHELL");
		if (shell_s == 0)
		    shell_s = "/bin/sh";
		shell = shell_s;
	    }
	}

	if (shell.contains("csh"))
	{
	    // csh, tcsh
	    if (!has_redirection(args, ">"))
		gdb_redirection += " >&! " + tty_name;
	}
	else if (shell.contains("rc"))
	{
	    // rc (from tim@pipex.net)
	    add_rc_redirection(gdb_redirection, tty_name, args);
	}
	else if (shell.contains("sh"))
	{
	    // sh, bsh, ksh, bash, zsh, sh5, ...
	    add_sh_redirection(gdb_redirection, tty_name, args);
	}
	else
	{
	    // Unknown shell - play it safe and don't redirect stderr
	    if (!has_redirection(args, ">"))
		gdb_redirection += " > " + tty_name;
	}

	break;
    }

    case PERL:
    {
	// In Perl, COMMAND is always interpreted by /bin/sh.
	add_sh_redirection(gdb_redirection, tty_name, args);
	break;
    }

    case DBX:
	if (gdb->has_err_redirection())
	{
	    // DEC DBX and AIX DBX use csh-style redirection.
	    if (!has_redirection(args, ">"))
		gdb_redirection +=  " >& " + tty_name;
	}
	else
	{
	    // SUN DBX 3.x interprets `COMMAND 2>&1' such that COMMAND
	    // runs in the background.

	    // Tuomo Takkula <tuomo@cs.chalmers.se> reports that SUN DBX
	    // (4.0 and later) cannot even parse '2>' properly.

	    // Consequence: Play it safe.  Do not redirect stderr.
	    if (!has_redirection(args, ">"))
		gdb_redirection += " > " + tty_name;
	}
	break;

    case XDB:
	// XDB uses ksh-style redirection.
	add_sh_redirection(gdb_redirection, tty_name, args);
	break;

    case DBG:
	break;			// Don't know if it's there.

    case JDB:
	break;			// No redirection in JDB

    case BASH:
	break;			// No redirection in BASH (for now)

    case MAKE:
	break;			// No redirection in MAKE (for now)

    case PYDB:
	break;			// No redirection in PYDB (for now)
    }

    string new_args;
    if (!gdb_redirection.empty() && !has_redirection(args, gdb_redirection))
    {
	if (args.empty())
	    new_args = gdb_redirection;
	else
	    new_args = gdb_redirection + " " + args;
    }

    if (!gdb_redirection.empty())
	gdb_out_ignore = " " + gdb_redirection;

    if (gdb->type() == PERL)
    {
	command = gdb->run_command(new_args);
    }
    else
    {
	command = base + " " + new_args;
    }
}

static void unredirect_reply(const string& answer, void *)
{
    post_gdb_message(answer);
}

// Restore original redirection
static void unredirect_process(string& command,
			       Widget origin = 0)
{
    if (!gdb_redirection.empty())
    {
	// Disable output redirection upon next run
	string base;
	string args;
	get_args(command, base, args);
	if (has_redirection(args, gdb_redirection))
	{
	    if (gdb->type() == GDB)
	    {
		static string empty;
		args.gsub(gdb_redirection, empty);
		strip_space(args);
		gdb_command("set args " + args, origin, unredirect_reply);
	    }
	    else if (gdb->type() == PERL)
	    {
		// If we start Perl with I/O redirection, redirection
		// after exec remains active until explicitly
		// redirected to /dev/tty.
		string new_args;
		add_sh_redirection(new_args, "/dev/tty", args);
		if (!args.empty())
		    new_args += " " + args;
		command = gdb->run_command(new_args);
	    }
	}
    }

    // Restore original tty
    gdb_set_tty();

    gdb_redirection = "";
    gdb_out_ignore  = "";
}


static inline void addcap(string& s, const char *cap, char *& b)
{
    const char *str = tgetstr(cap, &b);
    if (str)
	s += str;
}

// Initialize execution TTY with an appropriate escape sequence
static void initialize_tty(const string& tty_name, const string& tty_term)
{
    StatusDelay delay("Initializing execution window");

#ifdef SIGTTOU
    // Christopher Vickery <vickery@babbage.cs.qc.edu> says: On SunOS
    // 5.5, if I run ddd in the background (using the tcsh shell), ddd
    // cannot complete initialization of the execution window because
    // it tries to write to stdout, which causes it to block until the
    // job is returned to the foreground.  Nothing actually appears in
    // the terminal window when the ddd job is put in the foreground,
    // but the execution window finishes initializing OK.

    // Solution: block SIGTTOU signals during this function.
    SignalBlocker sb(SIGTTOU);
#endif

    // Send initialization sequence
    char buffer[2048];
    string init;

    int success = tgetent(buffer, tty_term.chars());
    if (success > 0)
    {
	char caps[2048];
	char *b = caps;

	addcap(init, "rs", b);	// Reset from strange mode
	addcap(init, "is", b);	// Ordinary reset
	addcap(init, "cl", b);	// Clear screen
    }

    if (remote_gdb())
    {
	string command = sh_command("cat > " + tty_name);
	FILE *fp = popen(command.chars(), "w");
	if (fp != 0)
	{
	    fwrite(init.chars(), init.length(), sizeof(char), fp);
	    pclose(fp);
	}
    }
    else
    {
	std::ofstream tty(tty_name.chars());
	tty << init;
    }
}

// Set the title of TTY_WINDOW to MESSAGE
static void set_tty_title(string message, Window tty_window)
{
    string init = "";

    string title = DDD_NAME ": Execution Window";
    string icon  = title;

    message = message.after(": ");
    static string empty;
    if (!gdb_out_ignore.empty())
	message.gsub(gdb_out_ignore, empty);

    string program = message;
    if (program.contains(' '))
	program = program.before(' ');

    if (!program.empty())
    {
	string program_base = program;
	if (program_base.contains('/'))
	    program_base = program_base.after('/', -1);

	title = DDD_NAME ": " + message;
	icon  = DDD_NAME ": " + program_base;
    }

    if (tty_window)
	wm_set_name(XtDisplay(command_shell), tty_window,
		    title, icon);
}

// Handle rerun
static void handle_rerun(string& command)
{
    if (!gdb->has_rerun_command())
	return;
    if (!is_running_cmd(command))
	return;

    static string last_args = "";

    string base;
    string args;
    get_args(command, base, args);
    bool rerun = base.contains("re", 0);

    if (rerun && !gdb->rerun_clears_args() && args.empty())
    {
	// `rerun' without args - Use last arguments
	command = base + " " + last_args;
    }
    else if (!rerun && gdb->rerun_clears_args() && args.empty())
    {
	// `run' without args - Use last arguments
	command = base + " " + last_args;
    }
    else
    {
	// Set last arguments
	last_args = args;
    }
}

// Create TTY if required
void handle_running_commands(string& command, Widget origin)
{
    // Make sure we see control messages such as `Starting program'
    if (is_running_cmd(command))
	show_next_line_in_status = true;

    if (is_run_cmd(command))
	startup_exec_tty(command, origin);
}

// Raise execution TTY.
void startup_exec_tty()
{
    string dummy = "";
    startup_exec_tty(dummy);
}

// Tie GDB buffering to existence of execution window
static void set_buffer_gdb_output()
{
    if (gdb && app_data.buffer_gdb_output == Auto)
    {
        // Buffer GDB output iff we have a separate exec window
	gdb->buffer_gdb_output(separate_tty_pid > 0);
    }
}

// Reset TTY settings after a restart
void reset_exec_tty()
{
    if (separate_tty_pid > 0)
    {
	initialize_tty(separate_tty_name, separate_tty_term);
	gdb_set_tty(separate_tty_name, app_data.term_type);
    }
    else
    {
	gdb_set_tty();
    }

    set_buffer_gdb_output();
}

// Raise execution TTY with command COMMAND.
void startup_exec_tty(string& command, Widget origin)
{
    handle_rerun(command);

    bool started = false;

    if (app_data.separate_exec_window 
	&& separate_tty_pid >= 0
	&& can_do_gdb_command()
	&& gdb->has_redirection())
    {
	// Launch separate tty if not running
	launch_separate_tty(separate_tty_name, 
			    separate_tty_pid,
			    separate_tty_term,
			    separate_tty_window,
			    origin);

	if (separate_tty_pid >= 0)
	{
	    // Initialize tty
	    initialize_tty(separate_tty_name, separate_tty_term);

	    // Set title from `starting program...' message
	    show_starting_line_in_tty = true;

	    // Tell GDB to redirect process I/O to this tty
	    redirect_process(command, separate_tty_name, origin);

	    // Reflect setting in options
	    app_data.separate_exec_window = True;
	    update_options();

	    started = true;
	}
    }

    if (!started)
    {
	// Close running tty
	kill_exec_tty();

	// Tell GDB not to redirect its process I/O
	unredirect_process(command, origin);
    }

    if (!command.empty())
	gdb_run_command = command;

    set_buffer_gdb_output();
}

// Set TTY title to TEXT
void set_tty_from_gdb(const string& text)
{
    if (private_gdb_input)
	return;
    if (!show_starting_line_in_tty)
	return;
    if (!text.contains("Starting program") && !text.contains("Running:"))
	return;

    show_starting_line_in_tty = false;

    if (separate_tty_pid <= 0)
	return;

    set_tty_title(text, separate_tty_window);
}

// Some cleanup actions...
void kill_exec_tty(bool killed)
{
    if (separate_tty_pid > 0)
    {
	StatusDelay delay(killed ?
			  "Execution window has been closed" :
			  "Closing execution window");

	if (remote_gdb())
	{
	    std::ostringstream os;
	    os << "kill -" << SIGHUP << " " << separate_tty_pid 
	       << " >/dev/null </dev/null 2>&1 &";
	    Agent agent(sh_command(string(os)));
	    agent.start();
	    agent.wait();
	}
	else
	{
	    kill(separate_tty_pid, SIGHUP);
	}

	// Tell GDB not to redirect its process I/O
	gdb_set_tty();
    }

    separate_tty_pid    = 0;
    separate_tty_window = 0;

    set_buffer_gdb_output();
}

// Verify if execution tty is still running
void exec_tty_running()
{
    if (separate_tty_window)
    {
	XEvent event;

	if (XCheckTypedWindowEvent(XtDisplay(gdb_w), separate_tty_window,
				   DestroyNotify, &event))
	{
	    // TTY window has been killed - kill process as well
	    kill_exec_tty(true);

	    // Restore original TTY for the time being
	    unredirect_process(gdb_run_command);

	    // If the user closed the TTY window, s/he probably has a
	    // reason for doing so.  Reflect change in options.
	    app_data.separate_exec_window = False;
	    update_options();
	}
    }
}
