// $Id$
// Diagnostic boxes

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

char DiagBox_rcsid[] = 
    "$Id$";

#include <sstream>

#include "PrimitiveB.h"
#include "StringBox.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "DiagBox.h"

DEFINE_TYPE_INFO_1(DiagBox, PrimitiveBox)

// DiagBox

// Draw DiagBox
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
    std::ostringstream oss;
    oss << space << '\0';
    const string ss(oss);

    // Draw it (centered)
    StringBox *s = new StringBox(ss);

    const BoxSize  stringSize = s->size();
    const BoxPoint stringOrigin = origin + space/2 - stringSize/2;

    XClearArea(XtDisplay(w), XtWindow(w), stringOrigin[X], stringOrigin[Y],
	stringSize[X], stringSize[Y], False);
    s->draw(w, BoxRegion(stringOrigin, stringSize), exposed,
	    gc, context_selected);

    s->unlink();
}

void DiagBox::dump(std::ostream& s) const
{
    s << "diag()";
}
