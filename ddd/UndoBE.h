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
#include "question.h"

struct UndoBufferEntry {
    StringStringAssoc status;	// Status fields
    bool exec_pos;		// True if execution position

    UndoBufferEntry()
	: status(), exec_pos(false)
    {}

    UndoBufferEntry(const UndoBufferEntry& entry)
	: status(entry.status), exec_pos(entry.exec_pos)
    {}

    UndoBufferEntry& operator = (const UndoBufferEntry& entry)
    {
	status   = entry.status;
	exec_pos = entry.exec_pos;

	return *this;
    }

    bool operator == (const UndoBufferEntry& entry) const;

    bool operator != (const UndoBufferEntry& entry) const
    {
	return !operator == (entry);
    }
};

typedef VarArray<UndoBufferEntry> UndoBufferArray;

#endif // _DDD_UndoBufferEntry_h
// DON'T ADD ANYTHING BEHIND THIS #endif
