// $Id$ 
// StringBox class

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

char StringBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "StringBox.h"
#include "printBox.h"

#include "strclass.h"
#include "assert.h"
#include "cook.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <ctype.h>
#include <strstream.h>
#include <iomanip.h>

#include "VSEFlags.h"

// set this to 1 if you want to have the box a font-specific height
#define USE_MAX_BOUNDS 1


DEFINE_TYPE_INFO_1(StringBox, PrimitiveBox)

// StringBox

FontTable *StringBox::fontTable = 0;
bool StringBox::quoted = false;

// Groesse neu berechnen
Box *StringBox::resize()
{
    if (_font != 0)
    {
	int direction, font_ascent, font_descent;
	XCharStruct overall;

	XTextExtents(_font, (char *)_string, _string.length(),
	    &direction, &font_ascent, &font_descent, &overall);

#if USE_MAX_BOUNDS
	XCharStruct max_bounds = _font->max_bounds;

	_ascent = max_bounds.ascent;
	thesize() = BoxSize(overall.width, 
			    max_bounds.ascent + max_bounds.descent);
#else
	_ascent = font_ascent;
	thesize() = BoxSize(overall.width, font_ascent + font_descent);
#endif
    }

    return this;
}

// StringBox anzeigen
void StringBox::_draw(Widget w, 
		      const BoxRegion& r, 
		      const BoxRegion&, 
		      GC gc,
		      bool) const
{
    BoxPoint origin = r.origin();

    if (_font != 0)
	XSetFont(XtDisplay(w), gc, _font->fid);

    XDrawString(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y] + _ascent,
	(char *)_string, _string.length());
}


void StringBox::dump(ostream& s) const
{
    char *quote = "\"";
    if (StringBox::quoted)
	quote = "\\\"";

    s << quote;
    for (unsigned i = 0; i < _string.length(); i++)
    {
	if (_string[i] == '\"')
	    s << quote;
	else
	    s << _string[i];
    }
    s << quote;

    if (VSEFlags::include_font_info)
	s << " (font: \"" << _fontname << "\")";
}

void StringBox::newFont(const string& fontname)
{
    _fontname = fontname;
    if (fontTable != 0)
	_newFont((*fontTable)[_fontname]);
}

// StringBox drucken
typedef struct fontmap {
	char *xfont ;
	char *psfont ;
	int figfont ;
} FONTMAP ;

// mapping between X11 fonts, PostScript fonts and xfig font numbers
static FONTMAP map[] = {
{"fixed",                                "/Courier",                     12},
{"-*-times-medium-r-*-",                 "/Times-Roman",                  0},
{"-*-times-medium-*-*-",                 "/Times-Italic",                 1},
{"-*-times-bold-r-*-",                   "/Times-Bold",                   2},
{"-*-times-bold-*-*-",                   "/Times-BoldItalic",             3},
{"-*-courier-medium-r-*-",               "/Courier",                     12},
{"-*-courier-medium-*-*-",               "/Courier-Oblique",             13},
{"-*-courier-bold-r-*-",                 "/Courier-Bold",                14},
{"-*-courier-bold-*-*-",                 "/Courier-BoldOblique",         15},
{"-*-helvetica-medium-r-*-",             "/Helvetica-Narrow",            16},
{"-*-helvetica-medium-*-*-",             "/Helvetica-NarrowOblique",     17},
{"-*-helvetica-bold-r-*-",               "/Helvetica",                   18},
{"-*-helvetica-bold-*-*-",               "/Helvetica-Oblique",           19},
{"-*-lucidatypewriter-medium-r-*-",      "/Courier",                     12},
{"-*-lucidatypewriter-medium-*-*-",      "/Courier-Oblique",             13},
{"-*-lucidatypewriter-bold-r-*-",        "/Courier-Bold",                14},
{"-*-lucidatypewriter-bold-*-*-",        "/Courier-BoldOblique",         15},
{"-*-new century schoolbook-bold-*-",    "/NewCenturySchlbk-BoldItalic", 27},
{"-*-new century schoolbook-bold-r-*",   "/NewCenturySchlbk-Bold",       26},
{"-*-new century schoolbook-medium-*-*", "/NewCenturySchlbk-Italic",     25},
{"-*-new century schoolbook-medium-r-*", "/NewCenturySchlbk-Roman",      24},
{"-*-symbol-medium-",                    "/Symbol",                      32},
};

static int mappings = sizeof (map) / sizeof (FONTMAP) ;

/*
 * matchFont
 */

static FONTMAP *matchFont(char *xfont) 
{
    FONTMAP *fmap = &map[0] ;
    int match = 1;
    int i = 0;
    
    while (i < mappings && match != 0) {
	match = strncmp (map[i].xfont, xfont, strlen(map[i].xfont)) ;
	i++ ;
    } 
    if (match == 0) {
	fmap = &map[--i] ;
    }
    return fmap;
}


void StringBox::_print(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc) const
{
    // Don't draw empty strings
    if (str() == "")
	return;

    BoxPoint origin = region.origin() ;
    FONTMAP *fmap = matchFont (fontName());

    if (gc.isFig()) {
	os << TEXTHEAD1 << fmap->figfont << " "
	   << size(Y) - 3 << " " << TEXTHEAD2
	   << size(X) << " " << size(Y) << " "
	   << origin[X] << " " << origin [Y] + size(Y) - 2 << " "
	   << str() << "\001\n";
    } else if (gc.isPostScript()) {
	os << fmap->psfont << " " << size(X) << " " << size(Y)
	   << " " << origin[X] << " " << origin[Y] + size(Y) << " "
	   << "(" << pscook(str()) << ") text*\n";
    }
}   
