// $Id$
// PosGraphNode class: GraphNode with position

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

#ifndef _DDD_PosGraphNode_h
#define _DDD_PosGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphNode.h"
#include "BoxPoint.h"

class PosGraphNode: public GraphNode {
public:
    DECLARE_TYPE_INFO

private:
    BoxPoint _pos;	// position

protected:
    // Copy Constructor
    PosGraphNode(const PosGraphNode& node):
	GraphNode(node),
	_pos(node._pos)
    {}

public:
    // Constructor
    PosGraphNode(const BoxPoint& initialPos = BoxPoint()):
	_pos(initialPos)
    {}

    // Destructor
    virtual ~PosGraphNode() {}

    // Position change handler: return true iff ok
    static bool (*MoveCB)(PosGraphNode *node, const BoxPoint& newPos);

    // Move
    virtual void moveTo(const BoxPoint& newPos)
    {
	if (_pos != newPos && MoveCB(this, newPos))
	    _pos = newPos;
    }

    // Attributes
    virtual const BoxPoint& pos() const
    {
	return _pos;
    }
};

#endif // _DDD_PosGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
