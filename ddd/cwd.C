// $Id$ -*- C++ -*-
// Get current working directory

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char cwd_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "cwd.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Return true iff S1 and S2 are identical
bool same_file(const string& s1, const string& s2)
{
    if (s1 == s2)
	return true;

    struct stat s1_stat;
    struct stat s2_stat;

    if (stat(s1, &s1_stat) == 0 &&
	stat(s2, &s2_stat) == 0 &&
	s1_stat.st_dev == s2_stat.st_dev &&
	s1_stat.st_ino == s2_stat.st_ino)
	return true;

    return false;
}

// Return current working directory
string cwd()
{
    // Try $PWD
    char *pwd = getenv("PWD");
    if (pwd != 0 && same_file(pwd, "."))
    {
	// $PWD points to the current dirctory
	return pwd;
    }

    // Try getcwd()
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) == buffer)
        return buffer;

    // Everything failed -- return "."
    return ".";
}
