// $Id$
// Exit Callbacks

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

char ExitCB_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include <unistd.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

#include "findParent.h"
#include "ExitCB.h"
#include "verify.h"

extern char **environ;

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 0
#endif

// callbacks

// leave program
void ExitCB(Widget, XtPointer, XtPointer)
{
    exit(EXIT_SUCCESS);
}

static char **saved_argv    = 0;
static char **saved_environ = 0;

// restart program
void RestartCB(Widget, XtPointer, XtPointer)
{
    environ = saved_environ;
    execvp(saved_argv[0], saved_argv);
    exit(EXIT_FAILURE);
}

// Save environment
void register_restart(char *argv[])
{
    int i;
    int argc = 0;
    while (argv[argc] != 0)
	argc++;

    saved_argv = new char *[argc + 1];
    for (i = 0; i < argc; i++)
    {
	saved_argv[i] = new char[strlen(argv[i]) + 1];
	strcpy(saved_argv[i], argv[i]);
    }
    saved_argv[argc] = 0;

    int envc = 0;
    while (environ[envc] != 0)
	envc++;

    saved_environ = new char *[envc + 1];
    for (i = 0; i < envc; i++)
    {
	saved_environ[i] = new char[strlen(environ[i]) + 1];
	strcpy(saved_environ[i], environ[i]);
    }
    saved_environ[envc] = 0;
}
