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

// Current entry we're processing
int UndoBuffer::current_entry = 0;

// True if we're undoing
bool UndoBuffer::undoing = false;

// If true, ignore all changes and additions
bool UndoBuffer::locked = false;

// If true, we're showing some older program state
bool UndoBuffer::_showing_earlier_state = false;

// Current command source
string UndoBuffer::current_source = "";

// Force new entry?
bool UndoBuffer::force_new_entry = true;


//-----------------------------------------------------------------------
// Position history
//-----------------------------------------------------------------------

// True if ENTRY has any effect
bool UndoBuffer::has_effect(const UndoBufferEntry& entry)
{
    for (StringStringAssocIter iter(entry); iter.ok(); iter++)
    {
	if (iter.key() != UB_SOURCE)
	    return true;
    }

    return false;
}

void UndoBuffer::add(const UndoBufferEntry& entry)
{
    restore_current_state();

    if (entry.has_exec_pos())
    {
	// Adding a new execution position - remove all non-exec
	// positions from history as well as all exec commands
	for (int i = 0; i < history.size(); i++)
	{
	    UndoBufferEntry& entry = history[i];

	    entry.remove(UB_POS);
	    entry.remove(UB_ADDRESS);
	    entry.remove(UB_EXEC_COMMAND);
	}

	// Remove all entries with no effect
	UndoBufferArray new_history;
	for (int i = 0; i < history.size(); i++)
	{
	    UndoBufferEntry& entry = history[i];

	    if (has_effect(entry))
	    {
		new_history += history[i];
	    }
	}
	history = new_history;

	history_position = history.size();
    }
    else
    {
	// Added a new position - remove all later entries, except for
	// exec positions
	UndoBufferArray new_history;
	for (int i = 0; i < history.size(); i++)
	{
	    if (i < history_position || history[i].has_exec_pos())
	    {
		UndoBufferEntry entry = history[i];

		if (i >= history_position)
		{
		    entry.remove(UB_POS);
		    entry.remove(UB_ADDRESS);
		    entry.remove(UB_COMMAND);
		    entry.remove(UB_EXEC_COMMAND);
		}

		new_history += entry;
	    }
	}
	history = new_history;
    }

    // Add at end
    history += entry;
    history_position = history.size();

    done();
}

// Add status NAME/VALUE to history
void UndoBuffer::add_status(const string& name, const string& value)
{
    if (locked)
	return;

#if LOG_UNDO_BUFFER
    clog << "Adding " << name << " = " << quote(value) << "\n";
#endif

    if (force_new_entry || history.size() == 0)
    {
	// Create new entry
	UndoBufferEntry new_entry;

	// Check whether we can copy the previous state
	if (history.size() == 0)
	{
	    // Nothing to copy from
	}
	else if (name == UB_EXEC_POS || name == UB_EXEC_ADDRESS)
	{
	    // This is an execution position.  Rely on new state.
	}
	else
	{
	    // This is a command.  Copy state from last entry.
	    UndoBufferEntry& last_entry = history[history.size() - 1];

	    new_entry = last_entry;

	    // Never copy commands.
	    new_entry.remove(UB_COMMAND);
	    new_entry.remove(UB_EXEC_COMMAND);

	    if (name == UB_POS || name == UB_ADDRESS)
	    {
		// When adding a lookup position, don't get overridden
		// by execution positions.
		new_entry.remove(UB_EXEC_POS);
		new_entry.remove(UB_EXEC_ADDRESS);
	    }
	}

	new_entry[name]      = value;
	new_entry[UB_SOURCE] = current_source;

	add(new_entry);

	force_new_entry = false;
    }
    else
    {
	// Add NAME/VALUE to last entry
	UndoBufferEntry& last_entry = history[history.size() - 1];

	last_entry[name]      = value;
	last_entry[UB_SOURCE] = current_source;
    }

    done();
}

