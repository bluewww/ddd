// $Id$
// DefCallNode class

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
// Revision 1.1  1995/05/01 15:47:36  zeller
// Initial revision
//
// Revision 10.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.17  1994/09/05  07:18:40  zeller
// Fix: copy constructors made const correct
//
// Revision 10.16  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.15  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.14  1993/07/28  12:55:16  zeller
// Fix: delete[] statt delete
//
// Revision 10.13  1993/05/22  20:16:55  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.12  1993/04/22  11:07:23  zeller
// Lizenz verbessert
// 
// Revision 10.11  1993/04/21  15:18:27  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// 
// Revision 10.10  1993/04/21  00:27:55  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.9  1993/04/20  22:06:18  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.8  1993/04/20  21:18:48  zeller
// Fix: Header korrigiert
// 
// Revision 10.7  1993/04/16  11:44:25  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1993/01/30  16:26:25  zeller
// All messages after german oversitted
// 
// Revision 10.5  1992/11/03  19:37:14  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.4  1992/06/01  16:51:16  zeller
// Neu: #pragma implementation
// 
// Revision 10.3  1992/06/01  14:58:26  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/05/20  15:37:21  zeller
// Dateinamen verkuerzt
// 
// Revision 10.1  1992/02/18  11:19:48  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:57:12  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:10  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:53:09  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.12  1991/05/24  12:03:22  zeller
// Neu: Aufruf reBase() nach Inlining
// 
// Revision 2.11  1991/05/24  10:09:29  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.10  1991/05/09  16:06:02  zeller
// Warnungen vermieden
// 
// Revision 2.9  1991/05/08  16:41:46  zeller
// inlineFuncs(): *Schweren* Fehler beseitigt:
// War ein Argument ein ListNode, wurde stets expandiert,
// unabhaengig von der Anzahl der Instantiierungen.
// 
// Revision 2.8  1991/04/29  13:57:28  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.7  1991/04/27  17:29:40  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.6  1991/03/01  19:09:21  zeller
// Neu: countInstances(): Instanzen bereits vor Ersetzung zaehlen
// 
// Revision 2.5  1991/03/01  18:13:28  zeller
// Neu: inlineFuncs()
// 
// Revision 2.4  1991/03/01  16:56:34  zeller
// Optimierungsfunktionen von 'VSLOptim' hierher eingefuegt;
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.3  1991/02/20  18:55:03  zeller
// Neues Typenkonzept eingefuehrt;
// Pattern und Argumente nicht mehr auf Listen beschraenkt
// 
// Revision 2.2  1991/02/20  13:15:39  zeller
// Argument CallNode ist jetzt VSLNode
// 
// Revision 2.1  1991/02/20  12:00:01  zeller
// Initial revision
// 

char DefCallNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <iostream.h>
#include <strstream.h>

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSEFlags.h"
#include "Box.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "DefCallN.h"
#include "BuiltinCN.h"
#include "ArgNode.h"

DEFINE_TYPE_INFO_1(DefCallNode, CallNode)


// DefCallNode

// DefCallNode erzeugen
DefCallNode::DefCallNode(VSLDef *def, VSLNode *a, char *type):
    CallNode(a, type), _def(def), _deflist(def->deflist)
{
    _deflist->references++;
}

// DefCallNode erzeugen
DefCallNode::DefCallNode(VSLDefList *deflist, VSLNode *a, char *type):
    CallNode(a, type), _def(0), _deflist(deflist)
{
    _deflist->references++;
}

// DefCallNode kopieren
DefCallNode::DefCallNode(const DefCallNode& node):
    CallNode(node), _def(node._def), _deflist(node._deflist)
{
    _deflist->references++;
}

// DefCallNode zerstoeren
DefCallNode::~DefCallNode()
{
    assert(_deflist->references >= 0);
    _deflist->references--;
}

// Selbstdefinierte Funktion auswerten
const Box *DefCallNode::call(Box *a) const
{
    const Box *box;

    // Funktion finden und aufrufen
    if (_def)
	box = _def->eval(a);
    else
	box = _deflist->eval(a);

    return box;
}

// Funktionsnamen zurueckgeben
char *DefCallNode::func_name() const
{
    return (char *)_deflist->f_name();
}


// Optimierung

