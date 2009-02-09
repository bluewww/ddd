// $Id$ -*- C++ -*-
// Determine file type

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003, 2005 Free Software Foundation, Inc.
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

char filetype_rcsid[] = 
    "$Id$";

#include "filetype.h"

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

extern "C" {
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <fcntl.h>
}

#if defined(HAVE_LIBELF_H) && defined(__powerpc__)
#include  <libelf.h>
#ifndef EM_PPC
#define EM_PPC 20
#endif
#endif // defined(HAVE_LIBELF_H) && defined(__powerpc__)

#include "regexps.h"

// Test for regular file - see stat(3)
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

// Test for directory - see stat(3)
#ifndef S_ISDIR
#define	S_ISDIR(m) (((m) & _IFMT) == _IFDIR)
#endif

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif


//-----------------------------------------------------------------------------
// Determine file types
//-----------------------------------------------------------------------------

// True if FILE_NAME is a regular file
bool is_regular_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name.chars(), &sb))
	return false;		// cannot stat

    return S_ISREG(sb.st_mode);
}

// True if FILE_NAME contains non-TEXT characters
bool is_binary_file(const string& file_name)
{
    char buf[16];

    int fd = open(file_name.chars(), O_RDONLY);
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
    if (stat(file_name.chars(), &sb))
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

// True if FILE_NAME is a PPC file
static bool is_ppc_file(const string& file_name)
{
#if defined(HAVE_LIBELF_H) && defined(__powerpc__)
    Elf32_Ehdr *   ehdr;
    Elf *          elf;
    int       	   fd;

    bool ret_code = true;

    if ((fd = open(file_name.chars(), O_RDONLY)) == -1)
	return false;		// cannot open

    // Obtain the ELF descriptor
    (void) elf_version(EV_CURRENT);
    if ((elf = elf_begin(fd, ELF_C_READ, 0)) == 0)
	ret_code = false;		// cannot get descriptor

    // Obtain ELF header
    else if ((ehdr = elf32_getehdr(elf)) == 0)
	ret_code = false;		// cannot get header

    else if (ehdr->e_type != ET_REL)
	ret_code = false;		// not relocatable
    
    else if (ehdr->e_machine != EM_PPC)
	ret_code = false;		// not a PowerPC
    
    else
	ret_code = true;		// not a PowerPC
    
    if (elf)
	elf_end(elf);

    close(fd);
    return ret_code;
#else
    (void) file_name;		// Use it
    return false;
#endif // defined(HAVE_LIBELF_H) && defined(__powerpc__)
}

// True if FILE_NAME is an executable binary debuggee
bool is_debuggee_file(const string& file_name)
{
    return is_ppc_file(file_name) || is_exec_file(file_name);
}

// True if FILE_NAME is a core file
bool is_core_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name.chars(), &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    // Let's try some heuristics to exclude other files...
    if (file_name.contains("core", -1))
	return true;		// looks like a core file

    // Paul E. Raines states: My source files are on an NFS mounted
    // VMS partition that in translation all files are marked
    // executable (like mounting DOS under Linux).
    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable

    if (!is_binary_file(file_name))
	return false;		// no binary character in header

    if (file_name.contains(".o", -1))
	return false;		// looks like an object file

    if (file_name.contains(".a", -1))
	return false;		// looks like an archive file

    // FILE_NAME is a core file iff `file FILE_NAME' issues `core'.
    const string cmd = "file " + file_name;
    FILE *fp = popen(cmd.chars(), "r");
    if (fp != 0)
    {
	char line[PATH_MAX];
	fgets(line, sizeof(line), fp);
	pclose(fp);

	string out(line);
#if RUNTIME_REGEX
	static regex rxcore(".*:.*core.*");
#endif
	return out.matches(rxcore);
    }

    return true;		// `file' not found
}

