// $Id$ 
// TTYAgent test program

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

char ttytest_rcsid[] = 
    "$Id$";

#include "TTYAgent.h"
#include "cook.h"

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
    exit(0);
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
}
