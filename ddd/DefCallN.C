// $Id$
// DefCallNode class

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

char DefCallNode_rcsid[] = 
    "$Id$";

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
    VSLDef *def;
    for (def = _deflist->first(); def != 0; def = def->listnext())
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

	VSLLib::eval_warning(string("no suitable definition for ") + 
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
    unsigned i;
    for (i = 0; i < _def->nargs(); i++)
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
