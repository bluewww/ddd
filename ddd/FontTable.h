// $Id$
// Klasse FontTable (Deklaration)

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

#ifndef _Nora_FontTable_h
#define _Nora_FontTable_h

#ifdef __GNUG__
#pragma interface
#endif


#include <X11/Xlib.h>
#include "strclass.h"
#include "TypeInfo.h"

#define MAX_FONTS 511 /* Max #Fonts */

struct FontTableHashEntry {
    XFontStruct *font;
    string name;
};

class FontTable {
public:
    DECLARE_TYPE_INFO

private:
    FontTableHashEntry table[MAX_FONTS];
    Display *_display;

public:
    FontTable(Display *display):
	_display(display)
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	{
	    table[i].font = 0;
	    table[i].name = "";
	}
    }
    virtual ~FontTable()
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	    if (table[i].font != 0)
		XFreeFont(_display, table[i].font);
    }
    XFontStruct *operator[](string& name);
};

#endif
