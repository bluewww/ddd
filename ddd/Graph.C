// $Id$
// Graph Class

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

char Graph_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "Graph.h"
#include "assert.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

DEFINE_TYPE_INFO_0(Graph)

// Destructor
Graph::~Graph()
{
    GraphNode *n = firstNode();
    while (n != 0)
    {
	GraphNode *next = nextNode(n);
	delete n;
	n = next;
    }

    GraphEdge *e = firstEdge();
    while (e != 0)
    {
	GraphEdge *next = nextEdge(e);
	delete e;
	e = next;
    }
}

// Copy Constructor
Graph::Graph(const Graph &org_graph)
    : _firstNode(0), _firstEdge(0)
{
    GraphNode *node, *new_node; 

    // Copy Nodes
    node = org_graph._firstNode;
   
    if (node != 0)
    {
	do
	{
	    new_node = node->dup();  // copy node
	    assert(new_node->next == 0);
	    assert(new_node->prev == 0);
	    assert(new_node->graph == 0);
	    new_node->next  = new_node;	     
	    new_node->prev  = new_node;
	    new_node->graph = this;
	    addNodes(new_node); // add new_node to graph
	    node = node->next;

	} while(node != org_graph._firstNode);
    }
     
    GraphEdge *edge, *new_edge;
    
    // Copy Edges
    edge = org_graph._firstEdge;
    
    if (edge != 0)
    {
	do {
	    new_edge = edge->dup();
	    assert(new_edge->next == 0);	// edge must not be used yet
	    assert(new_edge->prev == 0);
	    assert(new_edge->graph == 0);
	    new_edge->next  = new_edge;		// now it is used
	    new_edge->prev  = new_edge;
	    new_edge->graph = this;
	    addUsedEdges(new_edge); // because of "enqueue" in addEdges()
	    edge = edge->next;

	} while(edge != org_graph._firstEdge);
    }

    if (_firstEdge != 0)
    {
	// equal edge in original graph 
	GraphEdge *org_edge = org_graph._firstEdge;
    	edge = _firstEdge;
	GraphNode *from_node, *to_node;     
     
	// setup source and target of edges
	do { 
            // setup source node
	    from_node = getNode(org_edge->_from, org_graph);
	    edge->_from = from_node;

	    // setup target node
	    to_node = getNode(org_edge->_to, org_graph);	
	    edge->_to = to_node;
	
	    edge = edge->next;
	    org_edge = org_graph.nextEdge(org_edge);

	} while(edge != _firstEdge);

	// Enqueue edges
	edge = _firstEdge; 

	do{
	    edge->enqueue();
	    edge = edge->next;
	} while(edge != _firstEdge);
    }
}
  


// Add Nodes
void Graph::addNodes(GraphNode *nodes)
{
    // Add Nodes
    if (_firstNode == 0)
	_firstNode = nodes;
    else
    {
	// setup next chain
	_firstNode->prev->next = nodes;
	nodes->prev->next = _firstNode;

	// setup prev chain
	GraphNode *old_prev = nodes->prev;
	nodes->prev = _firstNode->prev;
	_firstNode->prev = old_prev;
    }
}

// Add Edges
void Graph::addEdges(GraphEdge *edges)
{
    // Enqueue edges
    GraphEdge *e = edges; 
    do {
	e->enqueue();
	e = e->next;
    } while (e != edges);
	
    // Add edges
    if (_firstEdge == 0)
	_firstEdge = edges;
    else
    {
	// setup next chain
	_firstEdge->prev->next = edges;
	edges->prev->next = _firstEdge;

	// setup prev chain
	GraphEdge *old_prev = edges->prev;
	edges->prev = _firstEdge->prev;
	_firstEdge->prev = old_prev;
    }
}

// Add used Edges, i.e. add edges of a graph
void Graph::addUsedEdges(GraphEdge *edges)
{
    // Add edges
    if (_firstEdge == 0)
	_firstEdge = edges;
    else
    {
	// setup next chain
	_firstEdge->prev->next = edges;
	edges->prev->next = _firstEdge;

	// setup prev chain
	GraphEdge *old_prev = edges->prev;
	edges->prev = _firstEdge->prev;
	_firstEdge->prev = old_prev;
    }
}

