// $Id$
// BoxGraphNode class: RegionGraphNode with box

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char BoxGraphNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BoxGraphN.h"
#include "printBox.h"
#include "CompositeB.h"
#include "ColorBox.h"


DEFINE_TYPE_INFO_1(BoxGraphNode, RegionGraphNode)

// Draw a BoxGraphNode
void BoxGraphNode::forceDraw(Widget w, 
			     const BoxRegion& /* exposed */,
			     const GraphGC& gc) const
{
    assert(box() != 0);
    // assert(box()->OK());

    // We do not check for exposures here --
    // boxes are usually small and partial display
    // doesn't work well with scrolling
    static BoxRegion exposed(BoxPoint(0, 0), BoxSize(INT_MAX, INT_MAX));

    if (selected() && highlight())
    {
	box()->draw(w, region(gc), exposed, gc.nodeGC, false);

	bool use_color = ColorBox::use_color;
	ColorBox::use_color = false;
	BoxRegion r = highlightRegion(gc);

	if (r <= exposed)
	{
	    XFillRectangle(XtDisplay(w), XtWindow(w), gc.clearGC,
			   r.origin(X), r.origin(Y),
			   r.space(X), r.space(Y));
	    highlight()->draw(w, r, r, gc.nodeGC, false);
	}
	ColorBox::use_color = use_color;
    }
    else if (selected())
    {
	bool use_color = ColorBox::use_color;
	ColorBox::use_color = false;
	box()->draw(w, region(gc), exposed, gc.nodeGC, false);
	ColorBox::use_color = use_color;
    }
    else
    {
	box()->draw(w, region(gc), exposed, gc.nodeGC, false);
    }
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
    assert (box() != 0);

    if (gc.printGC->isFig())
	startCompound(os, region(gc));

    RegionGraphNode::_print(os, gc);
    box()->_print(os, region(gc), *gc.printGC);

    if (gc.printGC->isFig())
	endCompound(os);
}

// MARK is a MarkBox in SRC.  Find equivalent box in DUP.
MarkBox *BoxGraphNode::find_mark(Box *dup, Box *src, Box *mark)
{
    assert (box() != 0);

    if (mark == 0)
	return 0;

    if (src == mark)
    {
	MarkBox *dup_mb = ptr_cast(MarkBox, dup);
	assert(dup_mb != 0);
	return dup_mb;
    }

    // Try Composite children
    CompositeBox *src_cb = ptr_cast(CompositeBox, src);
    if (src_cb != 0)
    {
	CompositeBox *dup_cb = ptr_cast(CompositeBox, dup);
	assert(dup_cb != 0);
	assert(src_cb->nchildren() == dup_cb->nchildren());

	for (int i = 0; i < src_cb->nchildren(); i++)
	{
	    MarkBox *mb = 
		find_mark((*dup_cb)[i], (*src_cb)[i], mark);
	    if (mb)
		return mb;
	}

	return 0;
    }

    // Try HatBox child
    HatBox *src_hb = ptr_cast(HatBox, src);
    if (src_hb != 0)
    {
	HatBox *dup_hb = ptr_cast(HatBox, dup);
	assert(dup_hb != 0);

	return find_mark(dup_hb->box(), src_hb->box(), mark);
    }

    return 0;
}


// Set the box.
void BoxGraphNode::setBox(Box *b)
{
    assert(b == 0 || b->OK());

    setHighlight(0);

    Box *old = _box;
    if (b)
	_box = b->link();
    else
	_box = 0;

    if (old)
	old->unlink();

    if (b)
	resize(b->size());
}


// Copy Constructor
BoxGraphNode::BoxGraphNode(const BoxGraphNode& node):
    RegionGraphNode(node),
    _box(node._box ? node._box->dup() : 0),
    _highlight(node._box ? find_mark(_box, node._box, node._highlight) : 0)
{}
