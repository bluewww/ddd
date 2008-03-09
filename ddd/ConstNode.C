// $Id$
// Constants

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

char ConstNode_rcsid[] = 
    "$Id$";

#include <iostream>
#include "assert.h"
#include "VSLLib.h"
#include "VSEFlags.h"
#include "VSLNode.h"
#include "ConstNode.h"
#include "ListBox.h"
#include "StringBox.h"

DEFINE_TYPE_INFO_1(ConstNode, VSLNode)

// ConstNode

// Evaluate
const Box *ConstNode::_eval(ListBox *) const
{
    return _box->link();
}

// Dump
void ConstNode::dump(std::ostream& s) const
{
    if (VSEFlags::include_const_info)
	s << *_box;
    else
	s << "const_" << _box->id();
}

// ...as tree
void ConstNode::_dumpTree(std::ostream& s) const
{
    bool old = StringBox::quoted;
    StringBox::quoted = true;
    s << "\"" << *_box << "\"";
    StringBox::quoted = old;
}

// Check whether argument list can be used directly
bool ConstNode::isStraight() const
{
    // Check for empty list
    return (_box->isListBox() && ((ListBox *)_box)->isEmpty());
}


// Debugging

// Representation invariant
bool ConstNode::OK() const
{
    assert (VSLNode::OK());
    assert (_box && _box->OK());

    return true;
}
