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

char LineBox_rcsid[] = 
    "$Id$";

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
