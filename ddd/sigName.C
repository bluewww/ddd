// $Id$
// Convert signal to name

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

extern "C" char *strsignal(int signo);

// Convert signal number into name
char *sigName(int signo)
{
    return strsignal(signo);
}

// Convert wait() status into name
char *statusName(int state)
{
    if (WIFEXITED(state))
    {
	static char buffer[256];

	sprintf(buffer, "Exit %d", (int)WEXITSTATUS(state));
	return buffer;
    }
    else if (WIFSIGNALED(state))
        return sigName(WTERMSIG(state));
    else if (WIFSTOPPED(state))
        return sigName(WSTOPSIG(state));

    return "Unknown state change";
}
