// $Id$
// Calling user-defined VSL functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
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

char DefCallNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>
#include <sstream>

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSEFlags.h"
#include "Box.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "DefCallN.h"
#include "BuiltinCN.h"
#include "ArgNode.h"

DEFINE_TYPE_INFO_1(DefCallNode, CallNode)


// DefCallNode

// Constructor
DefCallNode::DefCallNode(VSLDef *def, VSLNode *a, const char *type):
    CallNode(a, type), _def(def), _deflist(def->deflist)
{
    _deflist->inc();
}

// Constructor
DefCallNode::DefCallNode(VSLDefList *deflist, VSLNode *a, const char *type):
    CallNode(a, type), _def(0), _deflist(deflist)
{
    _deflist->inc();
}

// Copy
DefCallNode::DefCallNode(const DefCallNode& node):
    CallNode(node), _def(node._def), _deflist(node._deflist)
{
    _deflist->inc();
}

// Destructor
DefCallNode::~DefCallNode()
{
    assert(_deflist->references() >= 0);
    _deflist->dec();
}

// Call user-defined function
const Box *DefCallNode::call(Box *a) const
{
    const Box *box;

    // Find function and call it
    if (_def)
	box = _def->eval(a);
    else
	box = _deflist->eval(a);

    return box;
}

// Return function name
const char *DefCallNode::func_name() const
{
    static const string s1 = _deflist->f_name();
    return s1.chars();
}


// Optimization

// resolveDefs: shorten DefCalls
// Replace all calls f(arg1, arg2, ...), where there can be only *one*
// possible def of f(), by a direct call of f()

int DefCallNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    // Apply to arg
    int changes = CallNode::resolveDefs(cdef, complain_recursive);

    if (_def)               // Are we unambiguous already?
	return changes;

    // Match in both directions
    bool old_bothSidesCanMatch = VSLNode::bothSidesCanMatch;
    VSLNode::bothSidesCanMatch = true;

    // All arguments...
    bool old_ArgNodeMatchesAll = ArgNode::matchesAll;
    ArgNode::matchesAll = true;

    // ... and all function calls can match
    bool old_CallNodeMatchesAll = CallNode::matchesAll;
    CallNode::matchesAll = true;

    VSLDef *found = 0;
    VSLDef *def;
    for (def = _deflist->first(); def != 0; def = def->listnext())
	if (def->matches(arg()))
	    if (found == 0)
		found = def;        // First matching def
	    else
	    {
		found = 0;          // Second matching def: abort
		break;
	    }

    VSLNode::bothSidesCanMatch = old_bothSidesCanMatch;
    CallNode::matchesAll = old_CallNodeMatchesAll;
    ArgNode::matchesAll = old_ArgNodeMatchesAll;

    if (found == 0 && def == 0)
    {
        std::ostringstream s;
	s << *this;
	VSLLib::eval_warning("no suitable definition for " + string(s),
			     cdef);
    }

    if (found == cdef && complain_recursive)
	VSLLib::eval_error("infinite recursion", cdef);

    if (found == 0)
	return changes;

    // Set new definition
    _def = found;

    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": resolveDefs: resolving\n" 
	    << *this << "\nto " << _def->longname() << "\n";
	std::cout.flush();
    }

    return ++changes;
}



// resolveSynonyms: Resolve synonyms
// Replace all calls f() with f(...) = g(...), g(...) = h(...) by h()

int DefCallNode::resolveSynonyms(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);

    // Apply to all args
    int changes = CallNode::resolveSynonyms(cdef, node);

    // If ambiguous, we're done
    if (_def == 0)
	return changes;

    // Let f() be the called function;
    // If f() is not defined as f() = g(), abort

    VSLNode *syn = _def->expr();
    if (syn == 0 || !syn->isCallNode())
	return changes;

    CallNode *call_syn = (CallNode *)syn;   // dirty trick

    // If f() is not defined as f(<pattern>) = g(<pattern>), abort

    VSLNode *my_pattern = _def->node_pattern();
    VSLNode *his_pattern = call_syn->arg();
    if (*my_pattern != *his_pattern)
	return changes;

    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": resolveSynonyms: replacing\n" 
	    << *this << "\n";
	std::cout.flush();
    }

    if (call_syn->isDefCallNode())
    {
	DefCallNode *defcall_syn = (DefCallNode *)call_syn; // dirty trick

	// f(a1, ..., an) is defined as g(a1, ..., an)
	// Replace call to f() call to g()

	// Replace DefCallNode by other DefCallNode;
	// (simply change the deflist pointers)

	defcall_syn->_deflist->inc();
	_deflist->dec();

	_def = defcall_syn->_def;
	_deflist = defcall_syn->_deflist;

	changes++;
    }
    else if (call_syn->isBuiltinCallNode())
    {
	BuiltinCallNode *builtin_syn = 
	    (BuiltinCallNode *)call_syn; // dirty trick

	// f(a1, ..., an) is defined as g(a1, ..., an)
	// Replace call to f() by call to g().

	// Replace DefCallNode by BuiltinCallNode
	// (actual node change)

	BuiltinCallNode *newNode = new BuiltinCallNode(*builtin_syn, arg());

	*node = newNode;
	arg() = 0; delete this;

	changes++;
    }

    if (VSEFlags::show_optimize)
    {
	std::cout << "by " << **node << "\n";
	std::cout.flush();
    }

    return changes;
}


