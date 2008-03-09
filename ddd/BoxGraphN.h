// $Id$
// BoxGraphNode class: RegionGraphNode with box

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

#ifndef _DDD_BoxGraphNode_h
#define _DDD_BoxGraphNode_h

#include "RegionGN.h"
#include "Box.h"
#include "MarkBox.h"

class BoxGraphNode: public RegionGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    Box *_box;		 // the box
    MarkBox *_highlight; // box to be highlighted when selected

    BoxGraphNode& operator = (const BoxGraphNode&);

protected:
    // Draw
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const;

    // MARK is a MarkBox in SRC.  Find equivalent box in DUP.
    MarkBox *find_mark(Box *dup, Box *src, Box *mark);

    // Copy Constructor
    BoxGraphNode(const BoxGraphNode& node);

public:
    // Constructor
    BoxGraphNode(Box *b, const BoxPoint& initialPos = BoxPoint(), 
		 MarkBox *h = 0)
	: RegionGraphNode(initialPos, b->size()),
	  _box(b->link()),
	  _highlight(h)
    {}

    BoxGraphNode()
	: RegionGraphNode(),
	  _box(0),
	  _highlight(0)
    {}

    GraphNode *dup() const
    {
	return new BoxGraphNode(*this);
    }

    // Destructor
    virtual ~BoxGraphNode()
    {
	if (_box)
	    _box->unlink();
    }

    // Attributes
    Box *box() const           { return _box; }
    MarkBox *highlight() const { return _highlight; }
    virtual string str() const { return box()->str(); }

    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const
    { 
	if (_highlight)
	    return _highlight->__region();
	else
	    return RegionGraphNode::highlightRegion(gc);
    }

    // Modify
    // Set the highlight box.
    // The highlight box must be a child of the displayed box.
    void setHighlight(MarkBox *b = 0)
    {
	_highlight = b;
    }

    // Set the box.
    void setBox(Box *b);

    // Print
    virtual void _print(std::ostream& os, const GraphGC& gc) const;
};

#endif // _DDD_BoxGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
