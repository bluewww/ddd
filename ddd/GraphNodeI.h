// $Id$
// GraphNode class structure

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_GraphNodeI_h
#define _DDD_GraphNodeI_h

#ifdef __GNUG__
#pragma interface
#endif


#include "UniqueId.h"
#include "Box.h"
#include "bool.h"
#include "GraphGC.h"
#include <strstream.h>

class GraphNode {
public:
    DECLARE_TYPE_INFO

    friend class Graph;
    friend class GraphEdge;

private:
    UniqueId _id;		// Id
    bool _selected;		// Flag: selected by user?
    bool _hidden;		// Flag: not shown?
    bool _redraw;		// Flag: needs redrawing

    // maintained by GraphEdge
    GraphEdge *_firstFrom;	// first Edge e with e->from() == this
    GraphEdge *_firstTo;	// first Edge e with e->to()   == this

    // maintained by Graph
    GraphNode *next;		// For collectors
    GraphNode *prev;		// For collectors
    Graph *graph;		// For collectors

protected:
    // Copy Constructor
    GraphNode(const GraphNode& node):
	_selected(node._selected),
	_hidden(node._hidden),
	_redraw(node._redraw),
	_firstFrom(0),
	_firstTo(0),
	next(0),
	prev(0),
	graph(0)
    {}

public:
    int count;			// #predecessors (used in layout algorithms)

    // Constructor
    GraphNode():
	_selected(false), _hidden(false), _redraw(false),
	_firstFrom(0), _firstTo(0), next(0), prev(0), graph(0), count(0)
    {}

    // Destructor
    virtual ~GraphNode() {}

    // Duplication
    virtual GraphNode *dup() const = 0;

    // Resources
    unsigned long id() const { return _id; }
    bool& selected()      { return _selected; }
    bool selected() const { return _selected; }
    bool& hidden()        { return _hidden; }
    bool hidden() const   { return _hidden; }
    bool& redraw()        { return _redraw; }
    bool redraw() const   { return _redraw; }

    // Iteration on all edges starting here
    GraphEdge *firstFrom() const { return _firstFrom; }
    GraphEdge *nextFrom(GraphEdge *ref) const;

    // Iteration on all edges ending here
    GraphEdge *firstTo() const { return _firstTo; }
    GraphEdge *nextTo(GraphEdge *ref) const;

    // resources
    virtual const BoxPoint&  pos() const                              = 0;
    virtual const BoxRegion& region(const GraphGC& gc) const          = 0;
    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const = 0;
    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const = 0;
    virtual string str() const 
    { 
	ostrstream os;
	os << id();
	return string(os);
    }

    // types
    virtual bool isHint() const { return false; }

    // move
    virtual void moveTo(const BoxPoint& newPos) = 0;

    // draw
    virtual void draw(Widget, 
		      const BoxRegion&, 
		      const GraphGC&) const
    {
	// default: do nothing
    }

    // custom drawing functions
    void draw(Widget w, const BoxRegion& exposed) const
    {
	draw(w, exposed, GraphGC());
    }

    void draw(Widget w) const
    {
	draw(w, BoxRegion(BoxPoint(0, 0), BoxSize(INT_MAX, INT_MAX)),
	    GraphGC());
    }

    // printing
    virtual void _print(ostream&, const GraphGC&) const
    {
	// default: do nothing
    }

    // representation invariant
    virtual bool OK() const;
};

inline ostream& operator << (ostream& s, GraphNode& node)
{
    return s << node.str();
}

#endif // _DDD_GraphNodeI_h
// DON'T ADD ANYTHING BEHIND THIS #endif
