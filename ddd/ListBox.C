// $Id$
// Klasse ListBox (Implementation)

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
// Revision 1.1  1995/05/01 15:47:45  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:15:01  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  10:51:59  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:11:03  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:31:45  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/06/02  03:04:17  zeller
// #pragma besser formatiert
// 
// Revision 10.3  1992/06/01  16:34:17  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:30  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:04  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:51  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:43  zeller
// Installierte Version
// 
// Revision 1.18  1991/06/10  14:08:14  zeller
// Neu: DataLink mit Kopierfunktionen
// 
// Revision 1.17  1991/06/10  11:54:12  zeller
// Neu: info()
// 
// Revision 1.16  1991/06/09  17:49:24  zeller
// tag(): Nur, wenn Liste nichtleer
// 
// Revision 1.15  1991/06/09  17:38:07  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.14  1991/06/09  17:16:46  zeller
// atom() entfernt;
// Neu: tag() markiert alle Elemente der Liste (statt der Liste selbst)
// 
// Revision 1.13  1991/06/09  16:01:04  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.12  1991/05/24  18:36:35  zeller
// isMatchBox() durch isDummyBox() ersetzt
// 
// Revision 1.11  1991/05/23  11:27:41  zeller
// Neu: uncons() macht cons() rueckgaengig;
// cons() gibt Parameter fuer uncons() zurueck
// 
// Revision 1.10  1991/05/09  15:08:05  zeller
// Warnungen vermieden
// 
// Revision 1.9  1991/05/06  12:14:43  zeller
// Ausgabeformat Liste angepasst
// 
// Revision 1.8  1991/05/05  17:23:23  zeller
// Neu: atom()
// 
// Revision 1.7  1991/04/27  16:42:58  zeller
// Neu: CONS
// 
// Revision 1.6  1991/02/27  04:05:00  zeller
// Ansprechendere Ausgabe Listen eingefuehrt
// 
// Revision 1.5  1991/02/25  14:13:28  zeller
// Neu: leere Liste nicht mehr durch 0,
// sondern durch ListBox ohne Soehne dargestellt.
// 
// Revision 1.4  1991/02/24  21:17:20  zeller
// Neu: eigenes matches() eingefuehrt
// (CompositeBox::matches() verglich nsons() => Fehler);
// Neu: OK()
// 
// Revision 1.3  1991/02/20  18:08:58  zeller
// Eigenes dump() definiert
// 
// Revision 1.2  1991/02/20  18:07:19  zeller
// dump() neu definiert
// 
// Revision 1.1  1991/02/18  12:38:01  zeller
// Initial revision
// 

char ListBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <iostream.h>

#include "ListBox.h"
#include "VSEFlags.h"

DEFINE_TYPE_INFO_1(ListBox, CompositeBox)

// _last neu berechnen
void ListBox::_relast()
{
    for (const ListBox *l = this; !l->isEmpty(); l = l->tail())
	;
    _last = l;
}

// ListBox ausgeben
void ListBox::dump(ostream &s) const
{
    if (VSEFlags::include_list_info)
    {
	// Liste formal ausgeben
	s << "[";
	if (!isEmpty())
	    s << *head() << ":" << *tail();
	s << "]";
    }
    else
    {
	// Etwas geschickter formulieren

	s << "(";

	const ListBox *list = this;
	while (list)
	{
	    if (list->isEmpty())
	    {
		if (list->isDummyBox())
		    s << ", " << *list;
		list = 0;
	    }
	    else
	    {
		if (list != this)
		    s << ", ";

		s << *(list->head());
		list = list->tail();
	    }
	}

	 s << ")";
    }
}

// Liste anhaengen
ListBox *ListBox::cons(ListBox *b)
{
    // funktioniert nur, wenn Liste != [],
    // da in diesem Fall die Liste selbst ersetzt werden muesste
    assert(!isEmpty());

    if (!b->isEmpty())
    {
	// Letzte ListBox durch b ersetzen
	const ListBox *t = this;
	ListBox *attach = 0;

	while (!t->isEmpty())
	{
	    attach = (ListBox *)t;
	    t = t->tail();
	}

	assert (attach != 0);
	assert (attach->tail()->isEmpty());

	attach->tail()->unlink();
	attach->_tail() = b->link();
	attach->_last = b->_last;

	return attach;
    }

    return 0;
}


// Liste abhaengen
void ListBox::uncons(ListBox *attach)
{
    // Attach ist der Rueckgabewert einer fruehren cons()-Operation.
    // l.uncons(attach) macht attach = l.cons(b) rueckgaengig.

    if (attach)
    {
	attach->tail()->unlink();
	_last = (ListBox *)(attach->_tail() = new ListBox);
    }
}


// Auf Gleichheit pruefen
bool ListBox::matches(const Box &b, const Box *callbackArg) const
{
    return CompositeBox::matches(b);
}

// Box markieren
Box *ListBox::tag(Data *d, DataLink *dl)
{
    if (!isEmpty())
    {
	// In Listen: alle Soehne markieren
	_head() = _head()->tag(d, dl);
	_tail() = _tail()->tag(d, dl);
    }

    // Die Liste selbst nicht markieren, da sie nie angezeigt wird
    return this;
}


// Repraesentations-Invariante
bool ListBox::OK() const
{
    assert (CompositeBox::OK());
    assert (nchildren() == 0 || nchildren() == 2);
    assert (_last && _last->isEmpty());

    if (nsons() == 2)
    {
	assert (head() && head()->OK());
	assert (tail() && tail()->OK());
    }

    return true;
}
