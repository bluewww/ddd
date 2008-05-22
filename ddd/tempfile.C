// $Id$ -*- C++ -*-
// Return a temporary file name.

// Copyright (C) 2001 Universitaet Passau, Germany.
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

char tempfile_rcsid[] = 
    "$Id$";

#include "tempfile.h"
#include "config.h"
#include "version.h"

#if HAVE_UNISTD_H
#include <unistd.h>		// mkstemp (old)
#endif

#include <stdlib.h>		// mkstemp (new)
#include <stdio.h>		// tmpnam
#include <string.h>		// strcpy, strcat...

#if HAVE_MKSTEMP && !HAVE_MKSTEMP_DECL
extern "C" int mkstemp(char *templ);
#endif

// Return the name of a temporary file.
string tempfile()
{
    char templ[2048];

#if HAVE_MKSTEMP
    // Find a directory
    const char *tmpdir = getenv("TMPDIR");
    if (tmpdir == 0)
    {
#ifdef P_tmpdir
	tmpdir = P_tmpdir;
#else
	tmpdir = "/tmp";
#endif
    }

    const char *base = "/" ddd_NAME "XXXXXX";

    int max_tmpdir_len = sizeof(templ) - strlen(base);
    strncpy(templ, tmpdir, max_tmpdir_len);
    templ[max_tmpdir_len] = '\0';
    strcat(templ, base);

    // Use mkstemp.  Note: This also creates the file.
    int tempfd = mkstemp(templ);
    string tempfile = templ;
    close(tempfd);
    return tempfile;

#else  // !HAVE_MKSTEMP

    // Use tmpnam.  Unsafe.
    return string(tmpnam(templ));
#endif
}
