// $Id$
// GraphEdge Class

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
// Revision 1.22  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.21  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.20  1993/12/06  16:43:59  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.19  1993/10/22  08:30:52  zeller
// Neu: hidden() auch als const-Funktion
//
// Revision 1.18  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.17  1993/05/22  20:10:53  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.16  1993/04/22  11:00:32  zeller
// Lizenz verbessert
// 
// Revision 1.15  1993/04/21  00:18:14  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.14  1993/04/20  23:00:56  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.13  1993/04/16  11:38:32  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.12  1993/02/23  17:31:21  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.11  1992/11/13  19:08:10  zeller
// GraphGC jetzt ueberall const
// 
// Revision 1.10  1992/11/13  15:52:19  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.9  1992/06/01  16:58:56  zeller
// Neu: #pragma interface
// 
// Revision 1.8  1992/06/01  16:55:28  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.7  1992/03/16  13:20:46  zeller
// Neu: _hidden
// 
// Revision 1.6  1992/03/03  17:42:44  zeller
// Neu: Echo
// 
// Revision 1.5  1992/03/03  14:57:53  zeller
// Datenstruktur nach GraphEdgeI.h ausgelagert;
// Neu: Finden von Vorgaengern und Nachfolgern
// 
// Revision 1.4  1992/02/27  23:01:00  zeller
// Neu: GraphGC
// 
// Revision 1.3  1992/02/27  14:37:09  zeller
// Neu: offsetIfSelected
// 
// Revision 1.2  1992/02/25  17:14:53  zeller
// Neu: draw()
// 
// Revision 1.1  1992/02/25  09:39:38  zeller
// Initial revision
// 

#ifndef _Nora_GraphEdge_h
#define _Nora_GraphEdge_h

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

protected:
    // Graphics
    virtual void _draw(Widget w, 
		       const BoxRegion& exposed, 
		       const GraphGC& gc) const = 0;
    
    // Enqueue in node lists
    void enqueue();
    
    // Dequeue from node lists
    void dequeue();

public:
    // Constructor
    GraphEdge(GraphNode *f, GraphNode *t):
        _from(f), _to(t), 
        _hidden(false),
        _nextFrom(0), _nextTo(0),
        _prevFrom(0), _prevTo(0),
        next(0), prev(0)
    {}

    // Resources
    GraphNode *from() const 	{ return _from; }
    GraphNode *to() const 	{ return _to; }
    unsigned long id() const	{ return _id; }

    bool& hidden()           { return _hidden; }
    bool hidden() const      { return _hidden; }

    // Graphics
    void draw(Widget w, const BoxRegion& exposed = BoxRegion(BoxPoint(0, 0),
	BoxSize(MAXINT, MAXINT)), const GraphGC& gc = GraphGC()) const
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

#endif
