// $Id$ -*- C++ -*-
// Execution TTY

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

char exectty_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "exectty.h"

#include "AppData.h"
#include "Delay.h"
#include "LiterateA.h"
#include "ddd.h"
#include "disp-read.h"
#include "logo.h"
#include "post.h"
#include "question.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "windows.h"
#include "wm.h"
#include "commandQ.h"
#include "cook.h"

#include <fstream.h>
#include <signal.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>

extern "C" {
// The GNU termcap declarations should also work for non-GNU termcap
// implementations.  We do not include the system declaration files here
// (i.e. <curses.h> or <term.h>), since these include lots of other
// macros and declarations cluttering our namespace.
#include <termcap/termcap.h>
}


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

// Create a separate tty window; return its name in TTYNAME, its
// process id in PID, its terminal type in TERM, and its window id in
// WINDOWID.
static void launch_separate_tty(string& ttyname, pid_t& pid, string& term,
				Window& windowid, Widget origin)
{
    // If we're already running, all is done.
    if (pid > 0 && !remote_gdb() && kill(pid, 0) == 0)
	return;

    StatusDelay delay("Starting execution tty");

    string old_ttyname = ttyname;

    string command = 
	
	// Set up a temporary file in TMP.
	"tmp=${TMPDIR=/tmp}/ddd$$; export tmp; "

	// Be sure to remove it when exiting...
	"trap \"rm -f $tmp\" 0; "

	// ... or being interrupted.
	"trap 'exit 1' 1 2 15; "

	// Now execute the xterm command
	+ string(app_data.term_command) +

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
	ostrstream os;
	os << "kill -2 " << pid << " 2>/dev/null"
	   << " || ( " << command << " )";
	command = string(os);
    }

    command = sh_command(command);

    {
	Agent tty(command);
	tty.start();

	FILE *fp = tty.inputfp();
	if (fp != 0)
	{
	    char reply[BUFSIZ];
	    fgets(reply, BUFSIZ, fp);

	    if (strlen(reply) > 2)
	    {
		istrstream is(reply);
		is >> ttyname >> pid >> term >> windowid;
	    }
	}
    }

    // Sanity check
    if (ttyname == "" || ttyname[0] != '/')
	pid = -1;

    if (pid < 0)
	post_error("Could not start execution tty", "tty_exec_error", origin);

    // Set icon and group leader
    if (windowid)
    {
	wm_set_icon(XtDisplay(command_shell), windowid,
		    iconlogo(command_shell), iconmask(command_shell));
	wm_set_group_leader(XtDisplay(command_shell), windowid,
			    XtWindow(command_shell));
    }

    // Be sure to be notified when the TTY window is deleted
    if (windowid)
    {
	XSelectInput(XtDisplay(gdb_w), windowid, StructureNotifyMask);
    }
}

static void get_args(string command, string& base, string& args)
{
    // Find (last) arguments to `run' command
    base = command;
    args = "";

    if (command.contains(rxwhite))
    {
	base = command.before(rxwhite);
	args = command.after(rxwhite);
    }

    if (args == "" && gdb->type() == GDB)
    {
	args = gdb_question("show args");
	if (!args.contains("Arguments", 0))
	    args = "";
	else
	{
	    if (args.contains('"'))
	    {
		args = args.after('"');
		args = args.before('"', -1);
	    }
	}
    }
}


