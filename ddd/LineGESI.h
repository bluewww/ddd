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

#ifndef _DDD_LineGraphEdgeSelfInfo_h
#define _DDD_LineGraphEdgeSelfInfo_h

#ifdef __GNUG__
#pragma interface
#endif

#include "BoxRegion.h"
#include "GraphGC.h"

// Where to draw `self' edges
struct LineGraphEdgeSelfInfo {
    int radius;			// Radius of self edge
    int diameter;		// Diameter
    BoxPoint arc_pos;		// Upper left corner
    BoxPoint arc_center;	// Center
    int arc_start;		// Arc start (0..360)
    int arc_extend;		// Arc extend (0..360)
    BoxPoint anno_pos;		// Position of annotation
    BoxPoint arrow_pos;		// Position of arrow
    int arrow_angle;		// Arrow angle (0..360)
    double arrow_alpha;		// Arrow angle (0.0..2*PI)

    // Constructor
    explicit LineGraphEdgeSelfInfo(const BoxRegion& region, const GraphGC& gc);
};

#endif // _DDD_LineGraphEdgeSelfInfo_h
// DON'T ADD ANYTHING BEHIND THIS #endif
