// $Id$
// VSL function calls

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

char CallNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "VSEFlags.h"
#include "ConstNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(CallNode, VSLNode)

// CallNode

bool CallNode::matchesAll = false;


// Dump function name
void CallNode::dump(std::ostream& s) const
{
    s << func_name();

    if (VSEFlags::include_list_info)
	s << *_arg;
    else
	if (_arg->isArgNode())
	    s << "(" << *_arg << "...)";
	else
	    s << *_arg;
}

// ...as tree
void CallNode::_dumpTree(std::ostream& s) const
{
    s << "\"" << func_name() << "\", ";
    _arg->dumpTree(s);
}

// Evaluate function
const Box *CallNode::_eval(ListBox *arglist) const
{
    // Evaluate arg
    const Box *myarg = _arg->eval(arglist);
    if (myarg == 0)
	return 0;

    // Call function
    const Box *box = call((Box *)myarg);

    ((Box *)myarg)->unlink();

    return box;
}



// Optimization

// In general, all these apply the optimization on the function arguments

int CallNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    return arg()->resolveDefs(cdef, complain_recursive);
}

int CallNode::resolveSynonyms(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->resolveSynonyms(cdef, &arg());
}

int CallNode::foldOps(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->foldOps(cdef, &arg());
}

int CallNode::inlineFuncs(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->inlineFuncs(cdef, &arg());
}

int CallNode::instantiateArgs(VSLDef *cdef, VSLNode ** /* node */,
			      VSLNode *values[], unsigned base, unsigned n)
{
    // assert(this == *node); 
    return arg()->instantiateArgs(cdef, &arg(), values, base, n);
}

void CallNode::countArgNodes(VSLDef *cdef, int instances[],
    unsigned base, unsigned n)
{
    arg()->countArgNodes(cdef, instances, base, n);
}

void CallNode::compilePatterns(VSLDef *cdef) const
{
    arg()->compilePatterns(cdef);
}

void CallNode::uncompilePatterns(VSLDef *cdef) const
{
    arg()->uncompilePatterns(cdef);
}

int CallNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    return arg()->countSelfReferences(cdef, deflist);
}

int CallNode::resolveName(VSLDef *cdef, VSLNode ** /* node */, const string& name,
    unsigned id)
{
    // assert(this == *node); 
    return arg()->resolveName(cdef, &arg(), name, id);
}

int CallNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    return arg()->resolveNames(cdef, base);
}

int CallNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    return arg()->reBase(cdef, newBase);
}

void CallNode::rebind(const class VSLLib *lib)
{
    arg()->rebind(lib);
}

const string& CallNode::firstName() const
{
    return arg()->firstName();
}



// Fold constants
int CallNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Apply on all aguments
    changes += arg()->foldConsts(cdef, &arg());

    // If still non-constant, return
    if (!isConst())
	return changes;

    // Otherwise: evaluate function, avoiding all side effects
    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *result = eval(0);

    if (result)
    {
	assert(!sideEffectsOccured);

	// Replace CallNode by constant
	if (VSEFlags::show_optimize)
	{
	    std::cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		<< *this << '\n';
	    std::cout.flush();
	}

	*node = new ConstNode((Box *)result);

	if (VSEFlags::show_optimize)
	{
	    std::cout << "by " << **node << "\n";
	    std::cout.flush();
	}
	
	changes++;

	// Delete old CallNode and its subtree
	delete this;
    }

    sideEffectsProhibited = false;

    return changes;
}



// Debugging

// Representation invariant
bool CallNode::OK() const
{
      assert (VSLNode::OK());
      assert (_arg && _arg->OK());

      return true;
}
