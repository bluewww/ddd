// $Id$
// Deklaration Klasse ConstNode

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_ConstNode_h
#define _Nora_ConstNode_h

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
