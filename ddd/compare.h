// $Id$ -*- C++ -*-
// generic compare() function

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_compare_h
#define _DDD_compare_h

#ifdef __GNUG__
#pragma interface
#endif

// compare(t1, t2) returns 1/0/-1 if t1 is greater/equal/less than t2
// Was initially implemented as a template, but won't work correctly with gcc

#define DECLARE_COMPARE(T) \
inline int compare(T t1, T t2)     \
{                                  \
    if (t1 > t2)                   \
	return 1;                  \
    else if (t1 < t2)              \
        return -1;                 \
    else                           \
	return 0;                  \
}

DECLARE_COMPARE(int)
DECLARE_COMPARE(unsigned int)

DECLARE_COMPARE(long)
DECLARE_COMPARE(unsigned long)

DECLARE_COMPARE(short)
DECLARE_COMPARE(unsigned short)

DECLARE_COMPARE(char)
// DECLARE_COMPARE(signed char)
DECLARE_COMPARE(unsigned char)

DECLARE_COMPARE(float)
DECLARE_COMPARE(double)

inline int compare(void *t1, void *t2)
{
    return compare((unsigned long)t1, (unsigned long)t2);
}

#undef DECLARE_COMPARE

#endif // _DDD_compare_h
// DON'T ADD ANYTHING BEHIND THIS #endif
