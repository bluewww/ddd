// $Id$
// Graph Class

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_Graph_h
#define _DDD_Graph_h

#ifdef __GNUG__
#pragma interface
#endif

#include "assert.h"
#include "GraphGC.h"
#include "GraphNode.h"
#include "GraphEdge.h"
#include "Box.h"
#include "TypeInfo.h"

class Graph {
public:
    DECLARE_TYPE_INFO

private:
    GraphNode *_firstNode;	// circular list (0 if empty)
    GraphEdge *_firstEdge;	// circular list (0 if empty)

protected:
    void addNodes(GraphNode* nodes);
    void addEdges(GraphEdge* edges);
    void addUsedEdges(GraphEdge* edges);

    void removeNode(GraphNode* node);
    void removeEdge(GraphEdge* edge);

    bool haveNode(GraphNode* node) const { return node->graph == this; }
    bool haveEdge(GraphEdge* edge) const { return edge->graph == this; }

    // Needed by copy constructor
    GraphNode *getNode(GraphNode *node, const Graph& graph) const;

    // Copy constructor
    Graph(const Graph& graph);

public:
    // Constructors
    Graph():
	_firstNode(0), _firstEdge(0)
    {}

    // Destructor
    virtual ~Graph();
  
    // Duplication
    virtual Graph *dup() const
    {
	return new Graph (*this);
    }

    // Add Graph
    void operator += (const Graph& g)
    {
	Graph *graph = g.dup();

	if (graph->_firstNode)
	    addNodes(graph->_firstNode);
	if (graph->_firstEdge)
	    addEdges(graph->_firstEdge);

	graph->_firstNode = 0;
	graph->_firstEdge = 0;
	delete graph;
    }

    // Add Node
    void operator += (GraphNode *node)
    {
	assert(node->next == 0);	// node must not be used yet
	assert(node->prev == 0);
	assert(node->graph == 0);

	node->next  = node;		// now it is used
	node->prev  = node;
	node->graph = this;
	addNodes(node);
    }

    // Add Edge
    void operator += (GraphEdge *edge)
    {
	assert(edge->next == 0);	// edge must not be used yet
	assert(edge->prev == 0);
	assert(edge->graph == 0);

	edge->next  = edge;		// now it is used
	edge->prev  = edge;
	edge->graph = this;
	addEdges(edge);
    }

    // Remove Node
    void operator -= (GraphNode *node)
    {
	removeNode(node);
    }

    // Remove Edge
    void operator -= (GraphEdge *edge)
    {
	removeEdge(edge);
    }

    // Iteration on all nodes and edges
    // simulate a 0-terminated list
    GraphNode *firstNode() const { return _firstNode; } 
    GraphNode *nextNode(GraphNode *ref) const
    {
	return ref->next == _firstNode ? 0 : ref->next;
    }

    // Same, but iterate on non-hidden nodes only
    GraphNode *firstVisibleNode() const;
    GraphNode *nextVisibleNode(GraphNode *ref) const;

    // Same, but iterate on edges
    GraphEdge *firstEdge() const { return _firstEdge; }
    GraphEdge *nextEdge(GraphEdge *ref) const
    {
	return ref->next == _firstEdge ? 0 : ref->next;
    }

    // Same, but iterate on non-hidden edges only
    GraphEdge *firstVisibleEdge() const;
    GraphEdge *nextVisibleEdge(GraphEdge *ref) const;

    // Change position in node list
    void makeNodeFirst(GraphNode *node);
    void makeNodeLast(GraphNode *node);

    // Change position in edge list
    void makeEdgeFirst(GraphEdge *edge);
    void makeEdgeLast(GraphEdge *edge);

    // Drawing
    void draw(Widget w, const BoxRegion& exposed, const GraphGC& gc) const;
    void draw(Widget w, const BoxRegion& exposed) const
    {
	draw(w, exposed, GraphGC());
    }
    void draw(Widget w) const
    {
	draw(w, BoxRegion(BoxPoint(0, 0), BoxSize(INT_MAX, INT_MAX)));
    }

    // Printing
    void _print(ostream& os, const GraphGC& gc) const;
    void _printHeader(ostream& os, const GraphGC& gc) const
    {
	Box::_printHeader(os, region(gc), *gc.printGC);
    }
    void _printTrailer(ostream& os, const GraphGC& gc) const
    {
	Box::_printTrailer(os, region(gc), *gc.printGC);
    }

    // Custom printing function
    void print(ostream& os, const GraphGC& gc = GraphGC()) const
    {
	_printHeader(os, gc);
	_print(os, gc);
	_printTrailer(os, gc);
    }

    // Total Region
    BoxRegion region(const GraphGC& gc) const;

    // Representation invariant
    virtual bool OK() const;
};

// I/O
extern ostream& operator << (ostream& s, const Graph& g);

#endif // _DDD_Graph_h
// DON'T ADD ANYTHING BEHIND THIS #endif
