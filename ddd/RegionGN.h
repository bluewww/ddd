// $Id$
// RegionGraphNode class: PosGraphNode with rectangular region
// centered around position

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

#ifndef _Nora_RegionGraphNode_h
#define _Nora_RegionGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "PosGraphN.h"
#include "BoxRegion.h"

class RegionGraphNode: public PosGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    BoxRegion _region;		// region

protected:		
    // draw region
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const = 0;

    // center around position
    void center();

public:
    // Constructor
    RegionGraphNode(const BoxPoint& initialPos = BoxPoint(),
	const BoxSize& initialSize = NoSize):
	PosGraphNode(initialPos),
	_region(initialPos, initialSize)
    {
	center();
    }

    // Move
    virtual void moveTo(const BoxPoint& newPos)
    {
	PosGraphNode::moveTo(newPos);
	center();
    }
    // Assign new size
    virtual void resize(const BoxSize& newSize)
    {
	_region.space() = newSize;
	center();
    }

    // Draw
    virtual void draw(Widget w,
		      const BoxRegion& exposed,
		      const GraphGC& gc) const;

    // Print
    virtual void _print(ostream& os, const GraphGC& gc) const;

    // Attributes
    // Region occupied by this node
    virtual const BoxRegion& region(const GraphGC& gc) const
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

#endif
