// $Id$
// General-purpose Handler Manager

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

char HandlerList_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "assert.h"
#include "HandlerL.h"

// Create Handler List
HandlerList::HandlerList(unsigned n):
    _nTypes(n),
    handlers(new HandlerRec *[n]),
    active(new int[n])
{
    for (unsigned type = 0; type < _nTypes; type++)
    {
	handlers[type] = 0;
	active[type]   = 0;
    }
}


// Duplicate List
HandlerList::HandlerList(const HandlerList& l):
    _nTypes(l._nTypes),
    handlers(new HandlerRec *[l._nTypes]),
    active(new int [l._nTypes])
{
    for (unsigned type = 0; type < nTypes(); type++)
    {
	handlers[type] = 0;
	active[type]   = l.active[type];
    }

    add(l);
}

// Add Handler
void HandlerList::add(unsigned type, HandlerProc proc, void *client_data)
{
    assert(type < nTypes());

    // Adding handlers takes effect only the next time the list is processed
    handlers[type] = new HandlerRec(proc, client_data, handlers[type]);
}

// Remove Handler
// Note: this function is const, but the others are not.
void HandlerList::processRemovals(unsigned type) const
{
    assert(type < nTypes());
    assert(active[type] == 0);

    HandlerRec *prev = 0;
    HandlerRec *next = 0;
    for (HandlerRec *h = handlers[type]; h != 0; h = next)
    {
	next = h->next;
	if (h->remove_me)
	{
	    if (prev == 0)
		handlers[type] = next;
	    else
		prev->next = next;

	    delete h;
	}
	else
	{ 
	    prev = h;
	}
    }
}

// Remove Handler
void HandlerList::remove(unsigned type,
			 HandlerProc proc,
			 void *client_data)
{
    assert(type < nTypes());

    for (HandlerRec *h = handlers[type]; h != 0; h = h->next)
	if (proc == h->proc && client_data == h->client_data)
	    h->remove_me++;

    if (active[type] == 0)
	processRemovals(type);
}

// Remove all Handlers
void HandlerList::removeAll(unsigned type)
{
    assert(type < nTypes());

    for (HandlerRec *h = handlers[type]; h != 0; h = h->next)
	h->remove_me++;

    if (active[type] == 0)
	processRemovals(type);
}

// Remove all Handlers for all types
void HandlerList::removeAll()
{
    for (unsigned type = 0; type < nTypes(); type++)
	removeAll(type);
}

// Call Handlers
// We do not remove handlers while the list is being processed
void HandlerList::call(unsigned type,
		       void *const source, 
		       void *const call_data) const
{
    assert(type < nTypes());

    active[type]++;

    for (HandlerRec *h = handlers[type]; h != 0; h = h->next)
	h->proc(source, h->client_data, call_data);

    // Removing handlers takes effect only the next time the list is processed
    if (--active[type] == 0)
	processRemovals(type);
}

// Compare Handler lists
int HandlerList::compare(const HandlerList& l) const
{
    // Compare this
    if (this == &l)
	return 0;

    // Compare number of types
    int c = ::compare(nTypes(), l.nTypes());
    if (c)
	return c;

    // Compare individual lists
    for (unsigned t = 0; t < nTypes(); t++)
    {
	HandlerRec *r1 = handlers[t];
	HandlerRec *r2 = l.handlers[t];

	while (r1 != 0 && r2 != 0)
	{
	    c = compare(*r1, *r2);
	    if (c)
		return c;

	    r1 = r1->next;
	    r2 = r2->next;
	}

	// If one of the lists is longer, it is also the greater one
	c = ::compare((void *)r1, (void *)r2);
	if (c)
	    return c;
    }

    return 0;
}
