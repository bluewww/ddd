// $Id$ -*- C++ -*-
// DDD interface to GDB commands

// Copyright (C) 1996-1997 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

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
#define LOG_QUEUE 1
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

// Return auto-command for CMD
static string auto_command(const string& cmd)
{
    return gdb->echo_command(app_data.auto_command_prefix + cmd + "\n");
}

// Add auto-command prefix to CMD
void add_auto_command_prefix(string& cmd)
{
    cmd = auto_command(cmd);
}

// Strip auto-command prefix from CMD
void strip_auto_command_prefix(string& cmd)
{
    // Neither of these ever changes, so make them static
    static string dummy = "XYZZY";
    static string echo = auto_command(dummy);
    static string echo_prefix = echo.before(dummy);
    static string echo_suffix = echo.after(dummy);

    int i;
    if ((i = cmd.index(echo_prefix)) >= 0)
    {
	cmd.at(i, echo_prefix.length()) = "";
	if (echo_suffix != "")
	{
	    int suffix = cmd.index(echo_suffix, i);
	    if (suffix >= 0)
		cmd.at(suffix, echo_suffix.length()) = "";
	}

	cmd = uncook(cmd);
    }
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
	if (command == "run" || command == "R")
	    command = gdb->run_command(command.after(' '));
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
	else if (is_running_cmd(command))
	{
	    // Running commands (typically, `continue') may issue
	    // display output or positions that must be interpreted by DDD.
	    add_auto_command_prefix(command);
	}
    }
}

static string _current_gdb_command;

const string& current_gdb_command()
{
    return _current_gdb_command;
}

#if LOG_QUEUE
static ostream& operator<<(ostream& os, const Command& c)
{
    os << quote(c.command) << "<" << c.priority << ">";
    if (!c.start_undo)
	os << "*";

    return os;
}
#endif

// Process command C; do it right now.
static void _do_gdb_command(const Command& c, bool is_command = true)
{
    string cmd = c.command;

    if (is_command)
	_current_gdb_command = cmd;

#if LOG_QUEUE
    clog << "Command " << c << "\n";
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
	internal_command(cmd, c.callback, c.data, c.echo, c.verbose, c.prompt);
    }
    else
    {
	send_gdb_command(cmd, c.origin, c.callback, c.extra_callback, c.data, 
			 c.echo, c.verbose, c.prompt, c.check, c.start_undo);
    }
    messagePosition = XmTextGetLastPosition(gdb_w);
}

// True while do_gdb_command() is running
static bool processing_gdb_commands = false;

// True in the first 200ms after continuing
static bool continuing = false;

static XtIntervalId continuing_timeout = 0;

static void ClearContinuingCB(XtPointer, XtIntervalId *)
{
    continuing = false;
    processCommandQueue();
}

// True if GDB can run a command
bool can_do_gdb_command()
{
    if (processing_gdb_commands || continuing)
	return false;

    if (gdb->isReadyWithPrompt())
	return true;		// GDB is ready

    if (app_data.stop_and_continue && !gdb->recording())
    {
	if (is_cont_cmd(current_gdb_command()))
	    return true;	// We can interrupt `run' and `cond'
    }

    return false;		// Wait until later...
}

