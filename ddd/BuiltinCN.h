// $Id$
// Deklaration Klasse BuiltinCallNode

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_BuiltinCallNode_h
#define _DDD_BuiltinCallNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein BuiltinCallNode wertet zur Laufzeit eine der eingebauten
// Funktionen aus.


#include "assert.h"
#include <iostream.h>

#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "DefCallN.h"

// BuiltinCallNode

class BuiltinCallNode: public CallNode {
public:
    DECLARE_TYPE_INFO

    friend class DefCallNode;

private:
    int _index;

protected:
    BuiltinCallNode(const BuiltinCallNode& node):
	CallNode(node), _index(node._index)
    {}

    const Box *call(Box *arg) const;

    char *func_name() const { return VSLBuiltin::ext_name(_index); }
    bool isInfix() const { return VSLBuiltin::isInfix(_index); }

    bool matches (const VSLNode& node) const
    {
	return matchesAll || (CallNode::matches(node) &&
	    _index == ((BuiltinCallNode *)&node)->_index);  // dirty trick
    }

public:
    // Neuen Knoten erzeugen
    BuiltinCallNode(int i, VSLNode *a,
	char *type = "BuiltinCallNode"): 
	CallNode(a, type), _index(i)
    {}

    BuiltinCallNode(BuiltinCallNode &node, VSLNode *a,
	char *type = "BuiltinCallNode"):
	CallNode(a, type), _index(node._index)
    {}

    // BuiltinCallNode kopieren
    VSLNode *dup() const
    {
	return new BuiltinCallNode(*this);
    }

    int foldOps(VSLDef *cdef, VSLNode** node);
    int foldConsts(VSLDef *cdef, VSLNode** node);

    bool isBuiltinCallNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