// Add command COMMAND to history.
void UndoBuffer::add_command(const string& command, bool exec)
{
    string c = command;
    strip_space(c);
    if (c == "")
	return;

    string command_key = exec ? UB_EXEC_COMMAND : UB_COMMAND;

    if (!undoing)
    {
	// Regular command
	add_status(command_key, c);
	return;
    }

    // We're called from undo or redo via process_command() - 
    // replace currently executed command by its undo command.

#if LOG_UNDO_BUFFER
    clog << "Adding " << command_key << " = " << quote(command) << "\n";
#endif

    assert (history.size() > 0);
    assert (current_entry >= 0 && current_entry < history.size());

    UndoBufferEntry& entry = history[current_entry];

    if (entry.has(command_key))
	entry[command_key].prepend(c + '\n');
    else
	entry[command_key] = c;

    done();
}

// Define COMMAND as command source
void UndoBuffer::set_source(const string& command)
{
#if 0
    clog << "Source:\t" << quote(command) << "\n";
#endif

    current_source = action(command);

    if (!locked)
	force_new_entry = true;
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

	if (i == history_position)
	    clog << ">-------------\n";

	clog << i << '\t';

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

    if (undo_action() != NO_GDB_ANSWER)
	clog << "Undo " << undo_action() << "\n";
    if (redo_action() != NO_GDB_ANSWER)
	clog << "Redo " << redo_action() << "\n";

    clog << "\n";
#endif
}

void UndoBuffer::remap_breakpoint(string& cmd, int old_bp, int new_bp)
{
    string old_num = "@" + itostring(old_bp) + "@";
    string new_num = "@" + itostring(new_bp) + "@";
    cmd.gsub(old_num, new_num);
}

void UndoBuffer::remap_breakpoint(int old_bp, int new_bp)
{
    for (int i = 0; i < history.size(); i++)
    {
	if (history[i].has(UB_COMMAND))
	    remap_breakpoint(history[i][UB_COMMAND], old_bp, new_bp);
	if (history[i].has(UB_EXEC_COMMAND))
	    remap_breakpoint(history[i][UB_EXEC_COMMAND], old_bp, new_bp);
    }
}

void UndoBuffer::add_breakpoint_state(ostream& os, BreakPoint *bp)
{
    os << REMAP_COMMAND << "@" << bp->number() << "@\n";
    bp->get_state(os, bp->number());
}

static void get_confirm(const string& complete_answer, void *qu_data)
{
    bool *flag = (bool *)qu_data;
    *flag = complete_answer.contains("is on");
}

void UndoBuffer::process_command(UndoBufferEntry& entry)
{
    // Process command
    string commands;
    if (entry.has(UB_EXEC_COMMAND))
	commands = entry[UB_EXEC_COMMAND];
    else if (entry.has(UB_COMMAND))
	commands = entry[UB_COMMAND];
    else
	return;

    entry.remove(UB_COMMAND);
    entry.remove(UB_EXEC_COMMAND);

    string original_commands = commands;
    int bp_count = 0;
    undoing = true;

    bool confirm = false;

    if (commands != "" && gdb->type() == GDB)
    {
	gdb_command("show confirm", 0, get_confirm, &confirm);
	syncCommandQueue();
    }

    if (confirm)
    {
	// Turn confirmation off during undo/redo.
	gdb_question("set confirm off");
    }

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
	    int new_bp_nr = source_view->next_breakpoint_number() + bp_count++;

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

	// Execute command.  This will result in new redo command(s)
	// being passed to add_command().
	Command c(cmd);
	c.priority = COMMAND_PRIORITY_SYSTEM;
	gdb_command(c);

	// Wait until this command is processed
	syncCommandQueue();
    }

    if (confirm)
	gdb_question("set confirm on");

    if (!entry.has(UB_COMMAND) && !entry.has(UB_EXEC_COMMAND))
    {
	cerr << "Undo: " << quote(original_commands) << " cannot be redone\n";
    }

    undoing = false;
}

