// $Id$
// Abbreviations for frequently used VSL nodes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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

    TrueNode()
	: ConstNode(new TrueBox)
    {}
};

class FalseNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    FalseNode()
	: ConstNode(new FalseBox)
    {}
};

class NullNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    NullNode()
	: ConstNode(new NullBox)
    {}
};

class StringNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    StringNode(const string &str)
	: ConstNode(new StringBox(str))
    {}
};

class NumNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    NumNode(int num)
	: ConstNode(new SquareBox(num))
    {}
};

class EmptyListNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    EmptyListNode()
	: ConstNode(new ListBox)
    {}
};

class FixListNode: public ListNode {
public:
    DECLARE_TYPE_INFO

    // GCC 2.8.1 doesn't like these inlined, so we place them in the .C file
    FixListNode(VSLNode *arg);
    FixListNode(VSLNode *arg1, VSLNode *arg2);
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3);
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3, VSLNode *arg4);
};

#endif
