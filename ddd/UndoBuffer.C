// $Id$ -*- C++ -*-
// Undo/Redo buffer

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

char UndoBuffer_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "UndoBuffer.h"

#include "BreakPoint.h"
#include "Command.h"
#include "DataDisp.h"
#include "SourceView.h"
#include "buttons.h"
#include "cook.h"
#include "ddd.h"
#include "disp-read.h"
#include "index.h"
#include "misc.h"
#include "regexps.h"
#include "status.h"
#include "string-fun.h"

#ifndef LOG_UNDO_BUFFER
#define LOG_UNDO_BUFFER 0
#endif

#define REMAP_COMMAND "@remap "

UndoBuffer undo_buffer;

UndoBufferArray UndoBuffer::history;

// Last position in history + 1
int UndoBuffer::history_position = 0;

// Number of commands we still must undo
int UndoBuffer::own_commands = 0;

// Number of commands we have undone
int UndoBuffer::own_processed = 0;

// Direction we're currently working at
int UndoBuffer::own_direction = 0;

// If true, ignore all changes and additions
bool UndoBuffer::locked = false;

// If true, we're at some past execution position
bool UndoBuffer::_at_past_exec_pos = false;

// Current command source
string UndoBuffer::current_source = "";


//-----------------------------------------------------------------------
// Position history
//-----------------------------------------------------------------------

void UndoBuffer::add(const UndoBufferEntry& entry)
{
    if (entry.exec_pos)
    {
	// Adding a new execution position - remove all non-exec
	// positions from history, except for commands
	UndoBufferArray new_history;
	for (int i = 0; i < history.size(); i++)
	{
	    if (history[i].exec_pos || history[i].command)
		new_history += history[i];
	}
	history = new_history;
	history_position = history.size();
    }
    else
    {
	// Added a non-exec position - remove all later entries,
	// except for exec positions
	UndoBufferArray new_history;
	for (int i = 0; i < history.size(); i++)
	{
	    if (i < history_position || history[i].exec_pos)
		new_history += history[i];
	}
	history = new_history;
    }

    if (history_position < history.size())
    {
	// Insert after current
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	// >state         state
	//  entries...   >entry
	//                entries...
	UndoBufferArray new_history;
	int i;
	for (i = 0; i <= history_position - 1; i++)
	    new_history += history[i];
	new_history += entry;
	for (i = history_position; i < history.size(); i++)
	    new_history += history[i];

	history_position++;
    }
    else
    {
	// Add at end
	history += entry;
	history_position = history.size();
    }

    done();
}

// Add command COMMAND to history.
void UndoBuffer::add_command(const string& command)
{
    string c = command;
    strip_space(c);
    if (c == "")
	return;

    if (own_commands == 0)
    {
	// Regular command: Insert new entry before current
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	// >state         command
	//  entries...   >state
	//                entries...
	UndoBufferArray new_history;
	int i;
	for (i = 0; i < history_position - 1; i++)
	    new_history += history[i];

	// Add command
	UndoBufferEntry new_entry;
	new_entry[UB_COMMAND] = c;
	new_entry[UB_SOURCE] = current_source;
	new_entry.command = true;
	new_history += new_entry;

	// Add current
	new_history += history[history_position - 1];

	// Add remaining exec positions
	for (i = history_position; i < history.size(); i++)
	{
	    if (history[i].exec_pos)
		new_history += history[i];
	}

	history = new_history;

	history_position++;
    }
    else if (own_direction < 0 && own_processed == 0)
    {
	// Called from undo => a `redo' command: add after current
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	//  old_command  >state
	// >state         command
	//  entries...    entries...

	UndoBufferArray new_history;
	int i;
	for (i = 0; i < history_position - 2; i++)
	    new_history += history[i];

	// Add current
	new_history += history[history_position - 1];

	// Add command
	UndoBufferEntry new_entry;
	new_entry[UB_COMMAND] = c;
	new_entry[UB_SOURCE] = current_source;
	new_entry.command = true;
	new_history += new_entry;

	// Add remaining entries
	for (i = history_position; i < history.size(); i++)
	    new_history += history[i];

	history = new_history;

	history_position--;
	own_processed++;
    }
    else if (own_direction < 0 && own_processed > 0)
    {
	// Called from undo => another `redo' command; prepend before last
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	// >state        >state
	//  old_command   command + old_command
	//  entries...    entries...

	UndoBufferEntry& next_entry = history[history_position];
	next_entry[UB_COMMAND].prepend(c + '\n');
	own_processed++;
    }
    else if (own_direction > 0 && own_processed == 0)
    {
	// Called from redo => an `undo' command: insert before current
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	// >state         command
	//  old_command  >state
	//  entries...    entries...

	UndoBufferArray new_history;
	int i;
	for (i = 0; i < history_position - 1; i++)
	    new_history += history[i];

	// Add command
	UndoBufferEntry new_entry;
	new_entry[UB_COMMAND] = c;
	new_entry[UB_SOURCE] = current_source;
	new_entry.command = true;
	new_history += new_entry;

	// Add current
	new_history += history[history_position - 1];

	// Add remaining entries
	for (i = history_position + 1; i < history.size(); i++)
	    new_history += history[i];

	history = new_history;

	history_position++;
	own_processed++;
    }
    else if (own_direction > 0 && own_processed > 0)
    {
	// Called from redo => another `undo' command: prepend to previous
	//
	// BEFORE        AFTER
	// -----------   -----------
	//  entries...    entries...
	//  old_command   command + old_command
	// >state        >state
	//  entries...    entries...

	UndoBufferEntry& last_entry = history[history_position - 2];
	last_entry[UB_COMMAND].prepend(c + '\n');
	own_processed++;
    }

    done();
}



