// $Id$
// own assert() macros
// built on libg++-assert.h and making sure top-level abort() is called

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2004, 2004 Free Software Foundation, USA.
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

// Allow this file to be included multiple times with different
// settings of NDEBUG.

#ifdef NDEBUG
#undef assert
#undef _assert_fn
#define assert(ignore)

#else  // !NDEBUG

#if HAVE_CONFIG_H
#include "config.h"		// HAVE_PRETTY_FUNCTION
#endif

#include "casts.h"

#if defined __cplusplus
# define _ASSERT_VOID_CAST(x) STATIC_CAST(void,x)
#else
# define _ASSERT_VOID_CAST(x) (void)(x)
#endif

#include "attribute.h"
void ddd_assert_fail (const char *assertion, const char *file,
		      unsigned int line, const char *function)
     ATTRIBUTE_NORETURN;

#undef assert
#undef _assert_fn

#if HAVE_PRETTY_FUNCTION
#define _assert_fn __PRETTY_FUNCTION__
#else // !HAVE_PRETTY_FUNCTION
#define _assert_fn ""
#endif // !HAVE_PRETTY_FUNCTION

#define assert(ex) \
_ASSERT_VOID_CAST(unlikely(ex) ? \
  0 : \
  (ddd_assert_fail (#ex, __FILE__, __LINE__, _assert_fn), 0) \
  )

#endif // !NDEBUG
