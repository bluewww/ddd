// $Id$ -*- C++ -*-
// System-dependent type macros 

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@gnu.org).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// ICE is the incremental configuration engine.
// Contact ddd@gnu.org for details.

#ifndef _DDD_constref_h
#define _DDD_constref_h

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

#endif // _DDD_constref_h
// DON'T ADD ANYTHING BEHIND THIS #endif
