// $Id$
// LetNode class

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

char LetNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <iostream.h>
#include <strstream.h>

#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "LetNode.h"
#include "ArgNode.h"
#include "VSLDef.h"
#include "MatchBox.h"
#include "ListBox.h"
#include "VSEFlags.h"

DEFINE_TYPE_INFO_1(LetNode, CallNode)
DEFINE_TYPE_INFO_1(WhereNode, LetNode)

// LetNode

// Let ausgeben
void LetNode::dump(ostream& s) const
{
    s << "let " << *node_pattern() << " = "  << *args() 
      << "\n  in " << *body();
}

// where ausgeben
void WhereNode::dump(ostream& s) const
{
    s << *body() 
       << "\n  where " << *node_pattern() << " = " << *args();
}


// ...als Baum
void LetNode::_dumpTree(ostream& s) const
{
    node_pattern()->dumpTree(s);
    s << ", ";
    args()->dumpTree(s);
    s << ", ";
    body()->dumpTree(s);
}


// Let auswerten
const Box *LetNode::_eval(ListBox *arglst) const
{
    const Box *result = 0;

    if (arglst)
	assert(_base == arglst->length());

    // Argumente fuer Pattern bilden
    const Box *patternArgs = args()->eval(arglst);

    // Neue Argumentliste bilden
    ListBox *moreArgs = arglist(patternArgs);

    if (moreArgs)
    {
	ListBox *attach = 0;

	// Neue Argumentliste ggf. an bestehende anhaengen
	if (arglst && !(arglst->isEmpty()))
	{
	    attach = arglst->cons(moreArgs);
	    moreArgs->unlink();
	    moreArgs = (ListBox *)arglst->link();
	}

	result = body()->eval(moreArgs);
	moreArgs->unlink();

	// Neue Argumentliste wieder entfernen
	if (attach)
	    arglst->uncons(attach);
    }
    else
	VSLLib::eval_error("invalid arguments");

    ((Box *)patternArgs)->unlink();
    return result;
}


// Pattern-Matching

const int max_instances = 256;


// Pattern-Matching mit Boxen

static Box *box_instances[max_instances];

static void box_matches(int data, Box *box)
{
    box_instances[data] = box;
}

// Pruefen, ob Definition mit Parameter 'a' uebereinstimmt;
// hierbei gematchte Werte aus arg in box_instances ablegen
bool LetNode::domatch(const Box *a) const
{
    bool ret = false;

    // Fuer Dauer des Vergleiches gilt fuer alle x: MatchBox == x

    MatchBoxFunc oldCB = MatchBox::matchCallback;
    MatchBox::matchCallback = box_matches;

    MatchBox::matchesAll = true;

    if (_box_pattern == 0)
	compilePatterns(0);

    if (_box_pattern)
	ret = (*_box_pattern == *a);

    MatchBox::matchesAll = false;

    MatchBox::matchCallback = oldCB;
    return ret;
}


// Pattern-Matching mit Knoten

static VSLNode *node_instances[max_instances];

static void node_matches(int data, VSLNode *node)
{
    node_instances[data] = node;
}

// Pruefen, ob Definition mit Aufruf 'a' uebereinstimmt;
// hierbei gematchte Werte aus a in node_instances ablegen
bool LetNode::domatch(const VSLNode *a) const
{
    // Fuer Dauer des Vergleiches sind fuer alle x: ArgNode == x

    ArgNodeFunc oldCB = ArgNode::matchCallback;
    ArgNode::matchCallback = node_matches;

    bool oldFlag = ArgNode::matchesAll;
    ArgNode::matchesAll = true;

    bool ret = (*_node_pattern == *a);

    ArgNode::matchCallback = oldCB;
    ArgNode::matchesAll = oldFlag;

    return ret;
}

// mit 'a' gematch'te Knoten zurueckgeben
VSLNode **LetNode::nodelist(const VSLNode *a) const
{
    for (unsigned i = _base; i < _base + _nargs; i++)
	node_instances[i] = 0;

    bool ok = domatch(a);
    if (!ok)
	return 0;

    return node_instances;
}


