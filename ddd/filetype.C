// $Id$ -*- C++ -*-
// Determine file type

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char filetype_rcsid[] = 
    "$Id$";

#include "filetype.h"

#include "config.h"

extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <fcntl.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

// AIX and DEC alpha OSF/1 v3.0 require <sys/user.h> being included
// before <sys/core.h>.  Reported by bas@phys.uva.nl (Bas V. de
// Bakker) and dmitzel@everest.hitc.com.
#ifdef HAVE_SYS_USER_H
#ifndef __hpux
// On HP-UX, either don't include sys/user.h, or else you must include
// sys/param.h first.  Reported by neal@ctd.comsat.com (Neal Becker)
#include <sys/user.h>
#endif
#endif

#ifdef HAVE_SYS_CORE_H
#include <sys/core.h>		// CORE_MAGIC
#endif
}

// Test for regular file - see stat(3)
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

// Test for directory - see stat(3)
#ifndef S_ISDIR
#define	S_ISDIR(m) (((m) & _IFMT) == _IFDIR)
#endif

// True if FILE_NAME is a regular file
bool is_regular_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    return S_ISREG(sb.st_mode);
}

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
bool is_cmd_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name, &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    if (!(sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
	return false;		// not executable

    return true;
}

// True if FILE_NAME is an executable binary file
bool is_exec_file(const string& file_name)
{
    return is_cmd_file(file_name) && is_binary_file(file_name);
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
#if 0
    // Paul E. Raines states: My source files are on an NFS mounted
    // VMS partition that in translation all files are marked
    // executable (like mounting DOS under Linux)
    // Hence, this check would filter out all core files.

    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable
#endif

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

#if 0
    // Paul E. Raines states: My source files are on an NFS mounted
    // VMS partition that in translation all files are marked
    // executable (like mounting DOS under Linux)
    // Hence, this check would filter out all source files.

    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable
#endif

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
bool is_xwd_file(const string& /* file_name */)
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
