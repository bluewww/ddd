// $Id$
// Deklaration Klasse ConstNode

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

#ifndef _DDD_ConstNode_h
#define _DDD_ConstNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein ConstNode gibt zur Laufzeit eine Konstante zurueck.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "Box.h"

// ConstNode

class ConstNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    Box *_box;

protected:
    ConstNode(const ConstNode& node):
	VSLNode(node), _box(node._box->link())
    {}
    
    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

    bool matches(const VSLNode& node) const
    {
	return VSLNode::matches(node) &&
		*_box == *(((ConstNode *)&node)->_box); // dirty trick
    }

public:
    // ConstNode erzeugen
    ConstNode(Box *box, char *type = "ConstNode"): 
	VSLNode(type), _box(box)
    {}

    ~ConstNode()
    // ConstNode zerstoeren
    {
	_box->unlink();
    }

    // ConstNode kopieren
    VSLNode *dup() const
    {
	return new ConstNode(*this);
    }

    const Box *_eval(ListBox *arglist) const;

    bool isConst() const { return true; }
    bool isConstNode() const { return true; }

    bool isStraight() const;

    // Repraesentations-Invariante
    bool OK() const;
};



#endif
