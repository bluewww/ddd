// $Id$ -*- C++ -*-
// Shell commands

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char shell_rcsid[] = 
    "$Id$";

#include "shell.h"

#include "ddd.h"
#include "AppData.h"
#include "hostname.h"
#include "windows.h"
#include "environ.h"

#include <stdlib.h>
#include <iostream>

//-----------------------------------------------------------------------------
// Remote commands
//-----------------------------------------------------------------------------

// Host management
string gdb_host;		  // non-empty if separate host

bool remote_gdb()
{
    return !gdb_host.empty();
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
	display = string(fullhostname(host.chars())) + display.from(":");
    }

    string settings = "";
    if (!display.empty())
    {
	settings += 
	    "DISPLAY=${DISPLAY-" + sh_quote(display) + "}; export DISPLAY; ";
    }
    settings += set_environment_command();

    if (force_local || !remote_gdb())
    {
	if (command.empty())
	    return "";
	if (force_display_settings)
	    command = settings + command;
	return "/bin/sh -c " + sh_quote(command);
    }

    string rsh = app_data.rsh_command;
    const string login = app_data.debugger_host_login;
    if (!login.empty())
	rsh += " -l " + login;

    rsh += " " + gdb_host;

    if (!command.empty())
	rsh += " /bin/sh -c " + sh_quote(sh_quote(settings + command));

    return rsh;
}

string sh_command(const string& command, bool force_local)
{
    string ret = _sh_command(command, force_local);
    dddlog << "+  " << ret << "\n";
    dddlog.flush();
    return ret;
}
