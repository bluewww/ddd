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

#include "assert.h"
#include "bool.h"
#include "strclass.h"
#include "cook.h"
#include "config.h"

#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <setjmp.h>
#include <signal.h>
#include <string.h>		// strerror()

// Nico van Waes <nico@yegal.njit.edu> says: under Solaris 2.6, one
// must include <sys/types.h> before <termios.h>.
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_TERMIOS_H
#include <termios.h>
#endif

extern "C" {
#if HAVE_TCDRAIN && !HAVE_TCDRAIN_DECL && !defined(tcdrain)
    int tcdrain(int filedes);
#endif
}

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FAILNO 1
#endif

typedef void (*SignalProc)(SIGHANDLERARGS);

static jmp_buf main_loop_env;

// Handle interrupts
static int intr(int sig)
{
    signal(SIGINT, (SignalProc)intr);
    longjmp(main_loop_env, sig);

    return 0;			// Never reached
}

static const char *usage = 
"`/S' search S in commands  `:N' goto Nth command   `.' show expected command"
"\n"
"`/'  search again          `:'  list all commands  `!' issue expected command"
"\n";

static void put(const string& s)
{
    write(STDOUT_FILENO, s.chars(), s.length());

#if HAVE_TCDRAIN || defined(tcdrain)
    if (isatty(STDOUT_FILENO))
    {
	tcdrain(STDOUT_FILENO);
    }
#endif
}

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

    put("[Playing " + quote(logname) + ".  Use `?' for help]\n");

    static string out;
    static string last_out;
    static string ddd_line;
    static string last_prompt;
    static bool initializing = true;
    static bool scanning = false;
    static bool out_seen = false;
    static bool wrapped = false;
    static bool echoing = false;
    static streampos scan_start, last_input;
    static string expecting;
    static int command_no = 0;
    static int command_no_start = 0;
    static bool ignore_next_input = false;

    signal(SIGINT, (SignalProc)intr);
    static int sig = 0;
    if ((sig = setjmp(main_loop_env)) != 0)
    {
	put("Quit\n");

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

	// Read line from log
	char buffer[1024];
	log.getline(buffer, sizeof buffer);
	string log_line(buffer);

	if (out_seen && log_line.contains("   ", 0))
	{
	    // Continuation line
	    out += unquote(log_line.from('"'));
	}
	else if (out != "")
	{
	    // Send out accumulated output
	    if (!scanning)
	    {
		if (out.contains(ddd_line, 0))
		    echoing = true;
		put(out);
	    }

	    last_out += out;
	    out = "";
	}

	if (log_line.contains("<- ", 0))
	{
	    assert(out == "");

	    // Output line
	    out = unquote(log_line.from('"'));
	    out_seen = true;
	}

	if (out_seen && log_line.contains("-> ", 0))
	{
	    // Handle input
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
		    ostrstream os;
		    os << setw(4) << command_no << " " << in << "\n";
		    put(os);

		    if (c == '/' || pattern != "")
		    {
			// Stop here
			scanning = false;
			scan_start = current;
			command_no_start = command_no - 1;
			last_out = "";
		    }
		}
	    }

	    if (!scanning)
	    {
		last_input = scan_start;
		initializing = false;

		// Read command from DDD
		if (last_out.contains('\n'))
		{
		    string prompt = last_out.after('\n', -1);
		    if (prompt != "")
			last_prompt = prompt;
		}

		if (!last_out.contains(last_prompt, -1))
		    put(last_prompt);

		last_out = "";

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

		if (echoing && ddd_line != "" && !isalpha(ddd_line[0]))
		    put(ddd_line + "\r\n");

		if (ddd_line.contains('q', 0))
		    exit(EXIT_SUCCESS);

		if (ddd_line.contains("list 1,", 0))
		{
		    // Send the log file instead of a source
		    ifstream is(logname);
		    int line = 1;
		    bool at_start_of_line = true;

		    ostrstream os;
		    for (;;)
		    {
			char c;
			is.get(c);
			if (is.eof())
			    break;

			if (at_start_of_line)
			{
			    os << line << '\t';
			    at_start_of_line = false;
			}

			os << c;

			if (c == '\n')
			{
			    line++;
			    at_start_of_line = true;
			}
		    }

		    put(string(os));
		}
	    }

	    if (!scanning && ddd_line == ".")
	    {
		ostrstream os;
		os << "Expecting " 
		   << command_no << " " << quote(in) << "\n";
		put(os);
		log.seekg(scan_start);
		command_no = command_no_start;
	    }
	    else if (!scanning && ddd_line == "?")
	    {
		put(usage);
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
		if (echoing && (ddd_line == "" || isalpha(ddd_line[0])))
		    put(ddd_line + "\r\n");

		scanning = false;
		log.clear();
		log.seekg(scan_start);
		command_no = command_no_start;
	    }
	    else if (initializing)
	    {
		// No prompt found
		cerr << logname << ": invalid or incomplete log\n";
		exit(EXIT_FAILURE);
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
