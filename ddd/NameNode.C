// $Id$
// VSL Variable names

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

char NameNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "VSLLib.h"
#include "NameNode.h"
#include "ArgNode.h"
#include "VSEFlags.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(NameNode, VSLNode)


// NameNode

// Dump
void NameNode::dump(std::ostream& s) const
{
    s << _name;
}

// ...as tree
void NameNode::_dumpTree(std::ostream& s) const
{
    s << "\"" << _name << "\"";
}


// Replace

// Replace name by variable
int NameNode::resolveName(VSLDef *cdef, VSLNode **node, const string& name, 
			  unsigned id)
{
    assert (this == *node);

    if (_name == name)
    {
	// Replace NameNode by ArgNode(id)
	*node = new ArgNode(id);

	if (VSEFlags::show_vars)
	{
	    std::cout << "\n" << cdef->longname() << ":resolveNames: replacing " 
		<< *this << " by " << **node;
	    std::cout.flush();
	}

	delete this; 
	return 1;
    }

    return 0;
}

// Debugging

// Representation invariant
bool NameNode::OK() const
{
    assert (VSLNode::OK());
    return true;
}
