// $Id$
// General-purpose Handler Manager

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
// Revision 1.1  1995/05/01 15:47:44  zeller
// Initial revision
//
// Revision 1.14  1994/07/20  00:19:15  zeller
// Fix: #pragma implementation was missing
//
// Revision 1.13  1994/07/19  17:26:22  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.12  1994/01/21  15:27:36  zeller
// Neu: Jetzt auch Loeschen von laufenden Handlern moeglich
//
// Revision 1.11  1993/07/22  10:44:05  zeller
// Neu: Vergleiche und Ordnung
//
// Revision 1.10  1993/06/04  08:37:25  zeller
// Neu: Mehr assertions
//
// Revision 1.9  1993/05/22  20:12:02  zeller
// %Q% added by apply-q-flag.sh 1.5
// 
// Revision 1.8  1993/04/22  11:02:19  zeller
// Lizenz verbessert
// 
// Revision 1.7  1993/04/16  11:40:02  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.6  1993/02/04  10:44:44  zeller
// NORA-Kram eingebunden
// 
// Revision 1.5  1992/10/23  13:54:25  zeller
// call() hat jetzt konstante Argumente
// 
// Revision 1.4  1992/10/23  13:45:54  zeller
// Neu: callHandlers() ist const
// 
// Revision 1.3  1992/06/02  02:46:25  zeller
// Neu: #pragma interface
// 
// Revision 1.2  1992/05/20  15:29:44  zeller
// Dateinamen verkuerzt
// 
// Revision 1.1  1992/05/12  16:56:13  zeller
// Initial revision
// 

char Handler_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "assert.h"
#include "HandlerL.h"

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
