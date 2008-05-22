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

#ifndef _DDD_NameNode_h
#define _DDD_NameNode_h


// A NameNode is a place holder for a variable name.


#include "assert.h"
#include <iostream>
#include "strclass.h"

#include "VSLNode.h"


// NameNode

class NameNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    string _name;           // Variable name

protected:
    NameNode(const NameNode& node):
	VSLNode(node), 
	_name(node._name)
    {}

    void dump(std::ostream& s) const;
    void _dumpTree(std::ostream& s) const;

public:
    // Constructor
    NameNode(const string& nm, const char *type = "NameNode"):
	VSLNode(type), _name(nm)
    {}

    // Copy
    VSLNode *dup() const { return new NameNode(*this); }

    // Evaluate
    const Box *_eval(ListBox *) const        { assert(0); return 0; }

    // Properties
    bool isConst() const   { return false; }
    bool isNameNode() const { return true; }

    unsigned nargs() const    { return 1; }

    // Replace name by variable
    int resolveName(VSLDef *cdef, VSLNode **node, const string& name, unsigned id);
    const string& firstName() const { return _name; }

    // Representation invariant
    bool OK() const;
};

#endif
