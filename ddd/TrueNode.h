// $Id$
// Praktische Abkuerzungen

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_TrueNode_h
#define _DDD_TrueNode_h

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
