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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_FontTable_h
#define _DDD_FontTable_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Xlib.h>
#else
#include <GUI/Widget.h>
#endif

#include "gtk_wrapper.h"

#include "strclass.h"
#include "TypeInfo.h"
#include "assert.h"

#define MAX_FONTS 511 /* Max #Fonts */

struct FontTableHashEntry {
#if defined(IF_XM)
    XFontStruct *font;
#else
    GUI::RefPtr<GUI::Font> font;
#endif
    string name;

#if defined(IF_XM)
    FontTableHashEntry(): font(0), name() {}
#else
    FontTableHashEntry(): name() {}
#endif

private:
    FontTableHashEntry(const FontTableHashEntry&);
    FontTableHashEntry& operator = (const FontTableHashEntry&);
};

class FontTable {
public:
    DECLARE_TYPE_INFO

private:
    FontTableHashEntry table[MAX_FONTS];
#if defined(IF_XM)
    Display *_display;
#else
    GUI::RefPtr<GUI::Display> _display;
#endif

    FontTable(const FontTable&);
    FontTable& operator = (const FontTable&);

public:
#if defined(IF_XM)
    FontTable(Display *display):
	_display(display)
#else
    FontTable(GUI::RefPtr<GUI::Display> display):
	_display(display)
#endif
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	{
#if defined(IF_XM)
	    table[i].font = 0;
#else
	    table[i].font.clear();
#endif
	    table[i].name = "";
	}
    }
    virtual ~FontTable()
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	    if (table[i].font != 0) {
#if defined(IF_XM)
		XFreeFont(_display, table[i].font);
#else
		table[i].font.clear();
#endif
	    }
    }
#if defined(IF_XM)
    XFontStruct *operator[](const string& name);
#else
    GUI::RefPtr<GUI::Font> operator[](const string& name);
#endif
};

#endif