// inlineFuncs: Replace function calls by function bodies
//
// For example: Replace 
//            f(a, b, c, d) = max(a, b) + max(c, d) 
// with
//            max(a, b) = if a > b then a else b fi
// by
// f(a, b, c, d) = (if a > b then a else b fi) + (if c > d then c else d fi)

int DefCallNode::inlineFuncs(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Apply to all arguments
    changes += CallNode::inlineFuncs(cdef, node);

    // If we're ambiguous, we're done
    if (_def == 0 || _def->expr() == 0)
	return changes;

    // Create list of instances; if ambiguous, we're done
    VSLNode **values = _def->nodelist(arg());
    if (values == 0)
	return changes;

    // Create instance counter
    int *instances = new int [_def->nargs()];
    unsigned i;
    for (i = 0; i < _def->nargs(); i++)
	instances[i] = 0;

    // Count how often each arg is used
    _def->expr()->countArgNodes(cdef, instances, 0, _def->nargs());

    // Each instance must be defined
    bool fail = false;
    for (i = 0; i < _def->nargs(); i++)
	if (instances[i] > 0 && values[i] == 0)
	{
	    std::ostringstream os;
	    os << "cannot isolate arg " << i;
	    VSLLib::eval_warning(os, _def);
	    fail = true;
	}

    // Each instance must be used only once (efficiency)
    for (i = 0; i < _def->nargs(); i++)
	if (values[i] && instances[i] > 1)
	{
	    // However, if we replace an arg by a constant or another
	    // arg, the arg may be instantiated multiple times.

	    if (!values[i]->isConstNode() && !values[i]->isArgNode())
		fail = true;
	}

    delete[] instances;

    if (fail)
	return changes;
    
    // Now perform inlining.

    // Copy function body
    VSLNode *body = _def->expr()->dup();

    // The arg nodes in BODY refer to EXPR; not to the function of
    // this DefCallNode.  We must replace them by the arguments of
    // this DefCallNode.

    body->instantiateArgs(cdef, &body, values, 0, _def->nargs());

    // Remaining arg nides in BODY refer to LET constructs.  If the
    // current funtion and the new body have a different number of
    // args, we'll get into trouble.  So, renumber the arg nodes.

    body->reBase(cdef, _base);


    // Here's the actual replacement
    *node = body;

    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": inlineFuncs: replacing\n" 
	    << *this << "\nby " << **node << '\n';
	std::cout.flush();
    }

    delete this;

    return ++changes;
}




// countSelfReferences: Call references to outside functions

int DefCallNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    int changes = CallNode::countSelfReferences(cdef, deflist);

    if (_deflist == deflist)
    {
	if (VSEFlags::show_optimize)
	{
	    std::cout << "\n" << cdef->longname() 
		<< ": countSelfReferences: found self-reference to " 
		<< deflist->f_name();
	    std::cout.flush();
	}

	deflist->self_references++;
	changes++;
    }

    return changes;
}


void DefCallNode::rebind(const class VSLLib *lib)
{
    CallNode::rebind(lib);

    if (lib != _deflist->lib)
    {
	// Library has changed
	// std::clog << "Rebinding: call to " << _deflist->func_name() << "\n";

	// Remove reference to old library
	assert(_deflist->references() >= 0);
	_deflist->dec();
	_def = 0;

	// Reference deflist in new library
	const string& func_name = _deflist->func_name();
	_deflist = lib->deflist(func_name);
	assert(_deflist != 0);
	_deflist->inc();
    }
}



// Debugging

// Representation invariant
bool DefCallNode::OK() const
{
    assert (_deflist != 0);
    assert (_def == 0 || _def->deflist == _deflist);

    // assert (_def->OK()); // may result in endless loop
    assert (CallNode::OK());

    return true;
}
