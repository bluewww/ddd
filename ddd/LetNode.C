// $Id$
// LET..IN construct in VSL

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

char LetNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>
#include <sstream>

#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "LetNode.h"
#include "ArgNode.h"
#include "VSLDef.h"
#include "MatchBox.h"
#include "ListBox.h"
#include "VSEFlags.h"

DEFINE_TYPE_INFO_1(LetNode, CallNode)
DEFINE_TYPE_INFO_1(WhereNode, LetNode)

// LetNode

// Dump Let
void LetNode::dump(std::ostream& s) const
{
    s << "let " << *node_pattern() << " = "  << *args() 
      << "\n  in " << *body();
}

// Dump where
void WhereNode::dump(std::ostream& s) const
{
    s << *body() 
       << "\n  where " << *node_pattern() << " = " << *args();
}


// ...as tree
void LetNode::_dumpTree(std::ostream& s) const
{
    node_pattern()->dumpTree(s);
    s << ", ";
    args()->dumpTree(s);
    s << ", ";
    body()->dumpTree(s);
}


// Evaluate
const Box *LetNode::_eval(ListBox *arglst) const
{
    const Box *result = 0;

    if (arglst)
	assert(int(_base) == arglst->length());

    // Create arguments for pattern
    const Box *patternArgs = args()->eval(arglst);

    // Create new argument list
    ListBox *moreArgs = arglist(patternArgs);

    if (moreArgs)
    {
	ListBox *attach = 0;

	// If needed, append new arg list to existing one
	if (arglst && !(arglst->isEmpty()))
	{
	    attach = arglst->cons(moreArgs);
	    moreArgs->unlink();
	    moreArgs = (ListBox *)arglst->link();
	}

	result = body()->eval(moreArgs);
	moreArgs->unlink();

	// Remove new arg list
	if (attach)
	    arglst->uncons(attach);
    }
    else
	VSLLib::eval_error("invalid arguments");

    ((Box *)patternArgs)->unlink();
    return result;
}


// Pattern matching

const int max_instances = 256;	// Max number of args


// Pattern matching with boxes

static Box *box_instances[max_instances];

static void box_matches(int data, const Box *box)
{
    box_instances[data] = (Box *)box;
}

// Check if definition is conformant to argument A;
// store matched values from arg in box_instances
bool LetNode::domatch(const Box *a) const
{
    bool ret = false;

    // While comparing, for all X, MatchBox == X holds.

    MatchBoxFunc oldCB = MatchBox::matchCallback;
    MatchBox::matchCallback = box_matches;

    MatchBox::matchesAll = true;

    if (_box_pattern == 0)
	compilePatterns(0);

    if (_box_pattern)
	ret = (*_box_pattern == *a);

    MatchBox::matchesAll = false;

    MatchBox::matchCallback = oldCB;
    return ret;
}


// Pattern matching with nodes

static VSLNode *node_instances[max_instances];

// Check if definition is conformant to argument A;
// store matched values from arg in node_instances
static void node_matches(int data, const VSLNode *node)
{
    node_instances[data] = (VSLNode *)node;
}

bool LetNode::domatch(const VSLNode *a) const
{
    // While comparing, for all X, ArgNode == X holds.

    ArgNodeFunc oldCB = ArgNode::matchCallback;
    ArgNode::matchCallback = node_matches;

    bool oldFlag = ArgNode::matchesAll;
    ArgNode::matchesAll = true;

    bool ret = (*_node_pattern == *a);

    ArgNode::matchCallback = oldCB;
    ArgNode::matchesAll = oldFlag;

    return ret;
}

// Return nodes matched against A
VSLNode **LetNode::nodelist(const VSLNode *a) const
{
    for (unsigned i = _base; i < _base + _nargs; i++)
	node_instances[i] = 0;

    bool ok = domatch(a);
    if (!ok)
	return 0;

    return node_instances;
}


// Convert argument list into a format suitable for ArgNode instances.
ListBox* LetNode::arglist(const Box *a) const
{
    if (_straight)
    {
	assert (a->isListBox());
	return (ListBox *)((Box *)a)->link();
    }

    unsigned i;
    for (i = _base; i < _base + _nargs; i++)
	box_instances[i] = 0;

    bool ok = domatch(a);
    if (!ok)
	return (ListBox *)0;

    ListBox *list = new ListBox;
    for (i = _base; i < _base + _nargs; i++)
    {
	assert(box_instances[i] != 0);  // cannot isolate arg
	(*list) += box_instances[i];
    }

    return list;
}


// Convert node_pattern into box_pattern
void LetNode::compilePatterns(VSLDef *cdef) const
{
    // Compile args first
    CallNode::compilePatterns(cdef);

    // Protect against patterns like f(f(a)) = ...
    if (being_compiled)
    {
	VSLLib::eval_error("recursive pattern", cdef);
	return;
    }

    uncompilePatterns(cdef);

    // Use a list of match boxes as argument
    ListBox *list = new ListBox;
    unsigned i;
    for (i = 0; i < _base + _nargs; i++)
    {
	MatchBox *m = new MatchBox(i);
	(*list) += m;
	m->unlink();
    }

    MUTABLE_THIS(LetNode *)->being_compiled = true;
    const Box *result = node_pattern()->eval(list);
    MUTABLE_THIS(LetNode *)->being_compiled = false;

    list->unlink();

    // Check if valid
    if (result == 0)
	VSLLib::eval_error("cannot evaluate pattern", cdef);
    else
    {
	// Each MatchBox must be used exactly once
	int *instances = new int [_base + _nargs];
	for (i = _base; i < _base + _nargs; i++)
	    instances[i] = 0;
	result->countMatchBoxes(instances);

	for (i = _base; i < _base + _nargs; i++)
	{
	    if (instances[i] == 0)
	    {
		std::ostringstream os;
		os << "invalid pattern: arg" << i << " is never instantiated";
		VSLLib::eval_error(os);
	    }
	    if (instances[i] > 1)
	    {
		std::ostringstream os;
		os << "invalid pattern: arg" << i 
		   << " is instantiated several times";
		VSLLib::eval_error(os);
	    }
	}
	delete[] instances;
    }

    MUTABLE_THIS(LetNode *)->_box_pattern = (Box *)result;
}






