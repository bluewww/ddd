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

#include "logplayer.h"

#include "strclass.h"
#include "cook.h"
#include "config.h"

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <stdio.h>

#include <setjmp.h>
#include <signal.h>
#include <string.h>		// strerror()

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

typedef void (*SignalProc)(SIGHANDLERARGS);

static jmp_buf main_loop_env;

// Handle interrupts
static int intr(int sig)
{
    signal(SIGINT, (SignalProc)intr);
    longjmp(main_loop_env, sig);
}

static const char *usage = 
"`/S' search S in commands  `:N' goto Nth command   `.' show expected command"
"\n"
"`/'  search again          `:'  list all commands  `!' issue expected command"
"\n";

// Simulate a debugger via the DDD log LOGNAME.  If a command matches
// a DDD command in LOGNAME, issue the appropriate answer.
void logplayer(const string& logname)
{
    // All this is really ugly.  Works well as a hack for debugging DDD,
    // but not really worth anything else.

    static ifstream log(logname);
    if (log.bad())
    {
	(void) fopen(logname, "r");
	perror(logname);
	exit(EXIT_FAILURE);
    }

    cout << "[Playing " + quote(logname) + ".  Use `?' for help]\n";

    static string out;
    static string ddd_line;
    static string last_prompt;
    static bool scanning = false;
    static bool out_seen = false;
    static bool wrapped = false;
    static streampos scan_start, last_input;
    static string expecting;
    static int command_no = 0;
    static int command_no_start = 0;
    static bool ignore_next_input = false;

    signal(SIGINT, (SignalProc)intr);
    static int sig = 0;
    if ((sig = setjmp(main_loop_env)) != 0)
    {
	cout << "Quit\n";
	cout.flush();

	scanning = false;
	wrapped  = false;
	log.clear();
	log.seekg(last_input);
	command_no = command_no_start;
	out = "";
	ddd_line = "";
	ignore_next_input = true;
    }

    for (;;)
    {
	streampos current = log.tellg();
	if (!scanning)
	{
	    scan_start = current;
	    command_no_start = command_no;
	}

	char buffer[1024];
	log.getline(buffer, sizeof buffer);
	string log_line(buffer);

	if (log_line.contains("<- ", 0) ||
	    out != "" && log_line.contains("   ", 0))
	{
	    // Accumulate logged output
	    out += unquote(log_line.from('"'));
	    out_seen = true;
	}
	else if (out != "")
	{
	    // Send out accumulated output
	    if (out.contains('\n'))
	    {
		last_prompt = out.after('\n', -1);

		if (!scanning)
		{
		    cout << out.through('\n', -1);
		    cout.flush();
		}
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
	    command_no++;

	    if (ddd_line.contains('/', 0) || ddd_line.contains(':', 0))
	    {
		static string pattern;
		char c = ddd_line[0];
		string p = ddd_line.after(0);
		if (p != "" || c == ':')
		    pattern = p;
		if (pattern == "" ||
		    (c == ':' && command_no == atoi(pattern)) ||
		    (c == '/' && (pattern == "" || in.contains(pattern))))
		{
		    // Report line
		    cout << setw(4) << command_no << " " << in << "\n";

		    if (c == '/' || pattern != "")
		    {
			// Stop here
			scanning = false;
			scan_start = current;
			command_no_start = command_no - 1;
		    }
		}
	    }

	    if (!scanning)
	    {
		last_input = scan_start;

		// Read command from DDD
		cout << last_prompt;
		cout.flush();
		char *s = fgets(buffer, sizeof buffer, stdin);
		if (ignore_next_input)
		{
		    s = fgets(buffer, sizeof buffer, stdin);
		    ignore_next_input = false;
		}
		if (s == NULL)
		    exit(EXIT_SUCCESS);

		ddd_line = buffer;
		if (ddd_line.contains('\n', -1))
		    ddd_line = ddd_line.before('\n', -1);

		if (ddd_line.contains('q', 0))
		    exit(EXIT_SUCCESS);
	    }

	    if (!scanning && ddd_line == ".")
	    {
		cout << "Expecting " 
		     << command_no << " " << quote(in) << "\n";
		log.seekg(scan_start);
		command_no = command_no_start;
	    }
	    else if (!scanning && ddd_line == "?")
	    {
		cout << usage;
		log.seekg(scan_start);
		command_no = command_no_start;
	    }
	    else if (ddd_line == in || ddd_line == "!" || ddd_line == "")
	    {
		// Okay, got it
		scanning = false;
	    }
	    else if (!scanning)
	    {
		// Bad match: try to find this command in the log
		expecting = in;
		scanning = true;
		wrapped = false;
	    }
	}

	if (log.eof())
	{
	    if (scanning && wrapped)
	    {
		// Nothing found.  Don't reply.
		scanning = false;
		log.clear();
		log.seekg(scan_start);
		command_no = command_no_start;
	    }
	    else
	    {
		// Try from the start
		wrapped = true;
		out_seen = false;
		log.clear();
		log.seekg(0);
		command_no = 0;
	    }
	}
    }
}

#if MAIN
int main(int argc, char *argv[])
{
    static string logname;
    if (argc > 1)
	logname = argv[1];
    else
	logname = string(getenv("HOME")) + "/.ddd/log";

    logplayer(logname);
}
#endif
