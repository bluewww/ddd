// $Id$
// Calling user-defined VSL functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_DefCallNode_h
#define _DDD_DefCallNode_h

#ifdef __GNUG__
#pragma interface
#endif


// A DefCallNode evaluates a user-defined VSL function


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
    VSLDef *_def;           // points to definition if unambiguous, else 0
    VSLDefList *_deflist;   // points to possible definitions

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
    // Copy
    VSLNode *dup() const
    {
	return new DefCallNode(*this);
    }

    // Create from def
    DefCallNode(VSLDef *def, VSLNode *arg, char *type = "DefCallNode");

    // Create from list of defs
    DefCallNode(VSLDefList *deflist, VSLNode *arg, char *type = "DefCallNode");

    // Destructor
    ~DefCallNode();

    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    void rebind(const class VSLLib *lib);

    bool isDefCallNode() const { return true; }

    // Representation invariant
    bool OK() const;
};

#endif
