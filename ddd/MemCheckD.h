// $Id$ -*- C++ -*-
// MemCheck operator definitions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:22  zeller
// Initial revision
//
// Revision 1.1.1.1  1995/02/09  09:11:33  zeller
// ICE 0.5 distribution
//
// Revision 1.6  1995/01/14  14:48:27  zeller
// New license for ICE 0.5
//

// Usage:
// Include this file in your main module (or in a single module which is
// linked with your program). All allocation and deallocation
// will then go through the MemCheck class functions.

static char MemCheckDef_rcsid[] = 
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
{
    MemCheck::free(p);
}

#ifdef HAVE_ARRAY_OPERATOR_NEW
// Array new/delete ops - will be called for C++ array free store operations
void *operator new[](size_t nbytes)
{
    return MemCheck::alloc(nbytes);
}

void operator delete[](void *p)
{
    MemCheck::free(p);
}
#endif // defined(HAVE_ARRAY_OPERATOR_NEW)
