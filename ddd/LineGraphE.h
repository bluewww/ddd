// $Id$
// LineGraphEdge class: GraphEdge with drawing capabilities

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

#ifndef _DDD_LineGraphEdge_h
#define _DDD_LineGraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif

#include "GraphEdge.h"
#include "Box.h"
#include "EdgeA.h"
#include "explicit.h"

enum Side { North = 1, South = 2, East = 4, West = 8 };

class LineGraphEdge: public GraphEdge {
public:
    DECLARE_TYPE_INFO

private:
    EdgeAnnotation *_annotation; // The annotation to use

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

    // Print self edge.
    virtual void printSelf(ostream& os, const GraphGC &gc) const;

    // Find annotation position.
    virtual BoxPoint annotationPosition(const GraphGC &gc) const;


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
    LineGraphEdge(GraphNode *f, GraphNode *t, EdgeAnnotation *ann = 0):
	GraphEdge(f, t), _annotation(ann)
    {}

    // Destructor
    virtual ~LineGraphEdge()
    {
	delete _annotation;
    }

    // Resources
    void set_annotation(EdgeAnnotation *a)
    {
	if (a != _annotation)
	{
	    delete _annotation;
	    _annotation = a;
	}
    }

    EdgeAnnotation *annotation() const { return _annotation; }

protected:
    // Copy Constructor
    LineGraphEdge(const LineGraphEdge &edge):
	GraphEdge(edge), _annotation(0)
    {
	if (edge.annotation() != 0)
	    set_annotation(edge.annotation()->dup());
    }

public:
    // Printing
    void _print(ostream& os, const GraphGC &gc) const;

    GraphEdge *dup() const
    {
	return new LineGraphEdge(*this);
    }

    // Region occupied by edge - if none, BoxRegion()
    virtual BoxRegion region(const GraphGC&) const;
};

#endif // _DDD_LineGraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
