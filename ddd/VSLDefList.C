// $Id$
// Implementation Klasse VSLLib

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
// Revision 1.1  1995/05/01 15:47:55  zeller
// Initial revision
//
// Revision 10.12  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.11  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:17:54  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  11:07:43  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/21  00:28:10  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/16  11:44:43  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1993/01/30  16:26:32  zeller
// All messages after german oversitted
// 
// Revision 10.4  1992/11/03  19:37:29  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/01  16:51:29  zeller
// Neu: #pragma implementation
// 
// Revision 10.2  1992/06/01  14:58:38  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.1  1992/02/18  11:19:42  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:30  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:17  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:49  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.16  1991/05/21  19:57:21  zeller
// Neu: replace()
// 
// Revision 2.15  1991/05/09  16:05:35  zeller
// Warnungen vermieden
// 
// Revision 2.14  1991/05/06  10:16:47  zeller
// Fehlermeldung korrigiert (ein Klammerpaar zuviel)
// 
// Revision 2.13  1991/03/01  15:39:43  zeller
// Meldung "Hier ist die fruehere Definition" verbessert
// 
// Revision 2.12  1991/02/27  03:41:55  zeller
// Fehlermeldung bei Neudefinition nur, wenn neue != alte Definition
// 
// Revision 2.11  1991/02/26  13:07:46  zeller
// Fehlermeldung: \0 an Buffer gehaengt
// 
// Revision 2.10  1991/02/25  16:22:54  zeller
// NoNode, NoBox, NoExpr entfernt;
// Neues Konzept leere Listen eingefuehrt
// 
// Revision 2.9  1991/02/24  22:18:06  zeller
// Fall 'Mehrfache Definition' jetzt korrekt verarbeitet:
// pattern und expr sind stets einmalig.
// 
// Revision 2.8  1991/02/24  21:59:27  zeller
// Fehlermeldung "Keine passende Definition" verbessert
// 
// Revision 2.7  1991/02/24  19:06:32  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.6  1991/02/21  14:52:11  zeller
// 'pattern' stets vor 'expr' angegeben
// 
// Revision 2.5  1991/02/21  11:52:48  zeller
// Korrektur im Fall 'keine Definition gefunden'
// 
// Revision 2.4  1991/02/20  19:51:04  zeller
// PEQ.h eingefuehrt
// 
// Revision 2.3  1991/02/20  18:51:46  zeller
// Pattern und Argumente nicht mehr auf Listen beschraenkt
// 
// Revision 2.2  1991/02/20  11:58:20  zeller
// Jeder VSLNode-Klasse eigene Datei zugewiesen
// 
// Revision 2.1  1991/02/19  22:49:34  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.1  1991/02/17  17:26:19  zeller
// Initial revision
// 

char VSLDefList_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <iostream.h>
#include <strstream.h>
#include "assert.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "Box.h"
#include "ListBox.h"

#include "VSLNode.h"

DEFINE_TYPE_INFO_0(VSLDefList)

// VSLDefList

const Box *VSLDefList::eval(Box *arg) const
// Ausdruck auswerten
{
    VSLDef *d = def(arg);

    if (d == 0)
    {
	const int bufsize = 1000;
	char buffer[bufsize];

	ostrstream s(buffer, sizeof buffer);
	s << *arg << '\0';

	VSLLib::eval_error(string("no suiting definition for ") + 
	    f_name() + buffer);
    }

    return d ? d->eval(arg) : 0;
}

VSLDef* VSLDefList::add(bool &newFlag, VSLNode *pattern, VSLNode *expr,
    string filename, int lineno)
// Definition an Definitionsliste anhaengen;
// Wenn neue Definition hinzugekommen, newFlag setzen
{
    VSLDef *newdef = new VSLDef(this, pattern, expr, filename, lineno);

    // Pruefen, ob bereits deklariert
    for (VSLDef *d = _first; d != 0; d = d->listnext())
	if (*(d->node_pattern()) == *(newdef->node_pattern()))
	{
	    // Deklaration besteht bereits; 
	    // Neue Definition loeschen
	    newFlag = false;
	    newdef->expr() = 0; // Ausdruck erhalten
	    delete newdef;

	    if (expr == 0)
	    {
		// n-te (n > 1) Deklaration: ignorieren
		return d;
	    }

	    if (d->expr() == 0)
	    {
		// Bisher nur deklariert, nicht aber definiert
		// Definition eintragen und fertig.

		d->expr() = expr;
		return d;
	    }

	    if (*expr != *(d->expr()))
	    {
		// Bereits (abweichend) definiert
		VSLLib::parse_error(
		    string("'") + d->f_name() + "\' already defined");
		VSLLib::eval_echo("(this is the previous definition)", d);
		delete expr; return 0;
	    }

	    // Sonst: wie Neudeklaration behandeln
	    delete expr; return d;
	}

    // Neue Definitionen werden hintenangehaengt. Damit wird stets die *erste* 
    // (und nicht etwa die letzte) passende Definition ausgewaehlt.
    if (_last == 0)
	_first = _last = newdef;
    else
	_last->listnext() = newdef;
    _last = newdef;
    _ndefs++;

    newFlag = true; 
    return newdef;
}


// Passende Definition auswaehlen
VSLDef *VSLDefList::def(Box *arg) const
{
    for (VSLDef *d = _first; d != 0; d = d->listnext())
	if (d->matches(arg))
	    return d;

    return 0;
}


void VSLDefList::replace()
// Alle Definitionen loeschen
{
    // Definitionen ausketten
    for (VSLDef *son = first(); son != 0; son = son->listnext())
    {
	assert (son->deflist == this);

	// Nachfolger anpassen
	if (son->libnext())
	    son->libnext()->libprev() = son->libprev();
	else
	{
	    assert (son == lib->_last);
	    lib->_last = son->libprev();
	}

	// Vorgaenger anpassen
	if (son->libprev())
	    son->libprev()->libnext() = son->libnext();
	else
	{
	    assert (son == lib->_first);
	    lib->_first = son->libnext();
	}
    }

    // Alle VSLDef's in dieser VSLDefList loeschen
    delete _first;

    // Zeiger zuruecksetzen
    _first = _last = 0;
    _ndefs = 0;
}


VSLDefList::~VSLDefList()
// Definition *und alle Nachfolger* zerstoeren
{
    if (_next)
	delete _next;
    if (_first)
	delete _first;
}


bool VSLDefList::OK() const
// Pruefen, ob alles in Ordnung (*inklusive* Nachfolger)
{
    VSLDef *d = _first;
    VSLDef *lastdef = d;
    int count = 0;

    // Pruefen, ob _ndefs und _last ok
    while (d != 0)
    {
	lastdef = d;
	d = d->listnext();
	count++;
    }
    assert (count == _ndefs);
    assert (lastdef == _last);

    // Pruefen, ob Zeiger auf DefList ok
    d = _first;
    while (d != 0)
    {
	assert (d->deflist == this);
	d = d->listnext();
    }

    // Pruefen, ob Definitionen ok
    for (d = _first; d != 0; d = d->listnext())
	assert (d->OK());

    return true;
}
