// $Id$ -*- C++ -*-
// Debugger replacement, playing .log files

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
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

char logplayer_rcsid[] = 
    "$Id$";

#include "logplayer.h"

#include "assert.h"
#include "bool.h"
#include "strclass.h"
#include "cook.h"
#include "config.h"
#include "streampos.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>

// Nico van Waes <nico@yegal.njit.edu> says: under Solaris 2.6, one
// must include <sys/types.h> and <sys/int_types.h> before
// <termios.h>.
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if HAVE_SYS_INT_TYPES_H
#include <sys/int_types.h>
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

extern "C" {
typedef void (*SignalProc)(SIGHANDLERARGS);
}

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

static string last_output;

static void put(const string& s)
{
    write(STDOUT_FILENO, s.chars(), s.length());
    last_output += s;

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

    static std::ifstream log(logname.chars());
    if (log.bad())
    {
	(void) fopen(logname.chars(), "r");
	perror(logname.chars());
	exit(EXIT_FAILURE);
    }

    put("[Playing " + quote(logname) + ".  Use `?' for help]\n");

    static string out;
    static string ddd_line;
    static string last_prompt;
    static bool initializing = true;
    static bool scanning = false;
    static bool out_seen = false;
    static bool wrapped = false;
    static bool echoing = false;
    static STREAMPOS scan_start, last_input;
    static string expecting;
    static int command_no = 0;
    static int command_no_start = 0;
    static bool ignore_next_input = false;

    signal(SIGINT, (SignalProc)intr);
    if (setjmp(main_loop_env) != 0)
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
	STREAMPOS current = log.tellg();
	if (!scanning)
	{
	    scan_start = current;
	    command_no_start = command_no;
	}

	// Read line from log
	char buffer[65536];
	log.getline(buffer, sizeof buffer);
	if (!log.fail())
	{
	    string log_line(buffer);

	    if (out_seen && log_line.contains("   ", 0))
	    {
		// Continuation line
		out += unquote(log_line.from('"'));
	    }
	    else if (!out.empty())
	    {
		// Send out accumulated output
		if (!scanning)
		{
		    if (out.contains(ddd_line, 0))
			echoing = true;
		    put(out);
		}
		out = "";
	    }

	    if (log_line.contains("<- ", 0))
	    {
		assert(out.empty());

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
		    if (!p.empty() || c == ':')
			pattern = p;
		    if (pattern.empty() ||
			(c == ':' && command_no == atoi(pattern.chars())) ||
			(c == '/' && (pattern.empty() || in.contains(pattern))))
		    {
			// Report line
			char buffer[256];
			sprintf(buffer, "%4d", command_no);
			std::ostringstream os;
			os << buffer << " " << in << "\n";
			put(os);

			if (c == '/' || !pattern.empty())
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
		    initializing = false;

		    // Read command from DDD
		    if (last_output.contains('\n'))
		    {
			string prompt = last_output.after('\n', -1);
			if (!prompt.empty() && !prompt.contains('\\', 0))
			{
			    if (prompt.contains('('))
				prompt = prompt.from('(', -1);

			    last_prompt = prompt;
			}
		    }

		    if (!last_output.contains(last_prompt, -1))
			put(last_prompt);

		    last_output = "";

		    char *s = fgets(buffer, sizeof buffer, stdin);
		    if (ignore_next_input)
		    {
			s = fgets(buffer, sizeof buffer, stdin);
			ignore_next_input = false;
		    }
		    if (s == 0)
			exit(EXIT_SUCCESS);

		    ddd_line = buffer;
		    if (ddd_line.contains('\n', -1))
			ddd_line = ddd_line.before('\n', -1);

		    if (echoing && !ddd_line.empty() && !isalpha(ddd_line[0]))
			put(ddd_line + "\r\n");

		    if (ddd_line.contains('q', 0))
			exit(EXIT_SUCCESS);

		    if ((ddd_line.contains("list ", 0) || 
			 ddd_line.contains("l ", 0)) && 
			(ddd_line.contains(" 1,") || 
			 ddd_line.contains(":1,") || 
			 ddd_line.contains(" 1-")))
		    {
			// Send the log file instead of a source
			if (echoing)
			    put(ddd_line + "\r\n");

			std::ifstream is(logname.chars());
			int line = 1;
			bool at_start_of_line = true;

			std::ostringstream os;
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
			scanning = false;
		    }
		}

		if (!scanning && ddd_line == ".")
		{
		    std::ostringstream os;
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
		else if (ddd_line == in || ddd_line == "!" || ddd_line.empty())
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
	}

	if (log.eof() || log.fail())
	{
	    if (scanning && wrapped)
	    {
		// Nothing found.  Don't reply.
		if (echoing && (ddd_line.empty() || isalpha(ddd_line[0])))
		    put(ddd_line + "\r\n");

		scanning = false;
		log.clear();
		log.seekg(scan_start);
		out = "";
		command_no = command_no_start;
	    }
	    else if (initializing)
	    {
		// No prompt found
		std::cerr << logname << ": invalid or incomplete log\n";
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
