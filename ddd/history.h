// $Id$ -*- C++ -*-
// DDD command history

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_history_h
#define _DDD_history_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"

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

#endif // _DDD_history_h
// DON'T ADD ANYTHING BEHIND THIS #endif
