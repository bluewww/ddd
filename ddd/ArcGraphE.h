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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_ArcGraphEdge_h
#define _DDD_ArcGraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif

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

public:
    // Constructor
    ArcGraphEdge(GraphNode *from, GraphNode *to)
	: LineGraphEdge(from, to)
    {}

    // Destructor
    virtual ~ArcGraphEdge() {}

    // Printing
    void _print(ostream& os, const GraphGC &gc) const;

    // Duplicator
    GraphEdge *dup() const
    {
	return new ArcGraphEdge(*this);
    }
};

#endif // _DDD_ArcGraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
