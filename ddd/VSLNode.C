// $Id$
// VSL Nodes

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

char VSLNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>

#include "VSLLib.h"
#include "VSEFlags.h"

#include "VSLNode.h"

DEFINE_TYPE_INFO_0(VSLNode)

// VSLNode

// Flag: Are side effects allowed?
bool VSLNode::sideEffectsProhibited = false;

    // Flag: have side effects been observed?
bool VSLNode::sideEffectsOccured    = false;

// Dump VSL expression
std::ostream& operator << (std::ostream& s, const VSLNode& node)
{
    node.dump(s);
    return s;
}

// ...as tree
void VSLNode::dumpTree(std::ostream& s) const
{
    static unsigned depth = 1;

    s << "\n";
    for (unsigned i = 0; i < depth; i++)
	s << "  ";

    depth++;
    s << _type << "(";
    _dumpTree(s);
    s << ")";
    depth--;
}


// Evaluate
const Box *VSLNode::eval(ListBox *arglist) const
{
    if (VSEFlags::show_tiny_eval)
    {
	std::clog << *this;
	if (arglist)
	    std::clog << " " << *arglist;

	std::clog << "...";
    }

    const Box *box = _eval(arglist);

    if (VSEFlags::show_tiny_eval)
    {
	std::clog << *this;
	if (arglist)
	    std::cout << " " << *arglist;

	if (box == 0)
	    std::clog << " FAILS";
	else
	    std::clog << " = " << *box;

	std::clog << "\n";
    }

    return box;
}


// Compare boxes

bool VSLNode::bothSidesCanMatch = false;

bool VSLNode::operator == (const VSLNode &node) const
{
    if (VSEFlags::show_match_nodes)
    {
	std::cout << "\nNode match: " << *this << " ? " << node;
	std::cout.flush();
    }

    bool flag = (this == &node) || matches(node);

    if (bothSidesCanMatch)
	flag = flag || node.matches(*this);

    if (VSEFlags::show_match_nodes)
    {
	if (flag)
	    std::cout << "\nNode match: " << *this << " == " << node;
	else
	    std::cout << "\nNode match: " << *this << " != " << node;
	
	std::cout.flush();
    }

    return flag;
}


// Debugging

// Representation invariant
bool VSLNode::OK() const
{
    assert (_type != 0);
    return true;
}
