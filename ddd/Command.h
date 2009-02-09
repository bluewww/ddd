// $Id$ -*- C++ -*-
// DDD interface to GDB commands

// Copyright (C) 1996-1997 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000-2001 Universitaet Passau, Germany.
// Copyright (C) 2004 Free Software Foundation, Inc.
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

#ifndef _DDD_Command_h
#define _DDD_Command_h

#include "GDBAgent.h"
#include "question.h"		// NO_GDB_ANSWER

// Priorities.  The higher the priority, the earlier the command
// will be executed.
#define COMMAND_PRIORITY_WORK  -2  // Work procedures
#define COMMAND_PRIORITY_USER   0  // User-initiated commands
#define COMMAND_PRIORITY_BATCH  1  // Batch jobs (auto commands)
#define COMMAND_PRIORITY_INIT   2  // Init commands
#define COMMAND_PRIORITY_SYSTEM 3  // System-initiated commands
#define COMMAND_PRIORITY_AGAIN  5  // Try again
#define COMMAND_PRIORITY_CONT   7  // Continue after interrupt
#define COMMAND_PRIORITY_MULTI  8  // Multi-Line Command
#define COMMAND_PRIORITY_READY  9  // Wait until ready
#define COMMAND_PRIORITY_NOW   10  // Do it now

// Command Groups.  While a command group is active, commands will be
// undone as a group.
struct CommandGroup {
    static int active;
    static bool first_command;

    CommandGroup()
    {
	if (active++ == 0)
	    first_command = true;
    }

    ~CommandGroup()
    {
	--active;
    }
};
  
// Called from callback with the complete answer
typedef void (*OQCProc)(const string& complete_answer, void *qu_data);

// Commands
struct Command
{
    string command;		// Command text
    Widget origin;		// Origin
    OQCProc callback;		// Completion of COMMAND
    OACProc extra_callback;	// Completion of extra commands
    void *data;			// Data for callbacks
    bool echo;			// Flag: issue command in GDB console?
    bool verbose;		// Flag: issue answer in GDB console?
    bool prompt;		// Flag: issue prompt in GDB console?
    bool check;			// Flag: add extra commands to get GDB state?
    bool start_undo;		// Flag: individual undo command?
    int priority;		// Priority (highest get executed first)

private:
    static void clear_origin(Widget w, XtPointer client_data, 
			     XtPointer call_data);
    void add_destroy_callback();
    void remove_destroy_callback();

public:
#define COMMAND(TYPE) \
    Command(TYPE cmd, Widget w, OQCProc cb, void *d = 0, \
	    bool v = false, bool c = false, int p = COMMAND_PRIORITY_SYSTEM) \
	: command(cmd), origin(w), callback(cb), extra_callback(0), data(d), \
	  echo(v), verbose(v), prompt(v), check(c), \
	  start_undo(!CommandGroup::active || CommandGroup::first_command), \
	  priority(p) \
    { \
	add_destroy_callback(); \
	CommandGroup::first_command = false; \
    }

    COMMAND(const string&)
    COMMAND(const char *)
#undef COMMAND

#define COMMAND(TYPE) \
    Command(TYPE cmd, Widget w = 0) \
	: command(cmd), origin(w), callback(0), extra_callback(0), data(0), \
	  echo(true), verbose(true), prompt(true), check(true), \
	  start_undo(!CommandGroup::active || CommandGroup::first_command), \
	  priority(COMMAND_PRIORITY_USER) \
    { \
	add_destroy_callback(); \
	CommandGroup::first_command = false; \
    }

    COMMAND(const string&)
    COMMAND(const char *)
#undef COMMAND

    Command(const Command& c)
	: command(c.command), origin(c.origin), callback(c.callback),
	  extra_callback(c.extra_callback), data(c.data), 
	  echo(c.echo), verbose(c.verbose), prompt(c.prompt),
	  check(c.check), start_undo(c.start_undo), priority(c.priority)
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

	    command        = c.command;
	    origin         = c.origin;
	    callback       = c.callback;
	    extra_callback = c.extra_callback;
	    data           = c.data;
	    echo           = c.echo;
	    verbose        = c.verbose;
	    prompt         = c.prompt;
	    check          = c.check;
	    start_undo     = c.start_undo;
	    priority       = c.priority;

	    add_destroy_callback();
	}
	return *this;
    }
    bool operator == (const Command& c) const
    {
	return this == &c || 
	    command == c.command 
	    && origin == c.origin 
	    && callback == c.callback 
	    && extra_callback == c.extra_callback 
	    && data == c.data
	    && echo == c.echo
	    && verbose == c.verbose
	    && prompt == c.prompt
	    && check == c.check
	    && start_undo == c.start_undo
	    && priority == c.priority;
    }
};

// Enqueue COMMAND in command queue
extern void gdb_command(const Command& command);

// Custom calls
#define COMMAND(TYPE) \
inline void gdb_command(TYPE command, Widget origin, \
			OQCProc callback, void *data = 0, \
			bool verbose = false, bool check = false, \
			int priority = COMMAND_PRIORITY_SYSTEM) \
{ \
    gdb_command(Command(command, origin, callback, data, \
			verbose, check, priority)); \
}
COMMAND(const char *)
COMMAND(const string &)
#undef COMMAND

#define COMMAND(TYPE) \
inline void gdb_command(TYPE command, Widget origin = 0) \
{ \
    gdb_command(Command(command, origin)); \
}
COMMAND(const char *)
COMMAND(const string &)
#undef COMMAND

#define COMMAND(TYPE) \
inline void gdb_batch(TYPE command, Widget origin, \
		      OQCProc callback, void *data = 0, \
		      bool verbose = false, bool check = false, \
		      int priority = COMMAND_PRIORITY_BATCH) \
{ \
    gdb_command(Command(command, origin, callback, data, \
			verbose, check, priority)); \
}
COMMAND(const char *)
COMMAND(const string &)
#undef COMMAND

#define COMMAND(TYPE) \
inline void gdb_batch(TYPE command, Widget origin = 0) \
{ \
    gdb_command(Command(command, origin, OQCProc(0), 0, \
			false, true, COMMAND_PRIORITY_BATCH)); \
}
COMMAND(const char *)
COMMAND(const string &)
#undef COMMAND

// True if GDB can run a command
bool can_do_gdb_command();

// Pass the COMMAND given in CLIENT_DATA to gdb_command()
extern void gdbCommandCB(Widget w, XtPointer call_data, XtPointer client_data);

// Check if command queue is empty
extern bool emptyCommandQueue();

// Clear command queue
extern void clearCommandQueue();

// Synchronize with command queue
extern void syncCommandQueue();

// Return a shell widget according to last command origin
extern Widget find_shell(Widget w = 0);

// Process next element from command queue
extern void processCommandQueue(XtPointer = 0, XtIntervalId *id = 0);

// True if GDB processed any user command (= we had user interaction)
extern bool userInteractionSeen();

// Last user reply to a `y or n' question
extern string lastUserReply();

// Translate frequently used commands
extern void translate_command(string& command);

// Add and strip auto_command prefix
extern void add_auto_command_prefix(string& command);
extern void strip_auto_command_prefix(string& command);

#endif // _DDD_Command_h
// DON'T ADD ANYTHING BEHIND THIS #endif
