// $Id$
// Klasse DiagBox (Implementation)

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
// $Log$
// Revision 1.1  1995/05/01 15:47:38  zeller
// Initial revision
//
// Revision 10.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.12  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/12/07  11:04:55  zeller
// Fix: Anpassung ANSI-Overloading
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:14:08  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/16  11:31:31  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/06/02  03:04:08  zeller
// #pragma besser formatiert
// 
// Revision 10.5  1992/06/01  16:33:50  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.4  1992/06/01  14:13:45  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.3  1992/05/20  15:36:49  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:15  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:49  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:44:15  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:58  zeller
// Installierte Version
// 
// Revision 1.7  1991/06/09  17:37:59  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.6  1991/05/09  15:06:33  zeller
// Warnungen vermieden
// 
// Revision 1.5  1991/02/08  16:30:33  zeller
// dump() neu definiert
// 
// Revision 1.4  1990/10/04  18:19:50  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.3  1990/09/16  13:01:01  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.2  1990/09/11  11:18:37  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.1  1990/09/07  14:33:01  zeller
// Initial revision
// 

char DiagBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <strstream.h>

#include "PrimitiveB.h"
#include "StringBox.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "DiagBox.h"

DEFINE_TYPE_INFO_1(DiagBox, PrimitiveBox)

// DiagBox

// DiagBox anzeigen
void DiagBox::_draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion& exposed, GC gc,
		    bool context_selected) const
{
    const BoxSize space   = r.space();
    const BoxPoint origin = r.origin();

    // Draw a 10-pixel-grid
    for (BoxCoordinate i = 0; i < space[X]; i += 10)
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
	    origin[X] + i, origin[Y], origin[X] + i, origin[Y] + space[Y]);

    for (i = 0; i < space[Y]; i += 10)
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
	    origin[X], origin[Y] + i, origin[X] + space[X], origin[Y] + i);

    // Make space info
    ostrstream oss;
    ostream& os = oss;
    os << space << '\0';
    string ss = oss;

    // Draw it (centered)
    StringBox *s = new StringBox(ss);

    const BoxSize  stringSize = s->size();
    const BoxPoint stringOrigin = origin + space/2 - stringSize/2;

    XClearArea(XtDisplay(w), XtWindow(w), stringOrigin[X], stringOrigin[Y],
	stringSize[X], stringSize[Y], false);
    s->draw(w, BoxRegion(stringOrigin, stringSize), exposed,
	    gc, context_selected);

    s->unlink();
}

void DiagBox::dump(ostream& s) const
{
    s << "diag()";
}
