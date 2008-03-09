// $Id$ 
// Unique identifiers

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_UniqueId_h
#define _DDD_UniqueId_h

// UniqueId manages unique identifiers

#include <iostream>
#include "bool.h"

class UniqueId {
private:
    unsigned long _id;              // Current Id
    static unsigned long tics;      // Least recently used Id
    static unsigned long ids;       // Counter existing Id's
public:

    static bool debug;

#ifdef NDEBUG
    // Optimized variant

    // Create UniqueId
    UniqueId(): _id(tics++) {}

    // Destroy UniqueId
    ~UniqueId() {}
#else
    // Debugging variant

    // Create UniqueId
    UniqueId();

    // Destroy UniqueId
    ~UniqueId();
#endif

    // Return value
    operator unsigned long() const { return _id; }

    // Statistics
    static unsigned long inUse()   { return ids; }     // # of existing Ids
};

#endif