// Argumentliste in ein Format konvertieren, dass ArgNode's
// darauf zugreifen koennen.
ListBox* LetNode::arglist(const Box *a) const
{
    if (_straight)
    {
	assert (a->isListBox());
	return (ListBox *)((Box *)a)->link();
    }

    for (unsigned i = _base; i < _base + _nargs; i++)
	box_instances[i] = 0;

    bool ok = domatch(a);
    if (!ok)
	return (ListBox *)0;

    ListBox *list = new ListBox;
    for (i = _base; i < _base + _nargs; i++)
    {
	assert(box_instances[i] != 0);  // cannot isolate arg
	(*list) += box_instances[i];
    }

    return list;
}


// node_pattern in box_pattern umwandeln
void LetNode::compilePatterns(VSLDef *cdef) const
{
    // Zunaechst Argumente compilieren
    CallNode::compilePatterns(cdef);

    // Schutz gegen Pattern der Form f(f(a)) = ...
    if (being_compiled)
    {
	VSLLib::eval_error("recursive pattern", cdef);
	return;
    }

    uncompilePatterns(cdef);

    // Als Argumente Liste von MatchBoxen uebergeben
    ListBox *list = new ListBox;
    for (unsigned i = 0; i < _base + _nargs; i++)
    {
	MatchBox *m = new MatchBox(i);
	(*list) += m;
	m->unlink();
    }

    ((LetNode *)this)->being_compiled = true;
    const Box *result = node_pattern()->eval(list);
    ((LetNode *)this)->being_compiled = false;

    list->unlink();

    // Pruefen, ob Resultat gueltig
    if (result == 0)
	VSLLib::eval_error("cannot evaluate pattern", cdef);
    else
    {
	// MatchBoxen zaehlen: Jede muss genau 1x vorkommen
	int *instances = new int [_base + _nargs];
	for (i = _base; i < _base + _nargs; i++)
	    instances[i] = 0;
	result->countMatchBoxes(instances);

	for (i = _base; i < _base + _nargs; i++)
	{
	    if (instances[i] == 0)
	    {
		ostrstream os;
		os << "invalid pattern: arg" << i << " is never instantiated";
		VSLLib::eval_error(os);
	    }
	    if (instances[i] > 1)
	    {
		ostrstream os;
		os << "invalid pattern: arg" << i 
		   << " is instantiated several times";
		VSLLib::eval_error(os);
	    }
	}
	delete[] instances;
    }

    ((LetNode *)this)->_box_pattern = (Box *)result;
}






// Namen aufloesen
int LetNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    if (_nargs == 0)
	VSLLib::eval_warning("pattern without variables", cdef);

    // Zunaechst im Koerper und Pattern-Argumenten anwenden
    // Im Koerper Basis erhoehen: Wenn wir den Koerper auswerten,
    // haben wir _nargs Variablen mehr.
    int changes = 0;
    
    changes += args()->resolveNames(cdef, base);
    changes += body()->resolveNames(cdef, base + _nargs);

    // Jetzt alle NameNode's im Pattern durch entsprechende
    // ArgNode's ersetzen.
    string s = "";
    int offset = 0;

    while ((s = node_pattern()->firstName(), s) != "")
    {
	// Im Koerper ersetzen
	int ch = body()->resolveName(cdef, &body(), s, base + offset);
	if (ch == 0)
	    VSLLib::eval_warning("`" + s + "' is unused", cdef);
	changes += ch;

	// Im Pattern ersetzen
	ch = node_pattern()->resolveName(cdef, &node_pattern(), s, 
	    base + offset);
	assert(ch > 0);
	if (ch > 1)
	    VSLLib::eval_error("`" + s + "' is used several times", cdef);

	offset++;
    }

    assert(offset == _nargs);

    return changes;
}


// Optimierung

// inlineFuncs: Pattern-Variablen durch Funktionskoerper ersetzen
// Etwa: let a = 45 in a + a durch 45 + 45 ersetzen

