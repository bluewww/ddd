// $Id$
// BoxGraphNode class: RegionGraphNode with box

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

char BoxGraphNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BoxGraphN.h"
#include "printBox.h"


DEFINE_TYPE_INFO_1(BoxGraphNode, RegionGraphNode)

// Draw a BoxGraphNode
void BoxGraphNode::forceDraw(Widget w, 
			     const BoxRegion& exposed, 
			     const GraphGC& gc) const
{
    // We do not check for exposures here --
    // boxes are usually small and partial display
    // doesn't work well with scrolling

    box()->draw(w, region(gc), region(gc), gc.nodeGC, false);
}


// mark the following objects as one XFIG compound object
static void startCompound(ostream& os, BoxRegion region)
{
    BoxPoint origin = region.origin();
    BoxPoint width = region.space();

    os << CMPHEAD;
    os << origin[X] + width[X] + 1 << " " << origin[Y] - 1 << " ";
    os << origin[X] - 1 << " " << origin[Y] + width[Y] + 1 << "\n";
}

static void endCompound(ostream& os)
{
    os << CMPTAIL;
}


// Print a BoxGraphNode
void BoxGraphNode::_print(ostream& os, const GraphGC& gc) const
{
    if (gc.printGC->isFig())
	startCompound(os, region(gc));

    RegionGraphNode::_print(os, gc);
    box()->_print(os, (BoxRegion&)region(gc), *gc.printGC);

    if (gc.printGC->isFig())
	endCompound(os);
}

