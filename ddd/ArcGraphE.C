// $Id$ -*- C++ -*-
// Arc edges

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char ArcGraphEdge_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ArcGraphE.h"
#include "HintGraphN.h"
#include "misc.h"

#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

DEFINE_TYPE_INFO_1(ArcGraphEdge, LineGraphEdge);

inline double hypot(const BoxPoint& p)
{
    return hypot(p[X], p[Y]);
}

// Compute the center of the arc enclosing P1, P2, and P3.
// Return false iff P1, P2, and P3 form a line (i.e. there is no arc)
bool ArcGraphEdge::center(const BoxPoint& p1, const BoxPoint& p2, 
			  const BoxPoint& p3, double& x, double& y)
{
    if (p1[X] == p3[X] && p1[Y] == p3[Y])
	return false;

    BoxPoint d12 = p1 - p2;
    BoxPoint d13 = p1 - p3;

    double s12 = asin(d12[Y] / hypot(d12));
    double s13 = asin(d13[Y] / hypot(d13));
    const double epsilon = 0.01;
    if (fabs(s12 - s13) < epsilon)
	return false;

    double l1 = p1[X] * p1[X] + p1[Y] * p1[Y];
    double l2 = p2[X] * p2[X] + p2[Y] * p2[Y];
    double l3 = p3[X] * p3[X] + p3[Y] * p3[Y];

    y = (d12[X] * (l3 - l1) - d13[X] * (l2 - l1))
	/ (2.0 * (d13[X] * d12[Y] - d12[X] * d13[Y]));

    if (p1[X] != p3[X])
	x = (l3 + 2.0 * y * d13[Y] - l1) / (2.0 * -d13[X]);
    else
	x = (l2 + 2.0 * y * d12[Y] - l1) / (2.0 * -d12[X]);

    return true;
}


void ArcGraphEdge::drawLine(Widget w, 
			    const BoxRegion& exposed, 
			    const GraphGC& gc) const
{
    HintGraphNode   *arc_hint = 0;
    RegionGraphNode *arc_from = 0;
    RegionGraphNode *arc_to   = 0;

    if (from()->isHint() && to()->isHint())
    {
	// Edge between two hints
	LineGraphEdge::drawLine(w, exposed, gc);
	return;
    }
    else if (from()->isHint() && from()->firstTo() != 0)
    {
	arc_hint = ptr_cast(HintGraphNode, from());
	arc_from = ptr_cast(RegionGraphNode, arc_hint->firstTo()->from());
	arc_to   = ptr_cast(RegionGraphNode, to());

	if (arc_hint == 0 || arc_from == 0 || arc_to == 0 
	    || arc_hint->nextTo(arc_hint->firstTo()) != 0)
	{
	    // Bad nodes or hint with multiple edges
	    LineGraphEdge::drawLine(w, exposed, gc);
	    return;
	}
    }
    else if (to()->isHint() && to()->firstFrom() != 0)
    {
	arc_hint = ptr_cast(HintGraphNode, to());
	arc_to   = ptr_cast(RegionGraphNode, arc_hint->firstFrom()->to());
	arc_from = ptr_cast(RegionGraphNode, from());

	if (arc_hint == 0 || arc_from == 0 || arc_to == 0
	    || arc_hint->nextFrom(arc_hint->firstFrom()) != 0)
	{
	    // Bad nodes or hint with multiple edges
	    LineGraphEdge::drawLine(w, exposed, gc);
	    return;
	}
    }
    else
    {
	// Edge between two ordinary nodes
	LineGraphEdge::drawLine(w, exposed, gc);
	return;
    }

    BoxPoint pos_from     = arc_from->pos();
    BoxRegion region_from = arc_from->region(gc);
    if (arc_from->selected())
    {
	pos_from             += gc.offsetIfSelected;
	region_from.origin() += gc.offsetIfSelected;
    }

    BoxPoint pos_to     = arc_to->pos();
    BoxRegion region_to = arc_to->region(gc);
    if (arc_to->selected())
    {
	pos_to             += gc.offsetIfSelected;
	region_to.origin() += gc.offsetIfSelected;
    }

    BoxPoint pos_hint     = arc_hint->pos();
    BoxRegion region_hint = arc_hint->region(gc);
    if (arc_hint->selected())
    {
	pos_hint             += gc.offsetIfSelected;
	region_hint.origin() += gc.offsetIfSelected;
    }

    if (pos_hint <= region_from || pos_hint <= region_to)
    {
	// Hint within region
	LineGraphEdge::drawLine(w, exposed, gc);
	return;
    }

    BoxPoint new_pos_from, new_pos_to, dummy;
    findLine(pos_from, pos_hint, region_from, region_hint,
	     new_pos_from, dummy, gc);
    findLine(pos_hint, pos_to, region_hint, region_to,
	     dummy, new_pos_to, gc);
    pos_from = new_pos_from;
    pos_to   = new_pos_to;

    // Draw circle segment POS_FROM -> POS_HINT or POS_HINT -> POS_TO

    // Determine the arc center
    double x, y;
    bool ok = center(pos_from, pos_hint, pos_to, x, y);
    if (!ok)
    {
	// Nodes form a line
	LineGraphEdge::drawLine(w, exposed, gc);
	return;
    }

    BoxPoint c((BoxCoordinate)x, (BoxCoordinate)y);

    // Determine radius (easy when you have the center)
    double radius = hypot(c - pos_to);

    // Determine start and path of arc
    double alpha_from = -atan2(pos_from[Y] - c[Y], pos_from[X] - c[X]);
    double alpha_hint = -atan2(pos_hint[Y] - c[Y], pos_hint[X] - c[X]);
    double alpha_to   = -atan2(pos_to[Y] - c[Y],   pos_to[X] - c[X]);

    const int base = 360 * 64;

    int angle_from = (int(alpha_from * base / (M_PI * 2.0)) + base) % base;
    int angle_to   = (int(alpha_to   * base / (M_PI * 2.0)) + base) % base;
    int angle_hint = (int(alpha_hint * base / (M_PI * 2.0)) + base) % base;

    int path_from_hint = (base + angle_hint - angle_from) % base;
    int path_hint_to   = (base + angle_to - angle_hint) % base;

    if (abs(path_from_hint) > base / 2)
	path_from_hint = (path_from_hint - base) % base;
    if (abs(path_hint_to) > base / 2)
	path_hint_to = (path_hint_to - base) % base;

    if (sgn(path_from_hint) * sgn(path_hint_to) == -1)
    {
	// Hint is not between FROM and TO
	LineGraphEdge::drawLine(w, exposed, gc);
	return;
    }

    int angle, path;
    if (to()->isHint())
    {
	angle = angle_from;
	path  = path_from_hint;
    }
    else
    {
	angle = angle_hint;
	path  = path_hint_to;
    }

    XDrawArc(XtDisplay(w), XtWindow(w), gc.edgeGC,
	     c[X] - int(radius), c[Y] - int(radius),
	     unsigned(radius) * 2, unsigned(radius) * 2, angle, path);

    if (from()->isHint())
    {
	// Draw arrow head at POS_TO
	double alpha = atan2(pos_to[Y] - c[Y], pos_to[X] - c[X]);
	if (path > 0)
	    alpha += M_PI / 2.0;
	else
	    alpha -= M_PI / 2.0;
	drawArrowHead(w, exposed, gc, pos_to, alpha);
    }
}

void ArcGraphEdge::_print(ostream& os, const GraphGC &gc) const
{
    static int warning = 0;
    if (warning++ == 0)
	cerr << "Warning: arc printing is not supported\n";

    LineGraphEdge::_print(os, gc);
}
