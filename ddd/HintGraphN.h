// $Id$  -*- C++ -*-
// HintGraphNode class

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

#ifndef _Nora_HintGraphNode_h
#define _Nora_HintGraphNode_h

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

public:
    HintGraphNode(BoxPoint& initialPos = BoxPoint()):
       RegionGraphNode(initialPos, BoxSize(0,0))
    {}

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
	return r = BoxRegion(pos() - gc.hintSize / 2, gc.hintSize);
    }
};

#endif // _Nora_HintGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
