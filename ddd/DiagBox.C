// $Id$
// Klasse DiagBox (Implementation)

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char DiagBox_rcsid[] = 
    "$Id$";

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
    BoxCoordinate i;
    for (i = 0; i < space[X]; i += 10)
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
