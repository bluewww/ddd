// $Id$
// Box lists

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char ListBox_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>

#include "ListBox.h"
#include "VSEFlags.h"

DEFINE_TYPE_INFO_1(ListBox, CompositeBox)

// Recompute _last
void ListBox::_relast()
{
    const ListBox *l;
    for (l = this; !l->isEmpty(); l = l->tail())
	;
    _last = l;
}

// Dump
void ListBox::dump(std::ostream &s) const
{
    if (VSEFlags::include_list_info)
    {
	// Dump formally
	s << "[";
	if (!isEmpty())
	    s << *head() << ":" << *tail();
	s << "]";
    }
    else
    {
	// Use cuter syntax

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

// Append list
ListBox *ListBox::cons(ListBox *b)
{
    assert(!isEmpty());

    if (!b->isEmpty())
    {
	// Replace final ListBox by B
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


// Detach list
void ListBox::uncons(ListBox *attach)
{
    // ATTACH must be the return value of a cons() operation.
    // l.uncons(attach) undoes attach = l.cons(b).

    if (attach)
    {
	attach->tail()->unlink();
	_last = (ListBox *)(attach->_tail() = new ListBox);
    }
}


// Check for equality
bool ListBox::matches(const Box &b, const Box *) const
{
    return CompositeBox::matches(b);
}

// Mark list
Box *ListBox::tag(Data *d, DataLink *dl)
{
    if (!isEmpty())
    {
	// In a list: mark all children
	_head() = _head()->tag(d, dl);
	_tail() = _tail()->tag(d, dl);
    }

    // The list itself is never marked, since it is not drawn
    return this;
}


// Representation invariant
bool ListBox::OK() const
{
    assert (CompositeBox::OK());
    assert (nchildren() == 0 || nchildren() == 2);

    // _last may be inconsistent; it is recomputed before every access
    // assert (_last && _last->isEmpty());

    if (nchildren() == 2)
    {
	assert (head() && head()->OK());
	assert (tail() && tail()->OK());
    }

    return true;
}
