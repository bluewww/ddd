// $Id$
// own assert() macros
// built on libg++-assert.h and making sure top-level abort() is called

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

// Allow this file to be included multiple times with different
// settings of NDEBUG
#if defined(NDEBUG)
#undef assert
#undef _assert_fn
#define assert(ignore)

#elif HAVE_CONFIG_H
#include "config.h"
#include <stdlib.h>		// abort()
#include <iostream.h>

#undef assert
#undef _assert_fn

#if HAVE_PRETTY_FUNCTION
#define _assert_fn \
"\n" << __FILE__ << ": In function `" << __PRETTY_FUNCTION__ << "':"
#else // !HAVE_PRETTY_FUNCTION
#define _assert_fn ""
#endif // !HAVE_PRETTY_FUNCTION

#define assert(ex) \
((ex) ? 1 : (cerr << _assert_fn << "\n" << __FILE__ << ":" << __LINE__ \
	          << ": assertion `" #ex "' failed\n", \
	          ::abort(), 0))

#else // !defined(NDEBUG) && !HAVE_CONFIG_H

// This is weird.  In our projects, HAVE_CONFIG_H is always defined.
// Are we running `configure' with `.' in the <...> #include path?
// Revert to the original <assert.h>.

#if __GNUC__
#include_next <assert.h>	 // GNU C extension
#else
#include </usr/include/assert.h> // Dirty fix
#endif

#endif // HAVE_CONFIG_H && !defined(NDEBUG)
