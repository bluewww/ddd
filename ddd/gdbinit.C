// $Id$ -*- C++ -*-
// GDB initialization

// Copyright (C) 1996-1999 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2003, 2005, 2009 Free Software Foundation, Inc.
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

char gdbinit_rcsid[] = 
    "$Id$";

#include "gdbinit.h"

#include <X11/Intrinsic.h>
#include <iostream>
#include <fstream>
#include <ctype.h>

#include "assert.h"
#include "filetype.h"
#include "shell.h"
#include "string-fun.h"
#include "ddd.h"
#include "SourceView.h"
#include "TimeOut.h"

static void InvokeGDBFromShellHP(Agent *source, void *client_data, 
				 void *call_data);

//-----------------------------------------------------------------------------
// Create new GDB interface
//-----------------------------------------------------------------------------

GDBAgent *new_gdb(DebuggerType type,
		  const AppData& app_data,
		  XtAppContext app_context,
		  int argc, char *argv[])
{
    // Build call
    static string gdb_call = app_data.debugger_command;

    if (app_data.play_log != 0)
    {
	gdb_call += string(" --PLAY ") + app_data.play_log;
    }
    else
    {
	switch(type)
	{

	case BASH:
	    // Be sure to invoke the debugger.
	    gdb_call += " --debugger";
	    break;

	case DBG:
	    // Nothing special?
	    break;

	case DBX:
	    // Nothing special.  (Anyway, every DBX has its own sets of
	    // options, so there is not much we could do here.)
	    break;

	case GDB:
	    // Do not issue introductiory messages; output full file names.
	    gdb_call += " -q -fullname";
	    break;

	case JDB:
	    // Nothing special.
	    break;

	case MAKE:
	    // For now we have debuggable make installed as "remake"
	    // Be sure to invoke the debugger.
	    // gdb_call += " --debugger";
	    gdb_call = "remake --debugger -f";
	    break;

	case PERL:
	    // Be sure to invoke the debugger.
	    gdb_call += " -d";
	    break;

	case PYDB:
	    // Nothing special.
	    break;

	case XDB:
	    // Enable line mode.
	    gdb_call += " -L";
	    break;
	}
    }

    for (int i = 1; i < argc; i++) 
    {
	string arg = argv[i];
	gdb_call += " " + sh_quote(arg);
    }

    if (argc <= 1) 
    {
	if (type == PERL)
	{
	    // Invoked without args.  Add a dummy `eval' arg.
	    gdb_call += " -e 42";
	}
	else if (type == BASH)
	{
	    gdb_call += " -c ': type \\\"debug *script-name*\\\" to start your script.'";
	}
    }
    

    GDBAgent *gdb;
    if (app_data.debugger_rhost == 0 || app_data.debugger_rhost[0] == '\0')
    {
	// Use direct invocation
	gdb_call = sh_command("exec " + gdb_call);
	gdb = new GDBAgent(app_context, gdb_call, type);
    }
    else
    {
	// Use interactive rsh
	gdb = new GDBAgent(app_context, sh_command(), type);
	gdb_call = 
	    "exec " + _sh_command("exec " + gdb_call, true, true) + "\n";
	gdb->addHandler(Input, InvokeGDBFromShellHP, (void *)&gdb_call);
    }

    // Set up Agent resources
    switch (app_data.block_tty_input)
    {
    case On:
	gdb->block_tty_input(true);
	break;

    case Off:
	gdb->block_tty_input(false);
	break;

    case Auto:
	// Leave default setting unchanged
	break;
    }

    // Set up Agent resources
    switch (app_data.buffer_gdb_output)
    {
    case On:
	gdb->buffer_gdb_output(true);
	break;

    case Off:
	gdb->buffer_gdb_output(false);
	break;

    case Auto:
	// Tie buffering to existence of separate window
	gdb->buffer_gdb_output(app_data.separate_exec_window);
	break;
    }

    return gdb;
}

// Show call in output window
static void EchoTextCB(XtPointer client_data, XtIntervalId *)
{
    const string& gdb_call = *((const string *)client_data);
    _gdb_out(gdb_call);
}

