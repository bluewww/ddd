// $Id$ U%
// Implementation Klasse TestNode

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

char TestNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "assert.h"
#include <iostream.h>

#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "TestNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(TestNode, CallNode)


// TestNode

void TestNode::dump(ostream& s) const
// Test ausgeben
{
    s << "if "    << *test()
      << " then " << *thetrue()
      << " else " << *thefalse()
      << " fi";
}

// ...als Baum
void TestNode::_dumpTree(ostream& s) const
{
    test()->dumpTree(s);
    s << ", ";
    thetrue()->dumpTree(s);
    s << ", ";
    thefalse()->dumpTree(s);
}

const Box *TestNode::_eval(ListBox *arglist) const
// Test auswerten
{
    // Tester auswerten
    const Box *flagBox = test()->eval(arglist);
    if (flagBox == 0)
	return 0;

    BoxSize size = flagBox->size();
    ((Box *)flagBox)->unlink();

    if (!size.isValid())
    {
	VSLLib::eval_error("invalid if argument");
	return 0;
    }

    // Alternative
    if (size != 0)
	return thetrue()->eval(arglist);
    else
	return thefalse()->eval(arglist);
}


// Optimierung

// foldConsts: Funktionen mit konstanten Argumenten sofort auswerten
// Etwa: f(2 + 2) durch f(4) ersetzen.

int TestNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    int changes = 0;

    // Zunaechst auf Argumente anwenden.
    // (CallNode::foldConsts() wuerde die Listenstruktur zerstoeren)
    changes += test()->foldConsts(cdef, &test());
    changes += thetrue()->foldConsts(cdef, &thetrue());
    changes += thefalse()->foldConsts(cdef, &thefalse());

    // Wenn Test nicht konstant: fertig
    if (!test()->isConst())
	return changes;

    // Sonst: Abhaengig vom Test-Ergebnis durch true-Ast oder
    // false-Ast ersetzen.

    // Tester auswerten

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *flagBox = test()->eval(0);

    if (flagBox)
    {
	assert(!sideEffectsOccured);

	BoxSize size = flagBox->size();
	((Box *)flagBox)->unlink();

	if (size.isValid())
	{
	    // TestNode durch Konstante ersetzen
	    if (VSEFlags::show_optimize)
	    {
		cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		    << *this << '\n';
		cout.flush();
	    }

	    // 'this' durch entsprechenden Ast ersetzen
	    if (size != 0)
		*node = thetrue(), thetrue() = 0;
	    else
		*node = thefalse(), thefalse() = 0;


	    if (VSEFlags::show_optimize)
	    {
		cout << "by " << **node << "\n";
		cout.flush();
	    }
		
	    changes++;

	    // TestNode und daranhaengenden Teilbaum loeschen
	    delete this;
	}
	else
	    VSLLib::eval_error("invalid if argument", cdef);
    }

    sideEffectsProhibited = false;

    return changes;
}


// resolveDefs: Unterhalb eines TestNode nicht mehr ueber
// unendliche Rekursion laestern
int TestNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    int changes = 0;

    changes += test()->resolveDefs(cdef, complain_recursive);
    changes += thetrue()->resolveDefs(cdef, false);
    changes += thefalse()->resolveDefs(cdef, false);

    return changes;
}


// Debugging

// Pruefen, ob alles in Ordnung
bool TestNode::OK() const
{
    EmptyListNode empty;

    assert (CallNode::OK());

    // Pruefen, ob Argument 3-stellige Liste
    assert (arg() && arg()->isListNode());
    assert (_test() && _test()->tail() && _test()->tail()->isListNode());
    assert (_true() && _true()->tail() && _true()->tail()->isListNode());
    assert (_false() && _false()->tail() && *(_false()->tail()) == empty);

    // Pruefen, ob Listenelemente existieren
    assert (test());
    assert (thetrue());
    assert (thefalse());

    return true;
}
