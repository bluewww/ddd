// $Id$
// own assert() macros
// built on libg++-assert.h and making sure top-level abort() is called

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

// Allow this file to be included multiple times with different
// settings of NDEBUG.

#if NDEBUG
#undef assert
#undef _assert_fn
#define assert(ignore)

#else  // !NDEBUG

#if HAVE_CONFIG_H
#include "config.h"		// HAVE_PRETTY_FUNCTION
#endif

#include <stdlib.h>		// abort()
#include <iostream>

#undef assert
#undef _assert_fn

#if HAVE_PRETTY_FUNCTION
#define _assert_fn \
"\n" << __FILE__ << ": In function `" << __PRETTY_FUNCTION__ << "':"
#else // !HAVE_PRETTY_FUNCTION
#define _assert_fn ""
#endif // !HAVE_PRETTY_FUNCTION

#define assert(ex) \
((ex) ? 1 : (std::cerr << _assert_fn << "\n" << __FILE__ << ":" << __LINE__ \
	          << ": assertion `" #ex "' failed\n", \
	          ::abort(), 0))

#endif // !NDEBUG
