// $Id$
// Implementation Klasse TrueNode

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
// Revision 1.1  1995/05/01 15:47:53  zeller
// Initial revision
//
// Revision 8.7  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 8.6  1993/05/22  20:17:36  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 8.5  1993/04/22  11:07:29  zeller
// Lizenz verbessert
// 
// Revision 8.4  1993/04/16  11:44:37  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 8.3  1992/11/03  19:37:24  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 8.2  1992/06/01  16:51:24  zeller
// Neu: #pragma implementation
// 
// Revision 8.1  1991/07/06  15:02:57  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:53:22  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 1.2  1991/05/09  16:06:17  zeller
// Warnungen vermieden
// 
// Revision 1.1  1991/02/25  16:25:44  zeller
// Initial revision
// 

char TrueNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "TrueNode.h"

DEFINE_TYPE_INFO_1(EmptyListNode, ConstNode)
DEFINE_TYPE_INFO_1(FalseNode,     ConstNode)
DEFINE_TYPE_INFO_1(NullNode,      ConstNode)
DEFINE_TYPE_INFO_1(NumNode,       ConstNode)
DEFINE_TYPE_INFO_1(StringNode,    ConstNode)
DEFINE_TYPE_INFO_1(TrueNode,      ConstNode)
DEFINE_TYPE_INFO_1(FixListNode,   ListNode)

// FixListNode

FixListNode::FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3):
    ListNode(arg1, new FixListNode(arg2, arg3))
{}

FixListNode::FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3, 
    VSLNode *arg4):
    ListNode(arg1, new FixListNode(arg2, arg3, arg4))
{}