// Add status NAME/VALUE to history
void UndoBuffer::add(const string& name, const string& value, bool exec_pos)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;
    new_entry.exec_pos = exec_pos;

    if (history.size() > 0)
    {
	UndoBufferEntry& current_entry = history[history_position - 1];
	if (!current_entry.command && !current_entry.has(name))
	{
	    // Add NAME/VALUE to current position
	    current_entry[name] = value;
	    if (exec_pos)
		current_entry.exec_pos = true;
	}
	else if (current_entry.command || current_entry[name] != value)
	{
	    // VALUE has changed - use new entry instead
	    new_entry[name] = value;
	    new_entry[UB_SOURCE] = current_source;
	}
    }
    else
    {
	// No history yet: create new entry
	new_entry[name] = value;
	new_entry[UB_SOURCE] = current_source;
    }

    if (new_entry.has(name))
	add(new_entry);

    done();
}



void UndoBuffer::log()
{
#if LOG_UNDO_BUFFER
    clog << "Undo buffer:\n";
    for (int i = 0; i < history.size(); i++)
    {
#if 0
	// Only log the first 2 items around the current position
	if (abs(i - (history_position - 1)) > 2)
	    continue;
#endif

	const UndoBufferEntry& entry = history[i];

	if (i == history_position - 1)
	    clog << ">";
	else
	    clog << " ";
	clog << i;
	if (entry.exec_pos)
	    clog << "*";
	clog << '\t';

	bool first_line = true;
	for (StringStringAssocIter iter(entry); iter.ok(); iter++)
	{
	    if (!first_line)
		clog << "\n\t";
	    clog << iter.key() << " = " << quote(iter.value());
	    first_line = false;
	}
	clog << "\n";
    }
    clog << "\n";
#endif
}

void UndoBuffer::process(const UndoBufferEntry& entry, int direction)
{
    locked = true;

    assert(OK());

    if (entry.command)
	process_command(entry, direction);
    else
	process_status(entry, direction);

    locked = false;
}

void UndoBuffer::CommandDone(const string &answer, void *)
{
    if (answer == NO_GDB_ANSWER)
    {
	// Command has been canceled
	own_commands--;
    }
}

void UndoBuffer::ExtraDone(void *)
{
    if (own_processed == 0)
    {
	// Command had no effect on history.  Leave it unchanged.
	if (own_direction < 0)
	{
	    // Undo
	    add_command(history[history_position - 2][UB_COMMAND]);
	}
	else
	{
	    // Redo
	    add_command(history[history_position][UB_COMMAND]);
	}
    }

    // Command is done, and all extra commands, too.
    own_commands--;
}

