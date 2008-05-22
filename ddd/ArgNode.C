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

char ArgNode_rcsid[] = 
    "$Id$";

#include <sstream>
#include "assert.h"
#include "VSLLib.h"
#include "ArgNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(ArgNode, DummyNode)

// ArgNode

ArgNodeFunc ArgNode::matchCallback = 0;


// Evaluate argument
const Box *ArgNode::_eval(ListBox *arglist) const
{
    const Box *box = 0;

    if (arglist)
    {
	box = (*arglist)[_id];
	if (box)
	    box = ((Box *)box)->link();
    }

    if (box == 0)
    {
	std::ostringstream os;
	os << "arg" << _id << " cannot be isolated";
	VSLLib::eval_error(os);
    }

    return box;
}

// Dump ArgNode
void ArgNode::dump(std::ostream& s) const
{
    s << "arg" << _id;
}

// Dump as tree
void ArgNode::_dumpTree(std::ostream& s) const
{
    s << _id;
}

bool ArgNode::matches(const VSLNode &node) const
{
    bool ret = matchesAll ||
	VSLNode::matches(node) &&
	    _id == ((const ArgNode *)&node)->_id; // dirty trick

    if (ret && matchCallback)
	matchCallback(_id, &node);

    return ret;
}


// Optimization

 // instantiateArgs: in expressions f(arg1, arg2, ...), replace
// arg1 .. argn by actual arguments

// Replace NODE by corresponding expr from VALUES
int ArgNode::instantiateArgs(VSLDef *, VSLNode **node, VSLNode *values[],
			     unsigned base, unsigned n)
{
    assert(this == *node);

    // Abort if out of area
    if (_id < base || _id >= base + n)
	return 0;

    VSLNode *source = values[_id];
    assert (source != 0);

    VSLNode *newNode = source->dup();
    *node = newNode;

#if 0
    if (VSEFlags::show_optimize)
    {
	std::cout << "\n" << cdef->longname() << ": instantiateArgs: replacing\n" 
	    << *this << "\nby " << *newNode << '\n';
	std::cout.flush();
    }
#endif

    // Delete ArgNode
    delete this;

    return 1;
}

// Increase instance counter
void ArgNode::countArgNodes(VSLDef *, int instances[],
			    unsigned base, unsigned n)
{
    // Ignore if out of area
    if (_id < base || _id >= base + n)
	return;

    instances[_id]++;
}
