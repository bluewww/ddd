// $Id$
// LineGraphEdge class: GraphEdge with drawing capabilities

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

#ifndef _Nora_LineGraphEdge_h
#define _Nora_LineGraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphEdge.h"
#include "Box.h"

class LineGraphEdge: public GraphEdge {
public:
    DECLARE_TYPE_INFO

protected:
    // Find line between b1 and b2 in p1/p2
    static void findLine(BoxPoint& c1, BoxPoint& c2, 
			 BoxRegion& b1, BoxRegion& b2,
			 BoxPoint& p1, BoxPoint& p2, 
			 const GraphGC& gc);

    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& exposed, 
		       const GraphGC& gc) const;

public:
    // Constructor
    LineGraphEdge(GraphNode *f, GraphNode *t):
	GraphEdge(f, t)
    {}
};

#endif
