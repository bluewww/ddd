// $Id$
// Graph Class

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

// $Log$
// Revision 1.1  1995/05/01 15:47:41  zeller
// Initial revision
//
// Revision 1.24  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.23  1994/07/21  09:29:51  zeller
// Fix: OK() must be const
//
// Revision 1.22  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.21  1993/10/18  12:56:16  zeller
// Fix: Hints nicht ausdrucken
//
// Revision 1.20  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.19  1993/05/26  09:11:57  zeller
// Fix: const-Warnung unterdrueckt
//
// Revision 1.18  1993/05/22  20:10:45  zeller
// %Q% added by apply-q-flag.sh 1.5
// 
// Revision 1.17  1993/04/22  11:00:33  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/16  11:38:29  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.15  1993/02/24  15:29:55  zeller
// Neu: printSelectedNodesOnly
// 
// Revision 1.14  1993/02/23  17:31:19  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.13  1992/11/13  19:07:52  zeller
// forceDraw: const eingefuehrt; region() angepasst
// 
// Revision 1.12  1992/11/13  15:51:53  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.11  1992/11/05  19:03:04  zeller
// Neue Graphik-Kontexte -- vermeiden allzuhaeufiges Umschalten
// 
// Revision 1.10  1992/11/04  12:02:45  zeller
// "assert.h" statt <assert.h>
// 
// Revision 1.9  1992/06/02  00:55:04  zeller
// draw() vereinfacht
// 
// Revision 1.8  1992/06/01  17:02:35  zeller
// Neu: #pragma implementation
// 
// Revision 1.7  1992/03/03  17:42:35  zeller
// Neu: Echo
// 
// Revision 1.6  1992/03/03  14:56:45  zeller
// Neu: OK()
// 
// Revision 1.5  1992/02/27  23:00:50  zeller
// Neu: GraphGC
// 
// Revision 1.4  1992/02/27  14:19:34  zeller
// Neu: region()
// 
// Revision 1.3  1992/02/27  11:23:10  zeller
// Member-Funktionen als 'const' deklariert
// 
// Revision 1.2  1992/02/25  17:14:39  zeller
// Neu: draw()
// 
// Revision 1.1  1992/02/25  09:39:37  zeller
// Initial revision
// 

char Graph_rcsid[] = "$Id$";

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

// Remove Node
void Graph::removeNode(GraphNode *node)
{
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
    node->next = 0;
    node->prev = 0;
}

// Remove Edge
void Graph::removeEdge(GraphEdge *edge)
{
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
    edge->next = 0;
    edge->prev = 0;
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
    for (GraphEdge *edge = firstEdge(); edge != 0; edge = nextEdge(edge))
	edge->draw(w, exposed, gc);

    // draw all nodes
    for (GraphNode *node = firstNode(); node != 0; node = nextNode(node))
	node->draw(w, exposed, gc);
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
    for (GraphEdge *edge = firstEdge(); edge != 0; edge = nextEdge(edge))
	if (gc.printSelectedNodesOnly == false ||
	    (edge->from()->selected() && edge->to()->selected()))
	    edge->_print(os, gc);

    // print all nodes
    // if printSelectedNodesOnly, print only selected nodes
    for (GraphNode *node = firstNode(); node != 0; node = nextNode(node))
	if (gc.printSelectedNodesOnly == false || node->selected())
	    node->_print(os, gc);
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
    if (firstNode() == 0)
	return BoxRegion();

    BoxRegion r = firstNode()->region(gc);
    for (GraphNode *node = nextNode(firstNode()); node != 0;
	node = nextNode(node))
	r = r | (BoxRegion&)node->region(gc);

    return r;
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