// Invoke GDB upon rlogin
static void InvokeGDBFromShellHP(Agent *source, void *client_data, 
				 void *call_data)
{
    DataLength *d = (DataLength *)call_data;

    bool at_shell_prompt = false;
    if (d->length >= 1 && d->data[d->length - 1] == '>')
    {
	// We got input ending in `>' - probably a prompt.
	at_shell_prompt = true;
    }
    else if (d->length >= 2
	     && d->data[d->length - 1] == ' '
	     && ispunct(d->data[d->length - 2]))
    {
	// We got input ending in a punctuation character, followed by
	// a single space - probably a prompt.
	at_shell_prompt = true;
    }
    else
    {
	// (Any other prompt choices out there?  -AZ)
    }

    if (at_shell_prompt)
    {
	GDBAgent *gdb = ptr_cast(GDBAgent, source);
	static bool init_sent = false;

	if (!init_sent)
	{
	    // Send the initialization string.
	    if (app_data.rhost_init_commands != 0 
		&& app_data.rhost_init_commands[0] != '\0')
	    {
		string init = string(app_data.rhost_init_commands) + "\n";
		gdb->write(init.chars(), init.length());
	    }

	    init_sent = true;
	}
	else
	{
	    // Invoke GDB...
	    const string& gdb_call = *((const string *)client_data);
	    gdb->write(gdb_call.chars(), gdb_call.length());

	    // Echoing should be disabled by now.  Echo call manually...
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			    0, EchoTextCB, (XtPointer)client_data);

	    // ... and don't get called again.
	    gdb->removeHandler(Input, InvokeGDBFromShellHP, client_data);
	}
    }
}



//-----------------------------------------------------------------------------
// Guess inferior debugger type
//-----------------------------------------------------------------------------

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}


