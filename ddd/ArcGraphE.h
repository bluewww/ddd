// $Id$ -*- C++ -*-
// Arc edges

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ArcGraphEdge_h
#define _DDD_ArcGraphEdge_h

#include "LineGraphE.h"

class ArcGraphEdge: public LineGraphEdge {
public:
    DECLARE_TYPE_INFO

protected:
    // Copy Constructor
    ArcGraphEdge(const ArcGraphEdge& edge)
	: LineGraphEdge(edge)
    {}

    // Draw line (or arc).
    void drawLine(Widget w,
		  const BoxRegion& exposed,
		  const GraphGC& gc) const;

    // Compute the center of the arc enclosing P1, P2, and P3.
    // Return false iff P1, P2, and P3 form a line (i.e. there is no arc)
    static bool center(const BoxPoint& p1, const BoxPoint& p2, 
		       const BoxPoint& p3, double& x, double& y);

    // Joint function for drawing and printing
    void makeLine(Widget w,
		  const BoxRegion& exposed,
		  std::ostream& os,
		  const GraphGC& gc) const;

public:
    // Constructor
    ArcGraphEdge(GraphNode *from, GraphNode *to, EdgeAnnotation *ann = 0)
	: LineGraphEdge(from, to, ann)
    {}

    // Destructor
    virtual ~ArcGraphEdge() {}

    // Printing
    void _print(std::ostream& os, const GraphGC &gc) const;

    // Duplicator
    GraphEdge *dup() const
    {
	return new ArcGraphEdge(*this);
    }
};

#endif // _DDD_ArcGraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