// True if FILE_NAME is a source file
bool is_source_file(const string& file_name)
{
    if (file_name.contains(".o", -1))
	return false;		// looks like an object file

    if (file_name.contains(".a", -1))
	return false;		// looks like an archive file

    struct stat sb;
    if (stat(file_name.chars(), &sb))
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
    // Also note that Python and Perl have executable source files.

    if (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	return false;		// executable
#endif

    return true;
}

// True if FILE_NAME is a postscript file
bool is_postscript_file(const string& file_name)
{
    struct stat sb;
    if (stat(file_name.chars(), &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    char buf[2];

    int fd = open(file_name.chars(), O_RDONLY);
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
    if (stat(file_name.chars(), &sb))
	return false;		// cannot stat

    if (!S_ISREG(sb.st_mode))
	return false;		// not a regular file

    char buf[4];

    int fd = open(file_name.chars(), O_RDONLY);
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

// True if FILE_NAME is executable and its first line contains `#! PATTERN'.
static bool has_hashbang(const string& file_name, const string& pattern)
{
    if (!is_cmd_file(file_name))
	return false;

    char buf[BUFSIZ];

    int fd = open(file_name.chars(), O_RDONLY);
    if (fd < 0)
	return false;
    int n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n < 2)
	return false;

    if (buf[0] != '#' || buf[1] != '!')
	return false;

    string line(buf, n);
    line = line.before('\n');

    return line.contains(pattern);
}

// A Python file is a standard source file which ends in '.py'
bool is_python_file(const string& file_name)
{
    if (!is_source_file(file_name))
	return false;

    if (file_name.contains(".py", -1) || 
	file_name.contains(".python", -1) || 
	has_hashbang(file_name, "python"))
	return true;

    return false;
}

// A Perl file is a standard source file which ends in '.pl' or `.pm'
bool is_perl_file(const string& file_name)
{
    if (!is_source_file(file_name))
	return false;

    if (file_name.contains(".pl", -1) || 
	file_name.contains(".pm", -1) || 
	file_name.contains(".perl", -1) || 
	has_hashbang(file_name, "perl"))
	return true;

    return false;
}

// A Bash file is a standard source file which ends in '.sh' or 
// has #! bash. But we'll also take #! sh as well.
bool is_bash_file(const string& file_name)
{
    if (!is_source_file(file_name))
	return false;

    if (file_name.contains(".sh", -1) 
	|| has_hashbang(file_name, "bash")
	|| has_hashbang(file_name, "sh"))
	return true;

    return false;
}

// A GNU Makefile is a standard source file that usually has the name
// Makefile, GNUmakefilestarts that way or ends `.mk' or `.mak'
bool is_make_file(const string& file_name)
{
    if (!is_source_file(file_name))
	return false;

    if (file_name.contains(".mak", -1) 
	|| file_name.contains(".mk", -1)
	|| file_name.contains("GNUMakefile", -1)
	|| file_name.contains("Makefile", -1)
        || file_name.contains("makefile", -1))
	return true;

    return false;
}

// A PHP file is a standard source file which ends in '.php' or `.php4'
bool is_php_file(const string& file_name)
{
    if (!is_source_file(file_name))
	return false;

    if (file_name.contains(".php", -1) || 
	file_name.contains(".php4", -1))
	return true;

    return false;
}

// True if FILE_NAME is a directory
bool is_directory(const string& file_name)
{
    struct stat sb;
    if (stat(file_name.chars(), &sb))
	return false;

    if (!S_ISDIR(sb.st_mode))
	return false;		// not a directory

    return true;
}

// Return full file name of COMMAND
string cmd_file(const string& command)
{
    const char *path_s = getenv("PATH");
    if (path_s == 0 || command.contains('/'))
	return command;

    // Check if COMMAND occurs in any of the components of PATH
    char buf[2048];
    strncpy(buf, path_s, sizeof(buf) - 1);
    for (const char *dir = strtok(buf, ":"); dir != 0; dir = strtok(0, ":"))
    {
	string path = string(dir) + '/' + command;
	if (is_cmd_file(path))
	    return path;
    }

    return command;
}

// Return last modification time of FILE_NAME
time_t last_modification_time(const string& file_name)
{
    struct stat sb;
    if (stat(file_name.chars(), &sb))
	return 0;

    return sb.st_mtime;
}
