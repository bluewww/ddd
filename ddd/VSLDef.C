// $Id$
// VSLDef class

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char VSLDef_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <iostream.h>
#include <strstream.h>

#include "assert.h"
#include "hash.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "Box.h"
#include "ListBox.h"
#include "MatchBox.h"

#include "VSLNode.h"
#include "ArgNode.h"

DEFINE_TYPE_INFO_0(VSLDef)

// VSLDef

// Pattern-Matching

const int max_instances = 256;


// Pattern-Matching mit Knoten

static VSLNode *node_instances[max_instances];

static void node_matches(int data, const VSLNode *node)
{
    node_instances[data] = (VSLNode *)node;
}

// Pruefen, ob Definition mit Aufruf 'arg' uebereinstimmt;
// hierbei gematchte Werte aus arg in node_instances ablegen
bool VSLDef::matches(const VSLNode *arg) const
{
    // Fuer Dauer des Vergleiches sind fuer alle x: ArgNode == x

    ArgNodeFunc oldCB = ArgNode::matchCallback;
    ArgNode::matchCallback = node_matches;

    bool oldFlag = ArgNode::matchesAll;
    ArgNode::matchesAll = true;
    
    if (VSEFlags::show_match_defs)
    {
	cout << "\nDef Match: " << longname() << " ? " << *arg;
	cout.flush();
    }

    bool ret = (*_node_pattern == *arg);

    if (VSEFlags::show_match_defs)
    {
	if (ret)
	    cout << "\nDef Match: " << longname() << " matches " << *arg;
	else
	    cout << "\nDef Match: " << longname() << " does not match " 
		<< *arg;
	cout.flush();
    }

    ArgNode::matchCallback = oldCB;
    ArgNode::matchesAll = oldFlag;

    return ret;
}

// mit 'arg' gematch'te Knoten zurueckgeben
VSLNode **VSLDef::nodelist(const VSLNode *arg) const
{
    for (unsigned i = 0; i < nargs(); i++)
	node_instances[i] = 0;

    bool ok = matches(arg);
    if (!ok)
	return 0;

    return node_instances;
}


// Pattern-Matching mit Boxen

static Box *box_instances[max_instances];

static void box_matches(int data, const Box *box)
{
    box_instances[data] = (Box *)box;
}

// Pruefen, ob Definition mit Parameter 'arg' uebereinstimmt;
// hierbei gematchte Werte aus arg in box_instances ablegen
bool VSLDef::matches(const Box *arg) const
{
    bool ret = false;

    // Fuer Dauer des Vergleiches gilt fuer alle x: MatchBox == x

    MatchBoxFunc oldCB = MatchBox::matchCallback;
    MatchBox::matchCallback = box_matches;

    MatchBox::matchesAll = true;

    if (VSEFlags::show_match_defs)
    {
	cout << "\nDef Match: " << longname() << " ? " << *arg;
	cout.flush();
    }

    if (_box_pattern == 0)
	compilePattern();

    if (_box_pattern)
	ret = (*_box_pattern == *arg);

    if (VSEFlags::show_match_defs)
    {
	if (ret)
	    cout << "\nDef Match: " << longname() << " matches " << *arg;
	else
	    cout << "\nDef Match: " << longname() << " does not match " 
		<< *arg;
	cout.flush();
    }

    MatchBox::matchesAll = false;

    MatchBox::matchCallback = oldCB;
    return ret;
}


// node_pattern in box_pattern umwandeln
void VSLDef::compilePattern() const
{
    // Schutz gegen Pattern der Form f(f(a)) = ...
    if (being_compiled)
    {
	VSLLib::eval_error("recursive pattern", this);
	return;
    }

    uncompilePattern();

    // Als Argumente Liste von MatchBoxen uebergeben
    ListBox *list = new ListBox;
    unsigned i;
    for (i = 0; i < nargs(); i++)
    {
	MatchBox *m = new MatchBox(i);
	(*list) += m;
	m->unlink();
    }

    ((VSLDef *)this)->being_compiled = true;
    const Box *result = _node_pattern->eval(list);
    ((VSLDef *)this)->being_compiled = false;

    list->unlink();


    // Pruefen, ob Resultat gueltig
    if (result == 0)
	VSLLib::eval_error("cannot evaluate pattern", this);
    else
    {
	// MatchBoxen zaehlen: Jede muss genau 1x vorkommen
	int *instances = new int [nargs()];
	for (i = 0; i < nargs(); i++)
	    instances[i] = 0;
	result->countMatchBoxes(instances);

	for (i = 0; i < nargs(); i++)
	{
	    if (instances[i] == 0)
	    {
		ostrstream os;
		os << "invalid pattern: arg" << i 
		   << " is never instantiated";
		VSLLib::eval_error(os, this);
	    }
	    if (instances[i] > 1)
	    {
		ostrstream os;
		os << "invalid pattern: arg" << i 
		   << " is instantiated several times";
		VSLLib::eval_error(os, this);
	    }
	}
	delete[] instances;
    }

    ((VSLDef *)this)->_box_pattern = (Box *)result;
}



// Auswertung

// Backtrace

const VSLDef **VSLDef::backtrace = 0; 
const Box **VSLDef::backtrace_args = 0;
// (doesn't work without initializing --  why?)

