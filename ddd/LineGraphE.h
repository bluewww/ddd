// $Id$
// LineGraphEdge class: GraphEdge with drawing capabilities

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_LineGraphEdge_h
#define _DDD_LineGraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphEdge.h"
#include "Box.h"

enum Side { North = 1, South = 2, East = 4, West = 8 };

class LineGraphEdge: public GraphEdge {
public:
    DECLARE_TYPE_INFO

protected:
    // Find line from region B1 centered around C1 to region B2 centered
    // around C2.  Resulting line shall be drawn from P1 to P2.
    static void findLine(const BoxPoint& c1, const BoxPoint& c2, 
			 const BoxRegion& b1, const BoxRegion& b2,
			 BoxPoint& p1, BoxPoint& p2, 
			 const GraphGC& gc);

    // Draw.
    virtual void _draw(Widget w, 
		       const BoxRegion& exposed, 
		       const GraphGC& gc) const;

    // Draw line.
    virtual void drawLine(Widget w,
			  const BoxRegion& exposed,
			  const GraphGC& gc) const;

    // Draw self edge.
    virtual void drawSelf(Widget w,
			  const BoxRegion& exposed,
			  const GraphGC& gc) const;

    // Draw arrow head at POS.
    virtual void drawArrowHead(Widget w,
			       const BoxRegion& exposed,
			       const GraphGC& gc,
			       const BoxPoint& pos,
			       double alpha) const;

    // Copy Constructor
    LineGraphEdge(const LineGraphEdge &edge):
	GraphEdge(edge)
    {}

private:
    // Clip point P to side SIDE of region B.
    static void moveToSide  (const BoxRegion& b, int side,
			     BoxPoint& p, const BoxPoint& c);

    // Clip point P to side SIDE of region B centered around C.
    static void clipToSide  (const BoxRegion& b, int side,
			     BoxPoint& p, const BoxPoint& c);

    // Clip point P to side SIDE of region B centered around C.  Assume
    // that B contains a circle.
    static void clipToCircle(const BoxRegion& b, int side, 
			     BoxPoint& p, const BoxPoint& c);

public:
    // Constructor
    LineGraphEdge(GraphNode *f, GraphNode *t):
	GraphEdge(f, t)
    {}

    // Destructor
    virtual ~LineGraphEdge() {}

    // Printing
    void _print(ostream& os, const GraphGC &gc) const;

    GraphEdge *dup() const
    {
	return new LineGraphEdge(*this);
    }
};

#endif // _DDD_LineGraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
