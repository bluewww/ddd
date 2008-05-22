// $Id$
// Convert signal to name

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char sigName_rcsid[] = 
    "$Id$";

#include "config.h"

#include <stdio.h>
#include <string.h>

extern "C" {
#include <stddef.h>
#include <signal.h>
#include <sys/wait.h>
}

#include "strclass.h"
#include "sigName.h"

#if HAVE_UNISTD_H
extern "C" {
#include <unistd.h>
}
#endif

#if HAVE_STRSIGNAL && !HAVE_STRSIGNAL_DECL
extern "C" const char *strsignal(int signo);
#endif

// Convert signal number into name
const char *sigName(int signo)
{
#if HAVE_STRSIGNAL
    return strsignal(signo);
#else
    static char buffer[256];

    sprintf(buffer, "Signal %d", signo);
    return buffer;
#endif
}

// Convert wait() status into name
const char *statusName(int state)
{
    if (WIFEXITED(((state))))
    {
	static char buffer[256];

	sprintf(buffer, "Exit %d", (int)WEXITSTATUS(((state))));
	return buffer;
    }
    else if (WIFSIGNALED(((state))))
        return sigName(WTERMSIG(((state))));
    else if (WIFSTOPPED(((state))))
        return sigName(WSTOPSIG(((state))));

    return "Unknown state change";
}
