// $Id$
// class LineGraphEdge

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

char LineGraphEdge_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "LineGraphE.h"

#include "assert.h"

#include <math.h>
#include "pi.h"
#include "hypot.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "GraphNode.h"
#include "LineGESI.h"


DEFINE_TYPE_INFO_1(LineGraphEdge, GraphEdge)

// Find the points to draw line at

// Clip point P to side SIDE of region B.
void LineGraphEdge::moveToSide(const BoxRegion& b, int side, 
			       BoxPoint& p, const BoxPoint&)
{
    assert(side == North || side == South || side == East || side == West);

    p = b.origin();

    // Fetch points
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

    // Draw annotation
    BoxPoint anno_pos = annotationPosition(gc);
    if (annotation() != 0 && anno_pos.isValid())
    {
	annotation()->draw(w, anno_pos, exposed, gc);
    }

    // Get arrow angle
    double alpha = atan2(double(l1[Y] - l2[Y]), double(l1[X] - l2[X]));

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

    const double offset = gc.arrowAngle * PI / 180;	// Angle
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
	clog << "\nangle = " << (alpha / (PI * 2.0)) * 360.0  << "\n";
	for (int i = 0; i < 3; i++)
	    clog << "points[" << i << "] = "
		 << BoxPoint(points[i].x, points[i].y) << "\n";
#endif

    XFillPolygon(XtDisplay(w), XtWindow(w), gc.edgeGC, points,
		 XtNumber(points), Convex, CoordModeOrigin);
}


// Region occupied by edge
BoxRegion LineGraphEdge::region(const GraphGC& gc) const
{
    BoxRegion r;
    if (gc.drawAnnotations && annotation() != 0)
    {
	BoxPoint anno_pos = annotationPosition(gc);
	if (anno_pos.isValid())
	{
	    BoxRegion anno_region = annotation()->region(anno_pos, gc);
	    if (r.origin().isValid())
		r = r | anno_region;
	    else
		r = anno_region;
	}
    }

    if (from() == to())
    {
	BoxRegion region = from()->region(gc);
       	if (from()->selected())
	    region.origin() += gc.offsetIfSelected;

	LineGraphEdgeSelfInfo info(region, gc);

	BoxRegion self_region(info.arc_pos,
			      BoxSize(info.diameter, info.diameter));

	if (r.origin().isValid())
	    r = r | self_region;
	else
	    r = self_region;
    }

    return r;
}


// Draw self edge
void LineGraphEdge::drawSelf(Widget w,
			     const BoxRegion& exposed,
			     const GraphGC& gc) const
{
    assert(from() == to());

    // Get region
    BoxRegion region = from()->region(gc);
    if (from()->selected())
	region.origin() += gc.offsetIfSelected;

    LineGraphEdgeSelfInfo info(region, gc);

    XDrawArc(XtDisplay(w), XtWindow(w), gc.edgeGC, info.arc_pos[X],
	     info.arc_pos[Y], info.diameter, info.diameter,
	     info.arc_start * 64, info.arc_extend * 64);

    if (annotation() != 0)
    {
	// Draw annotation
	annotation()->draw(w, info.anno_pos, exposed, gc);
    }

    // Find arrow angle
    drawArrowHead(w, exposed, gc, info.arrow_pos, info.arrow_alpha);
}

void LineGraphEdge::printSelf(ostream& os, const GraphGC &gc) const
{
    assert(from() == to());

    if (!gc.printGC->isPostScript())
    {
	static int warning = 0;
	if (warning++ == 0)
	    cerr << "Warning: arc printing is not supported\n";

	return;
    }

    // Get region
    BoxRegion region = from()->region(gc);
    if (from()->selected())
	region.origin() += gc.offsetIfSelected;

    LineGraphEdgeSelfInfo info(region, gc);

    int start = (720 - info.arc_start - info.arc_extend) % 360 ;
    int end   = (720 - info.arc_start) % 360 ;

    BoxCoordinate line_width = 1;

    // Draw arc
    os << start << " " << end << " " 
       << info.radius << " " << info.radius << " "
       << info.arc_center[X] << " " << info.arc_center[Y] << " " 
       << line_width << " arc*\n";

    if (annotation() != 0)
    {
	// Print annotation
	annotation()->_print(os, info.anno_pos, gc);
    }

    // Now draw the arrow head
    int angle = (720 - info.arrow_angle) % 360;

    os << gc.arrowAngle << " " << gc.arrowLength << " " << angle << " "
       << info.arrow_pos[X] << " " << info.arrow_pos[Y] << " arrowhead*\n";
}

BoxPoint LineGraphEdge::annotationPosition(const GraphGC &gc) const
{
    if (from() == to())
    {
	BoxRegion region = from()->region(gc);
	if (from()->selected())
	    region.origin() += gc.offsetIfSelected;

	LineGraphEdgeSelfInfo info(region, gc);
	return info.anno_pos;
    }

    BoxPoint pos1     = from()->pos();
    BoxRegion region1 = from()->region(gc);

    BoxPoint pos2     = to()->pos();
    BoxRegion region2 = to()->region(gc);

    BoxPoint l1, l2;
    findLine(pos1, pos2, region1, region2, l1, l2, gc);

    if (from()->isHint() && to()->isHint())
    {
	// Between two hints -- don't draw anything
	return BoxPoint();
    }

    if (to()->isHint())
    {
	// Draw at hint position
	return to()->pos();
    }

    // Draw at mid-distance
    return l1 + (l2 - l1) / 2;
}

void LineGraphEdge::_print(ostream& os, const GraphGC &gc) const
{
    if (from() == to())
    {
	printSelf(os, gc);
	return;
    }

    GraphEdge::_print(os, gc);

    // Print annotation
    if (annotation() != 0)
    {
	BoxPoint anno_pos = annotationPosition(gc);
	if (anno_pos.isValid())
	{
	    annotation()->_print(os, anno_pos, gc);
	}
    }
}
