// $Id$
// Convert signal to name

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char sigName_rcsid[] = 
    "$Id$";

#include "config.h"

#include <stddef.h>
#include <signal.h>
#include <sys/wait.h>
#include <strstream.h>

#include "strclass.h"
#include "sigName.h"

extern "C" char *strsignal(int signo);

// convert signal number into name
char *sigName(int signo)
{
    return strsignal(signo);
}


// convert wait() status into name
char *statusName(int status)
{
    static string s;

    s = "Unknown State Change";

    if (WIFEXITED(status))
    {
	ostrstream os;
	os << "Exit " << WEXITSTATUS(status);
        s = string(os);
    }
    else if (WIFSIGNALED(status))
        s = sigName(WTERMSIG(status));
    else if (WIFSTOPPED(status))
        s = sigName(WSTOPSIG(status));

    return (char *)s;
}
