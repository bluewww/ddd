// $Id$
// VSL Nodes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char VSLNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <iostream.h>

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
ostream& operator << (ostream& s, const VSLNode& node)
{
    node.dump(s);
    return s;
}

// ...as tree
void VSLNode::dumpTree(ostream& s) const
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
	cout << "\n" << *this;
	if (arglist)
	    cout << " " << *arglist;

	cout << ": ";
	cout.flush();
    }

    const Box *box = _eval(arglist);

    if (VSEFlags::show_tiny_eval)
    {
	cout << "\n" << *this;
	if (arglist)
	    cout << " " << *arglist;

	if (box == 0)
	    cout << " FAILS";
	else
	    cout << " = " << *box;

	cout.flush();
    }

    return box;
}


// Compare boxes

bool VSLNode::bothSidesCanMatch = false;

bool VSLNode::operator == (const VSLNode &node) const
{
    if (VSEFlags::show_match_nodes)
    {
	cout << "\nNode match: " << *this << " ? " << node;
	cout.flush();
    }

    bool flag = (this == &node) || matches(node);

    if (bothSidesCanMatch)
	flag = flag || node.matches(*this);

    if (VSEFlags::show_match_nodes)
    {
	if (flag)
	    cout << "\nNode match: " << *this << " == " << node;
	else
	    cout << "\nNode match: " << *this << " != " << node;
	
	cout.flush();
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