// Process command C; do it right now.  Interrupt if needed.
static void do_gdb_command(Command& given_c, bool is_command = true)
{
    Command c(given_c);

    if ((continuing && !gdb->isReadyWithPrompt()) ||
	(processing_gdb_commands && c.priority < COMMAND_PRIORITY_NOW))
    {
	// Cannot do command right now - put it back into the queue
	c.priority = COMMAND_PRIORITY_READY;
	gdb_command(c);
	return;
    }

    if (continuing)
    {
	XtRemoveTimeOut(continuing_timeout);
	continuing = false;
    }

    processing_gdb_commands = true;

    if (c.command.contains('\n'))
    {
	// Multi-line command:
	// Handle only first command
	c.command = c.command.before('\n');
	c.callback = 0;
	c.extra_callback = 0;

	// Push back remainder into queue
	given_c.command = given_c.command.after('\n');
	given_c.priority = COMMAND_PRIORITY_MULTI;
	given_c.start_undo = false;
	gdb_command(given_c);
    }

    if (c.command == "")
    {
	// No command left
	processing_gdb_commands = false;
	return;
    }

    if (gdb->isReadyWithPrompt() || c.priority == COMMAND_PRIORITY_NOW)
    {
	// Process command right now
	if (is_cont_cmd(c.command))
	{
	    // Avoid interrupting a `cont' command just after it has been sent
	    continuing = true;
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			    200, ClearContinuingCB, XtPointer(0));
	}

	_do_gdb_command(c, is_command);
	processing_gdb_commands = false;
	return;
    }

    // Enqueue interrupt command
    Command interrupt(c);
    interrupt.command        = '\003';
    interrupt.callback       = 0;
    interrupt.extra_callback = 0;
    interrupt.priority       = COMMAND_PRIORITY_NOW;
    gdb_command(interrupt);

    // Re-enqueue original command
    c.priority = COMMAND_PRIORITY_READY;
    c.start_undo = false;
    gdb_command(c);

    bool continue_after_command = 
	!is_running_cmd(c.command) && 
	!starts_recording(c.command) &&
	!is_file_cmd(c.command, gdb) &&
	!is_define_cmd(c.command) &&
	!is_core_cmd(c.command);

    if (continue_after_command)
    {
	// Enqueue 'continue' command.  This goes after the last user command.
	Command cont(c);
	cont.command        = "cont";
	cont.callback       = 0;
	cont.extra_callback = 0;
	cont.start_undo     = false;
	cont.priority       = COMMAND_PRIORITY_CONT;

	gdb_command(cont);
    }

    processing_gdb_commands = false;
}

bool userInteractionSeen()
{
    return had_user_command;
}


//-----------------------------------------------------------------------------
// Command groups
//-----------------------------------------------------------------------------

int CommandGroup::active = 0;
bool CommandGroup::first_command = true;


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
	const Command& c = i();

	if (first)
	    first = false;
	else
	    os << ", ";

	os << c;
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

    bool control_command = 
	(c.command.length() == 1 && iscntrl(c.command[0]));

    if (!continuing && 
	(control_command || 
	 c.command == "no" || 
	 c.command == "yes" || 
	 c.priority == COMMAND_PRIORITY_NOW))
    {
 	// User interaction -- execute immediately
	bool is_command = (c.command == '\003');
	bool is_canceling = (c.priority <= COMMAND_PRIORITY_USER &&
			     c.command != "yes");

	last_user_reply = c.command;
	c.priority = COMMAND_PRIORITY_NOW;
	do_gdb_command(c, is_command);

	if (is_canceling)
	{
	    // Probably some canceling command - clear remaining commands
	    clearCommandQueue();
	}

	return;
    }

    if (can_do_gdb_command() && emptyCommandQueue())
    {
	// We're ready - process immediately
	do_gdb_command(c);
	return;
    }

    if (c.command == "")
    {
	// Empty command -- ignore
	return;
    }

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

    // Be sure to process the new command queue
    processCommandQueue();
}

void processCommandQueue(XtPointer, XtIntervalId *id)
{
    static XtIntervalId process_timeout = 0;

    if (id != 0)
	process_timeout = 0;		// Called by timeout

    if (emptyCommandQueue())
	return;

#if LOG_QUEUE
    clog << "processCommandQueue: "
	 << "processing_gdb_commands = " << processing_gdb_commands << ", "
	 << "continuing = " << continuing 
	 << "\n";
#endif

    if (can_do_gdb_command())
    {
	Command& c = commandQueue.first();
	Command cmd(c);
	commandQueue.dequeue(c);
	do_gdb_command(cmd);

	gdb_keyboard_command = false;

#if LOG_QUEUE
	clog << "Command queue: " << commandQueue << "\n";
#endif
    }

    if (process_timeout == 0)
    {
	// Check again later...
	process_timeout = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			    200, processCommandQueue, XtPointer(0));
    }
}

// Wait for command queue to drain
void syncCommandQueue()
{
    while (!(emptyCommandQueue() && can_do_gdb_command()))
    {
	// Process next command from queue
	processCommandQueue();

	// This should ensure ESC and ^C can interrupt this queue.
	// Hopefully we have some timer ready that checks for them...
	process_emergencies();

	// Process input from GDB
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
