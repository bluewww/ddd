// $Id$ -*- C++ -*-
// Memory checker

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

char MemCheck_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MemCheck.h"
#include "config.h"

#include <stdlib.h>

#if !HAVE_MALLOC_DECL
extern "C" void *malloc(size_t size);
#endif


// Data

int MemCheck::log = 0;		// For debugging

MemCheckHeader MemCheck::freebase;
MemCheckHeader MemCheck::allocbase;

MemCheckHeader *MemCheck::freep;  // Start of free list
MemCheckHeader *MemCheck::allocp; // Start of alloc list

unsigned MemCheck::tics;	// Next available ID

unsigned MemCheck::freeBytes;	// Bytes in free list
unsigned MemCheck::allocBytes;	// Bytes in alloc list

// Validate a header pointer
inline void MemCheck::validate(MemCheckHeader *p, char *src)
{
    if (p->s.magic != MAGIC)
    {
	cerr << src << ": memory corrupted at " << (void *)p << "\n";
	abort();
    }
}

// Allocate a block with size nbytes -- a la K&R
void *MemCheck::alloc(unsigned nbytes)
{
    init();

    unsigned nunits =
	(nbytes + sizeof(MemCheckHeader) - 1) / sizeof(MemCheckHeader) + 1;

    MemCheckHeader *prevp = freep;
    validate(freep, "alloc");
    for (MemCheckHeader *p = freep->s.ptr; ; prevp = p, p = p->s.ptr)
    {
	validate(p, "alloc");
	
	if (p->s.size >= nunits)
	{
	    // big enough
	    if (p->s.size == nunits)
	    {
		// suits fine
		prevp->s.ptr = p->s.ptr;
	    }
	    else
	    {
		// allocate from end of list

		p->s.size -= nunits;
		p += p->s.size;
		p->s.size = nunits;
	    }

	    // Insert debugging info
	    p->s.magic     = MAGIC;
	    p->s.requested = nbytes;
	    p->s.tic       = tics++;
	    
	    // insert in alloc list
	    p->s.ptr = allocp->s.ptr;
	    allocp->s.ptr = p;
	    allocp = p;

	    // sum up
	    allocBytes += nbytes;
	    freeBytes  -= nbytes;

	    // update free list
	    freep = prevp;

	    // return new block
	    void *ret = (void *)(p + 1);

	    if (log)
		clog << "alloc: " << nbytes << " bytes "
		     << "in block " << p->s.tic << " at " << ret << "\n";
	    
	    return ret;
	}

	if (p == freep)
	{
	    // we traversed the whole list: allocate more core
	    if ((p = morecore(nunits)) == 0)
		return 0;
	}
    }
}

// Return more core memory -- using malloc()
MemCheckHeader *MemCheck::morecore(unsigned nunits)
{
    const unsigned NALLOC = 1024;
    
    if (nunits < NALLOC)
	nunits = NALLOC;

    unsigned nbytes = nunits * sizeof(MemCheckHeader);

    void *cp = ::malloc(nbytes);

    if (cp == 0)
	return 0;

    freeBytes += nbytes;

    MemCheckHeader *up = (MemCheckHeader *)cp;

    // make a single block and "free" it
    up->s.size  = nunits;
    up->s.magic = MAGIC;
    _free(up);
    
    return freep;
}

// Free a block previously returned by alloc -- a la K&R
void MemCheck::free(void *ap)
{
    init();

    MemCheckHeader *bp = (MemCheckHeader *)ap - 1;

    validate(bp, "free");

    if (log)
	clog << "free:  " << bp->s.requested << " bytes "
	     << "in block " << bp->s.tic << " at " << ap << "\n";

    // wipe out memory
    char *wipeout = (char *)ap;
    for (unsigned i = 0; i < bp->s.requested; i++)
	*wipeout++ = WIPEOUT;
    
    // delete from alloc list
    MemCheckHeader *prevp = allocp;
    validate(allocp, "free");
    
    for (MemCheckHeader *p = allocp->s.ptr; ; prevp = p, p = p->s.ptr)
    {
	validate(p, "free");
	
	if (p == bp)
	{
	    prevp->s.ptr = p->s.ptr;
	    allocp = prevp;
	    break;
	}
	
	if (p == allocp)
	{
	    // we traversed the whole list without finding the block
	    cerr << "free: cannot find block at " << ap << "\n";
	    abort();
	}
    }
    
    // sum up
    allocBytes -= bp->s.requested;
    freeBytes  += bp->s.requested;

    // insert in free list
    _free(bp);

}

// Insert a block in free list -- a la K&R
void MemCheck::_free(MemCheckHeader *bp)
{
    // search neighbouring blocks in free list
    MemCheckHeader *p;
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    {
	validate(p, "free");
	
	if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
	    break;
    }

    if (bp + bp->s.size == p->s.ptr)
    {
	// merge with right neighbour
	bp->s.size += p->s.ptr->s.size;
	bp->s.ptr = p->s.ptr->s.ptr;
    }
    else
	bp->s.ptr = p->s.ptr;

    if (p + p->s.size == bp)
    {
	// merge with left neighbour
	p->s.size += bp->s.size;
	p->s.ptr = bp->s.ptr;
    }
    else
	p->s.ptr = bp;

    freep = p;
}

// Issue an allocation map -- using the alloc list
void MemCheck::map(ostream& os, unsigned start, char *prefix)
{
    init();

    unsigned found = 0;

    for (MemCheckHeader *p = allocp->s.ptr; ; p = p->s.ptr)
    {
	validate(p, "map");
	
	if (p->s.tic > 0 && p->s.tic >= start)
	{
	    if (prefix[0] != '\0')
		os << prefix << ": ";
	    os << p->s.requested << " bytes in block " << p->s.tic
	       << " at " << (void *)(p + 1) << "\n";
	    found++;
	}
		
	if (p == allocp)
	    break;
    }

    if (found)
    {
	if (prefix[0] != '\0')
	    os << prefix << ": ";
	os << allocBytes << "/" << allocBytes + freeBytes << " bytes in use\n";
    }
}

// Validate
int MemCheck::OK()
{
    init();

    for (MemCheckHeader *p = allocp->s.ptr; ; p = p->s.ptr)
    {
	validate(p, "OK");
	if (p == allocp)
	    break;
    }

    return 1;
}

    
