// $Id$
// GraphNode class

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
// Revision 1.10  1995/04/17  13:49:48  zeller
// RCSID fixed
//
// Revision 1.9  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.8  1993/12/07  08:33:11  zeller
// Fix: #pragma implementation "GraphNodeI.h" nachgereicht
//
// Revision 1.7  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.6  1993/05/22  20:11:10  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.5  1993/04/22  11:00:27  zeller
// Lizenz verbessert
// 
// Revision 1.4  1993/04/16  11:38:36  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/11/12  23:48:12  zeller
// 'const' eingefuehrt
// 
// Revision 1.2  1992/06/01  17:02:39  zeller
// Neu: #pragma implementation
// 
// Revision 1.1  1992/03/03  14:58:52  zeller
// Initial revision
// 

static const char rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "GraphNodeI.h"
#endif


#include "GraphNode.h"

DEFINE_TYPE_INFO_0(GraphNode)

// Representation invariant
bool GraphNode::OK() const
{
    GraphEdge *e;

    // check nodes
    for (e = firstFrom(); e != 0; e = nextFrom(e))
	assert(e->from() == this);

    for (e = firstTo(); e != 0; e = nextTo(e))
	assert(e->to() == this);

    return true;
}
