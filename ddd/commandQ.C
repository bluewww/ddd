// $Id$ -*- C++ -*-
// DDD command queue

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

char commandQueue_rcsid[] = 
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
#include "ddd.h"
#include "Queue.h"
#include "windows.h"
#include "GDBAgent.h"

#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <X11/StringDefs.h>

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

void _gdb_command(string command, Widget origin, OQCProc callback, void *data)
{
    if (gdb->isReadyWithPrompt())
    {
	set_status("");

	if (command.length() == 1 && iscntrl(command[0]))
	    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);

	handle_running_commands(command, origin);

	if (command.length() == 0)
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

    gdb_last_origin = find_shell(gdb_keyboard_command ? gdb_w : origin);

    if (gdb_last_origin != 0)
    {
	XtAddCallback(gdb_last_origin, XtNdestroyCallback, 
		      ClearOriginCB, 0);
    }

    user_cmdSUC(command, origin, callback, data);
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
    OQCProc callback;		// Associated callback
    void *data;			// Data for callback

private:
    static void clear_origin(Widget w, XtPointer client_data, 
			     XtPointer call_data);

    void add_destroy_callback()
    {
	if (origin != 0)
	    XtAddCallback(origin, XtNdestroyCallback, clear_origin, 
			  (XtPointer)this);
    }

    void remove_destroy_callback()
    {
	if (origin != 0)
	    XtRemoveCallback(origin, XtNdestroyCallback, clear_origin,
			     (XtPointer)this);
    }

public:
    Command(const string& cmd, Widget w = 0, OQCProc cb = 0, void *d = 0)
	: command(cmd), origin(w), callback(cb), data(d)
    {
	add_destroy_callback();
    }
    Command(const Command& c)
	: command(c.command), origin(c.origin), 
	  callback(c.callback), data(c.data)
    {
	add_destroy_callback();
    }
    ~Command()
    {
	remove_destroy_callback();
    }
    Command& operator = (const Command& c)
    {
	if (this != &c)
	{
	    remove_destroy_callback();

	    command  = c.command;
	    origin   = c.origin;
	    callback = c.callback;
	    data     = c.data;

	    add_destroy_callback();
	}
	return *this;
    }
    bool operator == (const Command& c)
    {
	return this == &c || 
	    command == c.command && origin == c.origin 
	    && callback == c.callback && data == c.data;
    }
};

void Command::clear_origin(Widget w, XtPointer client_data, XtPointer)
{
    (void) w;			// Use it

    // The widget is being destroyed.  Remove all references.
    Command *command = (Command *)client_data;
    assert(w == command->origin);
    command->origin = 0;
}

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

void gdb_command(const string& cmd, Widget origin, 
		 OQCProc callback, void *data)
{
    if (cmd.length() == 1 && iscntrl(cmd[0]) || cmd == "yes" ||	cmd == "no")
    {
	_gdb_command(cmd, origin, callback, data);
	clearCommandQueue();
	return;
    }

    if (gdb->isReadyWithPrompt() && commandQueue.isEmpty())
    {
	if (callback == 0)
	    add_to_history(cmd);

	_gdb_command(cmd, origin, callback, data);
    }
    else
	commandQueue += Command(cmd, origin, callback, data);
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

static bool ddd_is_idle = false;
static Boolean ddd_idle(XtPointer)
{
    ddd_is_idle = commandQueue.isEmpty() && gdb->isReadyWithPrompt();
    return ddd_is_idle;		// If idle, remove from the list of work procs
}

// Wait for command queue to drain
void syncCommandQueue()
{
    ddd_is_idle = false;
    XtAppAddWorkProc(XtWidgetToApplicationContext(command_shell),
		     ddd_idle, 0);

    while (!ddd_is_idle)
    {
	processCommandQueue(0, 0);
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
