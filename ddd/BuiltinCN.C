// $Id$
// Implementation Klasse BuiltinCallNode

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

char BuiltinCallNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"

#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "TrueBox.h"
#include "BuiltinCN.h"
#include "ListNode.h"
#include "VSLDef.h"
#include "TrueNode.h"

DEFINE_TYPE_INFO_1(BuiltinCallNode, CallNode)

// BuiltinCallNode

// Eingebaute Funktion auswerten
const Box *BuiltinCallNode::call(Box *a) const
{
    assert (a->isListBox());

    // Wenn Funktion Seiteneffekte hat, dies vermerken
    // und "Fehler" zurueckgeben, wenn verboten
    if (sideEffectsProhibited && VSLBuiltin::hasSideEffects(_index))
    {
	sideEffectsOccured = true;
	return 0;
    }

    // Funktion ueber Zeiger aufrufen
    BuiltinFunc func = VSLBuiltin::func(_index);
    return func((ListBox *)a);
}


// Optimierung

// foldOps: Fuer eingebaute Operationen grosse Argumentlisten bilden
// Allgemein: f(a, f(...), b) durch f(a, ..., b) ersetzen.

// Diese Implementation beruecksichtigt nur Faelle der Form
// f(f(a...), b...) => f(a..., b...)

int BuiltinCallNode::foldOps(VSLDef *cdef, VSLNode** node)
{
    assert (this == *node);
    int changes = 0;

    // Auf allen Argumenten ausfuehren
    changes += CallNode::foldOps(cdef, node);

    // Wenn keine Zusammenfassung moeglich (nicht-assoziativ), zurueckkehren
    if (!VSLBuiltin::isAssoc(_index))
	return changes;

    // Wenn Argument kein Listenknoten, zurueckkehren
    if (!arg()->isListNode())
	return changes;

    ListNode *args = (ListNode *)arg(); // dirty trick

    // Wenn erstes Argument kein BuiltinCall, zurueckkehren
    if (!args->head()->isBuiltinCallNode())
	return changes;

    BuiltinCallNode *callee = (BuiltinCallNode *)args->head(); // dirty trick

    // Wenn nicht gleiche Funktion, ebenfalls zurueckkehren
    if (_index != callee->_index)
	return changes;

    // Wenn Argument kein Listenknoten, ebenfalls zurueckkehren
    if (!callee->arg()->isListNode())
	return changes;

    ListNode *callArgs = (ListNode *)callee->arg(); // dirty trick

    // Sonst: Liste einfuegen
    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": foldOps: replacing\n" 
	    << *this << '\n';
	cout.flush();
    }

    int err = callArgs->append(args->tail());
    if (err)
    {
	if (VSEFlags::show_optimize)
	{
	    cout << "ABORTING (no replace) since append impossible\n";
	    cout.flush();
	}
	return changes;
    }

    VSLNode *newArgs = callee->arg();
    callee->arg() = 0; args->tail() = 0; delete args;
    arg() = newArgs;

    if (VSEFlags::show_optimize)
    {
	cout << "by " << *this << '\n';
	cout.flush();
    }

    changes++;

    return changes;
}


// foldConsts: Funktionen mit konstanten Argumenten sofort auswerten
// Etwa: f(2 + 2) durch f(4) ersetzen.

int BuiltinCallNode::foldConsts(VSLDef *cdef, VSLNode** node)
{
    // Zunaechst Standard-Optimierung anwenden
    int changes = CallNode::foldConsts(cdef, node);

    // Wenn Optimierung gelungen: fertig
    if (*node != this || isConst())
	return changes;

    // Wenn nicht assoziativ: fertig
    if (!VSLBuiltin::isAssoc(_index))
	return changes;

    // Sonst: konstante Teile in eigenen Unterausdruecken isolieren
    // und getrennt optimieren.
    for (VSLNode *a = arg();
	 a->isListNode() && ((ListNode *)a)->tail()->isListNode();
	 a = ((ListNode *)a)->tail())
    {
	ListNode *list = (ListNode *)a;
	ListNode *tail = (ListNode *)list->tail();

	VSLNode *arg1 = list->head();
	VSLNode *arg2 = tail->head();

	if (arg1->isConst() && arg2->isConst())
	{
	    if (VSEFlags::show_optimize)
	    {
		cout << "\n" << cdef->longname() << ": foldConsts: replacing\n"
		    << *this << '\n';
		cout.flush();
	    }

	    // 2 Argumente arg1, arg2 gefunden, die beide konstant sind
	    // Jetzt f(..., arg1, arg2, ...) durch
	    // f(..., f(arg1, arg2), ...) ersetzen

	    // f(arg1, arg2) anlegen
	    ListNode *new_args = new FixListNode(arg1, arg2);
	    BuiltinCallNode *new_f = new BuiltinCallNode(_index, new_args);

	    // nextarg in f(arg, nextarg) verschieben
	    list->head() = new_f;
	    list->tail() = tail->tail();

	    tail->head() = 0; tail->tail() = 0; delete tail;

	    if (VSEFlags::show_optimize)
	    {
		cout << "by " << *this << '\n';
		cout.flush();
	    }

	    changes++;
	}
    }

    // Jetzt noch einmal den Knoten selbst und alle Argumente optimieren
    changes += CallNode::foldConsts(cdef, node);

    return changes;
}



// Debugging

bool BuiltinCallNode::OK() const
// Pruefen, ob alles in Ordnung
{
    assert (CallNode::OK());
    return true;
}