// Definition auswerten
const Box *VSLDef::eval(Box *arg) const
{
    static int depth = 0;

    // Backtrace anlegen
    if (backtrace == 0)
    {
	backtrace = new const VSLDef *[VSEFlags::max_eval_nesting + 2];
	backtrace_args = new const Box *[VSEFlags::max_eval_nesting + 2];
    }

    backtrace[depth] = this;
    backtrace_args[depth] = arg->link();
    backtrace[depth + 1] = 0;   // Ende-Kennzeichen

    // Debugging
    if (VSEFlags::show_large_eval)
    {
	cout << "\n" << depth << " ";
	for (int i = 0; i < depth; i++)
	    cout << "  ";

	cout << longname() << *arg << ":";
	cout.flush();
    }

    // Eigentliche Funktion
    const Box *box = 0;

    if (depth < VSEFlags::max_eval_nesting)
    {
	ListBox *myarglist = arglist(arg);

	if (myarglist)
	{
	    depth++;
	    if (_expr)
		box = _expr->eval(myarglist);
	    else
		VSLLib::eval_error("undefined function");
	    depth--;

	    myarglist->unlink();
	}
	else
	    VSLLib::eval_error("invalid argument");
    }
    else
	VSLLib::eval_error("infinite recursion");

    // Debugging
    if (VSEFlags::show_large_eval)
    {
	cout << "\n" << depth << " ";
	for (int i = 0; i < depth; i++)
	    cout << "  ";

	cout << longname() << *arg;

	if (box == 0)
	    cout << " FAILS";
	else
	    cout << " = " << *box;

	cout.flush();
    }

    // Backtrace loeschen
    backtrace[depth] = 0;
    ((Box *)backtrace_args[depth])->unlink();

    return box;
}

// Argumentliste in ein Format konvertieren, dass ArgNode's
// darauf zugreifen koennen.
ListBox *VSLDef::arglist(const Box *arg) const
{
    if (straight())
    {
	assert (arg->isListBox());
	return (ListBox *)((Box *)arg)->link();
    }

    unsigned i;
    for (i = 0; i < nargs(); i++)
	box_instances[i] = 0;

    bool ok = matches(arg);
    if (!ok)
	return (ListBox *)0;

    ListBox *list = new ListBox;
    for (i = 0; i < nargs(); i++)
    {
	assert(box_instances[i] != 0);  // cannot isolate arg
	(*list) += box_instances[i];
    }

    return list;
}



// Namen aufloesen
int VSLDef::resolveNames()
{
    // Zunaechst im Koerper anwenden
    int changes = expr()->resolveNames(this, nargs());

    // Jetzt alle NameNode's im Pattern durch entsprechende
    // ArgNode's ersetzen.
    string s = "";
    unsigned offset = 0;

    while ((s = node_pattern()->firstName(), s) != "")
    {
	// Im Koerper ersetzen
	int ch = expr()->resolveName(this, &expr(), s, offset);
	if (ch == 0)
	    VSLLib::eval_warning("`" + s + "' unused", this);
	changes += ch;

	// Im Pattern ersetzen
	ch = node_pattern()->resolveName(this, &node_pattern(), s, offset);
	assert(ch > 0);
	if (ch > 1)
	    VSLLib::eval_error("`" + s + "' used several times", this);

	offset += ch;
    }

    assert(offset == nargs());

    // Verbleibende NameNode's entfernen (durch arg0 ersetzen)
    while ((s = expr()->firstName(), s) != "")
    {
	VSLLib::eval_error("`" + s + "' undefined", this);
	expr()->resolveName(this, &expr(), s, 0);
    }

    return changes;
}


// Ressourcen

// Funktionsargumente bilden (fuer Fehlermeldungen etc.)
string VSLDef::args() const
{
    // Konstante: hat kein Argument
    if ((deflist->func_name())[0] == '#')
	return string("");

    ostrstream os;

    if (_node_pattern->isArgNode())
	os << "(" << *_node_pattern << "...)";
    else
	os << *_node_pattern;

    return os;
}

// Internen Funktionsnamen zurueckgeben
string VSLDef::func_name() const
{
    return deflist->func_name() + args();
}

// Externen Funktionsnamen zurueckgeben
string VSLDef::f_name() const
{
    return deflist->f_name() + args();
}

// Externen Funktionsnamen incl. Auftreten zurueckgeben
string VSLDef::longname() const
{
    ostrstream os;
    ostream& s = os;
    
    if (_filename != "")
	s << _filename << ":" << _lineno << ": ";
    s << f_name();

    return os;
}

// Definition *und alle Nachfolger* zerstoeren
VSLDef::~VSLDef()
{
    if (_listnext)      delete _listnext;
    if (_expr)          delete _expr;
    if (_node_pattern)  delete _node_pattern;
    if (_box_pattern)   _box_pattern->unlink();
}

// Pruefen, ob alles in Ordnung (*ohne* Nachfolger)
bool VSLDef::OK() const
{
    // Pruefen, ob Ausdruecke ok
    assert (_expr == 0 || _expr->OK());
    assert (_node_pattern && _node_pattern->OK());
    assert (_box_pattern == 0 || _box_pattern->OK());

    // Pruefen, ob Zeiger auf Vorgaenger und Nachfolger ok
    assert (libnext() == 0 || libnext()->libprev() == this);
    assert (libprev() == 0 || libprev()->libnext() == this);

    return true;
}
