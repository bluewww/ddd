// $Id$ -*- C++ -*-
// DDD command queue

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

const char commandQueue_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Queue.h"
#endif

#include "commandQ.h"
#include "comm-manag.h"
#include "status.h"
#include "exectty.h"
#include "findParent.h"
#include "history.h"
#include "obscure.h"
#include "ddd.h"
#include "Queue.h"
#include "windows.h"
#include "GDBAgent.h"

#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>

// Origin of last command
static Widget gdb_last_origin;

//-----------------------------------------------------------------------------
// GDB command management
//-----------------------------------------------------------------------------

void _gdb_command(string command, Widget origin)
{
    set_status("");

    if (command.length() == 1 && iscntrl(command[0]))
	promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);

    handle_running_commands(command, origin);
    handle_obscure_commands(command, origin);

    gdb_keyboard_command = private_gdb_input;
    gdb_last_origin = (gdb_keyboard_command ? gdb_w : origin);
    user_cmdSUC(command, origin);
    messagePosition = XmTextGetLastPosition(gdb_w);
}


//-----------------------------------------------------------------------------
// Command queue
//-----------------------------------------------------------------------------

// Command queue
struct Command
{
    string command;		// Command text
    Widget origin;		// Origin

    Command(const string& cmd, Widget w = 0)
	: command(cmd), origin(w)
    {}
    Command(const Command& c)
	: command(c.command), origin(c.origin)
    {}
    Command& operator = (const Command& c)
    {
	if (this != &c)
	{
	    command = c.command;
	    origin = c.origin;
	}
	return *this;
    }
    bool operator == (const Command& c)
    {
	return this == &c || command == c.command && origin == c.origin;
    }
};

static Queue<Command> commandQueue;

void clearCommandQueue()
{
    while (!commandQueue.isEmpty())
	commandQueue -= commandQueue.first();
}

bool emptyCommandQueue()
{
    return commandQueue.isEmpty();
}

void gdb_command(const string& cmd, Widget origin)
{
    if (cmd.length() == 1 && iscntrl(cmd[0]) || cmd == "yes" ||	cmd == "no")
    {
	_gdb_command(cmd, origin);
	clearCommandQueue();
	return;
    }

    if (gdb->isReadyWithPrompt() && commandQueue.isEmpty())
    {
	add_to_history(cmd);
	_gdb_command(cmd, origin);
    }
    else
	commandQueue += Command(cmd, origin);
}

void processCommandQueue(XtPointer, XtIntervalId *)
{
    if (!gdb->isReadyWithPrompt())
    {
	// Try again later...
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			200, processCommandQueue, XtPointer(0));
	return;
    }

    if (!commandQueue.isEmpty())
    {
	Command& c = commandQueue.first();
	Command cmd(c);
	commandQueue -= c;

	add_to_history(cmd.command);
	_gdb_command(cmd.command, cmd.origin);
	gdb_keyboard_command = false;
    }
}

// Shell finder
Widget find_shell(Widget w)
{
    if (w == 0)
	w = gdb_last_origin;
    if (w == 0)
	return command_shell;

    Widget parent = findTopLevelShellParent(w);
    if (parent == 0)
	return command_shell;

    if (!XtIsRealized(parent))
	return command_shell;

    XWindowAttributes xwa;
    XGetWindowAttributes(XtDisplay(parent), XtWindow(parent), &xwa);
    if (xwa.map_state != IsViewable)
	return command_shell;

    return parent;
}
