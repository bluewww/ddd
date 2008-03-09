// $Id$
// ListNode class

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ListNode_h
#define _DDD_ListNode_h


// A ListNode combines other nodes k_i to lists.  Upon run-time, a
// ListNode creates a ListBox with the results of the k_i.


#include "assert.h"
#include <iostream>
#include "misc.h"       // max()

#include "VSLNode.h"


// ListNode

class ListNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_head;
    VSLNode *_tail;

protected:
    ListNode(const ListNode& node);

    bool matches(const VSLNode& node) const
    {
	return VSLNode::matches(node) &&
	    *_head == *(((const ListNode *)&node)->_head) && 
	    *_tail == *(((const ListNode *)&node)->_tail);    // dirty trick
    }

private:
    ListNode& operator = (const ListNode&);

public:
    ListNode(VSLNode *hd, VSLNode *tl, const char *type = "ListNode");
    VSLNode *dup() const { return new ListNode(*this); }

    // Destructor (including args)
    ~ListNode()
    {
	if (_head != 0)
	    delete _head;
	if (_tail != 0)
	    delete _tail;
    }

    // Resources
    VSLNode*& head() { return _head; }
    VSLNode*& tail() { return _tail; }
    VSLNode* head() const { return _head; }
    VSLNode* tail() const { return _tail; }

    const Box *_eval(ListBox *arglist) const;

    // Optimization
    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int foldOps(VSLDef *cdef, VSLNode **node);
    int foldConsts(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);
    int _reBase(VSLDef *cdef, unsigned newBase);

    // Other tree functions
    void compilePatterns(VSLDef *cdef) const;
    void uncompilePatterns(VSLDef *cdef) const;
    int resolveName(VSLDef *cdef, VSLNode **node, const string& name, unsigned id);
    int _resolveNames(VSLDef *cdef, unsigned base);
    void rebind(const class VSLLib *);
    const string& firstName() const;

    // Append to list
    int append(VSLNode *node);

    // Dump
    void dump(std::ostream& s) const;
    void _dumpTree(std::ostream& s) const;

    // Properties
    bool isConst() const {
      return _head->isConst() && _tail->isConst();
    }
    bool isListNode() const { return true; }
    bool isStraight() const;

    unsigned nargs() const;

    // Representation invariant
    bool OK() const;
};

#endif