// Set debugger TTY to TTY_NAME and terminal type to TERM_TYPE.
// Use without arguments to silently restore original TTY.
static int gdb_set_tty(string tty_name = "",
		       string term_type = "dumb",
		       Widget origin = 0)
{
    bool silent = false;
    if (tty_name == "")
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
		string tty_cmd = string("tty ") + tty_name;
		string reply = gdb_question(tty_cmd);

		if (reply == NO_GDB_ANSWER)
		{
		    if (!silent)
			post_error("GDB I/O error: cannot send tty command", 
				   "tty_command_error", origin);
		    return -1;
		}
		else if (reply != "")
		{
		    if (!silent)
			post_gdb_message(reply, origin);
		    return -1;
		}
		else
		    gdb_tty = tty_name;
	    }

	    // Set remote terminal type
	    string env_cmd = string("set environment TERM ") + term_type;
	    string reply = gdb_question(env_cmd);
	    if (reply == NO_GDB_ANSWER)
	    {
		if (!silent)
		    post_warning(string("Cannot set terminal type to ") 
				 + quote(term_type), "tty_type_error", origin);
	    }
	    else if (reply != "")
	    {
		if (!silent)
		    post_gdb_message(reply, origin);
		return -1;
	    }
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
		    else if (reply != "")
		    {
			if (!silent)
			    post_gdb_message(reply, origin);
			return -1;
		    }
		    else
		    {
			command = string("dbxenv run_pty ") + tty_name;
			reply = gdb_question(command);

			if (reply == NO_GDB_ANSWER)
			{
			    if (!silent)
				post_error("DBX I/O error: cannot send "
					   "dbxenv run_pty command",
					   "tty_command_error", origin);
			    return -1;
			}
			else if (reply != "")
			{
			    if (!silent)
				post_gdb_message(reply, origin);
			    return -1;
			}
			else
			    gdb_tty = tty_name;
		    }
		}
	    }

	    // Set remote terminal type
	    string env_cmd = string("setenv TERM ") + term_type;
	    string reply = gdb_question(env_cmd);
	    if (reply == NO_GDB_ANSWER)
	    {
		if (!silent)
		    post_warning(string("Cannot set terminal type to ") 
				 + quote(term_type), 
				 "tty_type_error", origin);
	    }
	    else if (reply != "")
	    {
		if (!silent)
		    post_gdb_message(reply, origin);
		return -1;
	    }
	}
	break;

    case XDB:
	// No way to set environment variables in XDB (FIXME)
	break;
    }

    return 0;
}

// Redirect COMMAND to TTY_NAME
static void redirect_process(string& command,
			     const string& tty_name,
			     Widget origin)
{
    // Try `tty' command to perform redirection
    int ret = gdb_set_tty(tty_name, app_data.term_type, origin);
    if (app_data.use_tty_command && ret == 0)
	return;

    // Use redirection directives
    string base;
    string args;
    get_args(command, base, args);

    static string empty;
    args.gsub(gdb_redirection, empty);

    gdb_redirection = "";
    if (!args.contains("<"))
	gdb_redirection = string("< ") + tty_name;

    if (!args.contains(">"))
    {
	switch (gdb->type())
	{
	case GDB:
	    {
		// In GDB, COMMAND is interpreted by the user's shell.
		static string shell;

		if (shell == "")
		{
		    // The shell is determined only once, as it cannot
		    // change.
		    if (remote_gdb())
		    {
			string sh = gdb_question("shell echo $SHELL");
			if (sh != NO_GDB_ANSWER)
			    shell = sh.before('\n');
		    }
		    else
		    {
			char *shell_s = getenv("SHELL");
			if (shell_s == 0)
			    shell_s = "/bin/sh";
			shell = shell_s;
		    }
		}

		if (shell.contains("csh"))
		{
		    // csh, tcsh
		    gdb_redirection += string(" >&! ") + tty_name;
		}
		else if (shell.contains("rc"))
		{
		    // rc (from tim@pipex.net)
		    gdb_redirection += string(" > ") + tty_name + " >[2=1]";
		}
		else if (shell.contains("sh"))
		{
		    // sh, bsh, ksh, bash, zsh, sh5, ...
		    gdb_redirection += string(" > ") + tty_name + " 2>&1";
		}
		else
		{
		    // Unknown shell - play it safe
		    gdb_redirection += string(" > ") + tty_name;
		}
	    }
	    break;

	case DBX:
	    {
		// DBX has its own parsing, in several variants.
		if (gdb->has_print_r_command())
		{
		    // SUN DBX 3.x uses ksh style redirection.

 		    // DBX interprets `COMMAND 2>&1' such that COMMAND
 		    // runs in the background.  Use this kludge instead.
 		    gdb_redirection = string("2> ") + tty_name + " " 
			+ gdb_redirection + " > " + tty_name;
		}
		else if (gdb->has_err_redirection())
		{
		    // DEC DBX and AIX DBX use csh style redirection.
		    gdb_redirection +=  string(" >& ") + tty_name;
		}
		else
		{
		    // SUN DBX 1.x does not allow to redirect stderr.
		    gdb_redirection += string(" > ") + tty_name;
		}
	    }
	    break;

	case XDB:
	    {
		// XDB uses ksh style redirection.
		gdb_redirection += string(" > ") + tty_name + " 2>&1";
	    }
	    break;
	}
    }

    string new_args;
    if (gdb_redirection != "" && !args.contains(gdb_redirection))
    {
	if (args == "")
	    new_args = gdb_redirection;
	else
	    new_args = gdb_redirection + " " + args;
    }

    gdb_out_ignore = gdb_redirection;
    command = base + " " + new_args;
}

