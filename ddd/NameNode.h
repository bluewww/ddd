// $Id$
// Deklaration Klasse NameNode

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_NameNode_h
#define _DDD_NameNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein NameNode ist ein Platzhalter fuer einen Variablennamen.


#include "assert.h"
#include <iostream.h>
#include "strclass.h"

#include "VSLNode.h"


// NameNode

class NameNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    string _name;           // Variablenname

protected:
    NameNode(const NameNode& node):
	VSLNode(node), 
	_name(node._name)
    {}

    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

public:
    // NameNode erzeugen
    NameNode(string nm, char *type = "NameNode"):
	VSLNode(type), _name(nm)
    {}

    // NameNode kopieren
    VSLNode *dup() const { return new NameNode(*this); }

    // Auswerten
    const Box *_eval(ListBox *) const        { assert(0); return 0; }

    // Ressourcen
    bool isConst() const   { return false; }
    bool isNameNode() const { return true; }

    unsigned nargs() const    { return 1; }

    // Namen durch Variable ersetzen
    int resolveName(VSLDef *cdef, VSLNode **node, string& name, unsigned id);
    string firstName() const { return _name; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
