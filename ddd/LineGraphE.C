// $Id$
// class LineGraphEdge

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

// Find the points to draw line at

// Clip point P to side SIDE of region B.
void LineGraphEdge::moveToSide(const BoxRegion& b, int side, 
			       BoxPoint& p, const BoxPoint&)
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


// Clip point P to side SIDE of region B centered around C.
void LineGraphEdge::clipToSide(const BoxRegion& b, int side, 
			       BoxPoint& p, const BoxPoint& c)
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


// Clip point P to side SIDE of region B centered around C.  Assume
// that B contains a circle.
void LineGraphEdge::clipToCircle(const BoxRegion& b, int /* side */, 
				 BoxPoint& p, const BoxPoint& c)
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

// Find line from region B1 centered around C1 to region B2 centered
// around C2.  Resulting line shall be drawn from P1 to P2
void LineGraphEdge::findLine(const BoxPoint& c1, const BoxPoint& c2,
			     const BoxRegion& b1, const BoxRegion& b2, 
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

    // Select appropriate clipping procedure
    typedef void (*ClipProc)(const BoxRegion& b, int side,
			     BoxPoint& p, const BoxPoint& c);

    struct ClipMapRec {
	EdgeAttachMode mode;
	ClipProc       proc;
    };

    static const ClipMapRec clipMap[] = 
    {
	{Straight, LineGraphEdge::clipToSide},
	{Circle,   LineGraphEdge::clipToCircle},
	{Centered, LineGraphEdge::moveToSide},
	{Straight, 0}
    };

    for (int i = 0; clipMap[i].proc != 0; i++)
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
			  const BoxRegion& exposed, 
			  const GraphGC& gc) const
{
    if (from() == to())
	drawSelf(w, exposed, gc);
    else
	drawLine(w, exposed, gc);
}

void LineGraphEdge::drawLine(Widget w, 
			     const BoxRegion& exposed, 
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

    // Get arrow angle
    double alpha = atan2(l1[Y] - l2[Y], l1[X] - l2[X]);

    // Draw arrow head at L2
    drawArrowHead(w, exposed, gc, l2, alpha);
}


// Draw arrow head at POS
void LineGraphEdge::drawArrowHead(Widget w,
				  const BoxRegion& /* exposed */,
				  const GraphGC& gc,
				  const BoxPoint& pos,
				  double alpha) const
{
    if (!gc.drawArrowHeads || to()->isHint())
	return;

    const double offset = gc.arrowAngle * M_PI/180;	// Angle
    const int length    = gc.arrowLength;		// Length

    // Get coordinates
    XPoint points[3];
    points[0].x = pos[X];
    points[0].y = pos[Y];
    points[1].x = short(pos[X] + length * cos(alpha + offset / 2));
    points[1].y = short(pos[Y] + length * sin(alpha + offset / 2));
    points[2].x = short(pos[X] + length * cos(alpha - offset / 2));
    points[2].y = short(pos[Y] + length * sin(alpha - offset / 2));

#if 0
	clog << "\nangle = " << (alpha / (M_PI * 2.0)) * 360.0  << "\n";
	for (int i = 0; i < 3; i++)
	    clog << "points[" << i << "] = "
		 << BoxPoint(points[i].x, points[i].y) << "\n";
#endif

    XFillPolygon(XtDisplay(w), XtWindow(w), gc.edgeGC, points,
		 XtNumber(points), Convex, CoordModeOrigin);
}

// Draw self edge
void LineGraphEdge::drawSelf(Widget w,
			     const BoxRegion& exposed,
			     const GraphGC& gc) const
{
    assert(from() == to());

    // Get arc start
    BoxRegion region = from()->region(gc);
    if (from()->selected())
	region.origin() += gc.offsetIfSelected;

    // Draw arc
    Dimension diameter = gc.selfEdgeDiameter;

    // Make sure edge is still attached to node
    diameter = min(diameter, region.space(X) + region.space(X) / 2);
    diameter = min(diameter, region.space(Y) + region.space(Y) / 2);

    // Be sure we don't make it too small
    diameter = max(diameter, 4);
    Dimension radius = (diameter + 1) / 2;
    diameter = radius * 2;

    BoxPoint position(region.origin());	// Upper left corner of the arc
    int start = 0;		        // Start of the arc (in degrees)
    const int extend = 270;	        // Extend of the arc (in degrees)

    switch (gc.selfEdgePosition)
    {
    case NorthEast:
	position += BoxPoint(region.space(X) - radius, -radius);
	start = 270;
	break;

    case SouthEast:
	position += 
	    BoxPoint(region.space(X) - radius, region.space(Y) - radius);
	start = 180;
	break;

    case SouthWest:
	position += BoxPoint(-radius, region.space(Y) - radius);
	start = 90;
	break;

    case NorthWest:
	position += BoxPoint(-radius, -radius);
	start = 0;
	break;
    }

    XDrawArc(XtDisplay(w), XtWindow(w), gc.edgeGC, position[X],
	     position[Y], diameter, diameter,
	     start * 64, extend * 64);

    // Find arrow angle
    int arrow_angle = 0;
    switch (gc.selfEdgeDirection)
    {
    case Clockwise:
	arrow_angle = 360 - (start + extend + 180) % 360;
	break;

    case Counterclockwise:
	arrow_angle = 360 - (start + 180) % 360;
	break;
    }
    double alpha = 2 * M_PI * arrow_angle / 360.0;

    // Incline arrow a little into the arc
    double cosine = gc.arrowLength / (2.0 * radius);
    double inclination = (M_PI / 2.0) - acos(cosine);

    // Draw arrow
    position = BoxPoint(region.origin());
    switch (gc.selfEdgeDirection)
    {
    case Clockwise:
	alpha -= inclination;
	switch (gc.selfEdgePosition)
	{
	case NorthEast:
	    position += BoxPoint(region.space(X), radius);
	    break;

	case SouthEast:
	    position += BoxPoint(region.space(X) - radius, region.space(Y));
	    break;

	case SouthWest:
	    position += BoxPoint(0, region.space(Y) - radius);
	    break;

	case NorthWest:
	    position += BoxPoint(radius, 0);
	    break;
	}
	break;

    case Counterclockwise:
	alpha += inclination;
	switch (gc.selfEdgePosition)
	{
	case NorthEast:
	    position += BoxPoint(region.space(X) - radius, 0);
	    break;

	case SouthEast:
	    position += BoxPoint(region.space(X), region.space(Y) - radius);
	    break;

	case SouthWest:
	    position += BoxPoint(radius, region.space(Y));
	    break;

	case NorthWest:
	    position += BoxPoint(0, radius);
	    break;
	}
	break;
    }

    drawArrowHead(w, exposed, gc, position, alpha);
}
