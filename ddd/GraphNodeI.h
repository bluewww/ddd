// $Id$
// GraphNode class structure

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
// Revision 1.1  1995/05/01 15:47:43  zeller
// Initial revision
//
// Revision 1.21  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.20  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.19  1993/12/06  16:43:40  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.18  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.17  1993/05/22  20:11:15  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.16  1993/04/22  11:00:29  zeller
// Lizenz verbessert
// 
// Revision 1.15  1993/04/21  14:43:28  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// 
// Revision 1.14  1993/04/21  00:18:15  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.13  1993/04/20  23:01:00  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.12  1993/04/16  11:38:38  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.11  1993/02/23  17:31:23  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.10  1992/11/13  19:09:35  zeller
// GraphGC jetzt ueberall const
// 
// Revision 1.9  1992/11/13  15:52:26  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.8  1992/11/12  23:48:37  zeller
// 'const' eingefuehrt;
// Neu: highlightRegion, sensitiveRegion
// 
// Revision 1.7  1992/06/02  00:56:11  zeller
// draw() vereinfacht: Anpassung an gcc 2.1
// 
// Revision 1.6  1992/06/01  16:59:09  zeller
// Neu: #pragma interface
// 
// Revision 1.5  1992/06/01  16:55:37  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.4  1992/03/16  13:22:08  zeller
// Neu: _hidden
// 
// Revision 1.3  1992/03/03  17:43:14  zeller
// Neu: Echo, str()
// 
// Revision 1.2  1992/03/03  17:23:52  zeller
// Neu: count
// 
// Revision 1.1  1992/03/03  14:58:34  zeller
// Initial revision
// 

#ifndef _Nora_GraphNodeI_h
#define _Nora_GraphNodeI_h

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

    // maintained by GraphEdge
    GraphEdge *_firstFrom;	// first Edge e with e->from() == this
    GraphEdge *_firstTo;	// first Edge e with e->to()   == this

    // maintained by Graph
    GraphNode *next;		// For collectors
    GraphNode *prev;		// For collectors

public:
    int count;			// #predecessors (used in layout algorithms)

    // Constructor
    GraphNode():
	_selected(false), _hidden(false),
	_firstFrom(0), _firstTo(0), next(0), prev(0), count(0)
    {}

    // Resources
    unsigned long id() const { return _id; }
    bool& selected()      { return _selected; }
    bool& hidden()        { return _hidden; }

    bool __selected() const { return _selected; }
    bool __hidden() const   { return _hidden; }

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
    virtual void draw(Widget w, 
		      const BoxRegion& exposed, 
		      const GraphGC& gc) const
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
	draw(w, BoxRegion(BoxPoint(0, 0), BoxSize(MAXINT, MAXINT)),
	    GraphGC());
    }


    // printing
    virtual void _print(ostream& os, const GraphGC& gc) const
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

#endif
