// $Id$ -*- C++ -*-
// DDD interface to GDB commands

// Copyright (C) 1996-1997 Technische Universitaet Braunschweig, Germany.
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

// Whenever I think of DDD, I'm reminded of that song by Trio that was 
// featured in those Volkswagen commercials -- "Da Da Da".
// 
// Tom "Dee Dee Dee" Harrington <tph@rmi.net> -- rec.humor.oracle.d


char Command_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Queue.h"
#endif

#ifndef LOG_QUEUE
#define LOG_QUEUE 0
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
#include "AppData.h"
#include "disp-read.h"

#include <ctype.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <X11/StringDefs.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

// Origin of last command
static Widget gdb_last_origin;

// True if a user command was processed
static bool had_user_command = false;


//-----------------------------------------------------------------------------
// Auto command management
//-----------------------------------------------------------------------------

// Strip auto-command prefix
void strip_auto_command_prefix(string& cmd)
{
    // Neither of these ever changes, so make it static
    static string echo = gdb->echo_command(app_data.auto_command_prefix);

    int i;
    while ((i = cmd.index(echo)) >= 0)
    {
	cmd.at(i, echo.length()) = "";
	int nl = cmd.index("\\n", i);
	if (nl >= 0)
	    cmd.at(nl, 2) = "";
    }
}

// Add it
void add_auto_command_prefix(string& cmd)
{
    cmd = gdb->echo_command(app_data.auto_command_prefix + cmd + "\n");
}


//-----------------------------------------------------------------------------
// GDB command management
//-----------------------------------------------------------------------------

static void ClearOriginCB(Widget w, XtPointer, XtPointer)
{
    if (gdb_last_origin == w)
	gdb_last_origin = 0;
}

// Translate frequently used commands
void translate_command(string& command)
{
    switch(gdb->type())
    {
    case XDB:
	// Although we have defined these as aliases, we translate
	// them anyway.  This way, DDD can find the documentation
	// (which won't work for aliases).
	if (command == "run")
	    command = "r";
	else if (command == "cont")
	    command = "c";
	else if (command == "next")
	    command = "S";
	else if (command == "step")
	    command = "s";
	else if (command == "quit")
	    command = "q";
	break;

    case JDB:
	// Typing ^C at JDB kills it, which is not what the user
	// expects.  Simply suspend all threads instead.
	if (command == '\003')
	    command = "suspend";

	// FALL THROUGH
    case DBX:
	// DBX and JDB use `step up' instead of `finish'.
	if (command == "finish")
	    command = "step up";
	break;

    case PYDB:
	// PYDB uses `return' instead of `finish'.
	if (command == "finish")
	    command = "return";
	break;

    case PERL:
	// PERL only has one-letter commands
	if (command == "run")
	    command = "r";
	else if (command == "cont")
	    command = "c";
	else if (command == "step")
	    command = "s";
	else if (command == "next")
	    command = "n";
	else if (command == "quit")
	    command = "q";
	else if (command == "finish")
	    command = "r";
	break;

    case GDB:
	break;
    }

    // When recording, realize certain commands as auto commands.
    if (gdb->recording())
    {
	if (is_graph_cmd(command))
	{
	    // Graph commands are handled by DDD.
	    add_auto_command_prefix(command);
	}
	else if (is_running_cmd(command, gdb))
	{
	    // Running commands (typically, `continue') may issue
	    // display output or positions that must be interpreted by DDD.
	    add_auto_command_prefix(command);
	}
    }
}

// Process command C; do it right now.
static void do_gdb_command(Command& c)
{
    string cmd = c.command;
    OQCProc callback       = c.callback;
    OACProc extra_callback = c.extra_callback;
    if (cmd.contains('\n'))
    {
	cmd = cmd.before('\n');
	callback = 0;
	extra_callback = 0;
    }
    c.command = c.command.after('\n');

#if LOG_QUEUE
    clog << "Command " << quote(cmd) << "\n";
#endif

    if (gdb->isReadyWithPrompt())
    {
	if (c.verbose)
	{
	    set_status("");
	    had_user_command = true;
	}

	if (cmd.length() == 1 && iscntrl(cmd[0]))
	{
	    promptPosition = messagePosition = XmTextGetLastPosition(gdb_w);
	}
	else if (cmd.length() > 0 && c.echo)
	{
	    add_to_history(cmd);
	}

	if (!gdb->recording())
	    handle_running_commands(cmd, c.origin);

	if (cmd.length() == 0 && c.prompt)
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

    translate_command(cmd);

    if (is_internal_command(cmd))
    {
	internal_command(cmd, callback, c.data, c.echo, c.verbose, c.prompt);
    }
    else
    {
	send_gdb_command(cmd, c.origin, callback, extra_callback, c.data, 
			 c.echo, c.verbose, c.prompt, c.check);
    }
    messagePosition = XmTextGetLastPosition(gdb_w);
}

bool userInteractionSeen()
{
    return had_user_command;
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

static string last_user_reply = "";

// Last user reply to a `y or n' question
string lastUserReply()
{
    return last_user_reply;
}

void gdb_command(const Command& c0)
{
    Command c(c0);

    if (c.command.length() == 1 && iscntrl(c.command[0]) 
	|| c.command == "no" || c.command == "yes")
    {
 	// User interaction -- execute immediately
	last_user_reply = c.command;
	do_gdb_command(c);

	if (last_user_reply != "yes")
	{
	    // Probably some canceling command - clear remaining commands
	    clearCommandQueue();
	}
    }
    else if (gdb->isReadyWithPrompt() && emptyCommandQueue())
    {
	do_gdb_command(c);
    }

    if (c.command != "")
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

    Command& c = commandQueue.first();
    if (c.command.contains('\n'))
    {
	do_gdb_command(c);
    }
    else
    {
	Command cmd(c);
	commandQueue.dequeue(c);
	do_gdb_command(cmd);
    }

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

	// This should ensure ESC and ^C can interrupt this queue.
	// Hopefully we have some timer ready that checks for them...
	process_emergencies();

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
    if (parent == 0 || !XtIsRealized(parent))
	return command_shell;

    XWindowAttributes xwa;
    XGetWindowAttributes(XtDisplay(parent), XtWindow(parent), &xwa);
    if (xwa.map_state != IsViewable)
	return command_shell;

    return parent;
}