void UndoBuffer::remap_breakpoint(string& cmd, int old_bp_nr, int new_bp_nr)
{
    string old_num = "@" + itostring(old_bp_nr) + "@";
    string new_num = "@" + itostring(new_bp_nr) + "@";
    cmd.gsub(old_num, new_num);
}

void UndoBuffer::remap_breakpoint(int old_bp_nr, int new_bp_nr)
{
    for (int i = 0; i < history.size(); i++)
    {
	if (!history[i].command)
	    continue;
	remap_breakpoint(history[i][UB_COMMAND], old_bp_nr, new_bp_nr);
    }
}

void UndoBuffer::add_breakpoint_state(ostream& os, BreakPoint *bp)
{
    os << REMAP_COMMAND << "@" << bp->number() << "@\n";
    bp->get_state(os, bp->number());
}

void UndoBuffer::process_command(const UndoBufferEntry& entry, int direction)
{
    // Process command
    string commands = entry[UB_COMMAND];

    int count = 0;
    own_processed = 0;
    while (commands != "")
    {
	string cmd;
	if (commands.contains('\n'))
	    cmd = commands.before('\n');
	else
	    cmd = commands;
	commands = commands.after('\n');

	// Handle breakpoint remappings
	if (cmd.contains(REMAP_COMMAND, 0))
	{
	    int old_bp_nr = atoi(cmd.chars() + strlen(REMAP_COMMAND "@"));
	    int new_bp_nr = source_view->next_breakpoint_number() + count++;

	    remap_breakpoint(old_bp_nr, new_bp_nr);
	    remap_breakpoint(commands, old_bp_nr, new_bp_nr);

	    continue;
	}

	// Replace all occurrences of `@N@' by N
#if RUNTIME_REGEX
	static regex rxnum("@[0-9]+@");
#endif
	int i;
	while ((i = index(cmd, rxnum, "@")) >= 0)
	{
	    int num = atoi(cmd.chars() + i + 1);
	    int j = cmd.index('@', i + 1);
	    cmd.at(i, j - i + 1) = itostring(num);
	}

	Command c(cmd);
	c.callback       = CommandDone;
	c.extra_callback = ExtraDone;
	c.priority       = COMMAND_PRIORITY_SYSTEM;
	own_commands++;
	own_direction    = direction;
	gdb_command(c);
    }

    // This is to avoid confusion due to multiple undo/redos
    syncCommandQueue();

    done();
}

void UndoBuffer::process_status(const UndoBufferEntry& entry,
				int direction)
{
    // Process position
    string pos = "";
    string address = "";
    if (entry.has(UB_POS))
	pos = entry[UB_POS];
    if (entry.has(UB_ADDRESS))
	address = entry[UB_ADDRESS];

    if (pos != "" || address != "")
    {
	string file = pos.before(':');
	string line = pos.after(':');
	source_view->goto_entry(file, atoi(line.chars()), 
				address, entry.exec_pos);
    }

    // Process displays
    StringArray displays;
    StringArray values;
    StringArray addrs;
    for (StringStringAssocIter iter(entry); iter.ok(); iter++)
    {
	if (iter.key().contains(UB_DISPLAY_PREFIX, 0))
	{
	    string name = iter.key().after(UB_DISPLAY_PREFIX);
	    const string& value = iter.value();
	    string addr = "";
	    if (entry.has(UB_DISPLAY_ADDRESS_PREFIX + name))
		addr = entry[UB_DISPLAY_ADDRESS_PREFIX + name];

	    displays += name;
	    values   += value;
	    addrs    += addr;
	}
    }
    data_disp->update_displays(displays, values, addrs);

    // Process threads
    string threads = "Unknown state";
    if (entry.has(UB_THREADS))
	threads = entry[UB_THREADS];
    source_view->process_threads(threads);

    // Process backtrace
    string where = "Unknown state";
    if (entry.has(UB_WHERE))
	where = entry[UB_WHERE];
    source_view->process_where(where);

    // Process frame
    if (entry.has(UB_WHERE))
    {
	string frame = entry[UB_FRAME];
	source_view->process_frame(frame);
    }

    // Process registers
    string registers = "Unknown state";
    if (entry.has(UB_REGISTERS))
	registers = entry[UB_REGISTERS];
    source_view->process_registers(registers);

    locked = false;

    history_position += direction;

    done();
}

