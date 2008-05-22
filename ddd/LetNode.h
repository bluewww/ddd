// $Id$
// LET..IN construct in VSL

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

#ifndef _DDD_LetNode_h
#define _DDD_LetNode_h

// A LetNode implements a LET..IN construct.

// A LetNode contains a list of length 2.
// Upon evaluation, the first list element is matched against a pattern.
// The variables found this way are used in evaluating the second element.


#include "assert.h"
#include <iostream>

#include "mutable.h"
#include "VSLNode.h"
#include "ListNode.h"
#include "CallNode.h"
#include "TrueNode.h"



// LetNode

class LetNode: public CallNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_node_pattern;	// Pattern
    mutable Box *_box_pattern;		// Compiled pattern

    unsigned _nargs;		// Number of args
    bool _straight;		// Flag: use argument list `as is'?

    mutable bool being_compiled;	// Protect against recursive patterns

    ListNode *_args() const      { return (ListNode *)arg(); }
    ListNode *_body() const      { return (ListNode *)(_args()->tail()); }

    ListBox *arglist(const Box *arg) const;       // Build arg list (Box)
    VSLNode **nodelist(const VSLNode *arg) const; // same, with nodes

    bool domatch(const Box *arg) const;        // Match against Box
    bool domatch(const VSLNode *arg) const;    // Match against Ausdruck

protected:
    void dump(std::ostream& s) const;
    void _dumpTree(std::ostream& s) const;

    LetNode(const LetNode& node):
	CallNode(node),
	_node_pattern(node._node_pattern->dup()),
	_box_pattern(0),
	_nargs(node._nargs),
	_straight(node._straight),
	being_compiled(false)
    {
	if (node._box_pattern)
	    _box_pattern = node._box_pattern->link();
    }

    // Never called
    const char *func_name() const       { assert(0); return "let"; }
    const Box *call(Box *) const  { assert(0); return 0; }

private:
    LetNode& operator = (const LetNode&);

public:
    // Constructor
    LetNode(VSLNode *p, VSLNode *a, VSLNode *b, const char *type = "LetNode"):
	CallNode(new FixListNode(a, b), type),
	_node_pattern(p),
	_box_pattern(0),
	_nargs(p->nargs()),
	_straight(p->isStraight()),
	being_compiled(false)
    {}

    // Destructor
    ~LetNode()
    {
	if (_node_pattern != 0)
	    delete _node_pattern;
	if (_box_pattern != 0)
	    _box_pattern->unlink();
    }

    // Resources
    VSLNode *&node_pattern()    { return _node_pattern; }
    Box *&box_pattern()         { return _box_pattern; }
    VSLNode *&args()            { return _args()->head(); }
    VSLNode *&body()            { return _body()->head(); }

    const VSLNode *node_pattern() const   { return _node_pattern; }
    const Box *box_pattern() const        { return _box_pattern; }
    const VSLNode *args() const           { return _args()->head(); }
    const VSLNode *body() const           { return _body()->head(); }

    // Compilation
    void compilePatterns(VSLDef *cdef) const;

    // Destroy BOX_PATTERN
    void uncompilePatterns(VSLDef *cdef) const
    {
	CallNode::uncompilePatterns(cdef);

	if (_box_pattern)
	    MUTABLE_THIS(LetNode *)->_box_pattern->unlink();
	MUTABLE_THIS(LetNode *)->_box_pattern = 0;
    }

    // Resolve all names
    int _resolveNames(VSLDef *cdef, unsigned base);

    // Optimization
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    int _reBase(VSLDef *cdef, unsigned newBase);

    // Copy
    VSLNode *dup() const { return new LetNode(*this); }

    const Box *_eval(ListBox *arglist) const;
    
    bool isLetNode() const { return true; }

    // Representation invariant
    bool OK() const;
};


// A WhereNode is identical to a LetNode, but is dumped as "where"
// instead of "let".

class WhereNode: public LetNode {
public:
    DECLARE_TYPE_INFO

protected:
    void dump(std::ostream& s) const;

    // Copy
    WhereNode(const WhereNode& node):
	LetNode(node)
    {}

public:
    // Create
    WhereNode(VSLNode *p, VSLNode *a, VSLNode *b):
	LetNode(p, a, b)
    {}
};

#endif
