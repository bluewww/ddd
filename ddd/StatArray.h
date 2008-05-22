// $Id$ -*- C++ -*-
// Array of `stat' entries

// Copyright (C) 1999 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_StatArray_h
#define _DDD_StatArray_h

#include "VarArray.h"
#include "DynArray.h"

#include "config.h"
#include "bool.h"

extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <fcntl.h>
}

inline bool operator==(const struct stat& sb1, const struct stat& sb2)
{
    return sb1.st_ino == sb2.st_ino && sb1.st_dev == sb2.st_dev;
}

typedef VarArray<struct stat> StatArray;

#endif // _DDD_StatArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
