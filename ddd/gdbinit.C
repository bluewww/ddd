// $Id$ -*- C++ -*-
// GDB initialization

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

char gdbinit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <X11/Intrinsic.h>
#include <iostream.h>
#include <fstream.h>

#include "gdbinit.h"
#include "shell.h"
#include "string-fun.h"
#include "ddd.h"

// GDB initialization file (may be remote)
static string gdb_init_file;

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
    char *initial_cmds = 0;
    char *settings = 0;

    switch (type)
    {
    case GDB:
	initial_cmds = app_data.gdb_initial_cmds;
	settings     = app_data.gdb_settings;
	break;

    case DBX:
	initial_cmds = app_data.dbx_initial_cmds;
	settings     = app_data.dbx_settings;
	break;

    case XDB:
	initial_cmds = app_data.xdb_initial_cmds;
	settings     = app_data.xdb_settings;
	break;
    }

    if ((initial_cmds == 0 || initial_cmds[0] == '\0')
	&& (settings == 0 || settings[0] == '\0'))
    {
	// No initial commands
	gdb_init_file = "";
    }
    else
    {
	// Set initial commands
	if (remote_gdb())
	{
	    gdb_init_file = string("${TMPDIR=/tmp}/ddd") + itostring(getpid());
	    string cmd = sh_command(string("cat > ") + gdb_init_file);
	    FILE *fp = popen(cmd, "w");
	    if (fp == 0)
	    {
		perror(gdb_init_file);
	    }
	    else
	    {
		if (settings)
		    fputs(settings, fp);
		if (initial_cmds)
		    fputs(initial_cmds, fp);
		pclose(fp);
	    }
	}
	else
	{
	    gdb_init_file = tmpnam(0);
	    ofstream os(gdb_init_file);
	    if (settings)
		os << settings << "\n";
	    if (initial_cmds)
		os << initial_cmds << "\n";
	}
    }

    // Build call
    static string gdb_call = app_data.debugger_command;

    switch(type)
    {
    case GDB:
	gdb_call += " -q -fullname";
	if (gdb_init_file != "")
	    gdb_call += string(" -x ") + gdb_init_file;
	break;

    case DBX:
	if (gdb_init_file != "")
	{
	    // When we specify a DBX init file, all default init
	    // files are overridden.  Specify them explicitly.
	    gdb_call += " -s .dbxrc -s $HOME/.dbxrc";
	    gdb_call += " -s .dbxinit -s $HOME/.dbxinit";
	    gdb_call += string(" -s ") + gdb_init_file;
	}
	break;

    case XDB:
	gdb_call += " -L ";
	if (gdb_init_file != "")
	    gdb_call += string(" -p ") + gdb_init_file;
	break;
    }

    for (int i = 1; i < argc; i++) {
	string arg = argv[i];
	gdb_call += string(" ") + sh_quote(arg);
    }

    GDBAgent *gdb;
    if (app_data.debugger_rhost == 0 || app_data.debugger_rhost[0] == '\0')
    {
	// Use direct invocation
	gdb_call = sh_command(string("exec ") + gdb_call);
	gdb = new GDBAgent(app_context, gdb_call, type);
    }
    else
    {
	// Use interactive rsh
	gdb = new GDBAgent(app_context, sh_command(), type);
	gdb_call = "exec " 
	    + _sh_command(string("exec ") + gdb_call, true, true) + "\n";
	gdb->addHandler(Input, InvokeGDBFromShellHP, (void *)&gdb_call);
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
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    DataLength *d = (DataLength *)call_data;

    if (d->length > 1 && d->data[d->length - 1] == ' ')
    {
	// We got input ending in ' '; probably a prompt.
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


// Cleanup
void remove_init_file()
{
    if (gdb_init_file != "")
    {
	if (remote_gdb())
	{
	    string cmd = sh_command(
		string("rm -f ") + gdb_init_file + " >/dev/null </dev/null 2>&1 &");
	    system(cmd);
	}
	else
	{
	    unlink(gdb_init_file.chars());
	}
	gdb_init_file = "";
    }
}
