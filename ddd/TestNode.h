// $Id$
// Deklaration Klasse TestNode

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

#ifndef _DDD_TestNode_h
#define _DDD_TestNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein TestNode implementiert ein if-then-else-fi Konstrukt.
// Ein TestNode  enthaelt eine Liste der Laenge 3; bei der Auswertung
// wird das erste Element der Liste ausgewertet; ist es != 0, ist das
// Ergebnis das zweite, sonst das dritte Element der Liste.


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

    // Dummy-Funktionen (werden nie aufgerufen)
    char *func_name() const       { assert(0); return "test"; }
    const Box *call(Box *) const  { assert(0); return 0; }

public:
    // TestNode erzeugen
    TestNode(VSLNode *tst, VSLNode *t, VSLNode *f, 
	char *type = "TestNode"):
	CallNode(new FixListNode(tst, t, f), type)
    {}

    // Ressourcen
    VSLNode *&test()     { return _test()->head();  }
    VSLNode *&thetrue()  { return _true()->head();  }
    VSLNode *&thefalse() { return _false()->head(); }

    VSLNode *test() const     { return _test()->head();  }
    VSLNode *thetrue() const  { return _true()->head();  }
    VSLNode *thefalse() const { return _false()->head(); }

    // TestNode kopieren
    VSLNode *dup() const { return new TestNode(*this); }

    const Box *_eval(ListBox *arglist) const;
    
    int foldConsts(VSLDef *cdef, VSLNode** node);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);

    bool isTestNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
