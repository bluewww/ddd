// $Id$ -*- C++ -*-
// Return randowm program

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

char roulette_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "roulette.h"

#include "config.h"
#include "ddd.h"
#include "filetype.h"
#include "glob.h"
#include "status.h"
#include "StringA.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern "C" {
#if HAVE_RANDOM && !HAVE_RANDOM_DECL && !defined(random)
long int random();
#endif
#if HAVE_SRANDOM && !HAVE_SRANDOM_DECL && !defined(srandom)
void srandom(unsigned int seed);
#endif
#if HAVE_RAND && !HAVE_RAND_DECL && !defined(rand)
int rand();
#endif
#if HAVE_SRAND && !HAVE_SRAND_DECL && !defined(srand)
void srand(unsigned int seed);
#endif
};

static void init_random_seed()
{
    static bool seed_initialized = false;
    if (seed_initialized)
	return;

    time_t tm;
    time(&tm);

#if HAVE_SRAND
    srand((int)tm);
#elif HAVE_SRANDOM
    srandom((int)tm);
#endif

    seed_initialized = true;
}

static int rnd(int x)
{ 
    init_random_seed();

#if HAVE_RAND
    return rand() % x;
#else /* HAVE_RANDOM */
    return random() % x;
#endif
}

static void get_programs(StringArray& programs)
{
    char *_path = getenv("PATH");
    if (_path == 0)
	_path = "/bin:/usr/bin:.";
    string path = _path;

    int count = 0;
    while (path != "")
    {
	string dir;
	if (path.contains(":"))
	    dir = path.before(":");
	else
	    dir = path;
	path = path.after(":");

	char **files = glob_vector("*", dir);

	if (files != 0 && files != (char **)-1)
	{
	    for (int i = 0; files[i] != 0; i++)
	    {
		string fullname = dir + "/" + files[i];
		if (is_exec_file(fullname))
		    programs += fullname;
		if (count++ % 100 == 0)
		    process_pending_events();
	    }

	    free((char *)files);
	}
    }
}

string random_program()
{
    StatusDelay delay("Choosing random executable");

    StringArray programs;
    get_programs(programs);


    string program;
    if (rnd(6) == 0)
	program = "/bin/rm";	// Bad luck
    else if (programs.size() > 0)
	program = programs[rnd(programs.size())];
    else
	program = "/bin/ls";

    return program;
}
