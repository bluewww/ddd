// $Id$ -*- C++ -*-
// Shell commands

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

char shell_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "shell.h"

#include "AppData.h"
#include "hostname.h"
#include "windows.h"
#include "environ.h"

#include <stdlib.h>
#include <iostream.h>

//-----------------------------------------------------------------------------
// Remote commands
//-----------------------------------------------------------------------------

// Host management
string gdb_host;		  // non-empty if separate host

bool remote_gdb()
{
    return gdb_host != "";
}

string sh_quote(string s)
{
    s.gsub('\'', "'\\''");
    return string('\'') + s + '\'';
}

string _sh_command(string command, bool force_local, 
		   bool force_display_settings)
{
    // Fetch display settings
    string display;
    if (command_shell != 0)
	display = XDisplayString(XtDisplay(command_shell));
    else if (getenv("DISPLAY") != 0)
	display = getenv("DISPLAY");
    else
	display = "";

    // Make sure display contains host name
    if (display.contains("unix:", 0) || display.contains(":", 0))
    {
	display = string(hostname()) + display.from(":");
    }

    // Make sure display contains fully qualified host name
    if (display.contains(":") && !display.contains("::"))
    {
	string host = display.before(':');
	display = string(fullhostname(host)) + display.from(":");
    }

    string settings = "";
    if (display != "")
	settings += "DISPLAY=" + sh_quote(display) + "; export DISPLAY; ";
    settings += set_environment_command();

    if (force_local || !remote_gdb())
    {
	if (command == "")
	    return "";
	if (force_display_settings)
	    command = settings + command;
	return "/bin/sh -c " + sh_quote(command);
    }

    string rsh = app_data.rsh_command;
    string login = app_data.debugger_host_login;
    if (login != "")
	rsh += " -l " + login;

    rsh += " " + gdb_host;

    if (command != "")
	rsh += " /bin/sh -c " + sh_quote(sh_quote(settings + command));

    return rsh;
}

string sh_command(string command, bool force_local)
{
    string ret = _sh_command(command, force_local);
    if (app_data.trace_shell_commands)
	clog << "+ " << ret << "\n";
    clog.flush();
    return ret;
}
