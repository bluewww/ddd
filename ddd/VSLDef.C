// $Id$
// VSLDef class

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
// Revision 1.1  1995/05/01 15:47:39  zeller
// Initial revision
//
// Revision 10.16  1995/04/10  19:56:04  zeller
// New: no more <builtin.h>
//
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.13  1994/02/19  20:23:11  zeller
// Fix: Fehlermeldungen korrigiert
// Neu: longname() jetzt im GNU-Format
//
// Revision 10.12  1993/12/07  12:41:54  zeller
// Fix: Anpassung ANSI-Overloading
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:17:48  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:44  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  15:20:38  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// Kommentare neu angeordnet
// 
// Revision 10.7  1993/04/21  00:28:08  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/16  11:44:41  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1993/01/30  16:26:31  zeller
// All messages after german oversitted
// 
// Revision 10.4  1992/11/03  19:37:27  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/01  16:51:28  zeller
// Neu: #pragma implementation
// 
// Revision 10.2  1992/06/01  14:58:35  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.1  1992/02/18  11:19:38  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:15  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:11  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:32  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.36  1991/05/24  17:28:23  zeller
// Spezial-Behandlung Variable "_" entfernt
// (grundsaetzlichere Loesung folgt)
// 
// Revision 2.35  1991/05/23  19:07:32  zeller
// "_" kann jetzt auch mehrfach in Pattern auftreten
// 
// Revision 2.34  1991/05/23  18:53:17  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.33  1991/05/23  10:47:14  zeller
// compile_pattern in compilePattern umbenannt
// 
// Revision 2.32  1991/05/20  19:14:27  zeller
// arglist(): Argumente ListBox freigeben
// 
// Revision 2.31  1991/05/09  16:05:22  zeller
// Warnungen vermieden
// 
// Revision 2.30  1991/05/07  17:16:56  zeller
// VSEFlags::show_match_defs eingefuehrt
// 
// Revision 2.29  1991/05/07  12:14:08  zeller
// Backtrace mit Argumenten versehen
// 
// Revision 2.28  1991/05/06  20:01:30  zeller
// Fall "Unpassende Argumente" mit Fehlermeldung versehen
// 
// Revision 2.27  1991/04/30  18:47:56  zeller
// Neue Codierung: einleitendes '$' bei lokalen Funktionen entfernt.
// Lokalitaet durch gesonderten Parameter angegeben.
// 
// Revision 2.26  1991/04/27  18:49:01  zeller
// compile_pattern():
// a) Pruefung auf Gueltigkeit des Patterns erweitert;
// b) Schutz gegen unendlich rekursiven Aufruf eingefuehrt;
// c) MatchBoxes werden jetzt nicht mehr von ArgNode's erzeugt
//    (erstens verwirrend, zweitens Probleme bei verschachtelten Funktionen),
//    sondern MatchBoxes werden jetzt hier in compile_pattern() angelegt
//    und wie sonstige Argumente auch durchgeschleust.
// 
// Revision 2.25  1991/03/01  16:58:40  zeller
// Neu: nodelist()
// 
// Revision 2.24  1991/02/27  19:06:59  zeller
// Neues Matching-Interface eingefuehrt
// 
// Revision 2.23  1991/02/27  06:25:16  zeller
// Ausgabe Funktionsnamen etc. ueberarbeitet
// 
// Revision 2.22  1991/02/26  18:00:47  zeller
// isStraight() eingefuehrt
// 
// Revision 2.21  1991/02/26  14:11:47  zeller
// Neu: nargs(), um Matching zu beschleunigen
// 
// Revision 2.20  1991/02/26  12:01:38  zeller
// Schutz gegen undefinierte Funktion eingefuehrt
// (kann auftreten, wenn Datei nicht gefunden oder bind() scheitert)
// 
// Revision 2.19  1991/02/25  16:22:11  zeller
// NoNode, NoBox, NoExpr entfernt;
// Neues Konzept fuer leere Listen eingefuehrt.
// 
// Revision 2.18  1991/02/24  20:43:58  zeller
// backtrace neu (als Zeiger) definiert;
// Compiler hat alte Definition falsch interpretiert
// 
// Revision 2.17  1991/02/24  20:21:17  zeller
// backtrace: Ende-Kennzeichen abgelegt
// 
// Revision 2.16  1991/02/24  20:12:36  zeller
// show-eval: Tiefe mit ausgeben
// 
// Revision 2.15  1991/02/24  19:29:01  zeller
// ~VSLDef, VSLDef::OK(): bessere Pruefungen eingebaut
// 
// Revision 2.14  1991/02/24  19:19:15  zeller
// show-eval: Prettyprinting eingefuehrt;
// Auch Aufruf mit ausgegeben
// 
// Revision 2.13  1991/02/24  19:06:10  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.12  1991/02/24  15:41:21  zeller
// MatchNode's durch ArgNode's ersetzt;
// compile_pattern(): ArgNode_matches aktiviert, um MatchBoxen zu erhalten
// 
// Revision 2.11  1991/02/21  15:20:50  zeller
// Neue VSE-Flags eingefuehrt
// 
// Revision 2.10  1991/02/21  15:17:22  zeller
// Neu: Eval-Debugging auf VSLDef-Ebene
// 
// Revision 2.9  1991/02/21  14:51:42  zeller
// Header korrigiert
// 
// Revision 2.8  1991/02/21  14:00:24  zeller
// args() korrigiert
// 
// Revision 2.7  1991/02/21  13:53:03  zeller
// Korrektur Konstruktion Argumentliste: letztes Argument ging verloren
// 
// Revision 2.6  1991/02/21  12:39:30  zeller
// _box_pattern jetzt nicht mehr ListBox
// 
// Revision 2.5  1991/02/21  12:34:51  zeller
// Fehlerbehandlung fuer den Fall
// 'compile_pattern() scheitert' eingefuehrt
// 
// Revision 2.4  1991/02/20  19:50:30  zeller
// PEQ eingefuehrt
// 
// Revision 2.3  1991/02/20  18:48:14  zeller
// Pattern kann jetzt beliebiger VSL-Baum sein
// 
// Revision 2.2  1991/02/20  11:58:09  zeller
// Jeder VSLNode-Klasse eigene Datei zugewiesen
// 
// Revision 2.1  1991/02/19  22:49:13  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.1  1991/02/17  17:26:16  zeller
// Initial revision
// 

char VSLDef_rcsid[] = "$Id$";

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

static void node_matches(int data, VSLNode *node)
{
    node_instances[data] = node;
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

static void box_matches(int data, Box *box)
{
    box_instances[data] = box;
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
    for (unsigned i = 0; i < nargs(); i++)
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
	
    for (unsigned i = 0; i < nargs(); i++)
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
    int offset = 0;

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
