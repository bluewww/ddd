// $Id$ -*- C++ -*-
// Improved `gcore' command.

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char gcore_rcsid[] = 
    "$Id$";

#include "config.h"
#include "version.h"

#if !HAVE_PTRACE || !HAVE_PTRACE_DUMPCORE
#error gcore requires a ptrace() interface and a PTRACE_DUMPCORE request
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>		// getopt

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#include <signal.h>

#if HAVE_PTRACE
extern "C" {
#if HAVE_SYS_PTRACE_H
#include <sys/ptrace.h>
#endif
#if !HAVE_PTRACE_DECL
extern int ptrace(int request, int pid, int addr, int data);
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
}
#endif

#include "strclass.h"


int main(int argc, char *argv[])
{
    int c;
    extern char *optarg;
    extern int optind;

    string filename = "core";
    int errors = 0;
    bool stop = (DDD_NAME != 0);
    while ((c = getopt(argc, argv, "o:s")) != -1)
    {
	switch (c)
	{
	case 'o':
	    filename = optarg;
	    break;
	case 's':
	    stop = true;
	    break;
	case '?':
	    errors++;
	    break;
	}
    }

    if (optind >= argc)
	errors++;

    if (errors)
    {
	std::cerr << "gcore: usage: gcore [-o file ] [-s] pid ...\n";
    }
    else
    {
	for (; optind < argc; optind++)
	{
	    int pid = atoi(argv[optind]);
	    ostrstream diag;
	    diag << "gcore: " << pid;
	    string msg(diag);

	    // 1. Attach to the process, using the ptrace() call.
	    int ok = ptrace(PTRACE_ATTACH, pid, 0, 0);
	    if (ok < 0)
	    {
		perror(msg);
		errors++;
		continue;
	    }

	    // 2. Get a core file from the running process
	    ostrstream target;
	    target << filename << "." << pid;
	    string gcore_target(target);
	    ok = ptrace(PTRACE_DUMPCORE, pid, 
			int(gcore_target.chars()), 0);
	    if (ok < 0)
	    {
		perror(msg);
		errors++;
		continue;
	    }

	    // 3. Detach from the debuggee, leaving it stopped
	    if (stop)
		kill(pid, SIGSTOP);
	    ok = ptrace(PTRACE_DETACH, pid, 0x1, (stop ? SIGSTOP : 0));

	    if (ok < 0)
	    {
		perror(msg);
		errors++;
		continue;
	    }

	    std::cerr << "gcore: " << gcore_target << " dumped\n";
	}
    }

    exit(errors ? EXIT_FAILURE : EXIT_SUCCESS);
}
