// $Id$
// Calling built-in VSL functions

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

#ifndef _DDD_BuiltinCallNode_h
#define _DDD_BuiltinCallNode_h

// A BuiltinCallNode calls a built-in VSL function upon run-time.


#include "assert.h"
#include <iostream>

#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "DefCallN.h"

// BuiltinCallNode

class BuiltinCallNode: public CallNode {
public:
    DECLARE_TYPE_INFO

    friend class DefCallNode;

private:
    int _index;

protected:
    BuiltinCallNode(const BuiltinCallNode& node):
	CallNode(node), _index(node._index)
    {}

    const Box *call(Box *arg) const;

    const char *func_name() const { return VSLBuiltin::ext_name(_index); }
    bool isInfix() const { return VSLBuiltin::isInfix(_index); }

    bool matches (const VSLNode& node) const
    {
	return matchesAll || (CallNode::matches(node) &&
	    _index == ((const BuiltinCallNode *)&node)->_index);  // dirty trick
    }

public:
    // Constructor
    BuiltinCallNode(int i, VSLNode *a,
		    const char *type = "BuiltinCallNode")
	: CallNode(a, type), _index(i)
    {}

    BuiltinCallNode(const BuiltinCallNode &node, VSLNode *a,
		    const char *type = "BuiltinCallNode")
	: CallNode(a, type), _index(node._index)
    {}

    // Copy
    VSLNode *dup() const
    {
	return new BuiltinCallNode(*this);
    }

    int foldOps(VSLDef *cdef, VSLNode** node);
    int foldConsts(VSLDef *cdef, VSLNode** node);

    bool isBuiltinCallNode() const { return true; }

    // Representation invariant
    bool OK() const;
};

#endif
