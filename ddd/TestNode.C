// $Id$
// VSL if..then..else..fi construct

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

char TestNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>

#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "TestNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(TestNode, CallNode)


// TestNode

// Dump
void TestNode::dump(std::ostream& s) const
{
    s << "if "    << *test()
      << " then " << *thetrue()
      << " else " << *thefalse()
      << " fi";
}

// ...as tree
void TestNode::_dumpTree(std::ostream& s) const
{
    test()->dumpTree(s);
    s << ", ";
    thetrue()->dumpTree(s);
    s << ", ";
    thefalse()->dumpTree(s);
}

// Evaluate
const Box *TestNode::_eval(ListBox *arglist) const
{
    // Evaluate tester
    const Box *flagBox = test()->eval(arglist);
    if (flagBox == 0)
	return 0;

    BoxSize size = flagBox->size();
    ((Box *)flagBox)->unlink();

    if (!size.isValid())
    {
	VSLLib::eval_error("invalid if argument");
	return 0;
    }

    // Alternative
    if (size != 0)
	return thetrue()->eval(arglist);
    else
	return thefalse()->eval(arglist);
}


// Optimization

// foldConsts: Evaluate function with constant args

int TestNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    int changes = 0;

    // Apply to args
    // (CallNode::foldConsts() would destroy the list structure)
    changes += test()->foldConsts(cdef, &test());
    changes += thetrue()->foldConsts(cdef, &thetrue());
    changes += thefalse()->foldConsts(cdef, &thefalse());

    // Is test constant?  If not, we're done
    if (!test()->isConst())
	return changes;

    // Otherwise: Depending on result, replace by first or second
    // alternative.

    // Evaluate tester

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *flagBox = test()->eval(0);

    if (flagBox)
    {
	assert(!sideEffectsOccured);

	BoxSize size = flagBox->size();
	((Box *)flagBox)->unlink();

	if (size.isValid())
	{
	    if (VSEFlags::show_optimize)
	    {
		std::cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		    << *this << '\n';
		std::cout.flush();
	    }

	    // Replace TestNode by first or second alternative
	    if (size != 0)
		*node = thetrue(), thetrue() = 0;
	    else
		*node = thefalse(), thefalse() = 0;


	    if (VSEFlags::show_optimize)
	    {
		std::cout << "by " << **node << "\n";
		std::cout.flush();
	    }
		
	    changes++;

	    // Delete TestNode and its subtree
	    delete this;
	}
	else
	    VSLLib::eval_error("invalid if argument", cdef);
    }

    sideEffectsProhibited = false;

    return changes;
}


// resolveDefs: below a TestNode, don't complain about endless recursion
int TestNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    int changes = 0;

    changes += test()->resolveDefs(cdef, complain_recursive);
    changes += thetrue()->resolveDefs(cdef, false);
    changes += thefalse()->resolveDefs(cdef, false);

    return changes;
}


// Debugging

// Representation invariant
bool TestNode::OK() const
{
    EmptyListNode empty;

    assert (CallNode::OK());

    // Check if argument is 3-element list
    assert (arg() && arg()->isListNode());
    assert (_test() && _test()->tail() && _test()->tail()->isListNode());
    assert (_true() && _true()->tail() && _true()->tail()->isListNode());
    assert (_false() && _false()->tail() && *(_false()->tail()) == empty);

    // Check if list elements exist
    assert (test());
    assert (thetrue());
    assert (thefalse());

    return true;
}
