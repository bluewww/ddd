// $Id$
// Klasse ListBox (Implementation)

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

char ListBox_rcsid[] = 
    "$Id$";

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
    const ListBox *l;
    for (l = this; !l->isEmpty(); l = l->tail())
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
bool ListBox::matches(const Box &b, const Box *) const
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

    if (nchildren() == 2)
    {
	assert (head() && head()->OK());
	assert (tail() && tail()->OK());
    }

    return true;
}
