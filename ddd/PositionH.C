// $Id$ -*- C++ -*-
// Position history (`Back' and `Forward' buttons)

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

char PositionHistory_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PositionH.h"

#include "SourceView.h"
#include "DataDisp.h"
#include "cook.h"
#include "ddd.h"
#include "string-fun.h"

#ifndef LOG_POSITION_HISTORY
#define LOG_POSITION_HISTORY 0
#endif


PositionHistory position_history;

PositionHistoryArray PositionHistory::history;

// Last position in history + 1
int PositionHistory::history_position = 1;

bool PositionHistory::locked = false;


//-----------------------------------------------------------------------
// Position history
//-----------------------------------------------------------------------

void PositionHistory::add(const PositionHistoryEntry& entry)
{
    if (history_position < history.size())
    {
	history[history_position++] = entry;
    }
    else
    {
	history += entry;
	history_position = history.size();
    }

    PositionHistoryArray new_history;
    for (int i = 0; i < history_position; i++)
	new_history += history[i];
    history = new_history;
}

// Add position to history
void PositionHistory::add_position(const string& source_name, int line)
{
    if (locked)
	return;

    PositionHistoryEntry entry;
    entry.file = source_name;
    entry.line = line;

    PositionHistoryEntry current_entry;
    if (history.size() > 0 && history_position > 0)
    {
	current_entry = history[history_position - 1];
	current_entry.address = "";
    }

    if (entry != current_entry)
	add(entry);

    log();
}

// Add address to history
void PositionHistory::add_address(const string& address)
{
    if (locked)
	return;

    PositionHistoryEntry new_entry;

    if (history.size() > 0)
    {
	PositionHistoryEntry& current_entry = history[history_position - 1];
	if (current_entry.address == "")
	{
	    // Append address to current position
	    current_entry.address = address;
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
void PositionHistory::add_displays(const string& displays)
{
    if (locked)
	return;

    PositionHistoryEntry new_entry;

    if (history.size() > 0)
    {
	PositionHistoryEntry& current_entry = history[history_position - 1];
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
void PositionHistory::add_display(const string& display)
{
    if (locked)
	return;

    PositionHistoryEntry new_entry;

    if (history.size() > 0)
    {
	PositionHistoryEntry& current_entry = history[history_position - 1];
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


void PositionHistory::log()
{
#if LOG_POSITION_HISTORY
    clog << "Position history:\n";
    for (int i = 0; i < history.size(); i++)
    {
	const PositionHistoryEntry& entry = history[i];

	clog << i << (i == history_position - 1 ? "*\t" : " \t");
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

void PositionHistory::goto_entry(const PositionHistoryEntry& entry)
{
    locked = true;

    // Lookup position in source
    source_view->goto_entry(entry.file, entry.line, entry.address);

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

bool PositionHistory::go_back()
{
    if (history_position > 1 && history.size() > 0)
    {
	const PositionHistoryEntry& entry = history[--history_position - 1];
	goto_entry(entry);
	return true;
    }

    return false;
}

bool PositionHistory::go_forward()
{
    if (history_position < history.size())
    {
	const PositionHistoryEntry& entry = history[history_position++];
        goto_entry(entry);
	return true;
    }

    return false;
}

// Clear history
void PositionHistory::clear()
{
    static PositionHistoryArray empty;
    history          = empty;
    history_position = 1;
    locked           = false;

    log();
}