int LetNode::inlineFuncs(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Zunaechst: Auf allen Argumenten ausfuehren
    changes += CallNode::inlineFuncs(cdef, node);

    // Instanz-Liste bilden: wenn nicht eindeutig, fertig.
    VSLNode **values = nodelist(args());
    if (values == 0)
	return changes;

    // Instanzen-Zaehler erzeugen
    int *instances = new int [_base + _nargs];
    for (unsigned i = 0; i < _base + _nargs; i++)
	instances[i] = 0;

    // Zaehlen, wie oft einzelne Variablen benutzt werden
    body()->countArgNodes(cdef, instances, _base, _nargs);

    // Pruefen, ob jede Instantiierung definiert
    bool fail = false;
    for (i = _base; i < _base + _nargs; i++)
	if (instances[i] > 0 && values[i] == 0)
	{
	    ostrstream os;
	    os << "cannot isolate arg" << i;
	    VSLLib::eval_warning(os, cdef);
	    fail = true;
	}

    // Fuer eine effiziente Instantiierung darf
    // jedes Argument maximal 1x verwendet werden.
    for (i = _base; i < _base + _nargs; i++)
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
    VSLNode *newbody = body()->dup();

    // Die ArgNodes im Funktionskoerper verweisen auf Argumente
    // der Funktion expr, nicht der Funktion, der dieser DefCallNode
    // angehoert. Deshalb ersetzen wir sie durch die Argumente
    // des DefCallNodes.

    newbody->instantiateArgs(cdef, &newbody, values, _base, _nargs);


    // Verbleibende ArgNodes im Funktionskoerper verweisen auf
    // LET-Konstrukte. Wenn die aktuelle Funktion und newbody
    // eine unterschiedliche Anzahl Argumente haben, kann es Aerger geben.
    // Deshalb nehmen wir eine Umbenennung der ArgNode's vor.

    newbody->reBase(cdef, _base);


    // Ersetzung vornehmen
    *node = newbody;

    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": inlineFuncs: replacing\n"
	    << *this << "\nby " << **node << '\n';
	cout.flush();
    }

    delete this;

    return ++changes;
}


// Neue Basis setzen
int LetNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    int changes = 0;

    // Alle LET-Variablen umbenennen: arg_i -> arg_(i + offset):
    int offset = newBase - _base;

    if (offset == 0)
	return changes;

    // Auf Pattern-Argument anwenden
    args()->reBase(cdef, newBase);

    // Wenn offset > 0, zuerst Koerper umbenennen
    // (um Kollisionen zu vermeiden)
    if (offset > 0)
	changes = body()->reBase(cdef, newBase + _nargs);

    VSLNode **argnodes = new VSLNode *[_base + _nargs];
    for (unsigned i = 0; i < _base; i++)
	argnodes[i] = 0;
    for (i = _base; i < _base + _nargs; i++)
	argnodes[i] = new ArgNode(i + offset);

    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": reBase: replacing\n" << *this;
	cout.flush();
    }

    // Variablen im Koerper umbenennen
    body()->instantiateArgs(cdef, &body(), argnodes, _base, _nargs);

    // Variablen im Pattern umbenennen
    node_pattern()->instantiateArgs(cdef, &node_pattern(), 
	argnodes, _base, _nargs);

    if (VSEFlags::show_optimize)
    {
	cout << "\nby " << *this << "\n";
	cout.flush();
    }

    for (i = _base; i < _base + _nargs; i++)
	delete argnodes[i];
    delete[] argnodes;

    // Wenn offset < 0, jetzt Koerper umbenennen
    if (offset < 0)
	changes = body()->reBase(cdef, newBase + _nargs);

    // Pattern neu uebersetzen
    _base = newBase;
    compilePatterns(cdef);

    return ++changes;
}


// Debugging

// Pruefen, ob alles in Ordnung
bool LetNode::OK() const
{
    EmptyListNode empty;

    assert (CallNode::OK());

    // Pruefen, ob Argument 2-stellige Liste
    assert (arg() && arg()->isListNode());
    assert (_args() && _args()->tail() && _args()->tail()->isListNode());
    assert (_body() && _body()->tail() && *(_body()->tail()) == empty);

    // Pruefen, ob Listenelemente existieren
    assert (args());
    assert (body());
    assert (node_pattern());

    return true;
}
