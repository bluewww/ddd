// $Id$
// Klasse FontTable (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_FontTable_h
#define _DDD_FontTable_h

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
