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

#ifndef _DDD_PositionHistory_h
#define _DDD_PositionHistory_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "PositionHE.h"

class PositionHistory {

private:
    static PositionHistoryArray history;

    // Last position in history + 1
    static int history_position;

    // True if position history is to stay unchanged
    static bool locked;

protected:
    // Add new entry
    void add(const PositionHistoryEntry& entry);

    // Lookup entry in source
    static void goto_entry(const PositionHistoryEntry& entry);

    // Log current position
    static void log();

public:
    // Add position to history
    void add_position(const string& file_name, int line);
    void add_address(const string& address);
    void add_displays(const string& displays);
    void add_display(const string& display);

    // Lookup previous/next position; return true iff successful
    static bool go_back();
    static bool go_forward();

    // Clear history
    static void clear();
};

extern PositionHistory position_history;

#endif // _DDD_PositionHistory_h
// DON'T ADD ANYTHING BEHIND THIS #endif