void UndoBuffer::process_state(UndoBufferEntry& entry, bool restore_state)
{
    // Process position
    string pos = "";
    string address = "";
    bool exec_pos = false;

    if (restore_state && entry.has(UB_EXEC_POS))
    {
	pos = entry[UB_EXEC_POS];
	exec_pos = true;
    }
    else if (entry.has(UB_POS))
    {
	pos = entry[UB_POS];
    }
    else if (entry.has(UB_EXEC_POS))
    {
	pos = entry[UB_EXEC_POS];
    }

    if (restore_state && entry.has(UB_EXEC_ADDRESS))
    {
	address = entry[UB_EXEC_ADDRESS];
	exec_pos = true;
    }
    else if (entry.has(UB_ADDRESS))
    {
	address = entry[UB_ADDRESS];
    }
    else if (entry.has(UB_EXEC_ADDRESS))
    {
	address = entry[UB_EXEC_ADDRESS];
    }

    if (pos != "" || address != "")
    {
	string file = pos.before(':');
	string line = pos.after(':');
	source_view->goto_entry(file, atoi(line.chars()), address, exec_pos);
    }
    else if (restore_state)
    {
	// No execution position found
	source_view->clear_execution_position();
    }

    if (restore_state)
    {
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
    }

    if (restore_state)
    {
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
	if (entry.has(UB_FRAME))
	{
	    string frame = entry[UB_FRAME];
	    source_view->process_frame(atoi(frame));
	}

	// Process registers
	string registers = "Unknown state";
	if (entry.has(UB_REGISTERS))
	    registers = entry[UB_REGISTERS];
	source_view->process_registers(registers);
    }

    locked = false;
}

void UndoBuffer::process_command(int entry)
{
    assert(OK());

    locked = true;

    current_entry = entry;
    process_command(history[entry]);

    locked = false;
}

void UndoBuffer::process_state(int entry, bool restore_state)
{
    assert(OK());

    locked = true;

    current_entry = entry;
    process_state(history[entry], restore_state);

    locked = false;
}

// Undo and redo commands
void UndoBuffer::undo()
{
    if (locked)
	return;

    if (history_position == 0)
    {
	set_status("Nothing to undo");
	return;
    }

    StatusDelay delay("Undoing " + undo_action());
   
    // Undo most recent command
    process_command(history_position - 1);

    if (history_position > 1)
    {
	// Restore previous state
	bool restore_state = 
	    (showing_earlier_state() || 
	     history[history_position - 1].has_exec_pos());

	process_state(history_position - 2, restore_state);
    }

    history_position--;

    done(&delay);
}

void UndoBuffer::redo()
{
    if (locked)
	return;

    if (history_position >= history.size())
    {
	set_status("Nothing to redo");
	return;
    }

    StatusDelay delay("Redoing " + redo_action());

    // Redo next command and restore state
    process_state(history_position, showing_earlier_state());
    process_command(history_position);
    
    history_position++;

    done(&delay);
}

// Action descriptions
string UndoBuffer::action(const string& command)
{
    string c = command;

    while (c.contains(REMAP_COMMAND, 0))
	c = c.after('\n');

    if (c.contains("graph ", 0))
	c = c.after("graph ");

    if (c.contains(' '))
	c = c.before(' ');

    strip_space(c);
    c.capitalize();
    return c;
}

string UndoBuffer::undo_action()
{
    if (gdb->recording() || history_position == 0)
	return NO_GDB_ANSWER;	// Nothing to undo

    const UndoBufferEntry& undo_entry = history[history_position - 1];

    if (!undo_entry.has_command() && history_position == 1)
	return NO_GDB_ANSWER;	// No state to restore

    if (undo_entry.has(UB_SOURCE))
	return action(undo_entry[UB_SOURCE]);

    return "";		// Generic action
}

