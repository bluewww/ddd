// $Id$ -*- C++ -*-
// DDD command history

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_history_h
#define _DDD_history_h

#include "strclass.h"
#include "bool.h"
#include "StringA.h"
#include "HistoryF.h"

#include <X11/Intrinsic.h>


// History management
extern void process_history_filename(string answer);
extern void process_history_size(string answer);
extern void add_to_history(const string& line);
extern void set_history_from_line(const string& line, 
				  bool ignore_history_commands = false);

// Set and get GDB history file
extern void set_gdb_history_file(const string& file);
extern string gdb_history_file();

// Load history from FILE
extern void load_history(const string& file);

// Save history into FILE
extern void save_history(const string& file, Widget origin = 0);

// Last command from history
extern string last_command_from_history();

// History actions
extern void prev_historyAct      (Widget, XEvent*, String*, Cardinal*);
extern void next_historyAct      (Widget, XEvent*, String*, Cardinal*);

// Search TEXT in history; return POS iff found, -1 if none
// DIRECTION = -1 means search backward, DIRECTION = +1 means search forward
// RESEARCH = true means skip current position
extern int search_history(const string& text, int direction,
			  bool research = false);

// Set history position to POS; -1 means last position.
extern void goto_history(int pos = -1);

// Invoke history dialog
extern void gdbHistoryCB(Widget, XtPointer, XtPointer);

// Tie a ComboBox to global history
extern void tie_combo_box_to_history(Widget text, HistoryFilter filter);

// Add FILE to recent file history
extern void add_to_recent(const string& file);

// Get recent file history (most recent first)
extern void get_recent(StringArray& arr);

// Tie a menu to recent files
class MMDesc;
extern void tie_menu_to_recent_files(MMDesc *items);

#endif // _DDD_history_h
// DON'T ADD ANYTHING BEHIND THIS #endif
