// $Id$
// A DummyNode returns a DummyBox

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

#ifndef _DDD_DummyNode_h
#define _DDD_DummyNode_h

// A DummyNode returns a DummyBox at run-time.
// A DummyNode matches all other nodes if `DummyNode::matchesAll' is true.

#include "assert.h"
#include <iostream>

#include "VSLNode.h"
#include "Box.h"
#include "DummyBox.h"

// DummyNode

class DummyNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

protected:
    DummyNode(const DummyNode& node):
	VSLNode(node)
    {}
    
    void dump(std::ostream& s) const
    {
	s << "_";
    }

    bool matches(const VSLNode& node) const
    {
	return matchesAll || VSLNode::matches(node);
    }

public:
    // Constructor
    DummyNode(const char *type = "DummyNode")
	: VSLNode(type)
    {}

    // Copy
    VSLNode *dup() const
    {
	return new DummyNode(*this);
    }

    const Box *_eval(ListBox *) const    
    { 
	return new DummyBox; 
    }

    static bool matchesAll;

    bool isConst() const { return true; }
    bool isDummyNode() const { return true; }
};

#endif
