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
// Revision 1.23  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.22  1994/07/21  09:29:59  zeller
// Fix: OK() must be const
//
// Revision 1.21  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.20  1993/12/06  16:40:47  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.19  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.18  1993/05/22  20:10:48  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.17  1993/04/22  11:00:33  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/20  23:00:56  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.15  1993/04/16  11:38:30  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.14  1993/02/23  17:31:20  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.13  1992/11/13  19:07:58  zeller
// region() angepasst
// 
// Revision 1.12  1992/11/13  15:52:04  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.11  1992/11/04  12:02:50  zeller
// "assert.h" statt <assert.h>
// 
// Revision 1.10  1992/06/02  00:55:18  zeller
// draw() vereinfacht; Anpassung an gcc 2.1
// 
// Revision 1.9  1992/06/01  16:58:54  zeller
// Neu: #pragma interface
// 
// Revision 1.8  1992/06/01  16:55:27  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.7  1992/03/03  17:42:40  zeller
// Neu: Echo
// 
// Revision 1.6  1992/03/03  14:56:54  zeller
// Neu: OK()
// 
// Revision 1.5  1992/02/27  23:00:55  zeller
// Neu: GraphGC
// 
// Revision 1.4  1992/02/27  14:19:39  zeller
// Neu: region()
// 
// Revision 1.3  1992/02/27  11:23:16  zeller
// Member-Funktionen als 'const' deklariert
// 
// Revision 1.2  1992/02/25  17:14:46  zeller
// Neu: draw()
// 
// Revision 1.1  1992/02/25  09:39:38  zeller
// Initial revision
// 

#ifndef _Nora_Graph_h
#define _Nora_Graph_h

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
    GraphNode *_firstNode;	// circular list (0, if empty)
    GraphEdge *_firstEdge;	// circular list (0, if empty)

protected:
    void addNodes(GraphNode* nodes);
    void addEdges(GraphEdge* edges);

    void removeNode(GraphNode* node);
    void removeEdge(GraphEdge* edge);

public:
    // Constructors
    Graph():
	_firstNode(0), _firstEdge(0)
    {}

    // Destructor
    virtual ~Graph();

    // Add Graph
    void operator += (Graph& graph)
    {
	if (graph._firstNode)
	    addNodes(graph._firstNode);
	if (graph._firstEdge)
	    addEdges(graph._firstEdge);
    }

    // Add Node
    void operator += (GraphNode *node)
    {
	assert(node->next == 0);	// node must not be used yet
	node->next = node;		// now it is used
	node->prev = node;
	addNodes(node);
    }

    // Add Edge
    void operator += (GraphEdge *edge)
    {
	assert(edge->next == 0);	// edge must not be used yet
	edge->next = edge;		// now it is used
	edge->prev = edge;
	addEdges(edge);
    }

    // Remove Node
    void operator -= (GraphNode *node)
    {
	assert(node->next != 0);	// node must be used
	removeNode(node);
    }

    // Remove Edge
    void operator -= (GraphEdge *edge)
    {
	assert(edge->next != 0);	// edge must be used
	removeEdge(edge);
    }

    // Iteration on all nodes and edges
    // simulate a 0-terminated list
    GraphNode *firstNode() const { return _firstNode; } 
    GraphNode *nextNode(GraphNode *ref) const
    {
	return ref->next == _firstNode ? 0 : ref->next;
    }

    GraphEdge *firstEdge() const { return _firstEdge; }
    GraphEdge *nextEdge(GraphEdge *ref) const
    {
	return ref->next == _firstEdge ? 0 : ref->next;
    }

    // Drawing
    void draw(Widget w, const BoxRegion& exposed, const GraphGC& gc) const;
    void draw(Widget w, const BoxRegion& exposed) const
    {
	draw(w, exposed, GraphGC());
    }
    void draw(Widget w) const
    {
	draw(w, BoxRegion(BoxPoint(0, 0), BoxSize(MAXINT, MAXINT)));
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

    // Custom function
    void print(ostream& os, const GraphGC& gc = GraphGC()) const
    {
	_printHeader(os, gc);
	_print(os, gc);
	_printTrailer(os, gc);
    }

    // Total Region
    BoxRegion region(const GraphGC& gc) const;

    // representation invariant
    virtual bool OK() const;
};

// Echo
extern ostream& operator << (ostream& s, const Graph& g);

#endif
