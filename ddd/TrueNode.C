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

char TrueNode_rcsid[] = 
    "$Id$";

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
