// $Id$  -*- C++ -*-
// HintGraphNode class

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

#ifndef _DDD_HintGraphNode_h
#define _DDD_HintGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif

#include "RegionGN.h"

class HintGraphNode: public RegionGraphNode {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed,
			   const GraphGC& gc) const;

    // Copy Constructor
    HintGraphNode(const HintGraphNode &node):
	RegionGraphNode(node)
    {}

public:
    // Constructor
    HintGraphNode(const BoxPoint& initialPos = BoxPoint()):
       RegionGraphNode(initialPos, BoxSize(0,0))
    {}

    // Destructor
    virtual ~HintGraphNode() {}

    GraphNode *dup() const
    {
	return new HintGraphNode(*this);
    }

    virtual bool isHint() const { return true; }

    virtual const BoxRegion& region(const GraphGC& gc) const
    {
	if (gc.drawHints)
	    return sensitiveRegion(gc);
	else
	    return RegionGraphNode::region(gc);
    }

    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const
    {
	static BoxRegion r;

	// return a size-pixel area around the current position
	r = BoxRegion(pos() - gc.hintSize / 2, gc.hintSize);
	return r;
    }

    // Print
    virtual void _print(ostream&, const GraphGC&) const 
    {
	// Do nothing
    }
};

#endif // _DDD_HintGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
