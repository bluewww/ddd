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

    // General scheme:
    //
    // History
    // -------
    //
    // `history' contains:
    //
    // 0                       \ 
    // 1 ...                    > entries (positions or commands) to be undone
    // (history_position - 1)  /
    // (history_position)      \ 
    // ...                      > entries (positions or commands) to be redone
    // (history.size() - 1)    /
    //
    //
    // Entries
    // -------
    //
    // Each entry is a list of key/value pairs.
    //
    // Basically, there are four kinds of entries:
    // * COMMANDS with the command name in the COMMAND key.
    //   The command is to be executed when reached.
    // * EXEC COMMANDS with the command name in the EXEC_COMMAND key.
    //   Like commands, but are dependent on the program state and
    //   thus get deleted when changing the program state.
    // * EXEC POSITIONS with the position in the EXEC_POS key.
    //   Other fields contain the current state, such as display values.
    // * POSITIONS with the position in the POS key.  Used in lookups.
    //
    // Upon undo, DDD re-executes the command at HISTORY_POSITION - 1
    // and attempts to re-create the state at HISTORY_POSITION - 2.
    // Redo works the other way around.
    //
    //
    // Sources
    // -------
    //
    // DDD starts a new entry each time the command source is set via
    // SET_SOURCE.

    // True if we have undone some exec position
    static bool _showing_earlier_state;

    // Current command source
    static string current_source;

    // True if position history is to stay unchanged
    static bool locked;

    // Helpers
    static void process_command(UndoBufferEntry& entry);
    static void process_state(UndoBufferEntry& entry, bool restore_state);

    // True if we're undoing
    static bool undoing;

    // Rename all breakpoints from OLD_BP_NR to NEW_BP_NR
    static void remap_breakpoint(int old_bp_nr, int new_bp_nr);
    static void remap_breakpoint(string& cmd, int old_bp_nr, int new_bp_nr);

    // Enter or leave `past exec' mode
    static void showing_earlier_state(bool set, StatusMsg *msg = 0);

protected:
    // Add new entry
    static void add(const UndoBufferEntry& entry);

    // Process entry
    static void process_command(int entry);
    static void process_state(int entry, bool restore_state);

    // Log current position
    static void log();

    // Call when all is done.  If MSG is set, leave the outcome there.
    static void done(StatusMsg *msg = 0);

    // Get a short action description from COMMAND
    static string action(const string& command);

    // True if ENTRY has any effect
    static bool has_effect(const UndoBufferEntry& entry);

public:
    // Set source command.
    static void set_source(const string& command);

    // Adding entries

    // If SET_SOURCE was called before any of these add_SOMETHING
    // calls, the call creates a new entry in the history.  Otherwise,
    // the values are added to the last entry, possibly overriding
    // existing values.

    // Add command COMMAND to history.  If EXEC is set, COMMAND is
    // treated as execution command, i.e. dependent on the current
    // program state.
    static void add_command(const string &command, bool exec = false);

    // Add status NAME/VALUE to history.
    static void add_status(const string& name, const string& value);

    // Custom calls
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

    // Invariant
    static bool OK();
};

extern UndoBuffer undo_buffer;

#endif // _DDD_UndoBuffer_h
// DON'T ADD ANYTHING BEHIND THIS #endif
