// $Id$
// Klasse LineBox (Implementation)

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
// Revision 1.1  1995/05/01 15:47:36  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:14:56  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  10:52:06  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/16  11:31:43  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/06/02  03:04:15  zeller
// #pragma besser formatiert
// 
// Revision 10.4  1992/06/01  16:34:15  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.3  1992/06/01  14:13:47  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/02/18  10:57:32  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:05  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:33  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:38  zeller
// Installierte Version
// 
// Revision 1.4  1991/06/09  17:38:12  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.3  1991/06/08  13:13:04  zeller
// CapStyle zusaetzlich auf CapProjecting setzen
// 
// Revision 1.2  1991/06/07  19:49:42  zeller
// Neu: Um Region Rahmen von halber Liniendicke freilassen
// 
// Revision 1.1  1991/06/07  19:18:32  zeller
// Initial revision
// 

char LineBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "LineBox.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

DEFINE_TYPE_INFO_1(LineBox, PrimitiveBox)

// LineBox

// LineBox anzeigen
void LineBox::_draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion& exposed, 
		    GC gc,
		    bool context_selected) const
{
    XGCValues gcvalues;

    // Linienbreite und Endpunkte setzen:
    // Ueber Endpunkt halbe Liniendicke hinausgehen
    gcvalues.line_width = _linethickness;
    gcvalues.cap_style = CapProjecting;
    XChangeGC(XtDisplay(w), gc, GCLineWidth | GCCapStyle, &gcvalues);

    // Um BoxRegion Rahmen von halber Liniendicke freilassen
    // (weil X sonst ueber BoxRegion hinaus schreibt)
    BoxPoint origin = r.origin();
    BoxSize space   = r.space();
    origin += _linethickness / 2;
    space  -= _linethickness;

    // Sohn anzeigen
    __draw(w, BoxRegion(origin, space), exposed, gc, context_selected);

    // Achtung: Wir lassen Linienbreite und CapStyle veraendert zurueck!
    // Das hat innerhalb von Box::draw() keine Auswirkungen,
    // aber der GC koennte danach veraendert sein.
}