// resolveDefs: DefCalls verkuerzen
// Alle Aufrufe f(arg1, arg2, ...), bei denen nur *eine* Definition fuer 
// f() in Frage kommt, durch einen direkten Aufruf dieser Definition ersetzen.

int DefCallNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    // Auf Argument anwenden
    int changes = CallNode::resolveDefs(cdef, complain_recursive);

    if (_def)               // Bereits eindeutig?
	return changes;

    // In beide Richtungen matchen
    bool old_bothSidesCanMatch = VSLNode::bothSidesCanMatch;
    VSLNode::bothSidesCanMatch = true;

    // Alle Argumente...
    bool old_ArgNodeMatchesAll = ArgNode::matchesAll;
    ArgNode::matchesAll = true;

    // und alle Funktionsaufrufe koennen matchen
    bool old_CallNodeMatchesAll = CallNode::matchesAll;
    CallNode::matchesAll = true;

    VSLDef *found = 0;
    for (VSLDef *def = _deflist->first(); def != 0; def = def->listnext())
	if (def->matches(arg()))
	    if (found == 0)
		found = def;        // Erste passende Definition
	    else
	    {
		found = 0;          // Zweite passende: Abbruch
		break;
	    }

    VSLNode::bothSidesCanMatch = old_bothSidesCanMatch;
    CallNode::matchesAll = old_CallNodeMatchesAll;
    ArgNode::matchesAll = old_ArgNodeMatchesAll;

    if (found == 0 && def == 0)
    {
	const int bufsize = 1000;
	char buffer[bufsize];
	ostrstream s(buffer, sizeof buffer);
	s << *this << '\0';

	VSLLib::eval_warning("no suitable definition for " + 
	    string(buffer), cdef);
    }

    if (found == cdef && complain_recursive)
	VSLLib::eval_error("infinite recursion", cdef);

    if (found == 0)
	return changes;

    // Neue Definition setzen
    _def = found;

    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": resolveDefs: resolving\n" 
	    << *this << "\nto " << _def->longname() << "\n";
	cout.flush();
    }

    // und fertig.
    return ++changes;
}



// resolveSynonyms: Synonyme aufloesen
// Alle Aufrufe f() mit f(...) = g(...), g(...) = h(...)
// durch h() ersetzen.

int DefCallNode::resolveSynonyms(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);

    // Auf allen Argumenten ausfuehren
    int changes = CallNode::resolveSynonyms(cdef, node);

    // Wenn nicht eindeutig, abbrechen
    if (_def == 0)
	return changes;

    // Sei f() die aufgerufene Funktion: 
    // Wenn f() nicht definiert ist als f() = g(), abbrechen

    VSLNode *syn = _def->expr();
    if (syn == 0 || !syn->isCallNode())
	return changes;

    CallNode *call_syn = (CallNode *)syn;   // dirty trick

    // Pruefen: Wenn f() nicht definiert ist als
    // f(<pattern>) = g(<pattern>), abbrechen

    VSLNode *my_pattern = _def->node_pattern();
    VSLNode *his_pattern = call_syn->arg();
    if (*my_pattern != *his_pattern)
	return changes;

    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": resolveSynonyms: replacing\n" 
	    << *this << "\n";
	cout.flush();
    }

    if (call_syn->isDefCallNode())
    {
	DefCallNode *defcall_syn = (DefCallNode *)call_syn; // dirty trick

	// f(a1, ..., an) ist definiert als g(a1, ..., an)
	// Aufruf f() durch Aufruf g() ersetzen.

	// DefCallNode durch anderen DefCallNode ersetzen
	// (einfach: nur Definitionszeiger austauschen)

	defcall_syn->_deflist->references++;
	_deflist->references--;

	_def = defcall_syn->_def;
	_deflist = defcall_syn->_deflist;

	changes++;
    }
    else if (call_syn->isBuiltinCallNode())
    {
	BuiltinCallNode *builtin_syn = 
	    (BuiltinCallNode *)call_syn; // dirty trick

	// f(a1, ..., an) ist definiert als g(a1, ..., an)
	// Aufruf f() durch Aufruf g() ersetzen.

	// DefCallNode durch BuiltinCallNode ersetzen
	// (konkretes Auswechseln)

	BuiltinCallNode *newNode = new BuiltinCallNode(*builtin_syn, arg());

	*node = newNode;
	arg() = 0; delete this;

	changes++;
    }

    if (VSEFlags::show_optimize)
    {
	cout << "by " << **node << "\n";
	cout.flush();
    }

    return changes;
}


