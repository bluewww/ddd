// $Id$
// Abbreviations for frequently used VSL nodes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2004 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_TrueNode_h
#define _DDD_TrueNode_h

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
private:
    TrueNode(const TrueNode&);
    TrueNode& operator = (const TrueNode&);
};

class FalseNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    FalseNode()
	: ConstNode(new FalseBox)
    {}
private:
    FalseNode(const FalseNode&);
    FalseNode& operator = (const FalseNode&);
};

class NullNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    NullNode()
	: ConstNode(new NullBox)
    {}
private:
    NullNode(const NullNode&);
    NullNode& operator = (const NullNode&);
};

class StringNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    StringNode(const string &str)
	: ConstNode(new StringBox(str))
    {}
private:
    StringNode(const StringNode&);
    StringNode& operator = (const StringNode&);
};

class NumNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    NumNode(int num)
	: ConstNode(new SquareBox(num))
    {}
private:
    NumNode(const NumNode&);
    NumNode& operator = (const NumNode&);
};

class EmptyListNode: public ConstNode {
public:
    DECLARE_TYPE_INFO

    EmptyListNode()
	: ConstNode(new ListBox)
    {}
private:
    EmptyListNode(const EmptyListNode&);
    EmptyListNode& operator = (const EmptyListNode&);
};

class FixListNode: public ListNode {
public:
    DECLARE_TYPE_INFO

    // GCC 2.8.1 doesn't like these inlined, so we place them in the .C file
    FixListNode(VSLNode *arg);
    FixListNode(VSLNode *arg1, VSLNode *arg2);
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3);
    FixListNode(VSLNode *arg1, VSLNode *arg2, VSLNode *arg3, VSLNode *arg4);
private:
    FixListNode(const FixListNode&);
    FixListNode& operator = (const FixListNode&);
};

#endif
