// $Id$
// Praktische Abkuerzungen

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

#ifndef _Nora_TrueNode_h
#define _Nora_TrueNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "ConstNode.h"
#include "ListNode.h"

#include "TrueBox.h"
#include "ListBox.h"
#include "StringBox.h"
#include "PrimitiveB.h"

class TrueNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    TrueNode(): ConstNode(new TrueBox) {}
};

class FalseNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    FalseNode(): ConstNode(new FalseBox) {}
};

class NullNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    NullNode(): ConstNode(new NullBox) {}
};

class StringNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    StringNode(string &str): ConstNode(new StringBox(str)) {}
};

class NumNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    NumNode(int num): ConstNode(new SquareBox(num)) {}
};

class EmptyListNode: public ConstNode {
public:
    DECLARE_TYPE_INFO
    EmptyListNode(): ConstNode(new ListBox) {}
};

class FixListNode: public ListNode {
public:
    DECLARE_TYPE_INFO
    FixListNode(VSLNode *arg): 
	ListNode(arg, new EmptyListNode) 
    {}
    FixListNode(VSLNode *arg1, VSLNode *arg2): 
	ListNode(arg1, new FixListNode(arg2)) 
    {}
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3);
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3, VSLNode *arg4);
};

#endif
