// $Id$ 
// StringBox class

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

// $Log$
// Revision 1.1  1995/05/01 15:47:51  zeller
// Initial revision
//
// Revision 10.26  1995/03/31  16:21:33  zeller
// Fix: xfig format was broken
//
// Revision 10.25  1995/03/27  16:03:22  zeller
// New: map lucidatypewriter to courier
//
// Revision 10.24  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.23  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.22  1994/07/19  21:16:36  zeller
// New: printing functions const-corrected
//
// Revision 10.21  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.20  1994/02/16  13:06:59  zeller
// Fix: Leere Strings machen PostScript Probleme
//
// Revision 10.19  1993/12/23  12:29:40  zeller
// Fix: FIG-Format: Hoehe und Breite von Texten angegeben
//
// Revision 10.18  1993/10/02  13:26:55  zeller
// Test: Helvetica-Schriften enger gemacht
//
// Revision 10.17  1993/10/01  14:37:38  zeller
// Neu: Symbol-Fonts
//
// Revision 10.16  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.15  1993/05/22  20:15:46  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.14  1993/04/26  09:58:43  zeller
// Fix: pscook() statt slashify() [slashify() konnte ( und ) nicht ausgeben]
// 
// Revision 10.13  1993/04/22  10:52:01  zeller
// Lizenz verbessert
// 
// Revision 10.12  1993/04/21  14:39:22  zeller
// Fix: Anpassung an iostream.h statt stream.h
// 
// Revision 10.11  1993/04/21  00:11:08  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.10  1993/04/16  11:31:57  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.9  1993/04/14  18:12:35  zeller
// Neu: Maximale Font-Hoehe als Standard-Hoehe fuer Boxen genommen
// 
// Revision 10.8  1993/03/01  09:58:35  zeller
// XFIG: Fonts verkleinert
// 
// Revision 10.7  1993/02/23  16:17:51  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.6  1992/10/13  20:49:08  zeller
// strclass.h statt String.h
// 
// Revision 10.5  1992/06/02  03:04:26  zeller
// #pragma besser formatiert
// 
// Revision 10.4  1992/06/01  16:34:36  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.3  1992/06/01  14:13:48  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/02/18  10:57:07  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:41  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:49:11  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:22  zeller
// Installierte Version
// 
// Revision 1.15  1991/07/02  14:43:53  zeller
// Quoting verbessert
// 
// Revision 1.14  1991/07/02  12:55:54  zeller
// Neu: Flag `quoted'
// 
// Revision 1.13  1991/06/09  17:37:42  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.12  1991/05/16  15:49:53  zeller
// Warnungen vermieden
// 
// Revision 1.11  1991/05/09  15:05:35  zeller
// Warnungen vermieden
// 
// Revision 1.10  1991/04/27  16:30:45  zeller
// newFont(String&) aus StringBox.h eingebunden
// 
// Revision 1.9  1991/02/10  12:24:41  zeller
// VSEFlags eingefuehrt
// 
// Revision 1.8  1991/02/08  16:29:34  zeller
// font_debug eingefuehrt;
// dump() neu definiert
// 
// Revision 1.7  1991/01/28  21:10:39  zeller
// Neu: Unterstuetzung FontTable
// 
// Revision 1.6  1991/01/27  15:13:45  zeller
// Unterstuetzung wechselnde Fonts eingefuehrt;
// Funktionen neu gegliedert.
// 
// Revision 1.5  1991/01/27  12:49:28  zeller
// resize() eingefuehrt
// 
// Revision 1.4  1990/10/04  18:19:41  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.3  1990/09/16  13:01:22  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.2  1990/09/11  11:18:18  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.1  1990/09/07  14:32:58  zeller
// Initial revision
// 

char StringBox_rcsid[] = "$Id$";

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
		      const BoxRegion& exposed, 
		      GC gc,
		      bool context_selected) const
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
