// $Id$ -*- C++ -*-
// Determine position of `self' edges

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char LineGraphEdgeSelfInfo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "LineGESI.h"

#include "assert.h"
#include <math.h>
#include "pi.h"
#include "hypot.h"

LineGraphEdgeSelfInfo::LineGraphEdgeSelfInfo(const BoxRegion& region,
					     const GraphGC& gc)
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

    // That's all, folks!
}
