// $Id$ 
// Unique identifiers

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration environment.
// For details, see the ICE World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ice/',
// or send a mail to the ICE developers <ice@ips.cs.tu-bs.de>.

#ifndef _ICE_UniqueId_h
#define _ICE_UniqueId_h

#ifdef __GNUG__
#pragma interface
#endif


// UniqueId manages unique identifiers

#include <iostream.h>
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
