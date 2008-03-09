// $Id$
// Font tables

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

#ifndef _DDD_FontTable_h
#define _DDD_FontTable_h

#include <X11/Xlib.h>
#include "strclass.h"
#include "TypeInfo.h"
#include "assert.h"

#define MAX_FONTS 511 /* Max #Fonts */

struct FontTableHashEntry {
    XFontStruct *font;
    string name;

    FontTableHashEntry(): font(0), name() {}

private:
    FontTableHashEntry(const FontTableHashEntry&);
    FontTableHashEntry& operator = (const FontTableHashEntry&);
};

class FontTable {
public:
    DECLARE_TYPE_INFO

private:
    FontTableHashEntry table[MAX_FONTS];
    Display *_display;

    FontTable(const FontTable&);
    FontTable& operator = (const FontTable&);

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
    XFontStruct *operator[](const string& name);
};

#endif
