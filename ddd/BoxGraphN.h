// $Id$
// BoxGraphNode class: RegionGraphNode with box

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _Nora_BoxGraphNode_h
#define _Nora_BoxGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "RegionGN.h"
#include "Box.h"
#include "MarkBox.h"

class BoxGraphNode: public RegionGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    Box *_box;		 // the box
    MarkBox *_highlight; // box to be highlighted when selected

protected:
    // Draw
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const;

public:
    // Constructor
    BoxGraphNode(Box *b, const BoxPoint& initialPos = BoxPoint(), 
		 MarkBox *h = 0):
	RegionGraphNode(initialPos, b->size()),
	_box(b->link()),
        _highlight(h)
    {}

    // Destructor
    virtual ~BoxGraphNode()
    {
	if (_highlight)
	    _highlight->unlink();
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
    void setBox(Box *b)
    {
	setHighlight(0);

	Box *old = _box;
	_box = b->link();
	old->unlink();
	resize(b->size());
    }


    // Print
    virtual void _print(ostream& os, const GraphGC& gc) const;
};

#endif

