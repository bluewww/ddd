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

char DiagBox_rcsid[] = 
    "$Id$";

#include <sstream>

#include "PrimitiveB.h"
#include "StringBox.h"
#ifdef IF_MOTIF
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#endif // IF_MOTIF
#include "DiagBox.h"

DEFINE_TYPE_INFO_1(DiagBox, PrimitiveBox)

// DiagBox

#if defined(IF_XM)

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
    for (i = 0; i < space[X]; i += 10) {
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
	    origin[X] + i, origin[Y], origin[X] + i, origin[Y] + space[Y]);
    }

    for (i = 0; i < space[Y]; i += 10) {
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
	    origin[X], origin[Y] + i, origin[X] + space[X], origin[Y] + i);
    }

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

#else

// Draw DiagBox
void DiagBox::_draw(GUI::Widget *w, 
		    const BoxRegion& r, 
		    const BoxRegion& exposed, GUI::RefPtr<GUI::GC> gc,
		    bool context_selected) const
{
    const BoxSize space   = r.space();
    const BoxPoint origin = r.origin();

    // Draw a 10-pixel-grid
    BoxCoordinate i;
    for (i = 0; i < space[X]; i += 10) {
	w->get_window()->draw_line(gc, origin[X] + i, origin[Y],
				   origin[X] + i, origin[Y] + space[Y]);
    }

    for (i = 0; i < space[Y]; i += 10) {
	w->get_window()->draw_line(gc, origin[X], origin[Y] + i,
				   origin[X] + space[X], origin[Y] + i);
    }

    // Make space info
    std::ostringstream oss;
    oss << space << '\0';
    const string ss(oss);

    // Draw it (centered)
    StringBox *s = new StringBox(ss);

    const BoxSize  stringSize = s->size();
    const BoxPoint stringOrigin = origin + space/2 - stringSize/2;

#ifdef NAG_ME
#warning No analogue for exposures argument?
#endif
    w->get_window()->clear_area(stringOrigin[X], stringOrigin[Y],
				stringSize[X], stringSize[Y]);
    s->draw(w, BoxRegion(stringOrigin, stringSize), exposed,
	    gc, context_selected);

    s->unlink();
}

#endif

void DiagBox::dump(std::ostream& s) const
{
    s << "diag()";
}
