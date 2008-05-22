// $Id$ -*- C++ -*-
// Declare `alloca()' properly

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_my_alloca_h
#define _DDD_my_alloca_h

#include "config.h"

// Check for alloca()
// AIX requires this to be the first thing in the file.
#ifdef __GNUC__
#define alloca(size) __builtin_alloca(size)
#elif HAVE_ALLOCA_H
#include <alloca.h>		// This should give us a declaration
#elif defined(_AIX)
 #pragma alloca
#else /* not AIX */
#include <stdlib.h>		// In GNU libc, alloca is declared here
#if !defined(alloca) && !HAVE_ALLOCA_DECL
extern "C" char *alloca(unsigned int size);
#endif /* no alloca decl */
#endif /* not AIX */

#endif // _DDD_my_alloca_h
// DON'T ADD ANYTHING BEHIND THIS #endif