void UndoBuffer::set_past_exec_pos(bool set)
{
    if (_at_past_exec_pos == set)
	return;

    _at_past_exec_pos = set;
    if (set)
    {
	set_status("Cannot restore previous program state; "
		   "showing earlier state instead");
    }
    else
    {
	set_status("Back at current program state");
    }

    XtVaSetValues(data_disp->graph_edit, XtNdashedLines, set, NULL);
    update_arg_buttons();
    data_disp->refresh_args();
    source_view->set_past_exec_pos(set);
}


// Refresh all settings
void UndoBuffer::done()
{
    log();
    assert(OK());

    bool set = false;
    for (int i = history_position; i < history.size(); i++)
	if (history[i].exec_pos)
	{
	    set = true;		// later exec pos
	    break;
	}

    set_past_exec_pos(set);
    refresh_buttons();
}

// Undo and redo commands
void UndoBuffer::undo()
{
    if (history_position > 1 && history.size() > 0)
    {
	set_status("Undoing " + undo_action());
	process(history[history_position - 2], -1);
	return;
    }

    set_status("Nothing to undo");
}

void UndoBuffer::redo()
{
    if (history_position < history.size())
    {
	set_status("Redoing " + redo_action());
        process(history[history_position], 1);
	return;
    }

    set_status("Nothing to redo");
}

// Action descriptions
string UndoBuffer::action(const string& command)
{
    string c = command;

    while (c.contains(REMAP_COMMAND, 0))
	c = c.after('\n');

    if (c.contains(' '))
	c = c.before(' ');

    strip_space(c);
    c.capitalize();
    return c;
}

string UndoBuffer::undo_action()
{
    if (history_position < 2 || history.size() == 0)
	return NO_GDB_ANSWER;	// Nothing to undo

    const UndoBufferEntry& previous_entry = history[history_position - 2];
    const UndoBufferEntry& current_entry  = history[history_position - 1];
    if (previous_entry.command)
	return action(previous_entry[UB_SOURCE]);

    if (!current_entry.exec_pos)
	return action("lookup");
    if (current_entry.has(UB_SOURCE))
	return action(current_entry[UB_SOURCE]);

    return "";		// Generic action
}

string UndoBuffer::redo_action()
{
    if (history_position >= history.size())
	return NO_GDB_ANSWER;	// Nothing to redo

    const UndoBufferEntry& entry = history[history_position];
    if (entry.command)
	return action(entry[UB_COMMAND]);
    if (entry.exec_pos && entry.has(UB_SOURCE))
	return action(entry[UB_SOURCE]);
    if (!entry.exec_pos)
	return action("lookup");

    return "";		// Generic action
}


// Clear history
void UndoBuffer::clear()
{
    static UndoBufferArray empty;
    history          = empty;
    history_position = 0;
    locked           = false;

    done();
}

// Clear execution positions
void UndoBuffer::clear_exec_pos()
{
    UndoBufferArray new_history;
    for (int i = 0; i < history.size(); i++)
    {
	if (!history[i].exec_pos)
	    new_history += history[i];
	else if (i < history_position)
	    history_position--;
    }

    history = new_history;

    done();
}

// Goto last known execution position
void UndoBuffer::goto_current_exec_pos()
{
    assert(OK());

    if (!at_past_exec_pos())
	return;

    UndoBufferArray new_history;
    for (int i = 0; i < history.size(); i++)
    {
	if (i < history_position || history[i].exec_pos)
	    new_history += history[i];
    }

    history = new_history;

    if (history.size() > 0)
    {
	history_position = history.size() - 1;
	process(history[history_position - 1], 1);
    }
    else
    {
	history_position = 0;
    }

    history_position = history.size();
    done();
}

// Invariant
bool UndoBuffer::OK()
{
    // HISTORY_POSITION must be within bounds.
    assert(history.size() == 0 || history_position > 0);
    assert(history_position <= history.size());

    return true;
}
