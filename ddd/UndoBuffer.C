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

// Add status NAME/VALUE to history
void UndoBuffer::add_status(const string& name, const string& value, 
			    bool exec_pos)
{
    if (locked)
	return;

    UndoBufferEntry new_entry;
    new_entry.exec_pos = exec_pos;

    if (history.size() > 0)
    {
	UndoBufferEntry& current_entry = history[history_position - 1];
	if (!current_entry.status.has(name))
	{
	    // Add NAME/VALUE to current position
	    current_entry.status[name] = value;
	    if (exec_pos)
		current_entry.exec_pos = true;
	}
	else if (current_entry.status[name] != value)
	{
	    // VALUE has changed - use new entry instead
	    new_entry.status[name] = value;
	}
    }
    else
    {
	// No history yet: create new entry
	new_entry.status[name] = value;
    }

    if (new_entry.status.has(name))
	add(new_entry);

    log();
}


void UndoBuffer::log()
{
#if LOG_UNDO_BUFFER
    clog << "Undo buffer:\n";
    for (int i = 0; i < history.size(); i++)
    {
	// Only log the first 2 items around the current position
	if (abs(i - (history_position - 1)) > 2)
	    continue;

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
	for (StringStringAssocIter iter(entry.status); iter.ok(); iter++)
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

void UndoBuffer::goto_entry(const UndoBufferEntry& entry)
{
    locked = true;

    // Lookup position in source
    string pos = "";
    string address = "";
    if (entry.status.has(UB_POS))
	pos = entry.status[UB_POS];
    if (entry.status.has(UB_ADDRESS))
	address = entry.status[UB_ADDRESS];

    if (pos != "" || address != "")
    {
	string file = pos.before(':');
	string line = pos.after(':');
	source_view->goto_entry(file, atoi(line.chars()), 
				address, entry.exec_pos);
    }

    // Re-process displays
    StringArray displays;
    StringArray values;
    for (StringStringAssocIter iter(entry.status); iter.ok(); iter++)
    {
	if (iter.key().contains(UB_DISPLAY_PREFIX, 0))
	{
	    displays += iter.key().after(UB_DISPLAY_PREFIX);
	    values += iter.value();
	}
    }
    data_disp->update_displays(displays, values);

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
