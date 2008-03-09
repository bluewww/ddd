// $Id$
// RegionGraphNode class: PosGraphNode with rectangular region
// centered around position

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_RegionGraphNode_h
#define _DDD_RegionGraphNode_h

#include "PosGraphN.h"
#include "BoxRegion.h"

class RegionGraphNode: public PosGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    BoxRegion _region;		// The region occupied by this node

protected:		
    // Draw region
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const = 0;

    // Center around position
    void center();

    // Copy Constructor
    RegionGraphNode(const RegionGraphNode& node):
        PosGraphNode(node),
        _region(node._region)
    {}

public:
    // Constructor
    RegionGraphNode(const BoxPoint& initialPos = BoxPoint(),
	const BoxSize& initialSize = BoxSize()):
	PosGraphNode(initialPos),
	_region(initialPos, initialSize)
    {
	center();
    }

    // Destructor
    virtual ~RegionGraphNode() {}

    // Move
    virtual void moveTo(const BoxPoint& newPos)
    {
	PosGraphNode::moveTo(newPos);
	center();
    }

    // Size change handler: return true iff ok
    static bool (*ResizeCB)(RegionGraphNode *node, 
			    const BoxSize& newSize);

    // Assign new size
    virtual void resize(const BoxSize& newSize)
    {
	if (newSize != _region.space() && ResizeCB(this, newSize))
	{
	    _region.space() = newSize;
	    center();
	}
    }

    // Compute position for ORIGIN
    virtual BoxPoint originToPos(const BoxPoint& origin, const GraphGC&) const;

    // Draw
    virtual void draw(Widget w,
		      const BoxRegion& exposed,
		      const GraphGC& gc) const;

    // Print
    virtual void _print(std::ostream& os, const GraphGC& gc) const;

    // Attributes
    // Region occupied by this node
    virtual const BoxRegion& region(const GraphGC&) const
    { 
	return _region; 
    }

    // Region to be highlighted
    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const 
    { 
	return region(gc); 
    }

    // Sensitive region
    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const 
    { 
	return region(gc); 
    }
};

#endif // _DDD_RegionGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
