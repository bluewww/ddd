// $Id$
// Deklaration Klasse DummyNode

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_DummyNode_h
#define _DDD_DummyNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein DummyNode gibt zur Laufzeit eine DummyBox zurueck.
// Ein DummyNode matcht alle anderen Knoten, wenn matchesAll == true.

#include "assert.h"
#include <iostream.h>

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
    
    void dump(ostream& s) const
    {
	s << "_";
    }

    bool matches(const VSLNode& node) const
    {
	return matchesAll || VSLNode::matches(node);
    }

public:
    // DummyNode erzeugen
    DummyNode(char *type = "DummyNode"): 
	VSLNode(type)
    {}

    // DummyNode kopieren
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
