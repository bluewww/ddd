// $Id$
// GraphEdge Class

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

#ifndef _DDD_GraphEdge_h
#define _DDD_GraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif

#include "UniqueId.h"
#include "Box.h"
#include "bool.h"
#include "GraphGC.h"
#include "TypeInfo.h"

class GraphEdge {
public:
    DECLARE_TYPE_INFO

    friend class Graph;
    friend class GraphNode;

private:
    UniqueId _id;		// Id

    GraphNode *_from;		// Source
    GraphNode *_to;		// Target

    bool _hidden;		// Flag: is edge hidden?

    // maintained by GraphNode
    // circular lists
    GraphEdge *_nextFrom;	// next Edge e with e->_from == this->_from
    GraphEdge *_nextTo;		// next Edge e with e->_to   == this->_to

    GraphEdge *_prevFrom;	// prev Edge e with e->_from == this->_from
    GraphEdge *_prevTo;		// prev Edge e with e->_to   == this->_to

    // maintained by Graph
    GraphEdge *next;		// For collectors
    GraphEdge *prev;
    Graph *graph;

protected:
    // Graphics
    virtual void _draw(Widget w, 
		       const BoxRegion& exposed, 
		       const GraphGC& gc) const = 0;
    
    // Enqueue in node lists
    void enqueue();
    
    // Dequeue from node lists
    void dequeue();

    // Copy Constructor
    GraphEdge(const GraphEdge& edge):
        _from(0),
        _to(0),
	_hidden(edge._hidden),
        _nextFrom(0),
        _nextTo(0),
        _prevFrom(0),
        _prevTo(0),
        next(0),
        prev(0),
	graph(0)
    {}

public:
    // Constructor
    GraphEdge(GraphNode *f, GraphNode *t):
        _from(f), _to(t), 
        _hidden(false),
        _nextFrom(0), _nextTo(0),
        _prevFrom(0), _prevTo(0),
        next(0), prev(0), graph(0)
    {}

    // Destructor
    virtual ~GraphEdge() {}

    // Duplication
    virtual GraphEdge *dup() const = 0;

    // Resources
    GraphNode *from() const 	{ return _from; }
    GraphNode *to() const 	{ return _to; }
    unsigned long id() const	{ return _id; }

    bool& hidden()           { return _hidden; }
    bool hidden() const      { return _hidden; }

    // Graphics
    void draw(Widget w, const BoxRegion& exposed = BoxRegion(BoxPoint(0, 0),
	BoxSize(INT_MAX, INT_MAX)), const GraphGC& gc = GraphGC()) const
    {
	if (!_hidden)
	    _draw(w, exposed, gc);
    }

    // Printing
    void _print(ostream& os, const GraphGC& gc) const;

    // representation invariant
    virtual bool OK() const;
};

extern ostream& operator << (ostream& s, GraphEdge& e);

#endif // _DDD_GraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
