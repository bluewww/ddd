// $Id$ -*- C++ -*-
// Debugger replacement, playing .log files

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

char logplayer_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "strclass.h"
#include "cook.h"
#include "config.h"

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>

#include <setjmp.h>
#include <signal.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

typedef void (*SignalProc)(SIGHANDLERARGS);

static jmp_buf main_loop_env;

int intr(int sig)
{
    signal(SIGINT, (SignalProc)intr);
    longjmp(main_loop_env, sig);
}

int main(int argc, char *argv[])
{
    static string logname = (argc > 1 ? argv[1] : "log");
    static ifstream log(logname);
    if (log.bad())
    {
	cerr << argv[0] << ": cannot access log\n";
	return EXIT_FAILURE;
    }

    static string out;
    static string ddd_line;
    static string last_prompt;
    static bool scanning = false;
    static bool out_seen = false;
    static bool wrapped = false;
    static streampos scan_start;
    static string expecting;
    static int times = 0;

    signal(SIGINT, (SignalProc)intr);
    static int sig = 0;
    if ((sig = setjmp(main_loop_env)) != 0)
    {
	cout << "Quit\n" << last_prompt;
	cout.flush();

	scanning = false;
	wrapped  = false;
	log.clear();
	log.seekg(scan_start);
	out = "";
	ddd_line = "";
    }

    for (;;)
    {
	if (!scanning)
	    scan_start = log.tellg();

	char buffer[1024];
	log.getline(buffer, sizeof buffer);
	string log_line(buffer);

	if (log_line.contains("<- ", 0) ||
	    out != "" && log_line.contains("   ", 0))
	{
	    out += unquote(log_line.from('"'));
	    out_seen = true;
	}
	else if (out != "")
	{
	    if (!scanning)
	    {
		cout << out;
		cout.flush();
	    }
	    if (out.contains('\n'))
	    {
		last_prompt = out.after('\n', -1);
	    }
	    else
	    {
		last_prompt = out;
	    }
	    out = "";
	}

	if (out_seen && log_line.contains("-> ", 0))
	{
	    string in = unquote(log_line.from('"'));
	    if (in.contains('\n', -1))
		in = in.before('\n', -1);

	    if (!scanning)
	    {
		cin.getline(buffer, sizeof buffer);
		ddd_line = buffer;

		if (cin.fail() || cin.eof() || ddd_line.contains('q', 0))
		    return EXIT_SUCCESS;

		cin.clear();
	    }

	    if (ddd_line == in || ddd_line == ".")
	    {
		// Okay, got it
		scanning = false;
		times = 0;
	    }
	    else if (!scanning)
	    {
		// Bad match: try to find this line in the log
		expecting = in;
		scanning = true;
		wrapped = false;
	    }
	}

	if (log.eof())
	{
	    if (scanning && wrapped)
	    {
		switch (times++)
		{
		case 0:
		    cerr << "Try " << quote(expecting);
		    break;

		case 1:
		    cerr << "Use \".\" to enter the expected command";
		    break;

		default:
		    cerr << "Use \"q\" to quit";
		    break;
		}

		cerr << "\n" <<last_prompt;
		scanning = false;
		log.clear();
		log.seekg(scan_start);
	    }
	    else
	    {
		// Try from the start
		wrapped = true;
		out_seen = false;
		log.clear();
		log.seekg(0);
	    }
	}
    }

    return EXIT_SUCCESS;
}
