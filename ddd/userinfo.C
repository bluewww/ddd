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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char userinfo_rcsid[] = 
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

#include <iostream.h>
#include <ctype.h>
#include <pwd.h>

// Return true if C is a letter found in real user names
inline bool is_letter(char c)
{
    return isalpha(c) || isspace(c) || c == '.';
}

// Issue the name of the building user, in the format
// REALNAME <USERNAME@HOSTNAME>
int main()
{
    // Fetch user
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);

    // Issue real name
    char *s = pwd->pw_gecos; 
    while (is_letter(*s))
	cout << *s++;

    // Issue user and host (probable mail address)
    cout << " <" << pwd->pw_name << "@" << fullhostname() << ">\n";
    return 0;
}
