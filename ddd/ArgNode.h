// $Id$
// Deklaration Klasse ArgNode

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

#ifndef _Nora_ArgNode_h
#define _Nora_ArgNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein ArgNode wertet zur Laufzeit ein Argument aus der Argumentliste aus.

// ArgNode's koennen fuer Pattern-Matching verwendet werden. 
// Ist 'matchAll' gesetzt, so matcht ein ArgNode alle anderen Knoten.
// Ist 'matchCallback' gesetzt, wird beim Matchen eine Funktion aufgerufen,
// die den gematch'ten Knoten erhaelt.


#include "assert.h"
#include <iostream.h>
#include "strclass.h"

#include "VSLNode.h"
#include "DummyNode.h"


// ArgNode

typedef void (*ArgNodeFunc)(int, const VSLNode *);

class ArgNode: public DummyNode {
public:
    DECLARE_TYPE_INFO

private:
    unsigned _id;           // #Argument in arglist: erstes 0, zweites 1...

protected:
    ArgNode(const ArgNode& node):
	DummyNode(node), 
	_id(node._id)
    {}

    bool matches(const VSLNode &node) const;

    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

public:
    // ArgNode erzeugen
    ArgNode(unsigned i, char *type = "ArgNode"):
	DummyNode(type), _id(i)
    {}

    // ArgNode kopieren
    VSLNode *dup() const { return new ArgNode(*this); }

    // Auswerten
    const Box *_eval(ListBox *arglist) const;

    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);

    bool isConst() const   { return false; }
    bool isArgNode() const { return true; }

    unsigned id()       { return _id; }

    unsigned nargs() const    { return 1; }

    static ArgNodeFunc matchCallback; // Flag: bei Matching Funktion aufrufen?
};

#endif
