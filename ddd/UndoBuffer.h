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

#ifndef _DDD_UndoBuffer_h
#define _DDD_UndoBuffer_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "UndoBE.h"
#include "string-fun.h"		// itostring()
#include "IntArray.h"

class BreakPoint;
class StatusMsg;

class UndoBuffer {

private:
    // The history itself
    static UndoBufferArray history;

    // Last position in history + 1
    static int history_position;

    // Currently processed entry
    static int current_entry;

    // If true, next `add' creates new entry
    static bool force_new_entry;

    // A collector for state values
    static UndoBufferEntry collector;

    // True if we have undone some exec position
    static bool _showing_earlier_state;

    // Current command source
    static string current_source;

    // True if position history is to stay unchanged
    static bool locked;

    // Helpers
    static bool process_command(UndoBufferEntry& entry);
    static bool process_state(UndoBufferEntry& entry);
    static bool process_frame(UndoBufferEntry& entry);
    static bool process_pos(UndoBufferEntry& entry);

    // True if we're undoing
    static bool undoing;

    // Rename all breakpoints from OLD_BP_NR to NEW_BP_NR
    static void remap_breakpoint(int old_bp_nr, int new_bp_nr);
    static void remap_breakpoint(string& cmd, int old_bp_nr, int new_bp_nr);

    // Enter or leave `past exec' mode
    static void showing_earlier_state(bool set, StatusMsg *msg = 0);

    // Remove all later entries, except for exec positions
    static void clear_after_position();

    // Remove all exec commands
    static void clear_exec_commands();

    // Remove all entries with no effect
    static void cleanup();


protected:
    // Add new entry
    static void add_entry(const UndoBufferEntry& entry);

    // Process entry
    static bool process_command(int entry);
    static bool process_state(int entry);
    static bool process_frame(int entry);
    static bool process_pos(int entry);

    // Log current position
    static void log();

    // Call when all is done.  If MSG is set, leave the outcome there.
    static void done(StatusMsg *msg = 0);

    // Get a short action description from COMMAND
    static string action(const string& command);

    // True if ENTRY has any effect
    static bool has_effect(const UndoBufferEntry& entry);

public:
    // Maximum depth (-1: unlimited)
    static int max_history_depth;

    // Set source command.
    static void set_source(const string& command);

    // Adding entries

    // If SET_SOURCE was called before any of these add_SOMETHING
    // calls, the call creates a new entry in the history.  Otherwise,
    // the values are added to the last entry, possibly overriding
    // existing values.

    // Add status NAME/VALUE to history.
    static void add_status(const string& name, const string& value);

    // Add command COMMAND to history.  If EXEC is set, COMMAND is
    // treated as execution command, i.e. dependent on the current
    // program state.
    static void add_command(const string &command, bool exec = false);

    // Custom calls
    static void add_state()
    {
	add_status(UB_STATE, "");
    }

    static void add_position(const string& file_name, int line, bool exec)
    {
	add_status(exec ? UB_EXEC_POS : UB_POS,
		   file_name + ":" + itostring(line));
    }

    static void add_address(const string& address, bool exec)
    {
	add_status(exec ? UB_EXEC_ADDRESS : UB_ADDRESS, address);
    }

    static void add_where(const string& where)
    {
	add_status(UB_WHERE, where);
    }

    static void add_frame(const string& frame)
    {
	add_status(UB_FRAME, frame);
    }

    static void add_registers(const string& registers)
    {
	add_status(UB_REGISTERS, registers);
    }

    static void add_threads(const string& threads)
    {
	add_status(UB_THREADS, threads);
    }

    static void add_display(const string& name, const string& value)
    {
	add_status(UB_DISPLAY_PREFIX + name, value);
    }

    static void add_display_address(const string& name, const string& addr)
    {
	add_status(UB_DISPLAY_ADDRESS_PREFIX + name, addr);
    }

    // Removals

    // Remove status NAME from current history entry.
    static void remove_status(const string& name);

    static void remove_position()
    {
	remove_status(UB_EXEC_POS);
	remove_status(UB_POS);
    }

    static void remove_address()
    {
	remove_status(UB_EXEC_ADDRESS);
	remove_status(UB_ADDRESS);
    }

    static void remove_where()
    {
	remove_status(UB_WHERE);
    }

    static void remove_frame()
    {
	remove_status(UB_FRAME);
    }

    static void remove_registers()
    {
	remove_status(UB_REGISTERS);
    }

    static void remove_threads()
    {
	remove_status(UB_THREADS);
    }

    static void remove_display(const string& name)
    {
	remove_status(UB_DISPLAY_PREFIX + name);
	remove_status(UB_DISPLAY_ADDRESS_PREFIX + name);
    }

    // Undo/Redo action
    static void undo();
    static void redo();

    // Undo/Redo action descriptions
    static string undo_action();
    static string redo_action();

    // True iff we're showing an earlier program state
    static bool showing_earlier_state() { return _showing_earlier_state; }

    // Restore current program state
    static void restore_current_state();

    // Clear history
    static void clear();

    // Clear all execution positions
    static void clear_exec_pos();

    // Add breakpoint state to OS
    static void add_breakpoint_state(ostream& os, BreakPoint *bp);

    // Return history of display NAME
    static string display_history(const string& name);

    // Invariant
    static bool OK();
};

extern UndoBuffer undo_buffer;

#endif // _DDD_UndoBuffer_h
// DON'T ADD ANYTHING BEHIND THIS #endif
