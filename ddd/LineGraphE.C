// $Id$
// class LineGraphEdge

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

char LineGraphEdge_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"

#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "LineGraphE.h"
#include "GraphNode.h"

DEFINE_TYPE_INFO_1(LineGraphEdge, GraphEdge)

// find the points to draw line at

enum Side { North = 1, South = 2, East = 4, West = 8 };

// Clip point <p> to side <side> of region <b>
static void moveToSide(BoxRegion& b, int side, BoxPoint& p, BoxPoint&)
{
    assert(side == North || side == South || side == East || side == West);

    p = b.origin();

    // fetch points
    if (side & (North | South))
    {
	p[X] += b.space(X) / 2;
	if (side & South)
	    p[Y] += b.space(Y);
    }

    if (side & (East | West))
    {
	p[Y] += b.space(Y) / 2;
	if (side & East)
	    p[X] += b.space(X);
    }
}


// Clip point <p> to side <side> of region <b> centered around <c>
static void clipToSide(BoxRegion& b, int side, BoxPoint& p, BoxPoint& c)
{
    assert(side == North || side == South || side == East || side == West);

    BoxDimension d1, d2;

    if (side & (North | South))
	d1 = X, d2 = Y;
    else
	d1 = Y, d2 = X;

    int offset;
    if (side & (North | West))
	offset = -1;
    else
	offset = 1;

    if (c[d1] != p[d1] && c[d2] != p[d2])
	p[d1] += offset * (b.space(d2) / 2) * (c[d1] - p[d1]) 
	    / (c[d2] - p[d2]);
    p[d2] += offset * b.space(d2) / 2;
}


// Clip point <p> to side <side> of region <b> centered around <c>
// Assume that b contains a circle
static void clipToCircle(BoxRegion& b, int /* side */, 
			 BoxPoint& p, BoxPoint& c)
{
    // assert(side == North || side == South || side == East || side == West);

    double radius = max(b.space(X), b.space(Y)) / 2;
    if (radius > 0.0)
    {
	double hyp = hypot(c[X] - p[X], c[Y] - p[Y]);

	p[X] += BoxCoordinate((radius * (c[X] - p[X])) / hyp);
	p[Y] += BoxCoordinate((radius * (c[Y] - p[Y])) / hyp);
    }
}


typedef void (*ClipProc)(BoxRegion& b, int side, BoxPoint& p, BoxPoint& c);

struct ClipMapRec {
    EdgeAttachMode mode;
    ClipProc       proc;
};

const ClipMapRec clipMap[] = {
    {Straight, clipToSide},
    {Circle,   clipToCircle},
    {Centered, moveToSide}
};

// Find line from region <b1> centered around <c1>
// to region <b2> centered around <c2>
// Resulting line shall be drawn from <p1> to <p2>
void LineGraphEdge::findLine(BoxPoint& c1, BoxPoint& c2,
			     BoxRegion& b1, BoxRegion& b2, 
			     BoxPoint& p1, BoxPoint& p2, 
			     const GraphGC& gc)
{
    // allow all sizes to begin
    int side1 = North | South | East | West;
    int side2 = North | South | East | West;

    // exclude opposite side
    if (c2[X] > c1[X]) { side1 &= ~West; side2 &= ~East; }
    else               { side1 &= ~East; side2 &= ~West; }

    if (c2[Y] > c1[Y]) { side1 &= ~North; side2 &= ~South; }
    else               { side1 &= ~South; side2 &= ~North; }

    // find edge cutting the line between the two center points c1, c2
    BoxCoordinate dx = abs(c1[X] - c2[X]);
    BoxCoordinate dy = abs(c1[Y] - c2[Y]);

    if (b1.space(Y) * dx > b1.space(X) * dy) side1 &= ~(North | South);
    else                                     side1 &= ~(East | West);

    if (b2.space(Y) * dx > b2.space(X) * dy) side2 &= ~(North | South);
    else                                     side2 &= ~(East | West);

    p1 = c1;
    p2 = c2;

    // select appropriate clipping procedure
    for (int i = 0; i < int(sizeof(clipMap)/sizeof(clipMap[0])); i++)
	if (gc.edgeAttachMode == clipMap[i].mode)
	{
	    clipMap[i].proc(b1, side1, p1, c2);
	    clipMap[i].proc(b2, side2, p2, c1);

	    return;
	}

    assert(0);
}


// Draw

void LineGraphEdge::_draw(Widget w, 
			  const BoxRegion&, 
			  const GraphGC& gc) const
{
    // Get node starting points
    BoxPoint pos1     = from()->pos();
    BoxRegion region1 = from()->region(gc);
    if (from()->selected())
    {
	pos1             += gc.offsetIfSelected;
	region1.origin() += gc.offsetIfSelected;
    }

    BoxPoint pos2     = to()->pos();
    BoxRegion region2 = to()->region(gc);
    if (to()->selected())
    {
	pos2             += gc.offsetIfSelected;
	region2.origin() += gc.offsetIfSelected;
    }

    // If nodes overlap, don't draw the edge.
    if (region1 <= region2)
	return;

    // Get the line points
    BoxPoint l1, l2;
    findLine(pos1, pos2, region1, region2, l1, l2, gc);

    // If there is no edge (adjacent nodes), don't draw it.
    if (l1 == l2)
	return;

    XDrawLine(XtDisplay(w), XtWindow(w), gc.edgeGC,
	l1[X], l1[Y], l2[X], l2[Y]);

    if (gc.drawArrowHeads && !to()->isHint())
    {
	// draw arrowhead at l2

	const double offset = gc.arrowAngle * M_PI/180;	// angle
	const int length    = gc.arrowLength;		// length

        // get arrow angle
	double alpha = atan2(l1[Y] - l2[Y], l1[X] - l2[X]);

        // get coordinates
	XPoint points[3];
	points[0].x = l2[X];
	points[0].y = l2[Y];
	points[1].x = (short)(l2[X] + length * cos(alpha + offset / 2));
	points[1].y = (short)(l2[Y] + length * sin(alpha + offset / 2));
	points[2].x = (short)(l2[X] + length * cos(alpha - offset / 2));
	points[2].y = (short)(l2[Y] + length * sin(alpha - offset / 2));

#if 0
        cout << "\nalpha = " << alpha / M_PI * 360 << "\n";
	for (int i = 0; i < 3; i++)
	    cout << "points[" << i << "] = "
		<< BoxPoint(points[i].x, points[i].y) << "\n";
#endif

	XFillPolygon(XtDisplay(w), XtWindow(w), gc.edgeGC, points, 
		     XtNumber(points), Convex, CoordModeOrigin);
    }
}
