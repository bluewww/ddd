// $Id$ -*- C++ -*-
// GDB initialization

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char gdbinit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "gdbinit.h"

#include <X11/Intrinsic.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>

#include "assert.h"
#include "filetype.h"
#include "shell.h"
#include "string-fun.h"
#include "ddd.h"
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
	case GDB:
	    // Do not issue introductiory messages; output full file names.
	    gdb_call += " -q -fullname";
	    break;

	case DBX:
	    // Nothing special.  (Anyway, every DBX has its own sets of
	    // options, so there is not much we could do here.)
	    break;

	case JDB:
	    // Nothing special.
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

    if (type == PERL && argc <= 1)
    {
	// Invoked without args.  Add a dummy `eval' arg.
	gdb_call += " -e 42";
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

    return gdb;
}

// Show call in output window
static void EchoTextCB(XtPointer client_data, XtIntervalId *)
{
    const string& gdb_call = *((string *)client_data);
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
		gdb->write(init, init.length());
	    }

	    init_sent = true;
	}
	else
	{
	    // Invoke GDB...
	    const string& gdb_call = *((string *)client_data);
	    gdb->write(gdb_call, gdb_call.length());

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

static string first_line(const string& file)
{
    ifstream is(file);
    if (is.bad())
	return "";

    char line[BUFSIZ];
    line[0] = '\0';

    is.getline(line, sizeof(line));

    return line;
}

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}

// Return an appropriate debugger type from ARGC/ARGV.
// Set SURE if debugger type could be deduced from args.
DebuggerType guess_debugger_type(int argc, char *argv[], bool& sure)
{
    sure = true;

    static bool have_perl   = have_cmd("perl");
    static bool have_python = have_cmd("python");

    // Check for Perl and Python scripts
    int i;
    for (i = 1; i < argc; i++)
    {
	string arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (!is_regular_file(arg) || is_binary_file(arg))
	    continue;		// Not a file or binary file

	string header = first_line(arg);

	if (have_perl && (arg.contains(".pl", -1) || 
			  header.contains("perl")))
	{
	    return PERL;
	}

	if (have_python && (arg.contains(".py", -1) || 
			    header.contains("python")))
	{
	    return PYDB;
	}
    }

    // Check for executables.
    static bool have_gdb = have_cmd("gdb");
    static bool have_dbx = have_cmd("dbx");
    static bool have_xdb = have_cmd("xdb");

    for (i = 1; i < argc; i++)
    {
	string arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (is_exec_file(arg))
	{
	    if (have_gdb)
		return GDB;

	    if (have_dbx)
		return DBX;

	    if (have_xdb)
		return XDB;
	}
    }

    // Search class path for Java classes.
    static bool have_jdb = have_cmd("jdb");
    if (have_jdb)
    {
	for (i = 1; i < argc; i++)
	{
	    string arg = argv[i];

	    if (arg.contains('-', 0))
		continue;		// Option
	    if (arg.contains('/', 0))
		continue;		// File

	    string classpath = 
		getenv("CLASSPATH") != 0 ? getenv("CLASSPATH") : ".";
	    while (classpath != "")
	    {
		string dir;
		if (classpath.contains(':'))
		    dir = classpath.before(':');
		else
		    dir = classpath;
		classpath = classpath.after(':');

		if (dir == "")
		    dir = ".";
		if (!dir.contains('/', -1))
		    dir += '/';

		if (is_regular_file(dir + arg + ".java"))
		    return JDB;

		if (is_regular_file(dir + arg + ".class"))
		    return JDB;
	    }
	}
    }

    sure = false;

    // Return a default inferior debugger
    if (have_gdb)
	return GDB;

    if (have_dbx)
	return DBX;

    if (have_xdb)
	return XDB;

    // Nothing found -- try GDB.
    return GDB;
}

static struct table {
    DebuggerType type;
    char *cmd;
} debuggers[] =
{
    { GDB,  "gdb"  },
    { DBX,  "dbx"  },
    { XDB,  "xdb"  },
    { JDB,  "jdb"  },
    { PYDB, "pydb" },
    { PERL, "perl" }
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

char *default_debugger(DebuggerType type)
{
    for (int i = 0; i < int(XtNumber(debuggers)); i++)
	if (type == debuggers[i].type)
	    return debuggers[i].cmd;

    // This can't happen.
    assert(0);
    return "false";
}
