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

#ifndef LOG_UNDO_BUFFER
#define LOG_UNDO_BUFFER 0
#endif

UndoBuffer undo_buffer;

UndoBufferArray UndoBuffer::history;

// Last position in history + 1
int UndoBuffer::history_position = 1;

bool UndoBuffer::locked = false;


//-----------------------------------------------------------------------
// Position history
//-----------------------------------------------------------------------

void UndoBuffer::add(const UndoBufferEntry& entry)
{
    if (entry.exec_pos)
    {
	// Adding a new execution position - remove all non-exec
	// positions from history
	UndoBufferArray new_history;
	for (int i = 0; i < history.size(); i++)
	{
	    if (history[i].exec_pos)
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
	    // Adding a non-exec position - remove all later non-exec
	    // entries in history
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
}

// Add position to history
void UndoBuffer::add_position(const string& source_name, int line,
				   bool exec_pos)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;
    new_entry.file     = source_name;
    new_entry.line     = line;
    new_entry.exec_pos = exec_pos;

    UndoBufferEntry current_entry;
    if (history.size() > 0 && history_position > 0)
    {
	current_entry = history[history_position - 1];
	current_entry.address = "";
	if (exec_pos)
	    current_entry.exec_pos = true;
    }

    if (new_entry != current_entry)
	add(new_entry);

    log();
}

// Add address to history
void UndoBuffer::add_address(const string& address, bool exec_pos)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;
    new_entry.exec_pos = exec_pos;

    if (history.size() > 0)
    {
	UndoBufferEntry& current_entry = history[history_position - 1];
	if (current_entry.address == "")
	{
	    // Append address to current position
	    current_entry.address  = address;
	    if (exec_pos)
		current_entry.exec_pos = true;
	}
	else if (current_entry.address != address)
	{
	    // Create new entry
	    new_entry = current_entry;
	    new_entry.address = address;
	}
    }
    else
    {
	// No source position yet: add address
	new_entry.address = address;
    }

    if (new_entry.address != "")
	add(new_entry);

    log();
}


// Add displays to history
void UndoBuffer::add_displays(const string& displays)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;

    if (history.size() > 0)
    {
	UndoBufferEntry& current_entry = history[history_position - 1];
	if (current_entry.displays == NO_GDB_ANSWER)
	{
	    // Append displays to current position
	    current_entry.displays = displays;
	}
	else if (current_entry.displays != displays)
	{
	    // Create new entry
	    new_entry = current_entry;
	    new_entry.displays = displays;
	}
    }
    else
    {
	// No position yet: add displays
	new_entry.displays = displays;
    }

    if (new_entry.displays != NO_GDB_ANSWER)
	add(new_entry);

    log();
}


// Add single display to history
void UndoBuffer::add_display(const string& display)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;

    if (history.size() > 0)
    {
	UndoBufferEntry& current_entry = history[history_position - 1];
	if (current_entry.displays == NO_GDB_ANSWER)
	{
	    // Append display to current position
	    current_entry.displays = display;
	}
	else if (!current_entry.displays.contains(display))
	{
	    // Create new entry, adding the new display
	    new_entry = current_entry;
	    new_entry.displays += display;
	}
    }
    else
    {
	// No position yet: add displays
	new_entry.displays = display;
    }

    if (new_entry.displays != NO_GDB_ANSWER)
	add(new_entry);

    log();
}


void UndoBuffer::log()
{
#if LOG_UNDO_BUFFER
    clog << "Undo buffer:\n";
    for (int i = 0; i < history.size(); i++)
    {
	const UndoBufferEntry& entry = history[i];

	if (i == history_position - 1)
	    clog << ">";
	else
	    clog << " ";
	clog << i;
	if (entry.exec_pos)
	    clog << "*";
	clog << '\t';

	if (entry.file != "")
	    clog << entry.file << ":" << entry.line;
	else
	    clog << ":";

       	clog << ":" << entry.address;

 	if (entry.displays != NO_GDB_ANSWER)
	    clog << ":" << quote(entry.displays);

	clog << "\n";
    }
    clog << "\n";
#endif
}

void UndoBuffer::goto_entry(const UndoBufferEntry& entry)
{
    locked = true;

    // Lookup position in source
    source_view->goto_entry(entry.file, entry.line, entry.address, 
			    entry.exec_pos);

    // Re-process displays
    if (entry.displays != NO_GDB_ANSWER)
    {
	string displays = entry.displays;
	bool disabling_occurred = false;
	data_disp->process_displays(displays, disabling_occurred);
    }

    locked = false;

    log();
}


// True iff we're at the last known execution position
bool UndoBuffer::at_last_exec_pos()
{
    for (int i = history_position; i < history.size(); i++)
	if (history[i].exec_pos)
	    return false;	// later exec pos

    return true;
}


bool UndoBuffer::undo()
{
    if (history_position > 1 && history.size() > 0)
    {
	const UndoBufferEntry& entry = history[--history_position - 1];
	goto_entry(entry);
	return true;
    }

    return false;
}

bool UndoBuffer::redo()
{
    if (history_position < history.size())
    {
	const UndoBufferEntry& entry = history[history_position++];
        goto_entry(entry);
	return true;
    }

    return false;
}

// Clear history
void UndoBuffer::clear()
{
    static UndoBufferArray empty;
    history          = empty;
    history_position = 1;
    locked           = false;

    log();
}
