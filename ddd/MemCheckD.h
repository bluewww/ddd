// $Id$ -*- C++ -*-
// MemCheck operator definitions

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

// Usage:
// Include this file in your main module (or in a single module which is
// linked with your program). All allocation and deallocation
// will then go through the MemCheck class functions.

char MemCheckDef_rcsid[] = 
    "$Id$";

#include "MemCheck.h"

#include "config.h"
#include <sys/types.h>

// Std new/delete ops - will be called for C++ non-array free store operation
void *operator new(size_t nbytes)
{
    return MemCheck::alloc(nbytes);
}

void operator delete(void *p)
#if HAVE_EXCEPTIONS
    throw()
#endif
{
    MemCheck::free(p);
}

#if HAVE_ARRAY_OPERATOR_NEW
// Array new/delete ops - will be called for C++ array free store operations
void *operator new[](size_t nbytes)
{
    return MemCheck::alloc(nbytes);
} 

void operator delete[](void *p)
#if HAVE_EXCEPTIONS
    throw()
#endif
{
    MemCheck::free(p);
}
#endif // HAVE_ARRAY_OPERATOR_NEW
