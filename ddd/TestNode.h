// $Id$
// VSL if..then..else..fi construct

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

#ifndef _DDD_TestNode_h
#define _DDD_TestNode_h

#ifdef __GNUG__
#pragma interface
#endif


// A TestNode implements an if-then-else-fi construct.

// A TestNode contains a List of length 3.  Upon evaluation, the wird
// first list elem is evaluated.  If it is != 0, the result is the
// second list element; otherise, the third element in the list.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "ListNode.h"
#include "CallNode.h"
#include "TrueNode.h"



// TestNode

class TestNode: public CallNode {
public:
    DECLARE_TYPE_INFO

private:
    ListNode *_test() const  { return (ListNode *)arg(); }
    ListNode *_true() const  { return (ListNode *)(_test()->tail()); }
    ListNode *_false() const { return (ListNode *)(_true()->tail()); }

protected:
    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

    TestNode(const TestNode& node):
	CallNode(node)
    {}

    // Dummy functions (never called)
    char *func_name() const       { assert(0); return "test"; }
    const Box *call(Box *) const  { assert(0); return 0; }

public:
    // Constructor
    TestNode(VSLNode *tst, VSLNode *t, VSLNode *f, 
	     char *type = "TestNode")
	: CallNode(new FixListNode(tst, t, f), type)
    {}

    // Resources
    VSLNode *&test()     { return _test()->head();  }
    VSLNode *&thetrue()  { return _true()->head();  }
    VSLNode *&thefalse() { return _false()->head(); }

    VSLNode *test() const     { return _test()->head();  }
    VSLNode *thetrue() const  { return _true()->head();  }
    VSLNode *thefalse() const { return _false()->head(); }

    // Copy
    VSLNode *dup() const { return new TestNode(*this); }

    const Box *_eval(ListBox *arglist) const;
    
    int foldConsts(VSLDef *cdef, VSLNode** node);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);

    bool isTestNode() const { return true; }

    // Representation invariant
    bool OK() const;
};

#endif
