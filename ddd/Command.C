// $Id$ -*- C++ -*-
// DDD interface to GDB commands

// Copyright (C) 1996-1997 Technische Universitaet Braunschweig, Germany.
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

char Command_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Queue.h"
#endif

#include "Command.h"
#include "comm-manag.h"
#include "status.h"
#include "exectty.h"
#include "cmdtty.h"
#include "findParent.h"
#include "history.h"
#include "ddd.h"
#include "cook.h"
#include "Queue.h"
#include "windows.h"
#include "GDBAgent.h"
#include "TimeOut.h"

#include <ctype.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <X11/StringDefs.h>

#define LOG_QUEUE 0

// Origin of last command
static Widget gdb_last_origin;

//-----------------------------------------------------------------------------
// GDB command management
//-----------------------------------------------------------------------------

static void ClearOriginCB(Widget w, XtPointer, XtPointer)
{
    if (gdb_last_origin == w)
	gdb_last_origin = 0;
}

void _gdb_command(const Command& c)
{
    string cmd = c.command;
    if (gdb->isReadyWithPrompt())
    {
	if (c.verbose)
	    set_status("");

	if (cmd.length() == 1 && iscntrl(cmd[0]))
	{
	    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
	}
	else if (c.command.length() > 0 && c.verbose 
		 /* && c.priority == COMMAND_PRIORITY_USER */)
	{
	    add_to_history(cmd);
	}

	handle_running_commands(cmd, c.origin);

	if (cmd.length() == 0)
	{
	    _gdb_out(gdb->prompt());
	    return;
	}
    }

    gdb_keyboard_command = private_gdb_input;

    if (gdb_last_origin != 0)
    {
	XtRemoveCallback(gdb_last_origin, XtNdestroyCallback, 
			 ClearOriginCB, 0);
    }

    gdb_last_origin = find_shell(gdb_keyboard_command ? gdb_w : c.origin);

    if (gdb_last_origin != 0)
    {
	XtAddCallback(gdb_last_origin, XtNdestroyCallback, 
		      ClearOriginCB, 0);
    }

    send_gdb_command(cmd, c.origin, c.callback, c.data, c.verbose, c.check);
    messagePosition = XmTextGetLastPosition(gdb_w);
}


//-----------------------------------------------------------------------------
// Command queue
//-----------------------------------------------------------------------------

void Command::add_destroy_callback()
{
    if (origin != 0)
	XtAddCallback(origin, XtNdestroyCallback, clear_origin, 
		      (XtPointer)this);
}

void Command::remove_destroy_callback()
{
    if (origin != 0)
	XtRemoveCallback(origin, XtNdestroyCallback, clear_origin,
			 (XtPointer)this);
}

void Command::clear_origin(Widget w, XtPointer client_data, XtPointer)
{
    (void) w;			// Use it

    // The widget is being destroyed.  Remove all references.
    Command *command = (Command *)client_data;
    assert(w == command->origin);
    command->origin = 0;
}

typedef Queue<Command> CommandQueue;
typedef QueueIter<Command> CommandQueueIter;

static CommandQueue commandQueue;

#if LOG_QUEUE
static ostream& operator<<(ostream& os, const CommandQueue& queue)
{
    os << "[";
    bool first = true;
    for (CommandQueueIter i = queue; i.ok(); i = i.next())
    {
	if (first)
	    first = false;
	else
	    os << ", ";

	os << quote(i().command) << "<" << i().priority << ">";
    }
    return os << "]";
}
#endif

void clearCommandQueue()
{
    CommandQueue oldCommandQueue(commandQueue);
    static CommandQueue emptyQueue;
    commandQueue = emptyQueue;

    while (!oldCommandQueue.isEmpty())
    {
	const Command& cmd = oldCommandQueue.first();
	if (cmd.callback != 0)
	{
	    // We're deleting a command with associated callback.
	    // Call callback with NO_GDB_ANSWER such that it can clean
	    // up the associated data.  Commands added by the callback
	    // will be added to the command queue.
	    cmd.callback(NO_GDB_ANSWER, cmd.data);
	}
	oldCommandQueue -= cmd;
    }

#if LOG_QUEUE
    clog << "Command queue: " << commandQueue << "\n";
#endif
}

bool emptyCommandQueue()
{
    return commandQueue.isEmpty();
}

void gdb_command(const Command& c)
{
    if (c.command.length() == 1 && iscntrl(c.command[0]) 
	|| c.command == "no" || c.command == "yes")
    {
	// User interaction -- execute immediately
	_gdb_command(c);

	if (c.command != "yes")
	{
	    // Probably some canceling command - clear remaining commands
	    clearCommandQueue();
	}
	return;
    }

    if (gdb->isReadyWithPrompt() && emptyCommandQueue())
    {
	_gdb_command(c);
    }
    else
    {
	// Enqueue before first command with lower priority.  This
	// ensures that user commands are placed at the end.
	CommandQueueIter i(commandQueue);
	CommandQueueIter pos(commandQueue);

	while (i.ok() && c.priority <= i().priority)
	{
	    pos = i; i = i.next();
	}

	if (!i.ok())
	{
	    assert(!pos.ok() || pos().priority >= c.priority);

	    // End of queue reached
	    commandQueue.enqueue_at_end(c);
	}
	else if (pos().priority >= c.priority)
	{
	    assert(pos().priority >= c.priority && c.priority > i().priority);

	    // Enqueue after POS
	    commandQueue.enqueue_after(c, pos);
	}
	else
	{
	    CommandQueueIter start(commandQueue);
	    (void) start;	// Use it
	    assert(!start.ok() || start().priority < c.priority);

	    // Higher priority than first element
	    commandQueue.enqueue_at_start(c);
	}

#if LOG_QUEUE
	clog << "Command queue: " << commandQueue << "\n";
#endif
    }
}

void processCommandQueue(XtPointer, XtIntervalId *)
{
    if (emptyCommandQueue())
	return;

    if (!gdb->isReadyWithPrompt())
    {
	// Try again later...
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			200, processCommandQueue, XtPointer(0));
	return;
    }

    const Command& c = commandQueue.first();
    Command cmd(c);
    commandQueue.dequeue(c);

    _gdb_command(cmd);

    gdb_keyboard_command = false;

#if LOG_QUEUE
    clog << "Command queue: " << commandQueue << "\n";
#endif
}

// Wait for command queue to drain
void syncCommandQueue()
{
    while (!(emptyCommandQueue() && gdb->isReadyWithPrompt()))
    {
	if (gdb->isReadyWithPrompt())
	    processCommandQueue();

	XtAppProcessEvent(XtWidgetToApplicationContext(command_shell),
			  XtIMTimer | XtIMAlternateInput);
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

    XWindowAttributes xwa;
    XGetWindowAttributes(XtDisplay(parent), XtWindow(parent), &xwa);
    if (xwa.map_state != IsViewable)
	return command_shell;

    return parent;
}
