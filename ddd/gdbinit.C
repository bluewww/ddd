// $Id$ -*- C++ -*-
// GDB initialization

// Copyright (C) 1996-1999 Technische Universitaet Braunschweig, Germany.
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

static bool have_cmd(const string& cmd)
{
    return cmd_file(cmd).contains('/', 0);
}

// Return an appropriate debugger type from ARGC/ARGV.
// Set SURE if debugger type could be deduced from args.
DebuggerType guess_debugger_type(int argc, char *argv[], bool& sure)
{
    DebuggerType fallback = DebuggerType(-1);
    get_debugger_type(app_data.debugger, fallback);

    sure = true;

    static bool have_perl   = (fallback == PERL || have_cmd("perl"));
    static bool have_python = (fallback == PYDB || have_cmd("python"));

    // 1. Check for Perl and Python scripts as given.

    int i;
    for (i = 1; i < argc; i++)
    {
	string arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (have_perl && is_perl_file(arg))
	    return PERL;

	if (have_python && is_python_file(arg))
	    return PYDB;
    }


    // 2. Check for executable binary as given.

    static bool have_gdb = (fallback == GDB || have_cmd("gdb"));
    static bool have_dbx = 
	(fallback == DBX || have_cmd("dbx") || have_cmd("ladebug"));
    static bool have_xdb = (fallback == XDB || have_cmd("xdb"));

    for (i = 1; i < argc; i++)
    {
	string arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	if (is_exec_file(arg))
	{
	    if (fallback == GDB || fallback == DBX || fallback == XDB)
		return fallback;

	    if (have_gdb)
		return GDB;

	    if (have_dbx)
		return DBX;

	    if (have_xdb)
		return XDB;
	}
    }


    // 3. Check for Java class in current directory.

    static bool have_jdb = (fallback == JDB || have_cmd("jdb"));

    if (have_jdb)
    {
	for (i = 1; i < argc; i++)
	{
	    string arg = argv[i];

	    if (arg.contains('-', 0))
		continue;		// Option
	    if (arg.contains('/', 0))
		continue;		// File

	    arg.gsub('.', '/');

	    if (is_regular_file(arg + ".java"))
		return JDB;

	    if (is_regular_file(arg + ".class"))
		return JDB;
	}
    }


    // 4. Check for executable binary in PATH.

    for (i = 1; i < argc; i++)
    {
	string arg = argv[i];

	if (arg.contains('-', 0))
	    continue;		// Option

	char *path_s = getenv("PATH");
	if (path_s == 0)
	    path_s = ".";

	string path = path_s;
	while (path != "")
	{
	    string dir;
	    if (path.contains(':'))
		dir = path.before(':');
	    else
		dir = path;
	    path = path.after(':');

	    if (dir == "")
		dir = ".";
	    if (!dir.contains('/', -1))
		dir += '/';

	    if (is_exec_file(dir + arg))
	    {
		if (fallback == GDB || fallback == DBX || fallback == XDB)
		    return fallback;

		if (have_gdb)
		    return GDB;

		if (have_dbx)
		    return DBX;

		if (have_xdb)
		    return XDB;
	    }
	}
    }


    // 5. Check for Java class in CLASSPATH.

    if (have_jdb)
    {
	for (i = 1; i < argc; i++)
	{
	    string arg = argv[i];

	    if (arg.contains('-', 0))
		continue;		// Option
	    if (arg.contains('/', 0))
		continue;		// File

	    char *classpath_s = getenv("CLASSPATH");
	    if (classpath_s == 0)
		classpath_s = ".";

	    string classpath = classpath_s;
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

		arg.gsub('.', '/');

		if (is_regular_file(dir + arg + ".java"))
		    return JDB;

		if (is_regular_file(dir + arg + ".class"))
		    return JDB;
	    }
	}
    }


    // 6. Use fallback.

    sure = false;

    if (fallback != DebuggerType(-1))
	return fallback;


    // 7. All fails.  Use GDB.
    return GDB;
}

static struct table {
    DebuggerType type;
    char *cmd;
} debuggers[] =
{
    { GDB,  "gdb"  },
    { DBX,  "dbx"  },
    { DBX,  "ladebug" },
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
    int i;
    for (i = 0; i < int(XtNumber(debuggers)); i++)
	if (type == debuggers[i].type && have_cmd(debuggers[i].cmd))
	    return debuggers[i].cmd;

    for (i = 0; i < int(XtNumber(debuggers)); i++)
	if (type == debuggers[i].type)
	    return debuggers[i].cmd;

    // This can't happen.
    assert(0);
    return "false";
}
