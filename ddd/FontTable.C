// $Id$
// Klasse FontTable (Implementation)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

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

// Return hash code
inline unsigned hash(const char *name)
{
    return hashpjw(name) % MAX_FONTS;
}

// Return XFontStruct for given font name NAME
XFontStruct *FontTable::operator[](string& name)
{
    int i = hash(name);
    while (table[i].font != 0 && name != table[i].name)
    {
	assert (i < MAX_FONTS);   // Too many fonts
	i = (i >= MAX_FONTS) ? 0 : i + 1;
    }

    if (table[i].font == 0 && name != table[i].name)
    {
	// Insert new font
	table[i].name = name;
	table[i].font = XLoadQueryFont(_display, name);

	if (table[i].font == 0)
	{
	    cerr << "Warning: Could not load font \"" << name << "\"";

	    // Try default font
	    GC default_gc = 
		DefaultGCOfScreen(DefaultScreenOfDisplay(_display));
	    XGCValues gc_values;
	    if (XGetGCValues(_display, default_gc, GCFont, &gc_values))
	    {
		const Font& font_id = gc_values.font;
		XFontStruct *font = XQueryFont(_display, font_id);
		if (font != 0)
		{
		    cerr << ", using default font instead\n";
		    table[i].font = font;
		}
	    }
	}

	if (table[i].font == 0)
	{
	    // Try "fixed" font
	    XFontStruct *font = XLoadQueryFont(_display, "fixed");
	    if (font != 0)
	    {
		cerr << ", using font \"fixed\" instead\n";
		table[i].font = font;
	    }
	}

	if (table[i].font == 0)
	    cerr << "\n";
    }

    return table[i].font;
}
