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

#ifndef _DDD_HandlerList_h
#define _DDD_HandlerList_h

#ifdef __GNUG__
#pragma interface
#endif

#include "compare.h"

typedef void (*HandlerProc)(void *source,	// handler source
			    void *client_data,  // data supplied by client
			    void *call_data);   // data supplied by caller



class HandlerList {
private:
    struct HandlerRec {
	HandlerProc proc;		// handling procedure
	void        *client_data;	// data supplied by client
	HandlerRec  *next;		// for collectors
	int     remove_me;              // Flag: to be removed?
	
	// Constructor
	HandlerRec(HandlerProc p, void *c, HandlerRec *n = 0):
	    proc(p), client_data(c), next(n), 
	    remove_me(0)
        {}
    };

    static int compare(const HandlerRec& l1, const HandlerRec& l2)
    {
	int c = ::compare((void *)l1.proc, (void *)l2.proc);
	return c ? c : ::compare(l1.client_data, l2.client_data);
    }

    // Data
    unsigned _nTypes;	        // #types
    HandlerRec  **handlers;	// handlers
    int *active;		// Flag: currently running?

protected:
    // Remove all handlers marked as "remove me"
    void processRemovals(unsigned type) const;

public:
    // Constructor
    HandlerList(unsigned n = 10);

    // Duplicator
    HandlerList(const HandlerList& l);

    // Comparison
    int compare(const HandlerList& l) const;

    // Resources
    unsigned nTypes() const { return _nTypes; }

    // Add Handler
    void add(unsigned type, HandlerProc proc, void *client_data = 0);

    // Add Handler list
    void add(const HandlerList& l)
    {
	for (unsigned type = 0; type < l.nTypes(); type++)
	    for (HandlerRec *h = l.handlers[type]; h != 0; h = h->next)
		add(type, h->proc, h->client_data);
    }

    // Remove Handler
    void remove(unsigned type, HandlerProc proc, void *client_data = 0);

    // Remove all Handlers
    void removeAll(unsigned type);

    // Remove all Handlers for all types
    void removeAll();

    // Call Handlers
    void call(unsigned type, void *const source = 0, 
	      void *const call_data = 0) const;

    // Check if Handler available
    int has(unsigned type) const
    {
	return type < nTypes() && handlers[type] != 0;
    }

    // Destructor
    ~HandlerList()
    {
	removeAll();
	for (unsigned type = 0; type < nTypes(); type++)
	    processRemovals(type);

	delete[] handlers;
	delete[] active;
    }
};

// Alternative interfaces to member function 'compare'
inline int compare(const HandlerList& l1, const HandlerList& l2)
{
    return l1.compare(l2);
}

inline int operator == (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) == 0;
}

inline int operator != (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) != 0;
}

inline int operator < (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) < 0;
}

inline int operator > (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) > 0;
}

inline int operator <= (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) <= 0;
}

inline int operator >= (const HandlerList& l1, const HandlerList& l2)
{
    return compare(l1, l2) >= 0;
}

#endif
