// $Id$
// LineGraphEdge class: GraphEdge with drawing capabilities

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
// Revision 1.1  1995/05/01 15:47:45  zeller
// Initial revision
//
// Revision 1.16  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.15  1993/12/06  16:43:34  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.14  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.13  1993/05/22  20:11:25  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.12  1993/04/22  11:00:16  zeller
// Lizenz verbessert
// 
// Revision 1.11  1993/04/20  23:01:03  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.10  1993/04/16  11:38:41  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.9  1992/11/13  19:10:39  zeller
// GraphGC jetzt ueberall const
// 
// Revision 1.8  1992/11/13  15:52:32  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.7  1992/06/01  16:59:11  zeller
// Neu: #pragma interface
// 
// Revision 1.6  1992/06/01  16:55:38  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.5  1992/03/16  16:01:10  zeller
// Neue Verfahren, um Verbindungspunkte der Kanten zu bestimmen
// 
// Revision 1.4  1992/03/16  13:21:13  zeller
// Neu: _draw() statt draw()
// 
// Revision 1.3  1992/02/27  23:01:33  zeller
// Neu: GraphGC
// 
// Revision 1.2  1992/02/27  14:37:24  zeller
// Neu: offsetIfSelected
// 
// Revision 1.1  1992/02/25  17:15:18  zeller
// Initial revision
// 

#ifndef _Nora_LineGraphEdge_h
#define _Nora_LineGraphEdge_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphEdge.h"
#include "Box.h"

class LineGraphEdge: public GraphEdge {
public:
    DECLARE_TYPE_INFO

protected:
    // Find line between b1 and b2 in p1/p2
    static void findLine(BoxPoint& c1, BoxPoint& c2, 
			 BoxRegion& b1, BoxRegion& b2,
			 BoxPoint& p1, BoxPoint& p2, 
			 const GraphGC& gc);

    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& exposed, 
		       const GraphGC& gc) const;

public:
    // Constructor
    LineGraphEdge(GraphNode *f, GraphNode *t):
	GraphEdge(f, t)
    {}
};

#endif