// Restore original redirection
static void unredirect_process(string& command,
			       Widget origin = 0)
{
    if (gdb_redirection != "")
    {
	// Disable output redirection upon next run
	string base;
	string args;
	get_args(command, base, args);
	if (args.contains(gdb_redirection) && gdb->type() == GDB)
	{
	    static string empty;
	    args.gsub(gdb_redirection, empty);
	    strip_final_blanks(args);
	    string reply = gdb_question(string("set args ") + args);
	    if (reply != "")
		post_gdb_message(reply, origin);
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
    StatusDelay delay("Initializing execution tty");

    char buffer[2048];
    string init;

    int success = tgetent(buffer, tty_term);
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
	string command = sh_command(string("cat > ") + tty_name);
	FILE *fp = popen(command, "w");
	fwrite((char *)init, init.length(), sizeof(char), fp);
	pclose(fp);
    }
    else
    {
	ofstream tty(tty_name);
	tty << init;
    }
}

// Set the title of TTY_WINDOW to MESSAGE
static void set_tty_title(string message, Window tty_window)
{
    string init = "";

    string title = string(DDD_NAME) + ": Execution Window";
    string icon  = title;

    message = message.after(": ");
    static string empty;
    if (gdb_out_ignore != "")
	message.gsub(gdb_out_ignore, empty);

    string program = message;
    if (program.contains(' '))
	program = program.before(' ');

    if (program != "")
    {
	string program_base = program;
	if (program_base.contains('/'))
	    program_base = program_base.after('/', -1);

	title = string(DDD_NAME) + ": " + message;
	icon  = string(DDD_NAME) + ": " + program_base;
    }

    if (tty_window)
	wm_set_name(XtDisplay(command_shell), tty_window,
		    title, icon);
}

// Create TTY if required
void handle_running_commands(string& command, Widget origin)
{
    // Make sure we see control messages such as `Starting program'
    if (is_running_cmd(command, gdb))
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

// Raise execution TTY with command COMMAND.
void startup_exec_tty(string& command, Widget origin)
{
    if (app_data.separate_exec_window 
	&& separate_tty_pid >= 0
	&& gdb->isReadyWithPrompt())
    {
	// Launch separate tty if not running
	launch_separate_tty(separate_tty_name, 
			    separate_tty_pid,
			    separate_tty_term,
			    separate_tty_window,
			    origin);

	if (separate_tty_pid < 0)
	    return;

	// Initialize tty
	initialize_tty(separate_tty_name, separate_tty_term);

	// Set title from `starting program...' message
	show_starting_line_in_tty = true;

	// Tell GDB to redirect process I/O to this tty
	redirect_process(command, separate_tty_name, origin);
    }
    else
    {
	// Close running tty
	kill_exec_tty();

	// Tell GDB not to redirect its process I/O
	unredirect_process(command, origin);
    }

    if (command != "")
	gdb_run_command = command;
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
void kill_exec_tty()
{
    if (separate_tty_pid > 0)
    {
	StatusDelay delay("Closing execution tty");

	if (remote_gdb())
	{
	    ostrstream os;
	    os << "kill -" << SIGHUP << " " << separate_tty_pid 
	       << " >/dev/null </dev/null 2>&1 &";
	    Agent agent(sh_command(string(os)));
	    agent.start();
	    agent.wait();
	}
	else
	    kill(separate_tty_pid, SIGHUP);
    }

    separate_tty_pid    = 0;
    separate_tty_window = 0;
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
	    kill_exec_tty();

	    // Restore original TTY for the time being
	    unredirect_process(gdb_run_command);
	}
    }
}
