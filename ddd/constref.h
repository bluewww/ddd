// $Id$ -*- C++ -*-
// System-dependent type macros 

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

#ifndef _ICE_constref_h
#define _ICE_constref_h

#include "config.h"

// Use `CONST_REF(TYPE) X' instead of `const TYPE& X' if X may be
// initialized from a temporary.  This prevents destruction on non-
// ANSI-C++-compliant compilers before the function call is complete.

#ifdef HAVE_ANSI_LIFETIME_OF_TEMPORARIES
#define REF(type) type &
#else
#define REF(type) type
#endif

#define CONST_REF(type) const REF(type)

#endif // _ICE_constref_h
// DON'T ADD ANYTHING BEHIND THIS #endif
