// $Id$
// PosGraphNode class: GraphNode with position

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
// Revision 1.1  1995/05/01 15:47:48  zeller
// Initial revision
//
// Revision 1.5  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.4  1993/05/22  20:11:30  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.3  1993/04/22  11:00:12  zeller
// Lizenz verbessert
// 
// Revision 1.2  1993/04/16  11:38:42  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.1  1992/11/06  00:28:56  zeller
// Initial revision
// 

char PosGraphNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PosGraphN.h"

DEFINE_TYPE_INFO_1(PosGraphNode, GraphNode)
