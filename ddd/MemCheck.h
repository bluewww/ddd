// $Id$ -*- C++ -*-
// Memory Checker

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

#ifndef _ICE_MemCheck_h
#define _ICE_MemCheck_h

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>
#include "strclass.h"

// Include this file in any module you wish to debug.
// Include "MemCheckD.h" in your main module to enable memory checking.

// MemCheckHeader -- for internal use only

const unsigned MAGIC = 3917580; // Marks start of block
const char WIPEOUT   = '@';	// Filled in free()'d blocks
typedef double Align;
    
class MemCheckHeader {
    friend class MemCheck;

private:
    union {
	struct Block {
	    MemCheckHeader *ptr;    // Next block
	    unsigned size;	    // Actual size of this block
	    unsigned magic;	    // Magic number (debugging only)
	    unsigned requested;	    // Requested size (debugging only)
	    unsigned tic;	    // ID of this block (debugging only)
	    unsigned dummy;	    // Required by gcc-2.5.5 (otherwise crash)
	} s;
	Align x;
    };

public:
    void init()
    {
	// build a single circular list
	s.ptr       = this;
	s.size      = 0;
	s.magic     = MAGIC;
	s.requested = 0;
	s.tic       = 0;
    }
};


// The MemCheck class is just a prefix for common operations.
// These are usually enabled with the new/delete operators (see below).
// 
// Other common usages include:
// MemCheck::map(cout);   // show all allocated blocks
// MemCheck::log = 1;     // log all new/delete operations

class MemCheck {
    friend class MemChecker;
    
private:
    static MemCheckHeader *freep;	// Free list
    static MemCheckHeader *allocp;	// Alloc list

    static MemCheckHeader freebase;	// Start of free list
    static MemCheckHeader allocbase;	// Start of alloc list

    static unsigned tics;	        // Next available ID
    static unsigned freeBytes;		// Sum of bytes in free list
    static unsigned allocBytes;	        // Sum of bytes in alloc list

    static MemCheckHeader *morecore(unsigned nunits);
    static void _free(MemCheckHeader *bp);
    static void validate(MemCheckHeader *bp, char *src);

    // initialize
    static void init();

public:
    // Allocate nbytes of memory
    static void *alloc(unsigned nbytes);

    // Free block at p
    static void free(void *p);

    // Issue a map of all allocated blocks (starting with block #start)
    static void map(ostream& os, unsigned start = 0, char *prefix = "");

    // Return current block number
    static unsigned tic() { return tics; }

    // Return current memory usage
    static unsigned in_free()  { return freeBytes; }
    static unsigned in_alloc() { return allocBytes; }

    // If non-zero, log all alloc/free operations
    static int log;

    // validate
    static int OK();
};


// A MemChecker object may be used to show all allocations that occured
// during its lifetime. For instance,
//
// {
//     MemChecker m("f()");
//     f();
// } // all allocations done by f will be reported here

class MemChecker {
private:
    string prefix;
    unsigned start;

public:
    MemChecker(char *pfx = ""):
        prefix(pfx), start(MemCheck::tic())
    {}

    ~MemChecker()
    {
	MemCheck::map(clog, start, prefix);
    }
};

// Setup if required
inline void MemCheck::init()
{
    if (freebase.s.magic != MAGIC)
    {
	freebase.init();
	allocbase.init();
	freep      = &freebase;
	allocp     = &allocbase;
	freeBytes  = 0;
	allocBytes = 0;
	tics       = 1;
    }
}

#endif // _ICE_MemCheck_h
// DON'T ADD ANYTHING BEHIND THIS #endif
