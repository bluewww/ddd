// $Id$
// PosGraphNode class: GraphNode with position

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

#ifndef _DDD_PosGraphNode_h
#define _DDD_PosGraphNode_h

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
