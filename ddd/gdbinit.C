// $Id$ -*- C++ -*-
// GDB initialization

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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

    case XDB:
	// Enable line mode.
	gdb_call += " -L";
	break;
    }

    for (int i = 1; i < argc; i++) {
	string arg = argv[i];
	gdb_call += " " + sh_quote(arg);
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
    if (d->length >= 1)
    {
	char last_character = d->data[d->length - 1];
	switch (last_character)
	{
	case ' ':
	case '>':
	    // We got input ending in ' ' or `>' - probably a prompt.
	    // (Any other prompt choices out there?  -AZ)
	    at_shell_prompt = true;
	    break;
	}
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
