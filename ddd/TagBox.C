// $Id$
// Tagged boxes

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

char TagBox_rcsid[] = 
    "$Id$";

#include "assert.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "TagBox.h"
#include "VSEFlags.h"


DEFINE_TYPE_INFO_1(TagBox, MarkBox)

// TagBox

// Default functions

// Default duplicator
static Data *default_link(Data *dta)
{
    return dta;
}

// Default deletion
static void default_unlink(Data *)
{
    return;
}

// Default selection
static bool default_selected(Data *)
{
    return false;   // Default: non-selected
}

// Default tag name
static string default_name(Data *)
{
    return "";      // Default: no name
}

// Default tag info
static string default_info(Data *dta)
{
    return default_name(dta);   // Default: none
}

// All default functions
DataLink TagBox::default_datalink =
{
    default_link,
    default_unlink,
    default_selected,
    default_name,
    default_info
};


// Find TagBox within P
const TagBox *TagBox::findTag(const BoxPoint& p) const
{
    // Special case: return outermost tag
    if (p == BoxPoint(-1, -1))
	return this;

    if (!(p <= _region))
    {
	return 0;       // P does not lie withing the region of this box
    }

    // Check children
    const TagBox *subbox = HatBox::findTag(p);

    if (subbox != 0)
	return subbox;  // found lower child
    else
	return this;    // take this one
}


// Draw TagBox
void TagBox::_draw(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed,
		   GC gc, 
		   bool context_selected) const
{
    // Draw child
    bool nodeSelected = __selected();
    MarkBox::_draw(w, r, exposed, gc, nodeSelected);

    if (context_selected != nodeSelected)
    {
	// Invert the BoxRegion just drawn

	BoxRegion clipRegion = exposed & r;    // Schnittmenge

	XGCValues gcvalues;
	XGetGCValues(XtDisplay(w), gc, GCFunction, &gcvalues);
	XSetFunction(XtDisplay(w), gc, GXinvert);

	XFillRectangle(XtDisplay(w), XtWindow(w), gc,
	    clipRegion.origin(X), clipRegion.origin(Y),
	    clipRegion.space(X), clipRegion.space(Y));

	XSetFunction(XtDisplay(w), gc, gcvalues.function);
    }
}


// Dump TagBox
void TagBox::dump(std::ostream& s) const
{
    MarkBox::dump(s);

    if (VSEFlags::include_tag_info)
    {
	s << "\n// Tag " << _data << ": ";
	s.flush();

	s << "(" << info() << ") " << _region << " ";

	if (selected())
	    s << " (selected)";
	else
	    s << " (not selected)";

	s << "\n\n";
	s.flush();
    }
}
