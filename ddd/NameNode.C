// $Id$
// Implementation Klasse NameNode

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char NameNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"
#include "NameNode.h"
#include "ArgNode.h"
#include "VSEFlags.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(NameNode, VSLNode)


// NameNode

// NameNode ausgeben
void NameNode::dump(ostream& s) const
{
    s << _name;
}

// ...in Baum-Notation
void NameNode::_dumpTree(ostream& s) const
{
    s << "\"" << _name << "\"";
}


// Ersetzen

// Namen durch Variable ersetzen
int NameNode::resolveName(VSLDef *cdef, VSLNode **node, string& name, 
    unsigned id)
{
    assert (this == *node);

    if (_name == name)
    {
	// NameNode durch ArgNode(id) ersetzen
	*node = new ArgNode(id);

	if (VSEFlags::show_vars)
	{
	    cout << "\n" << cdef->longname() << ":resolveNames: replacing " 
		<< *this << " by " << **node;
	    cout.flush();
	}

	delete this; 
	return 1;
    }

    return 0;
}

// Debugging

bool NameNode::OK() const
// Pruefen, ob alles in Ordnung
{
    assert (VSLNode::OK());
    return true;
}