// Make NODE first node in node list
void Graph::makeNodeFirst(GraphNode *node)
{
    if (!haveNode(node))
	return;
    if (node == _firstNode)
	return;

    // Chain out NODE
    node->prev->next = node->next;
    node->next->prev = node->prev;

    // Chain in at FIRSTNODE.
    node->next = _firstNode;
    node->prev = _firstNode->prev;

    node->next->prev = node;
    node->prev->next = node;

    // Have FIRSTNODE point at NODE.
    _firstNode = node;

    assert(OK());
}

// Make NODE last node in node list
void Graph::makeNodeLast(GraphNode *node)
{
    if (!haveNode(node))
	return;
    if (node == _firstNode->prev)
	return;

    // Chain out NODE
    node->prev->next = node->next;
    node->next->prev = node->prev;
    if (_firstNode == node)
	_firstNode = node->next;

    // Chain in at FIRSTNODE.
    node->next = _firstNode;
    node->prev = _firstNode->prev;

    node->next->prev = node;
    node->prev->next = node;

    // Have FIRSTNODE point at NODE's successor.
    _firstNode = node->next;

    assert(OK());
}

// Make EDGE first edge in edge list
void Graph::makeEdgeFirst(GraphEdge *edge)
{
    if (!haveEdge(edge))
	return;
    if (edge == _firstEdge)
	return;

    // Chain out EDGE
    edge->prev->next = edge->next;
    edge->next->prev = edge->prev;

    // Chain in at FIRSTEDGE.
    edge->next = _firstEdge;
    edge->prev = _firstEdge->prev;

    edge->next->prev = edge;
    edge->prev->next = edge;

    // Have FIRSTEDGE point at EDGE.
    _firstEdge = edge;

    assert(OK());
}

// Make EDGE last edge in edge list
void Graph::makeEdgeLast(GraphEdge *edge)
{
    if (!haveEdge(edge))
	return;
    if (edge == _firstEdge->prev)
	return;

    // Chain out EDGE
    edge->prev->next = edge->next;
    edge->next->prev = edge->prev;
    if (_firstEdge == edge)
	_firstEdge = edge->next;

    // Chain in at FIRSTEDGE.
    edge->next = _firstEdge;
    edge->prev = _firstEdge->prev;

    edge->next->prev = edge;
    edge->prev->next = edge;

    // Have FIRSTEDGE point at EDGE's successor.
    _firstEdge = edge->next;

    assert(OK());
}

// Remove Node
void Graph::removeNode(GraphNode *node)
{
    if (!haveNode(node))
	return;

    GraphEdge *e;

    // Remove edges
    while ((e = node->firstFrom()) != 0)
	removeEdge(e);
    while ((e = node->firstTo()) != 0)
	removeEdge(e);

    if (node == _firstNode)
	_firstNode = node->next;

    if (node == _firstNode)
    {
	assert(node->prev == node);
	assert(node->next == node);
	_firstNode = 0;
    }
    else
    {
	// remove node from next and prev chain
	node->prev->next = node->next;
	node->next->prev = node->prev;
    }

    node->next  = 0;
    node->prev  = 0;
    node->graph = 0;
}

// Remove Edge
void Graph::removeEdge(GraphEdge *edge)
{
    if (!haveEdge(edge))
	return;

    edge->dequeue();

    if (edge == _firstEdge)
	_firstEdge = edge->next;

    if (edge == _firstEdge)
    {
	assert(edge->prev == edge);
	assert(edge->next == edge);
	_firstEdge = 0;
    }
    else
    {
	// remove edge from next and prev chain
	edge->prev->next = edge->next;
	edge->next->prev = edge->prev;
    }

    edge->next  = 0;
    edge->prev  = 0;
    edge->graph = 0;
}


// Get the equal node to "org_node"
// needed by the Copy-Constructor
GraphNode *Graph::getNode(GraphNode *org_node, const Graph& org_graph) const
{
    GraphNode *search_node = org_graph._firstNode;
    GraphNode *dup_node = _firstNode;
     
     while (search_node != org_node)
     {
	 dup_node = dup_node->next;
	 search_node = search_node->next;
     }
 
     return dup_node;
}