string UndoBuffer::redo_action()
{
    if (gdb->recording() || history_position >= history.size())
	return NO_GDB_ANSWER;	// Nothing to redo

    const UndoBufferEntry& redo_entry = history[history_position];

#if 0
    if (redo_entry.has(UB_EXEC_COMMAND))
	return action(redo_entry[UB_EXEC_COMMAND]);
    if (redo_entry.has(UB_COMMAND))
	return action(redo_entry[UB_COMMAND]);
#endif

    if (redo_entry.has(UB_SOURCE))
	return action(redo_entry[UB_SOURCE]);

    return "";		// Generic action
}

void UndoBuffer::showing_earlier_state(bool set, StatusMsg *msg)
{
    if (_showing_earlier_state == set)
    {
	if (set && msg != 0)
	    msg->outcome += ".  Still showing earlier program state";
	return;
    }

    _showing_earlier_state = set;

    if (msg != 0)
    {
	if (set)
	    msg->outcome = "failed.  Showing earlier program state instead";
	else
	    msg->outcome += ".  Back in current program state";
    }

    XtVaSetValues(data_disp->graph_edit, XtNdashedLines, set, NULL);
    update_arg_buttons();
    data_disp->refresh_args();
    source_view->showing_earlier_state(set);

    if (!set)
    {
	// Re-activate user displays
	data_disp->make_sane();
    }
}

// Refresh all settings
void UndoBuffer::done(StatusMsg *msg)
{
    log();
    assert(OK());

    // Check whether we're showing an earlier state
    bool earlier = false;

    if (history.size() > 0)
    {
	// Check the current and all redoable entries.  If one of them
	// contains a state field whose value differs from the last
	// one, we're in an earlier state.
	const UndoBufferEntry& last = history[history.size() - 1];

	for (int i = history_position - 1;
	     !earlier && i < history.size() - 1; i++)
	{
	    if (in_earlier_state(history[i], last))
		earlier = true;
	}
    }

    showing_earlier_state(earlier, msg);
    refresh_buttons();
}

// Return true if ENTRY is in an earlier state than LAST
bool UndoBuffer::in_earlier_state(const UndoBufferEntry& entry,
				  const UndoBufferEntry& last)
{
    if (!entry.has_exec_pos())
	return false;

    for (StringStringAssocIter iter(entry); iter.ok(); iter++)
    {
	const string& key   = iter.key();
	const string& value = iter.value();

	if (key == UB_COMMAND || key == UB_EXEC_COMMAND || key == UB_SOURCE)
	    continue;

	if (last.has(key) && last[key] != value)
	    return true;
    }

    return false;
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

// Clear execution positions - after a `run', for instance.
// Keep commands only.
void UndoBuffer::clear_exec_pos()
{
    if (locked)
	return;

    UndoBufferArray new_history;
    int old_history_position = history_position;
    for (int i = 0; i < history.size(); i++)
    {
	const UndoBufferEntry& entry = history[i];
	
	if (entry.has(UB_COMMAND))
	{
	    UndoBufferEntry new_entry;
	    new_entry[UB_COMMAND] = entry[UB_COMMAND];
	    if (entry.has(UB_SOURCE))
		new_entry[UB_SOURCE] = entry[UB_SOURCE];

	    new_history += new_entry;
	}
	else
	{
	    // Ignore this entry
	    if (i < old_history_position)
		history_position--;
	}
    }

    history = new_history;

    done();
}

// Restore current program state
void UndoBuffer::restore_current_state()
{
    if (locked)
	return;

    assert(OK());

    if (!showing_earlier_state())
	return;

    StatusDelay delay("Restoring state");

    UndoBufferArray new_history;
    for (int i = 0; i < history.size(); i++)
    {
	if (i < history_position || history[i].has_exec_pos())
	    new_history += history[i];
    }

    history = new_history;

    if (history.size() > 0)
    {
	process_command(history.size() - 1);
	process_state(history.size() - 1, true);
    }

    history_position = history.size();

    done(&delay);
}

// Invariant
bool UndoBuffer::OK()
{
    // HISTORY_POSITION must be within bounds.
    assert(history_position >= 0);
    assert(history_position <= history.size());

    return true;
}
