// $Id$
// Deklaration Klasse DefCallNode

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

#ifndef _Nora_DefCallNode_h
#define _Nora_DefCallNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein DefCallNode wertet zur Laufzeit eine selbstdefinierte Funktion aus.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "CallNode.h"

#include "VSLDefList.h"
#include "VSLDef.h"

// DefCallNode

class DefCallNode: public CallNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLDef *_def;           // Wenn eindeutig: Zeiger auf Definition, sonst 0
    VSLDefList *_deflist;   // Zeiger auf Definitionsliste

protected:
    DefCallNode(const DefCallNode& node);

    const Box *call(Box *arg) const;

    char *func_name() const;

    bool matches(const VSLNode& node) const
    {
	return matchesAll ||
	    (CallNode::matches(node) &&
		_deflist == ((DefCallNode *)&node)->_deflist); // dirty trick
    }

public:
    // DefCallNode kopieren
    VSLNode *dup() const
    {
	return new DefCallNode(*this);
    }

    // DefCallNode aus Definition erzeugen
    DefCallNode(VSLDef *def, VSLNode *arg, char *type = "DefCallNode");

    // DefCallNode aus Definitionsliste erzeugen
    DefCallNode(VSLDefList *deflist, VSLNode *arg, char *type = "DefCallNode");

    // DefCallNode zerstoeren
    ~DefCallNode();

    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);

    bool isDefCallNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