// inlineFuncs: Funktionsaufrufe durch Funktionskoerper ersetzen
// Etwa: f(a, b, c, d) = max(a, b) + max(c, d) mit 
// max(a, b) = if a > b then a else b fi durch
// f(a, b, c, d) = (if a > b then a else b fi) + (if c > d then c else d fi)
// ersetzen.

int DefCallNode::inlineFuncs(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Zunaechst: Auf allen Argumenten ausfuehren
    changes += CallNode::inlineFuncs(cdef, node);

    // Wenn nicht eindeutig, fertig
    if (_def == 0 || _def->expr() == 0)
	return changes;

    // Instanz-Liste bilden: wenn nicht eindeutig, fertig
    VSLNode **values = _def->nodelist(arg());
    if (values == 0)
	return changes;

    // Instanzen-Zaehler erzeugen
    int *instances = new int [_def->nargs()];
    for (unsigned i = 0; i < _def->nargs(); i++)
	instances[i] = 0;

    // Zaehlen, wie oft einzelne Variablen benutzt werden
    _def->expr()->countArgNodes(cdef, instances, 0, _def->nargs());

    // Pruefen, ob jede Instantiierung definiert
    bool fail = false;
    for (i = 0; i < _def->nargs(); i++)
	if (instances[i] > 0 && values[i] == 0)
	{
	    ostrstream os;
	    os << "cannot isolate arg " << i;
	    VSLLib::eval_warning(os, _def);
	    fail = true;
	}

    // Fuer eine effiziente Instantiierung darf
    // jedes Argument maximal 1x verwendet werden.
    for (i = 0; i < _def->nargs(); i++)
	if (values[i] && instances[i] > 1)
	{
	    // Wenn wir ein Argument jedoch durch eine Konstante
	    // oder ein anderes Argument ersetzen,
	    // erlauben wir auch mehrfache Instantiierungen.

	    if (!values[i]->isConstNode() && !values[i]->isArgNode())
		fail = true;
	}

    delete[] instances;

    if (fail)
	return changes;
    
    // Sonst: eigentliche Ersetzung vornehmen

    // Kopie des Funktionskoerpers holen
    VSLNode *body = _def->expr()->dup();

    // Die ArgNodes im Funktionskoerper verweisen auf Argumente
    // der Funktion expr, nicht der Funktion, der dieser DefCallNode
    // angehoert. Deshalb ersetzen wir sie durch die Argumente
    // des DefCallNodes.

    body->instantiateArgs(cdef, &body, values, 0, _def->nargs());

    // Verbleibende ArgNodes im Funktionskoerper verweisen auf
    // LET-Konstrukte. Wenn die aktuelle Funktion und newbody
    // eine unterschiedliche Anzahl Argumente haben, kann es Aerger geben.
    // Deshalb nehmen wir eine Umbenennung der ArgNode's vor.

    body->reBase(cdef, _base);


    // Ersetzung vornehmen
    *node = body;

    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": inlineFuncs: replacing\n" 
	    << *this << "\nby " << **node << '\n';
	cout.flush();
    }

    delete this;

    return ++changes;
}




// countSelfReferences: Referenzen auf Funktionen zaehlen,
// die innerhalb der Funktionsdefinition auftreten.

int DefCallNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    int changes = CallNode::countSelfReferences(cdef, deflist);

    if (_deflist == deflist)
    {
	if (VSEFlags::show_optimize)
	{
	    cout << "\n" << cdef->longname() 
		<< ": countSelfReferences: found self-reference to " 
		<< deflist->f_name();
	    cout.flush();
	}

	deflist->self_references++;
	changes++;
    }

    return changes;
}



// Debugging

bool DefCallNode::OK() const
// Pruefen, ob alles in Ordnung
{
    assert (_deflist != 0);
    assert (_def == 0 || _def->deflist == _deflist);

    // assert (_def->OK()); // koennte unendliche Schleife erzeugen
    assert (CallNode::OK());

    return true;
}
