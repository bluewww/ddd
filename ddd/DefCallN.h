// $Id$
// Deklaration Klasse DefCallNode

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_DefCallNode_h
#define _DDD_DefCallNode_h

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

private:
    DefCallNode& operator = (const DefCallNode&)
    {
	assert(0); return *this;
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
