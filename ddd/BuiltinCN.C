// $Id$
// Builtin call VSL nodes

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

char BuiltinCallNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "VSLLib.h"

#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "TrueBox.h"
#include "BuiltinCN.h"
#include "ListNode.h"
#include "VSLDef.h"
#include "TrueNode.h"

DEFINE_TYPE_INFO_1(BuiltinCallNode, CallNode)

// BuiltinCallNode

// Evaluate builtin function
const Box *BuiltinCallNode::call(Box *a) const
{
    assert (a->isListBox());

    // If side effects are prohibited, return error.
    if (sideEffectsProhibited && VSLBuiltin::hasSideEffects(_index))
    {
	sideEffectsOccured = true;
	return 0;
    }

    // Call function via function pointer
    BuiltinFunc func = VSLBuiltin::func(_index);
    return func((ListBox *)a);
}


// Optimization

// foldOps: Build `large' argument lists
// In general: replace f(a, f(...), b) by f(a, ..., b)

// This Implementation only considers the case
// f(f(a...), b...) => f(a..., b...)

int BuiltinCallNode::foldOps(VSLDef *cdef, VSLNode** node)
{
    assert (this == *node);
    int changes = 0;

    // Apply on all arguments
    changes += CallNode::foldOps(cdef, node);

    // If non-associative, return
    if (!VSLBuiltin::isAssoc(_index))
	return changes;

    // If arg is not a list, return
    if (!arg()->isListNode())
	return changes;

    ListNode *args = (ListNode *)arg(); // dirty trick

    // First arg must be a builtin call
    if (!args->head()->isBuiltinCallNode())
	return changes;

    BuiltinCallNode *callee = (BuiltinCallNode *)args->head(); // dirty trick

    // First arg must call the same function
    if (_index != callee->_index)
	return changes;

    // Arg must be a list
    if (!callee->arg()->isListNode())
	return changes;

    ListNode *callArgs = (ListNode *)callee->arg(); // dirty trick

    // Insert list
    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": foldOps: replacing\n" 
	    << *this << '\n';
	std::cout.flush();
    }

    int err = callArgs->append(args->tail());
    if (err)
    {
	if (VSEFlags::show_optimize)
	{
	    std::cout << "ABORTING (no replace) since append impossible\n";
	    std::cout.flush();
	}
	return changes;
    }

    VSLNode *newArgs = callee->arg();
    callee->arg() = 0; args->tail() = 0; delete args;
    arg() = newArgs;

    if (VSEFlags::show_optimize)
    {
	std::cout << "by " << *this << '\n';
	std::cout.flush();
    }

    changes++;

    return changes;
}


// foldConsts: Evaluate functions with constant args right now
// Example: replace f(2 + 2) by f(4)

int BuiltinCallNode::foldConsts(VSLDef *cdef, VSLNode** node)
{
    // Apply standard optimization
    int changes = CallNode::foldConsts(cdef, node);

    // If optimization was a success, return
    if (*node != this || isConst())
	return changes;

    // If non-associative, return
    if (!VSLBuiltin::isAssoc(_index))
	return changes;

    // Otherwise: isolate constant args in constant subexpressions and
    // optimize them separately
    for (VSLNode *a = arg();
	 a->isListNode() && ((ListNode *)a)->tail()->isListNode();
	 a = ((ListNode *)a)->tail())
    {
	ListNode *list = (ListNode *)a;
	ListNode *tail = (ListNode *)list->tail();

	VSLNode *arg1 = list->head();
	VSLNode *arg2 = tail->head();

	if (arg1->isConst() && arg2->isConst())
	{
	    if (VSEFlags::show_optimize)
	    {
		std::cout << "\n" << cdef->longname() << ": foldConsts: replacing\n"
		    << *this << '\n';
		std::cout.flush();
	    }

	    // Found 2 args arg1, arg2 that are both constant: Replace
	    // f(..., arg1, arg2, ...) by f(..., f(arg1, arg2), ...)

	    // Create f(arg1, arg2)
	    ListNode *new_args = new FixListNode(arg1, arg2);
	    BuiltinCallNode *new_f = new BuiltinCallNode(_index, new_args);

	    // Move nextarg into f(arg, nextarg)
	    list->head() = new_f;
	    list->tail() = tail->tail();

	    tail->head() = 0; tail->tail() = 0; delete tail;

	    if (VSEFlags::show_optimize)
	    {
		std::cout << "by " << *this << '\n';
		std::cout.flush();
	    }

	    changes++;
	}
    }

    // Now try optimization once again
    changes += CallNode::foldConsts(cdef, node);

    return changes;
}



// Debugging

// Invariant
bool BuiltinCallNode::OK() const
{
    assert (CallNode::OK());
    return true;
}