// Draw
void Graph::draw(Widget w, const BoxRegion& exposed, const GraphGC& _gc) const
{
    GraphGC gc(_gc);

    // get default gcs
    if (gc.nodeGC   == 0)
	gc.nodeGC   = DefaultGCOfScreen(XtScreen(w));
    if (gc.edgeGC   == 0)
	gc.edgeGC   = DefaultGCOfScreen(XtScreen(w));
    if (gc.invertGC == 0)
	gc.invertGC = DefaultGCOfScreen(XtScreen(w));
    if (gc.clearGC  == 0)
	gc.clearGC  = DefaultGCOfScreen(XtScreen(w));

    // draw all edges
    for (GraphEdge *edge = firstVisibleEdge(); edge != 0; 
	 edge = nextVisibleEdge(edge))
	edge->draw(w, exposed, gc);

    // draw all nodes
    for (GraphNode *node = firstVisibleNode(); node != 0; 
	 node = nextVisibleNode(node))
    {
	if (node->redraw() || !gc.redraw)
	    node->draw(w, exposed, gc);
	if (gc.redraw)
	    node->redraw() = false;
    }
}


// Print
void Graph::_print(ostream& os, const GraphGC& _gc) const
{
    // We cannot print hints, so leave them alone
    GraphGC gc(_gc);
    gc.drawHints = false;
    gc.hintSize  = 0;

    // print all edges
    // if printSelectedNodesOnly, print only edges between selected nodes
    for (GraphEdge *edge = firstVisibleEdge(); edge != 0; 
	 edge = nextVisibleEdge(edge))
    {
	if (gc.printSelectedNodesOnly == false ||
	    (edge->from()->selected() && edge->to()->selected()))
	    edge->_print(os, gc);
    }

    // print all nodes
    // if printSelectedNodesOnly, print only selected nodes
    for (GraphNode *node = firstVisibleNode(); node != 0; 
	 node = nextVisibleNode(node))
    {
	if (gc.printSelectedNodesOnly == false || node->selected())
	    node->_print(os, gc);
    }
}
    

// Echo
ostream& operator << (ostream& s, const Graph& g)
{
    // echo all edges
    for (GraphEdge *edge = g.firstEdge(); edge != 0; edge = g.nextEdge(edge))
	s << *edge << "\n";

    return s;
}


// Region occupied by graph
BoxRegion Graph::region(const GraphGC& gc) const
{
    if (firstVisibleNode() == 0)
	return BoxRegion();

    BoxRegion r = firstVisibleNode()->region(gc);
    for (GraphNode *node = nextVisibleNode(firstVisibleNode()); node != 0;
	node = nextVisibleNode(node))
    {
	r = r | node->region(gc);
    }

    for (GraphEdge *edge = firstVisibleEdge(); edge != 0; 
	 edge = nextVisibleEdge(edge))
    {
	BoxRegion edge_region = edge->region(gc);
	if (edge_region.origin().isValid())
	    r = r | edge_region;
    }

    return r;
}

// Visible nodes and edges
GraphNode *Graph::firstVisibleNode() const
{
    GraphNode *node = firstNode();
    while (node != 0 && node->hidden())
	node = nextNode(node);
    return node;
}

GraphNode *Graph::nextVisibleNode(GraphNode *ref) const
{
    GraphNode *node = nextNode(ref);
    while (node != 0 && node->hidden())
	node = nextNode(node);
    return node;
}

GraphEdge *Graph::firstVisibleEdge() const
{
    GraphEdge *edge = firstEdge();
    while (edge != 0 && (edge->hidden() 
			 || edge->from()->hidden() || edge->to()->hidden()))
	edge = nextEdge(edge);
    return edge;
}

GraphEdge *Graph::nextVisibleEdge(GraphEdge *ref) const
{
    GraphEdge *edge = nextEdge(ref);
    while (edge != 0 && (edge->hidden() 
			 || edge->from()->hidden() || edge->to()->hidden()))
	edge = nextEdge(edge);
    return edge;
}

// representation invariant
bool Graph::OK() const
{
    for (GraphNode *n = firstNode(); n != 0; n = nextNode(n))
    {
	assert(n->prev->next == n);
	assert(n->next->prev == n);
	assert(n->OK());
    }

    for (GraphEdge *e = firstEdge(); e != 0; e = nextEdge(e))
    {
	assert(e->prev->next == e);
	assert(e->next->prev == e);
	assert(e->OK());
    }

    return true;
}

