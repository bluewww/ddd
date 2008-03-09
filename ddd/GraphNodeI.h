// $Id$
// GraphNode class structure

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

#ifndef _DDD_GraphNodeI_h
#define _DDD_GraphNodeI_h

#include "UniqueId.h"
#include "Box.h"
#include "bool.h"
#include "GraphGC.h"
#include <sstream>

class GraphEdge;
class Graph;

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

    GraphNode& operator = (const GraphNode&);

protected:
    // Copy Constructor
    GraphNode(const GraphNode& node):
	_id(),
	_selected(node._selected),
	_hidden(node._hidden),
	_redraw(node._redraw),
	_firstFrom(0),
	_firstTo(0),
	next(0),
	prev(0),
	graph(0),
	count(0)
    {}

public:
    int count;			// #predecessors (used in layout algorithms)

    // Constructor
    GraphNode():
	_id(), _selected(false), _hidden(false), _redraw(false),
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

    // Resources
    virtual const BoxPoint&  pos() const                              = 0;
    virtual const BoxRegion& region(const GraphGC& gc) const          = 0;
    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const = 0;
    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const = 0;
    virtual string str() const 
    { 
	std::ostringstream os;
	os << id();
	return string(os);
    }

    // Shortcuts
    const BoxPoint& origin(const GraphGC& gc) const
    {
	return region(gc).origin();
    }
    const BoxSize& space(const GraphGC& gc) const
    {
	return region(gc).space();
    }

    // Types
    virtual bool isHint() const { return false; }

    // Move
    virtual void moveTo(const BoxPoint& newPos) = 0;

    // Compute position for ORIGIN
    virtual BoxPoint originToPos(const BoxPoint& origin, const GraphGC&) const
    {
	return origin;
    }

    // Draw
    virtual void draw(Widget, 
		      const BoxRegion&, 
		      const GraphGC&) const
    {
	// Default: do nothing
    }

    // Custom drawing functions
    void draw(Widget w, const BoxRegion& exposed) const
    {
	draw(w, exposed, GraphGC());
    }

    void draw(Widget w) const
    {
	draw(w, BoxRegion(BoxPoint(0, 0), BoxSize(INT_MAX, INT_MAX)),
	    GraphGC());
    }

    // Printing
    virtual void _print(std::ostream&, const GraphGC&) const
    {
	// Default: do nothing
    }

    // Representation invariant
    virtual bool OK() const;
};

inline std::ostream& operator << (std::ostream& s, const GraphNode& node)
{
    return s << node.str();
}

#endif // _DDD_GraphNodeI_h
// DON'T ADD ANYTHING BEHIND THIS #endif
