// $Id$ -*- C++ -*-
// getrlimit() decl

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_getrlimit_h
#define _DDD_getrlimit_h

#ifdef __GNUG__
#pragma interface
#endif

#include "config.h"

#if HAVE_GETRLIMIT && HAVE_SETRLIMIT
#if HAVE_SYS_RESOURCE_H

#if HAVE_SYS_TIME_H
#include <sys/time.h>		// SunOS wants this
#endif // HAVE_SYS_TIME_H

extern "C" {
// Harry Mangalam <hjm@darwin.bio.uci.edu> says he had to comment out
// the following line `on an Alpha PC164LX system running Linux kernel
// ver 2.0.33 to get it to compile (otherwise a bunch of RLIMIT errors
// stopped it).'
#include <sys/resource.h>

#if HAVE_GETRLIMIT && !HAVE_GETRLIMIT_DECL
int getrlimit (int resource, struct rlimit *rlp);
#endif // HAVE_GETRLIMIT && !HAVE_GETRLIMIT_DECL
#if HAVE_SETRLIMIT && !HAVE_SETRLIMIT_DECL
int setrlimit (int resource, struct rlimit *rlp);
#endif // HAVE_SETRLIMIT && !HAVE_SETRLIMIT_DECL
}

#endif // HAVE_SYS_RESOURCE_H
#endif // HAVE_GETRLIMIT && HAVE_SETRLIMIT

#endif // _DDD_getrlimit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
