// $Id$ 
// Klasse "UniqueId" (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

#ifndef _ICE_UniqueId_h
#define _ICE_UniqueId_h

#ifdef __GNUG__
#pragma interface
#endif


// UniqueId definiert eine Klasse zum Verwalten von eindeutigen Id's.

#include <iostream.h>
#include "bool.h"

class UniqueId {
private:
    unsigned long _id;              // Aktuelle Id
    static unsigned long tics;      // Letzte vergebende Id
    static unsigned long ids;       // Zaehler existierende Id's
public:

    static bool debug;

#ifdef NDEBUG
    // Optimierte Version
    // UniqueId anlegen
    UniqueId(): _id(tics++) {}

    // UniqueId zerstoeren
    ~UniqueId() {}
#else
    // Debugging-Version
    // UniqueId anlegen
    UniqueId();

    // UniqueId zerstoeren
    ~UniqueId();
#endif

    // Wert zurueckgeben
    operator unsigned long() const { return _id; }

    // Statistik
    static unsigned long inUse()   { return ids; }     // #Existierende Id's
};

#endif
