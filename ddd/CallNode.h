// $Id$
// Function call VSL nodes

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

#ifndef _DDD_CallNode_h
#define _DDD_CallNode_h

// A CallNode calls a VSL function (builtin or user-defined)

#include "assert.h"
#include <iostream>

#include "VSLNode.h"

#include "VSLDefList.h"
#include "VSLDef.h"


// CallNode

class CallNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

protected:
    VSLNode *_arg;

    CallNode(const CallNode& node):
	VSLNode(node), 
	_arg(node._arg->dup())
    {}

    // Call function
    virtual const Box *call(Box *arg) const = 0;

    // Dump name
    void dump(std::ostream& s) const;
    void _dumpTree(std::ostream& s) const;

    // Return name of callee
    virtual const char *func_name() const = 0;

    // Flag: Dump function name infix?
    virtual bool isInfix() const { return false; }

    bool matches(const VSLNode& node) const
    {
	return matchesAll ||
	    (VSLNode::matches(node) &&
	    *_arg == *(((const CallNode *)(&node))->_arg));   // dirty trick
    }

private:
    CallNode& operator = (const CallNode&);

public:
    // Resources
    VSLNode*& arg() { return _arg; }
    VSLNode* arg() const { return _arg; }

    // Constructor
    CallNode(VSLNode *a, const char *type = "CallNode"):
	VSLNode(type), _arg(a)
    {}

    // Destructor (includes arg)
    ~CallNode()
    {
	if (_arg != 0)
	    delete _arg;
    }

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

    bool isConst() const { return _arg->isConst(); }
    bool isCallNode() const { return true; }

    unsigned nargs() const { return _arg->nargs(); }

    static bool matchesAll;

    // Representation invariant
    bool OK() const;
};

#endif
