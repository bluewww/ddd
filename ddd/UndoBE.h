// $Id$ -*- C++ -*-
// Undo Buffer Entry

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

#ifndef _DDD_UndoBufferEntry_h
#define _DDD_UndoBufferEntry_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringSA.h"
#include "VarArray.h"
#include "bool.h"

// Special value keys
#define UB_POS           "pos"	        // Current source position
#define UB_ADDRESS       "address"      // Current program counter
#define UB_EXEC_POS      "exec_pos"	// Current execution position
#define UB_EXEC_ADDRESS  "exec_address" // Current execution program counter
#define UB_WHERE         "where"        // Current backtrace
#define UB_FRAME         "frame"        // Current frame
#define UB_REGISTERS     "registers"    // Current register values
#define UB_THREADS       "threads"      // Current threads
#define UB_COMMAND       "command"      // Command restoring state (undoing)
#define UB_EXEC_COMMAND  "exec_command" // Same, but requires current state
#define UB_SOURCE        "source"       // Command causing the undo

// Prefix for current displays; followed by display name
#define UB_DISPLAY_PREFIX         "display "  // Display value
#define UB_DISPLAY_ADDRESS_PREFIX "&display " // Display address

class UndoBufferEntry: public StringStringAssoc {
public:
    UndoBufferEntry()
	: StringStringAssoc()
    {}

    UndoBufferEntry(const UndoBufferEntry& entry)
	: StringStringAssoc(entry)
    {}

    UndoBufferEntry& operator = (const UndoBufferEntry& entry)
    {
	StringStringAssoc::operator = (entry);
	return *this;
    }

    bool operator == (const UndoBufferEntry& entry) const;

    bool operator != (const UndoBufferEntry& entry) const
    {
	return !operator == (entry);
    }

    bool has_exec_pos() const
    {
	return has(UB_EXEC_POS) || has(UB_EXEC_ADDRESS);
    }

    bool has_pos() const
    {
	return has(UB_POS) || has(UB_ADDRESS);
    }

    bool has_command() const
    {
	return has(UB_COMMAND) || has(UB_EXEC_COMMAND);
    }
};

typedef VarArray<UndoBufferEntry> UndoBufferArray;

#endif // _DDD_UndoBufferEntry_h
// DON'T ADD ANYTHING BEHIND THIS #endif
