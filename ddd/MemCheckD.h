// $Id$ -*- C++ -*-
// MemCheck operator definitions

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
