// $Id$ -*- C++ -*-
// Shell commands

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char shell_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "shell.h"

#include "AppData.h"
#include "hostname.h"

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

static string _sh_command(string command, bool force_local)
{
    if (force_local || !remote_gdb())
	return "/bin/sh -c " + sh_quote(command);

    string rsh = app_data.rsh_command;
    string login = app_data.debugger_host_login;
    if (login != "")
	rsh += " -l " + login;

    string display = getenv("DISPLAY");

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

    string settings = "DISPLAY='" + display + "'; export DISPLAY; ";
    command = settings + command;

    rsh += " " + gdb_host + " /bin/sh -c " + sh_quote(sh_quote(command));

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