// Resolve function names
int LetNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    if (_nargs == 0)
	VSLLib::eval_warning("pattern without variables", cdef);

    // First apply to body and pattern args
    // Within body, increase the base: While evaluating the body,
    // we have _nargs more variables.
    int changes = 0;
    
    changes += args()->resolveNames(cdef, base);
    changes += body()->resolveNames(cdef, base + _nargs);

    // Now replace all NameNodes in the pattern by equivalent ArgNodes.
    string s = "";
    unsigned offset = 0;

    while (!(s = node_pattern()->firstName(), s).empty())
    {
	// Replace in body
	int ch = body()->resolveName(cdef, &body(), s, base + offset);
	if (ch == 0)
	    VSLLib::eval_warning("`" + s + "' is unused", cdef);
	changes += ch;

	// Replace in pattern
	ch = node_pattern()->resolveName(cdef, &node_pattern(), s, 
	    base + offset);
	assert(ch > 0);
	if (ch > 1)
	    VSLLib::eval_error("`" + s + "' is used several times", cdef);

	offset++;
    }

    assert(offset == _nargs);

    return changes;
}


// Optimization

// inlineFuncs: Replace pattern vars by function body
// Example: Replace `let a = 45 in a + a' by `45 + 45'

int LetNode::inlineFuncs(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Apply to all args
    changes += CallNode::inlineFuncs(cdef, node);

    // Create instance list: abort if ambiguous
    VSLNode **values = nodelist(args());
    if (values == 0)
	return changes;

    // Create instance counter
    int *instances = new int [_base + _nargs];
    unsigned i;
    for (i = 0; i < _base + _nargs; i++)
	instances[i] = 0;

    // Count how often the variables are used
    body()->countArgNodes(cdef, instances, _base, _nargs);

    // Check if each instantiation is defined
    bool fail = false;
    for (i = _base; i < _base + _nargs; i++)
	if (instances[i] > 0 && values[i] == 0)
	{
	    std::ostringstream os;
	    os << "cannot isolate arg" << i;
	    VSLLib::eval_warning(os, cdef);
	    fail = true;
	}

    // Each instance must be used only once (efficiency)
    for (i = _base; i < _base + _nargs; i++)
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
    VSLNode *newbody = body()->dup();

    // The arg nodes in BODY refer to EXPR; not to the function of
    // this DefCallNode.  We must replace them by the arguments of
    // this DefCallNode.

    newbody->instantiateArgs(cdef, &newbody, values, _base, _nargs);


    // Remaining arg nides in BODY refer to LET constructs.  If the
    // current funtion and NEWBODY have a different number of args,
    // we'll get into trouble.  So, renumber the arg nodes.

    newbody->reBase(cdef, _base);


    // Here's the actual replacement
    *node = newbody;

    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": inlineFuncs: replacing\n"
	    << *this << "\nby " << **node << '\n';
	std::cout.flush();
    }

    delete this;

    return ++changes;
}


// Set a new base
int LetNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    int changes = 0;

    // Rename all LET variables: arg_i -> arg_(i + offset):
    int offset = newBase - _base;

    if (offset == 0)
	return changes;

    // Apply to pattern args
    args()->reBase(cdef, newBase);

    // If offset > 0, rename in body first (to prevent collisions)
    if (offset > 0)
	changes = body()->reBase(cdef, newBase + _nargs);

    VSLNode **argnodes = new VSLNode *[_base + _nargs];
    unsigned i;
    for (i = 0; i < _base; i++)
	argnodes[i] = 0;
    for (i = _base; i < _base + _nargs; i++)
	argnodes[i] = new ArgNode(i + offset);

    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": reBase: replacing\n" << *this;
	std::cout.flush();
    }

    // Rename vars in body
    body()->instantiateArgs(cdef, &body(), argnodes, _base, _nargs);

    // Rename vars in pattern
    node_pattern()->instantiateArgs(cdef, &node_pattern(), 
	argnodes, _base, _nargs);

    if (VSEFlags::show_optimize)
    {
	std::cout << "\nby " << *this << "\n";
	std::cout.flush();
    }

    for (i = _base; i < _base + _nargs; i++)
	delete argnodes[i];
    delete[] argnodes;

    // If offset < 0, now rename vars in body
    if (offset < 0)
	changes = body()->reBase(cdef, newBase + _nargs);

    // Recompile pattern
    _base = newBase;
    compilePatterns(cdef);

    return ++changes;
}


// Debugging

// Representation invariant
bool LetNode::OK() const
{
    EmptyListNode empty;

    assert (CallNode::OK());

    // Arg must be list of length 2
    assert (arg() && arg()->isListNode());
    assert (_args() && _args()->tail() && _args()->tail()->isListNode());
    assert (_body() && _body()->tail() && *(_body()->tail()) == empty);

    // All list elems must exist
    assert (args());
    assert (body());
    assert (node_pattern());

    return true;
}
