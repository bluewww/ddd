// $Id$ -*- C++ -*-
// Determine file type

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:22  zeller
// Initial revision
//
// Revision 1.3  1995/04/29  12:14:15  zeller
// Fix: AIX requires <sys/user.h> before <sys/core.h>
//
// Revision 1.2  1995/04/21  16:35:44  zeller
// New: check for picture files
//
// Revision 1.1  1995/03/22  19:26:17  zeller
// Initial revision
//

static const char rcsid[] = 
    "$Id$";

#include "filetype.h"

#include "config.h"

#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <fcntl.h>

#ifdef HAVE_SYS_USER_H
// AIX requires this before including <sys/core.h>.
// Reported by bas@phys.uva.nl (Bas V. de Bakker).
#include <sys/user.h>
#endif

#ifdef HAVE_SYS_CORE_H
#include <sys/core.h>		// CORE_MAGIC
#endif


// True if FILE_NAME contains non-TEXT characters
bool is_binary_file(const string& file_name)
{
    char buf[16];

    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
	return false;
    int n = read(fd, buf, sizeof(buf));
    close(fd);

    for (int i = 0; i < n; i++)
    {
	char c = buf[i];
	if (iscntrl(c) && c != '\t' && c != '\n' && c != '\r' && c != '\b'
	    && c != '\v')
	    return true;
    }

    return false;
}


// True if FILE_NAME is an executable file
bool is_exec_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    if (!(sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
	return false;		// not executable

    if (!is_binary_file(file_name))
	return false;		// no binary character in header

    return true;
}

// True if FILE_NAME is a core file
bool is_core_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

#ifdef CORE_MAGIC
    int magic = 0;

    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
	return false;
    read(fd, &magic, sizeof(magic));
    close(fd);

    if (magic != CORE_MAGIC)
	return false;		// no core file
#else
    // Let's try some heuristics to exclude other files...
    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable

    if (!is_binary_file(file_name))
	return false;		// no binary character in header

    if (file_name.contains(".o", -1))
	return false;		// looks like an object file

    if (file_name.contains(".a", -1))
	return false;		// looks like an archive file
#endif

    return true;
}

// True if FILE_NAME is a source file
bool is_source_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    if (is_binary_file(file_name))
	return false;		// binary character in header

    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable

    return true;
}

// True if FILE_NAME is a postscript file
bool is_postscript_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    char buf[2];

    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
	return false;
    int n = read(fd, buf, sizeof(buf));
    close(fd);

    return n == 2 
	&& buf[0] == '%'
	&& buf[1] == '!';
}

// True if FILE_NAME is a FIG file
bool is_fig_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    char buf[4];

    int fd = open(file_name, O_RDONLY);
    if (fd < 0)
	return false;
    int n = read(fd, buf, sizeof(buf));
    close(fd);

    return n == 4 
	&& buf[0] == '#' 
	&& buf[1] == 'F' 
	&& buf[2] == 'I' 
	&& buf[3] == 'G';
}

// True if FILE_NAME is an XWD file
bool is_xwd_file(const string& file_name)
{
    // No simple way to determine this portably... *sigh*
    return false;
}

// True if FILE_NAME is a directory
bool is_directory(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;

    if (!S_ISDIR(sb.st_mode))
	return false;		// not a directory

    return true;
}
