// $Id$
// Klasse FontTable (Implementation)

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char FontTable_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "hash.h"
#include "strclass.h"

#include <iostream.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "FontTable.h"

DEFINE_TYPE_INFO_0(FontTable)

// FontTable

// Hash-Code erzeugen
inline unsigned hash(const char *name)
{
    return hashpjw(name) % MAX_FONTS;
}

// XFontStruct zu Fontnamen zurueckgeben
XFontStruct *FontTable::operator[](string& name)
{
    int i = hash(name);
    while (table[i].font != 0 && name != table[i].name)
    {
	assert (i < MAX_FONTS);   // Ueberlauf: Zuviele Fonts
	i = (i >= MAX_FONTS) ? 0 : i + 1;
    }

    if (table[i].font == 0 && name != table[i].name)
    {
	// Neuen Font eintragen
	table[i].name = name;
	table[i].font = XLoadQueryFont(_display, name);

	if (table[i].font == 0)
	    cerr << "Warning: could not load font \"" << name << "\"\n";
    }

    return table[i].font;
}
