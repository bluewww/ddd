// $Id$ -*- C++ -*-
// Issue the name of building user

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char userinfo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "hostname.h"
#include "bool.h"

extern "C" {
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
}

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>

#define NO_UNAME_AGENTS
#include "hostname.C"

// Return true if C is a letter found in real user names
inline bool is_letter(char c)
{
    return c && (isalpha(c) || isspace(c) || c == '.' || c == '-');
}

int userinfo(char *arg = 0)
{
    struct passwd *pwd = 0;

    if (arg == 0)
    {
	// Get info for current user
	pwd = getpwuid(getuid());
    }
    else if (isdigit(arg[0]))
    {
	// Get info for numerical id
	pwd = getpwuid(atoi(arg));
    }
    else
    {
	// Get info for user name
	pwd = getpwnam(arg);
    }

    if (pwd == 0)
    {
	fputs(arg, stderr);
	fputs(": no such passwd entry\n", stderr);
	return 1;
    }

    // Issue real name
    char *s = pwd->pw_gecos;
    while (is_letter(*s))
	putchar(*s++);

    if (s == pwd->pw_gecos)
    {
	// No real name given; try user id
	if (pwd->pw_name[0])
	{
	    s = pwd->pw_name;
	    while (is_letter(*s++))
		;

	    if (*s)
	    {
		// User id contains non-letters and is probably a
		// symbolic name => leave it unchanged.
		fputs(pwd->pw_name + 1, stdout);
	    }
	    else
	    {
		// User id is a real name => capitalize it.
		fputc(toupper(pwd->pw_name[0]), stdout);
		fputs(pwd->pw_name + 1, stdout);
	    }
	}
	else
	{
	    // No user id.  This is weird.
	    fputs("Unknown ", stdout);
	}
    }

    // Issue user id and host (probable mail address)
    fputs(" <", stdout);
    fputs(pwd->pw_name, stdout);
    fputs("@", stdout);
    fputs(fullhostname(), stdout);
    fputs(">\n", stdout);

    return 0;
}

// Issue the name of the building user, in the format
// ``REALNAME <USERNAME@HOSTNAME>''
int main(int argc, char *argv[])
{
    if (argc == 1)
	return userinfo();

    int ret = 0;
    for (int i = 1; i < argc; i++)
	ret |= userinfo(argv[i]);

    return ret;
}
