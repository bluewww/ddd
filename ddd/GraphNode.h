// $Id$
// GraphNode Class

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
// Revision 1.14  1993/12/06  16:43:47  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.13  1993/05/22  20:11:12  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.12  1993/04/22  11:00:23  zeller
// Lizenz verbessert
// 
// Revision 1.11  1993/04/20  23:00:59  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.10  1993/04/16  11:38:38  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.9  1992/11/13  15:52:24  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.8  1992/06/01  16:59:08  zeller
// Neu: #pragma interface
// 
// Revision 1.7  1992/06/01  16:55:35  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.6  1992/03/03  14:58:14  zeller
// Datenstruktur nach GraphNodeI.h ausgelagert;
// Neu: Finden von Vorgaengern und Nachfolgern
// 
// Revision 1.5  1992/02/27  23:01:04  zeller
// Neu: GraphGC
// 
// Revision 1.4  1992/02/26  16:42:18  zeller
// moveTo() oeffentlich gemacht
// 
// Revision 1.3  1992/02/26  11:21:42  zeller
// Neu: selected()
// 
// Revision 1.2  1992/02/25  17:15:03  zeller
// Neu: draw(), pos(), region()
// 
// Revision 1.1  1992/02/25  09:39:39  zeller
// Initial revision
// 

#ifndef _Nora_GraphNode_h
#define _Nora_GraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphNodeI.h"
#include "GraphEdge.h"

inline GraphEdge *GraphNode::nextFrom(GraphEdge *ref) const
{
    assert(ref->from() == this);
    return ref->_nextFrom == _firstFrom ? 0 : ref->_nextFrom;
}

inline GraphEdge *GraphNode::nextTo(GraphEdge *ref) const
{
    assert(ref->to() == this);
    return ref->_nextTo == _firstTo ? 0 : ref->_nextTo;
}

#endif