// Return an appropriate debugger type from ARGC/ARGV.
// Set ARG if debugger type could be deduced from an argument.
DebuggerInfo::DebuggerInfo(int argc, const char * const argv[])
    : type(DebuggerType(-1)),
      arg("")
{
    DebuggerType fallback = DebuggerType(-1);
    get_debugger_type(app_data.debugger, fallback);

    static bool have_bash   = (fallback == BASH || have_cmd("bash"));
    static bool have_dbg    = (fallback == DBG  || have_cmd("dbg"));
    static bool have_perl   = (fallback == PERL || have_cmd("perl"));
    static bool have_python = (fallback == PYDB || have_cmd("python"));

    // 1. Check for Perl and Python scripts as given.

    int i;
    for (i = 1; i < argc; i++)
    {
	arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (have_perl && is_perl_file(arg))
	{
	    type = PERL;
	    return;
	}

	if (have_bash && is_bash_file(arg))
	{
	    type = BASH;
	    return;
	}

	if (have_python && is_python_file(arg))
	{
	    type = PYDB;
	    return;
	}
	
	if (have_dbg && is_php_file(arg))
	{
	    type = DBG;
	    return;
	}
    }
    // 2. Check for Makefile as given

    for (i = 1; i < argc; i++)
    {
	arg = argv[i];
	
	if (is_make_file(arg))
        {
	    type = MAKE;
	    return;
	}
    }
    // 3. Check for executable binary as given.

    static bool have_gdb = (fallback == GDB || have_cmd("gdb"));
    static bool have_dbx = 
	(fallback == DBX || have_cmd("dbx") || have_cmd("ladebug"));
    static bool have_xdb = (fallback == XDB || have_cmd("xdb"));

    for (i = 1; i < argc; i++)
    {
	arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (is_debuggee_file(arg))
	{
	    if (fallback == GDB || fallback == DBX || fallback == XDB)
	    {
		type = fallback;
		return;
	    }

	    if (have_gdb)
	    {
		type = GDB;
		return;
	    }

	    if (have_dbx)
	    {
		type = DBX;
		return;
	    }

	    if (have_xdb)
	    {
		type = XDB;
		return;
	    }
	}
    }


    // 4. Check for Java class in current directory.

    static bool have_jdb = (fallback == JDB || have_cmd("jdb"));

    if (have_jdb)
    {
	for (i = 1; i < argc; i++)
	{
	    arg = argv[i];

	    if (arg.contains('-', 0))
		continue;		// Option
	    if (arg.contains('/', 0))
		continue;		// File

	    arg.gsub('.', '/');

	    if (is_regular_file(arg + ".java"))
	    {
		type = JDB;
		return;
	    }

	    if (is_regular_file(arg + ".class"))
	    {
		type = JDB;
		return;
	    }
	}
    }


    // 5. Check for executable binary in PATH.

    for (i = 1; i < argc; i++)
    {
	arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	const char *path_s = getenv("PATH");
	if (path_s == 0)
	    path_s = ".";

	string path = path_s;
	while (!path.empty())
	{
	    string dir;
	    if (path.contains(':'))
		dir = path.before(':');
	    else
		dir = path;
	    path = path.after(':');

	    if (dir.empty())
		dir = ".";
	    if (!dir.contains('/', -1))
		dir += '/';

	    if (is_debuggee_file(dir + arg))
	    {
		if (fallback == GDB || fallback == DBX || fallback == XDB)
		{
		    type = fallback;
		    return;
		}

		if (have_gdb)
		{
		    type = GDB;
		    return;
		}

		if (have_dbx)
		{
		    type = DBX;
		    return;
		}

		if (have_xdb)
		{
		    type = XDB;
		    return;
		}
	    }
	}
    }


    // 6. Check for Java class in CLASSPATH.

    if (have_jdb)
    {
	for (i = 1; i < argc; i++)
	{
	    arg = argv[i];

	    if (arg.contains('-', 0))
		continue;		// Option
	    if (arg.contains('/', 0))
		continue;		// File

	    const char *classpath_s = getenv("CLASSPATH");
	    if (classpath_s == 0)
		classpath_s = ".";

	    string classpath = classpath_s;
	    while (!classpath.empty())
	    {
		string dir;
		if (classpath.contains(':'))
		    dir = classpath.before(':');
		else
		    dir = classpath;
		classpath = classpath.after(':');

		if (dir.empty())
		    dir = ".";
		if (!dir.contains('/', -1))
		    dir += '/';

		string path = arg;
		path.gsub('.', '/');

		if (is_regular_file(dir + path + ".java"))
		{
		    type = JDB;
		    return;
		}

		if (is_regular_file(dir + path + ".class"))
		{
		    type = JDB;
		    return;
		}
	    }
	}
    }


    // 7. Use fallback.

    arg = "";

    if (fallback != DebuggerType(-1))
    {
	type = fallback;
	return;
    }


    // 8. All fails.  Use GDB.
    type = GDB;
}

static struct table {
    DebuggerType type;
    const char *cmd;
} debuggers[] =
{
    { BASH, "bash" },
    { DBG,  "dbg" },
    { DBX,  "dbx"  },
    { DBX,  "ladebug" },
    { GDB,  "gdb"  },
    { GDB,  "wdb"  },
    { JDB,  "jdb"  },
    { MAKE, "make" },
    { PERL, "perl" },
    { PYDB, "pydb" },
    { XDB,  "xdb"  }
};

// Determine debugger type from DEBUGGER_NAME
bool get_debugger_type(const string& debugger_name, DebuggerType& type)
{
    for (int i = 0; i < int(XtNumber(debuggers)); i++)
	if (debugger_name.contains(debuggers[i].cmd))
	{
	    // Found
	    type = debuggers[i].type;
	    return true;
	}

    // Failed
    return false;
}

const char *default_debugger(const string& debugger_name, DebuggerType type)
{
    if (type == DBX && debugger_name == "ladebug")
    {
	// This happens when using the `--ladebug' option.
	return "ladebug";
    }

    if (type == GDB && debugger_name == "wdb")
    {
	// This happens when using the `--wdb' option.
	return "wdb";
    }

    int i;
    for (i = 0; i < int(XtNumber(debuggers)); i++)
	if (type == debuggers[i].type && have_cmd(debuggers[i].cmd))
	    return debuggers[i].cmd;

    for (i = 0; i < int(XtNumber(debuggers)); i++)
	if (type == debuggers[i].type)
	    return debuggers[i].cmd;

    // This can't happen.
    assert(0);
    ::abort();
    return "false";
}
