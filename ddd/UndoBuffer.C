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

#include "SourceView.h"
#include "DataDisp.h"
#include "cook.h"
#include "ddd.h"
#include "string-fun.h"
#include "misc.h"
#include "comm-manag.h"
#include "Command.h"

#ifndef LOG_UNDO_BUFFER
#define LOG_UNDO_BUFFER 0
#endif

UndoBuffer undo_buffer;

UndoBufferArray UndoBuffer::history;

// Last position in history + 1
int UndoBuffer::history_position = 0;

// Number of commands we're currently undoing
int UndoBuffer::own_commands = 0;

// Direction we're currently working at
int UndoBuffer::own_direction = 0;

// If true, ignore all changes and additions
bool UndoBuffer::locked = false;


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

    if (history_position < history.size())
    {
	history[history_position++] = entry;

	if (!entry.exec_pos)
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
    }
    else
    {
	history += entry;
	history_position = history.size();
    }

    log();
    assert(OK());
}

// Add command COMMAND to history.
void UndoBuffer::add_command(const string& command)
{
    string c = command;
    strip_space(c);
    if (c == "")
	return;

    log();
    assert(OK());

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
    else if (own_direction < 0)
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
	new_entry.command = true;
	new_history += new_entry;

	// Add remaining entries
	for (i = history_position; i < history.size(); i++)
	    new_history += history[i];

	history = new_history;

	history_position--;
    }
    else if (own_direction > 0)
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
	new_entry.command = true;
	new_history += new_entry;

	// Add current
	new_history += history[history_position - 1];

	// Add remaining entries
	for (i = history_position + 1; i < history.size(); i++)
	    new_history += history[i];

	history = new_history;

	history_position++;
    }

    log();
    assert(OK());
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
	}
    }
    else
    {
	// No history yet: create new entry
	new_entry[name] = value;
    }

    if (new_entry.has(name))
	add(new_entry);

    log();
    assert(OK());
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

    assert(OK());

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
    // Command is done, and all extra commands, too.
    own_commands--;
}

void UndoBuffer::process_command(const UndoBufferEntry& entry, int direction)
{
    // Process command
    string cmd = entry[UB_COMMAND];

    while (cmd != "")
    {
	string line;
	if (cmd.contains('\n'))
	    line = cmd.before('\n');
	else
	    line = cmd;
	cmd = cmd.after('\n');

	fix_symbols(line);

	Command c(line);
	c.callback       = CommandDone;
	c.extra_callback = ExtraDone;
	own_commands++;
	own_direction = direction;
	gdb_command(c);
    }
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
    if (entry.has(UB_THREADS))
    {
	string threads = entry[UB_THREADS];
	source_view->process_threads(threads);
    }

    // Process backtrace
    if (entry.has(UB_WHERE))
    {
	string where = entry[UB_WHERE];
	source_view->process_where(where);
    }

    // Process registers
    if (entry.has(UB_REGISTERS))
    {
	string registers = entry[UB_REGISTERS];
	source_view->process_registers(registers);
    }

    history_position += direction;

    locked = false;

    log();
}


// True iff we're at some past execution position
bool UndoBuffer::at_past_exec_pos()
{
    assert(OK());

    for (int i = history_position; i < history.size(); i++)
	if (history[i].exec_pos)
	    return true;	// later exec pos

    return false;
}

// Goto last known execution position
void UndoBuffer::goto_current_exec_pos()
{
    assert(OK());

    if (!at_past_exec_pos())
	return;

    for (int i = history.size(); i >= 0; i--)
    {
	if (history[i].exec_pos)
	{
	    history_position = i + 1;
	    process(history[i], 1);
	    log();
	    return;
	}
    }
}

bool UndoBuffer::undo()
{
    if (history_position > 1 && history.size() > 0)
    {
	process(history[history_position - 2], -1);
	return true;
    }

    return false;
}

bool UndoBuffer::redo()
{
    if (history_position < history.size())
    {
        process(history[history_position], 1);
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

    log();
    assert(OK());
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

    log();
    assert(OK());
}

// Invariant
bool UndoBuffer::OK()
{
    // HISTORY_POSITION must be within bounds.
    assert(history.size() == 0 || history_position > 0);
    assert(history_position <= history.size());

    // All non-exec positions must follow exec positions.
    bool non_exec_found = false;
    bool exec_found = true;
    for (int i = 0; i < history.size(); i++)
    {
	if (history[i].command)
	{
	    assert(!history[i].exec_pos);
	}
	else if (history[i].exec_pos)
	{
	    assert(!non_exec_found);
	    exec_found = true;
	}
	else
	{
	    non_exec_found = true;
	}
    }

    return true;
}
