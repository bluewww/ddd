// $Id$ 
// TTYAgent test program

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

char ttytest_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include "TTYAgent.h"
#include "cook.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

// Trace communication
static void trace(const string& prefix, void *call_data)
{
    DataLength* dl    = (DataLength *) call_data;
    string s(dl->data, dl->length);

    bool s_ends_with_nl = false;
    if (s.length() > 0 && s[s.length() - 1] == '\n')
    {
	s_ends_with_nl = true;
	s = s.before(int(s.length() - 1));
    }

    s = quote(s);
    string nl = string("\\n\"\n") + replicate(' ', prefix.length()) + "\"";
    s.gsub("\\n", nl);

    if (s_ends_with_nl)
	s(s.length() - 1, 0) = "\\n";

    cerr << prefix << s << '\n';
}
    
void traceInputHP(Agent *, void *, void *call_data)
{
    trace("<- ", call_data);
}

void traceOutputHP(Agent *, void *, void *call_data)
{
    trace("-> ", call_data);
}

void traceErrorHP (Agent *, void *, void *call_data)
{
    trace("<= ", call_data);
}

void sendInputHP(Agent *, void *client_data, void *call_data)
{
    TTYAgent& tty = *((TTYAgent *)client_data);
    DataLength* dl    = (DataLength *) call_data;

    tty.write(dl->data, dl->length);
}

void exitHP(Agent *, void *client_data, void *)
{
    cerr << "EOF\n";

    TTYAgent& tty = *((TTYAgent *)client_data);
    tty.shutdown();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    XtAppContext app_context;

    // Initialize it all
    Widget toplevel = 
	XtAppInitialize(&app_context, "TTYtest", 0, 0, &argc, argv, 0, 0, 0);

    string command;
    for (int i = 1; i < argc; i++)
    {
	if (command != "")
	    command += " ";
	command += argv[i];
    }

    if (command == "")
    {
	cerr << XtName(toplevel) 
	     << ": usage: " << XtName(toplevel) << " COMMAND [ARGS...]\n";
	return 1;
    }

    TTYAgent tty(app_context, command);
    tty.addHandler(Input,  traceInputHP);
    tty.addHandler(Output, traceOutputHP);
    tty.addHandler(Error,  traceErrorHP);

    LiterateAgent me(app_context);
    me.addHandler(Input, sendInputHP, &tty);
    me.addHandler(InputEOF, exitHP, &tty);

    tty.start();
    me.start();

    XtAppMainLoop(app_context);

    return EXIT_SUCCESS;
}
