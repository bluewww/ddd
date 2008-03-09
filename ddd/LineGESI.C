// $Id$ -*- C++ -*-
// Determine position of `self' edges

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char LineGraphEdgeSelfInfo_rcsid[] = 
    "$Id$";

#include "LineGESI.h"

#include "assert.h"
#include <math.h>
#include "pi.h"
#include "hypot.h"

LineGraphEdgeSelfInfo::LineGraphEdgeSelfInfo(const BoxRegion& region,
					     const GraphGC& gc)
    : radius(0), diameter(0), arc_pos(), arc_center(),
      arc_start(0), arc_extend(0),
      anno_pos(), arrow_pos(),
      arrow_angle(0), arrow_alpha(0.0)
{
    // Find edge position
    diameter = gc.selfEdgeDiameter;

    // Make sure edge is still attached to node
    diameter = min(diameter, region.space(X) + region.space(X) / 2);
    diameter = min(diameter, region.space(Y) + region.space(Y) / 2);

    // Be sure we don't make it too small
    diameter = max(diameter, 4);
    radius = (diameter + 1) / 2;
    diameter = radius * 2;

    arc_pos = region.origin();	// Upper left corner of the arc
    arc_center = region.origin(); // Center of the arc
    anno_pos = region.origin();	// Position of annotation
    arc_start = 0;		// Start of the arc (in degrees)
    arc_extend = 270;	        // Extend of the arc (in degrees)

    switch (gc.selfEdgePosition)
    {
    case NorthEast:
	arc_pos  += BoxPoint(region.space(X) - radius, -radius);
	arc_center += BoxPoint(region.space(X), 0);
	anno_pos += BoxPoint(region.space(X) + radius, -radius);
	arc_start = 270;
	break;

    case SouthEast:
	arc_pos  += BoxPoint(region.space(X) - radius,
			     region.space(Y) - radius);
	arc_center += BoxPoint(region.space(X), region.space(Y));
	anno_pos += BoxPoint(region.space(X) + radius,
			     region.space(Y) + radius);
	arc_start = 180;
	break;

    case SouthWest:
	arc_pos  += BoxPoint(-radius, region.space(Y) - radius);
	arc_center += BoxPoint(0, region.space(Y));
	anno_pos += BoxPoint(-radius, region.space(Y) + radius);
	arc_start = 90;
	break;

    case NorthWest:
	arc_pos  += BoxPoint(-radius, -radius);
	arc_center += BoxPoint(0, 0);
	anno_pos += BoxPoint(-radius, -radius);
	arc_start = 0;
	break;
    }

    // Find arrow position and angle
    arrow_angle = 0;
    switch (gc.selfEdgeDirection)
    {
    case Clockwise:
	arrow_angle = 360 - (arc_start + arc_extend + 180) % 360;
	break;

    case Counterclockwise:
	arrow_angle = 360 - (arc_start + 180) % 360;
	break;
    }
    arrow_alpha = 2 * PI * arrow_angle / 360.0;
    
    // Incline arrow a little into the arc
    double cosine = gc.arrowLength / (2.0 * radius);
    double alpha_inclination = (PI / 2.0) - acos(cosine);
    int angle_inclination = 
	- (360 + int((alpha_inclination * 360.0 / (2.0 * PI)))) % 360;

    arrow_pos = region.origin();
    switch (gc.selfEdgeDirection)
    {
    case Clockwise:
	arrow_alpha -= alpha_inclination;
 	arrow_angle -= angle_inclination;
	switch (gc.selfEdgePosition)
	{
	case NorthEast:
	    arrow_pos += BoxPoint(region.space(X), radius);
	    break;

	case SouthEast:
	    arrow_pos += BoxPoint(region.space(X) - radius, region.space(Y));
	    break;

	case SouthWest:
	    arrow_pos += BoxPoint(0, region.space(Y) - radius);
	    break;

	case NorthWest:
	    arrow_pos += BoxPoint(radius, 0);
	    break;
	}
	break;

    case Counterclockwise:
	arrow_alpha += alpha_inclination;
 	arrow_angle += angle_inclination;
	switch (gc.selfEdgePosition)
	{
	case NorthEast:
	    arrow_pos += BoxPoint(region.space(X) - radius, 0);
	    break;

	case SouthEast:
	    arrow_pos += BoxPoint(region.space(X), region.space(Y) - radius);
	    break;

	case SouthWest:
	    arrow_pos += BoxPoint(radius, region.space(Y));
	    break;

	case NorthWest:
	    arrow_pos += BoxPoint(0, radius);
	    break;
	}
	break;
    }

    // Find FIG positions
    fig_pos[0] = region.origin();
    fig_pos[1] = region.origin();
    fig_pos[2] = region.origin();

    int offset = int(sqrt(double(radius * radius) / 2.0));

    switch (gc.selfEdgePosition)
    {
    case NorthEast:
	fig_pos[0] += BoxPoint(region.space(X), radius);
	fig_pos[1] += BoxPoint(region.space(X) + offset, -offset);
	fig_pos[2] += BoxPoint(region.space(X) - radius, 0);
	break;

    case SouthEast:
	fig_pos[0] += BoxPoint(region.space(X) - radius, region.space(Y));
	fig_pos[1] += BoxPoint(region.space(X) + offset, 
			       region.space(Y) + offset);
	fig_pos[2] += BoxPoint(region.space(X), region.space(Y) - radius);
	break;

    case SouthWest:
	fig_pos[0] += BoxPoint(region.space(X), region.space(Y) - radius);
	fig_pos[1] += BoxPoint(-offset, region.space(Y) + offset);
	fig_pos[2] += BoxPoint(region.space(X) + radius, region.space(Y));
	break;

    case NorthWest:
	fig_pos[0] += BoxPoint(radius, 0);
	fig_pos[1] += BoxPoint(-offset, -offset);
	fig_pos[2] += BoxPoint(0, radius);
	break;
    }

    if (gc.selfEdgeDirection == Clockwise)
    {
	BoxPoint pivot = fig_pos[0];
	fig_pos[0] = fig_pos[2];
	fig_pos[2] = pivot;
    }

    // That's all, folks!
}
