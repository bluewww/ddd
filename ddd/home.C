// $Id$ -*- C++ -*-
// Return home directory

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

char home_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "home.h"
#include "strclass.h"

#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

// Return home dir
char *gethome()
{
    static string home = "";

    // Try using $HOME
    if (home == "")
    {
	char *h = getenv("HOME");
	if (h != NULL)
	    home = h;
    }

    // Try using passwd entry
    if (home == "")
    {
	struct passwd *pw = 0;

	char *user = getenv("USER");
	if (user == 0)
	    user = getenv("LOGNAME");

	if (user != 0)
	    pw = getpwnam(user);
	if (pw == 0)
	    pw = getpwuid(getuid());
	if (pw != 0)
	    home = pw->pw_dir;
    }

    // Revert to current dir
    if (home == "")
	home = ".";

    return home;
}
