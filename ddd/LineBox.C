// $Id$
// Line boxes

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

char LineBox_rcsid[] = 
    "$Id$";

#include "LineBox.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

DEFINE_TYPE_INFO_1(LineBox, PrimitiveBox)

// LineBox

// Draw
void LineBox::_draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion& exposed, 
		    GC gc,
		    bool context_selected) const
{
    XGCValues gcvalues;

    // Set width and cap style; project beyond end point up to 1/2
    // line thickness
    gcvalues.line_width = _linethickness;
    gcvalues.cap_style = CapProjecting;
    XChangeGC(XtDisplay(w), gc, GCLineWidth | GCCapStyle, &gcvalues);

    // Keep an empty frame of 1/2 line thickness around R (X may cross
    // R's boundaries otherwise)
    BoxPoint origin = r.origin();
    BoxSize space   = r.space();
    origin += _linethickness / 2;
    space  -= _linethickness;

    // Draw children
    __draw(w, BoxRegion(origin, space), exposed, gc, context_selected);

    // Attention: We leave LINE_WIDTH and CAP_STYLE changed!
    // (Works within Box::draw(), but the used GC may be changed)
}
