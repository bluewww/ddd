// $Id$ U%
// Implementation Klasse TestNode

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

// $Log$
// Revision 1.1  1995/05/01 15:47:23  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:17:31  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  11:07:47  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:28:04  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:44:36  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1993/01/30  16:26:28  zeller
// All messages after german oversitted
// 
// Revision 10.3  1992/11/03  19:37:22  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:23  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:19:53  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:58:25  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:51  zeller
// Installierte Version
// 
// Revision 4.3  1991/07/05  13:22:58  zeller
// Pruefung auf gueltige Groesse eingefuehrt
// 
// Revision 4.2  1991/07/02  14:38:20  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:53:18  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.11  1991/05/21  13:04:52  zeller
// foldConsts(): Ergebnis freigegeben
// 
// Revision 2.10  1991/05/09  16:06:12  zeller
// Warnungen vermieden
// 
// Revision 2.9  1991/05/03  13:46:06  zeller
// foldConsts: Pruefung auf gueltiges Ergebnis/Seiteneffekte verbessert
// 
// Revision 2.8  1991/04/29  13:05:44  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.7  1991/03/01  16:56:50  zeller
// Optimierungsfunktionen von 'VSLOptim' hierher eingefuegt;
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.6  1991/03/01  14:40:50  zeller
// OK() verbessert
// 
// Revision 2.5  1991/03/01  14:34:09  zeller
// test(), true(), false() zeigen jetzt direkt auf Argumente
// 
// Revision 2.4  1991/02/25  16:24:51  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.3  1991/02/24  19:05:53  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.2  1991/02/20  18:58:48  zeller
// Neues Typenkonzept eingefuehrt;
// Neue Fehlerbehandlung: Bei Fehler NOT_A_BOX zurueckgeben
// 
// Revision 2.1  1991/02/20  12:00:08  zeller
// Initial revision
// 

char TestNode_rcsid[] = "$Id$";

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
