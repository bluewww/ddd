// $Id$
// RegionGraphNode class

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char RegionGraphNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "RegionGN.h"
#include "printBox.h"

DEFINE_TYPE_INFO_1(RegionGraphNode, PosGraphNode)

// center around position
void RegionGraphNode::center()
{
    _region.origin() = pos() - (_region.space() / 2);
}


// Draw a RegionGraphNode
void RegionGraphNode::draw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const
{
    const BoxRegion& r = region(gc);

    // if not exposed or invisible, return
    if (!(r <= exposed) || r.space(X) == 0 || r.space(Y) == 0)
	return;

    Display *display = XtDisplay(w);
    Window window = XtWindow(w);

    // clear the area
    XFillRectangle(display, window, gc.clearGC,
		   r.origin(X), r.origin(Y),
		   r.space(X), r.space(Y));

    // draw contents
    forceDraw(w, exposed, gc);

    // if selected, invert area
    if (selected())
    {
	const BoxRegion& h = highlightRegion(gc);

	XFillRectangle(display, window, gc.invertGC,
		       h.origin(X), h.origin(Y),
		       h.space(X), h.space(Y));
    }
};



// cleanRegion
// clean a region with white ink
//
static void cleanRegion (ostream& os, const GraphGC& gc, BoxRegion region)
{
      BoxPoint origin = region.origin();
      BoxPoint width = region.space();

      if (gc.printGC->isPostScript())
      {
	  os << origin[X] << " " << origin[Y] << " ";
	  os << origin[X] + width[X] << " " << origin[Y];
	  os << " ";
	  os << origin[X] + width[X] << " ";
	  os << origin[Y] + width[Y] << " ";
	  os << origin[X] << " " << origin[Y] + width[Y];
	  os << " clean*\n";
      }
      else if (gc.printGC->isFig())
      {
	  os << CLEANHEAD;
	  os << origin[X] << " " << origin[Y] << " ";
	  os << origin[X] + width[X] << " " << origin[Y];
	  os << " ";
	  os << origin[X] + width[X] << " ";
	  os << origin[Y] + width[Y] << " ";
	  os << origin[X] << " " << origin[Y] + width[Y];
	  os << " ";
	  os << origin[X] << " "<< origin[Y] << " 9999 9999\n";
      }	  
}

// Print a RegionGraphNode
void RegionGraphNode::_print(ostream& os, const GraphGC& gc) const
{
    cleanRegion(os, gc, region(gc));
}
