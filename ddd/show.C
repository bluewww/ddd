// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char show_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "show.h"

#include "AppData.h"
#include "LiterateA.h"
#include "configinfo.h"
#include "cook.h"
#include "ddd.h"
#include "host.h"
#include "shell.h"
#include "status.h"
#include "version.h"
#include "filetype.h"
#include "HelpCB.h"

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>


//-----------------------------------------------------------------------------
// Show version
//-----------------------------------------------------------------------------

void show_version()
{
    cout << DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
	"Copyright (C) 1995, 1996 TU Braunschweig, Germany.\n";
}

//-----------------------------------------------------------------------------
// Show invocation
//-----------------------------------------------------------------------------

void show_invocation(DebuggerType type)
{
    string gdb_version = "";
    string options     = "";
    string base        = "";

    string gdb_help = sh_command(string(app_data.debugger_command) 
				 + " -h");
    switch (type)
    {
    case GDB:
    {
	base = "GDB, the GNU debugger.";

	Agent help(gdb_help);
	help.start();

	FILE *fp = help.inputfp();
	if (fp)
	{
	    enum { Init, Options, Other, Done } state = Init;
	    char buf[BUFSIZ];

	    while (fgets(buf, sizeof(buf), fp))
	    {
		if (buf[0] && buf[strlen(buf) - 1] == '\n')
		    buf[strlen(buf) - 1] = '\0';

		string option;
		switch (state)
		{
		case Init:
		    gdb_version = string(buf) + "\n";
		    state = Other;
		    break;

		case Other:
		    if (string(buf).contains("Options:"))
			state = Options;
		    break;

		case Options:
		    option = buf;
		    if (option == "")
			state = Done;
		    else
			options += option + "\n";
		    break;

		case Done:
		    break;
		}
	    }
	}
	break;

    }
    case DBX:
	base = "DBX, the UNIX debugger.";
	options = "  [DBX options]      Pass option to DBX.\n";
	break;

    case XDB:
	base = "XDB, the HP-UX debugger.";
	options = "  [XDB options]      Pass option to XDB.\n";
	break;
    }

    show_version();
    cout << gdb_version <<
	"This is DDD, the data display debugger, based on "
	<< base << "\n" << 
	"Usage:\n"
	"    " ddd_NAME " [options]"
	" executable-file [core-file or process-id]]\n"
	"Options:\n"
	<< options <<
	"  --dbx              Invoke DBX as inferior debugger.\n"
	"  --gdb              Invoke GDB as inferior debugger.\n"
	"  --xdb              Invoke XDB as inferior debugger.\n"
	"  --debugger NAME    Invoke inferior debugger as NAME.\n"
	"  --host HOST        Run inferior debugger on HOST.\n"
	"  --login LOGIN      Use LOGIN for connecting to host.\n"
	"  --vsl-library LIB  Load VSL library LIB.\n"
	"  --vsl-path PATH    Look for VSL libraries in PATH.\n"
	"  --trace-dialog     Show inferior debugger I/O on standard error.\n"
	"  --trace-shell      Show shell commands on standard error.\n"
	"  --exec-window      Create a window for"
	" running debugged programs.\n"
	"  --no-exec-window   Do not create a window for"
	" running debugged programs.\n"
	"  --attach-windows   Attach data and source windows to"
	" command window.\n"
	"  --separate-windows Do not attach data and source windows to"
	" command window.\n"
	"  --scrolled-graph   Use Motif scrollbars"
	" for moving the data display.\n"
	"  --panned-graph     Use an Athena panner"
	" for moving the data display.\n"
        "  --tty              Use the tty as additional debugger interface.\n"
	"  --version          Show the DDD version and exit.\n"
	"  --configuration    Show the DDD configuration flags and exit.\n"
	"  --manual           Show the DDD manual and exit.\n"
	"  --vsl-help         Show VSL options and exit.\n"
	"\n"
	"Standard X options are also accepted, such as:\n"
	"  -display DISPLAY    Run on X server DISPLAY.\n"
	"  -geometry GEOMETRY  Specify initial size and location.\n"
	"  -foreground COLOR   Use COLOR as foreground color.\n"
	"  -background COLOR   Use COLOR as background color.\n"
	"  -xrm RESOURCE       Specify a resource name and value.\n"
	"\n"
	"For more information, consult the " DDD_NAME " `Help' menu,"
	" type `help' from\n"
	"within " DDD_NAME ", "
	"or consult the manual pages of " DDD_NAME " and the"
	" inferior debugger.\n";
}


//-----------------------------------------------------------------------------
// Show Configuration
//-----------------------------------------------------------------------------

#define _stringize(x) #x
#define stringize(x) _stringize(x)

void show_configuration()
{    
    show_version();
    cout << 
	"Using X" stringize(X_PROTOCOL) "R" stringize(XlibSpecificationRelease)
	 ", Xt" stringize(X_PROTOCOL) "R" stringize(XtSpecificationRelease)
	 ", Motif " stringize(XmVERSION) "." stringize(XmREVISION)
#ifdef XmVERSION_STRING
	 " (" XmVERSION_STRING ")"
#endif
	"\n";
    cout << config_info;
}


//-----------------------------------------------------------------------------
// Show Resources
//-----------------------------------------------------------------------------

void show_resources(XrmDatabase db)
{
    string tmpfile = tmpnam(0);
    XrmPutFileDatabase(db, tmpfile);
    
    {
	ifstream is(tmpfile);
	int c;
	while ((c = is.get()) != EOF)
	    cout.put(char(c));
    }

    unlink(tmpfile);
}


//-----------------------------------------------------------------------------
// Show Manual Page
//-----------------------------------------------------------------------------

#define HUFFTEXT "ddd.man.huff.C"
#include "huffdecode.C"

void ddd_man(ostream& os)
{
    huffdecode(os);
}

void show_manual()
{
    FILE *pager = 0;
    if (isatty(fileno(stdout)))
    {
	// Try, in that order:
	// 1. The pager specified in the $PAGER environment variable
	// 2. less
	// 3. more
	// 4. cat  (I wonder if this can ever happen)
	string cmd = "less || more || cat";

	char *env_pager = getenv("PAGER");
	if (env_pager != 0)
	    cmd = string(env_pager) + " || " + cmd;
	cmd = "( " + cmd + " )";
	pager = popen(cmd, "w");
    }

    if (pager == 0)
    {
	ddd_man(cout);
	cout << flush;
    }
    else
    {
	ostrstream man;
	ddd_man(man);
	string s(man);

	fputs((char *)s, pager);
	pclose(pager);
    }
}

//-----------------------------------------------------------------------------
// WWW Page
//-----------------------------------------------------------------------------

void DDDWWWPageCB(Widget, XtPointer, XtPointer)
{
    string url = app_data.www_page;
    string cmd = app_data.www_command;

    StatusDelay delay("Invoking WWW browser for " + quote(url));

    cmd.gsub("@URL@", url);
    cmd += " &";
    cmd = sh_command(cmd, true);
    system(cmd);
}

//-----------------------------------------------------------------------------
// Manual
//-----------------------------------------------------------------------------

void DDDManualCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusDelay delay("Formatting manual page");

    ostrstream man;
    ddd_man(man);
    string s(man);
    ManualStringHelpCB(w, (char *)s, call_data);
}
