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

char VSLDefList_rcsid[] = 
    "$Id$";

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
