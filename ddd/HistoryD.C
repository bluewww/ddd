// $Id$ -*- C++ -*-
// History-based prompt dialog

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char HistoryDialog_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "HistoryD.h"

#include "GDBAgent.h"
#include "cook.h"
#include "ddd.h"
#include "disp-read.h"
#include "history.h"
#include "regexps.h"
#include "string-fun.h"


//-----------------------------------------------------------------------------
// Argument filters
//-----------------------------------------------------------------------------

static bool try_arg(const string& cmd, string prefix, string& arg)
{
    if (prefix == "")
	return false;		// No such command
    if (!prefix.contains(" ", -1))
	prefix += " ";

    if (cmd.contains(prefix, 0))
    {
	arg = cmd.after(prefix);
	strip_space(arg);

	// Strip format stuff
	while (arg.contains('/', 0))
	    arg = arg.after(' ');

	if (arg.contains("'", 0))
	{
	    // Quoted arg -- add as unquoted
	    arg = unquote(arg);
	    return true;
	}
	else if (is_file_pos(arg))
	{
	    // FILE:LINE arg -- ignore
	    return false;
	}
	else if (arg.matches(rxint))
	{
	    // LINE arg -- ignore
	    return false;
	}
	else
	{
	    // Other arg -- add unchanged
	    return true;
	}
    }

    // Bad prefix -- ignore
    return false;
}

string arg_history_filter(const string& cmd)
{
    string arg;

    // Check for common GDB commands
    if (try_arg(cmd, gdb->print_command("", false), arg) ||
	try_arg(cmd, gdb->display_command(""), arg) ||
	try_arg(cmd, gdb->whatis_command(""), arg) ||
	try_arg(cmd, "x", arg) ||
	try_arg(cmd, "info line", arg) ||
	try_arg(cmd, "list", arg))
    {
	return arg;
    }

    arg = break_history_filter(cmd);
    if (arg != "")
	return arg;

    arg = watch_history_filter(cmd);
    if (arg != "")
	return arg;

    arg = display_history_filter(cmd);
    if (arg != "" && !arg.contains("`", 0))
	return arg;

    return "";			// Not found
}

string break_history_filter(const string& cmd)
{
    string arg;

    if (try_arg(cmd, "break", arg) ||
	try_arg(cmd, "tbreak", arg) ||
	try_arg(cmd, "clear", arg) ||
	try_arg(cmd, "stop in", arg) ||
	try_arg(cmd, "stop at", arg) ||
	try_arg(cmd, "b", arg) ||
	try_arg(cmd, "ba", arg))
    {
	strip_space(arg);
	if (arg.contains(" if "))
	    arg = arg.before(" if ");
	if (arg.contains(" -if "))
	    arg = arg.before(" -if ");
	if (arg.contains(" {if "))
	    arg = arg.before(" {if ");
	return arg;
    }

    return "";
}

string watch_history_filter(const string& cmd)
{
    string arg;

    if (try_arg(cmd, gdb->watch_command("", WATCH_CHANGE), arg) ||
	try_arg(cmd, gdb->watch_command("", WATCH_READ), arg) ||
	try_arg(cmd, gdb->watch_command("", WATCH_WRITE), arg) ||
	try_arg(cmd, gdb->watch_command("", WATCH_ACCESS), arg))
    {
	strip_space(arg);
	return arg;
    }

    return "";
}

string display_history_filter(const string& cmd)
{
    if (cmd.contains("graph display ", 0))
    {
	string arg = cmd.after("graph display ");
	if (arg.contains("dependent on"))
	    arg = arg.before("dependent on");
	if (arg.contains("now or"))
	    arg = arg.before("now or");
	if (arg.contains("when in"))
	    arg = arg.before("when in");

	strip_space(arg);

	// Strip format stuff
	while (arg.contains('/', 0))
	    arg = arg.after(' ');

	strip_space(arg);
	return arg;
    }

    return "";
}

string set_history_filter(const string& cmd)
{
    string arg;

    if (cmd.contains(" = "))
	arg = cmd.after(" = ");
    else if (cmd.contains(" := "))
	arg = cmd.after(" := ");

    if (arg != "")
    {
	strip_space(arg);
	return arg;
    }

    return "";
}
