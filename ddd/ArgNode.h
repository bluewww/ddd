// $Id$
// ArgNode class

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

#ifndef _DDD_ArgNode_h
#define _DDD_ArgNode_h


// Upon run-time, an ArgNode evaluates an argument from the argument list

// ArgNodes may be used for pattern matching.
// If 'ArgNode::matchAll' is set, an ArgNode matches all other VSL nodes.
// If 'ArgNode::matchCallback' is set, matching calls a function with
// the matched node as argument.


#include "assert.h"
#include <iostream>
#include "strclass.h"

#include "VSLNode.h"
#include "DummyNode.h"


// ArgNode

typedef void (*ArgNodeFunc)(int, const VSLNode *);

class ArgNode: public DummyNode {
public:
    DECLARE_TYPE_INFO

private:
    unsigned _id;           // #Argument in arglist: erstes 0, zweites 1...

protected:
    ArgNode(const ArgNode& node):
	DummyNode(node), 
	_id(node._id)
    {}

    bool matches(const VSLNode &node) const;

    void dump(std::ostream& s) const;
    void _dumpTree(std::ostream& s) const;

public:
    // Constructor
    ArgNode(unsigned i, const char *type = "ArgNode"):
	DummyNode(type), _id(i)
    {}

    // Copy
    VSLNode *dup() const { return new ArgNode(*this); }

    // Evaluate
    const Box *_eval(ListBox *arglist) const;

    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);

    bool isConst() const   { return false; }
    bool isArgNode() const { return true; }

    unsigned id() const    { return _id; }

    unsigned nargs() const    { return 1; }

    static ArgNodeFunc matchCallback; // Call when matching
};

#endif
