// $Id$
// LineGraphEdge class: GraphEdge with drawing capabilities

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

#ifndef _DDD_LineGraphEdge_h
#define _DDD_LineGraphEdge_h

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

    // Copy Constructor
    LineGraphEdge(const LineGraphEdge &edge):
	GraphEdge(edge)
    {}

public:
    // Constructor
    LineGraphEdge(GraphNode *f, GraphNode *t):
	GraphEdge(f, t)
    {}

    GraphEdge *dup() const
    {
	return new LineGraphEdge(*this);
    }
};

#endif
