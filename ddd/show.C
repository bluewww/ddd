// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char show_rcsid[] = 
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
#include "post.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"
#include "version.h"
#include "filetype.h"

#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <string.h>
#include <errno.h>

#if defined(HAVE_XPM) && defined(HAVE_X11_XPM_H)
#include <X11/xpm.h>
#endif

#include "HelpCB.h"

extern void process_pending_events();

//-----------------------------------------------------------------------------
// Show version
//-----------------------------------------------------------------------------

void show_version()
{
    cout << DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
	"Copyright (C) 1997 TU Braunschweig.\n";
}

//-----------------------------------------------------------------------------
// Show invocation
//-----------------------------------------------------------------------------

void show_invocation(DebuggerType type)
{
    string gdb_version = "";
    string options     = "";
    string title       = "";
    string base        = "";

    string gdb_get_help = 
	sh_command(string(app_data.debugger_command) + " -h");
    string gdb_get_version = 
	sh_command(string(app_data.debugger_command) + " -v");

    switch (type)
    {
    case GDB:
	{
	    title = "GDB";
	    base  = "GDB, the GNU debugger.";

	    Agent help(gdb_get_help);
	    help.start();

	    FILE *fp = help.inputfp();
	    if (fp)
	    {
		enum { Init, Options, Other, Done } state = Init;
		char buf[BUFSIZ];

		while (fgets(buf, sizeof(buf), fp) && state != Done)
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
	{
	    title = "DBX";
	    base  = "DBX, the UNIX debugger.";
	    options = "  [DBX options]      Pass option to DBX.\n";
	}
	break;

    case XDB:
	{
	    title = "XDB";
	    base  = "XDB, the HP-UX debugger.";

	    Agent version(gdb_get_version);
	    version.start();

	    FILE *fp = version.inputfp();
	    if (fp)
	    {
		char buf[BUFSIZ];
		while (fgets(buf, sizeof(buf), fp))
		{
		    if (buf[0] && buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';
		    gdb_version = string(buf) + "\n";
		}
	    }

	    Agent help(gdb_get_help);
	    help.start();

	    fp = help.errorfp();
	    if (fp)
	    {
		enum { Other, Options, Done } state = Other;
		char buf[BUFSIZ];

		while (fgets(buf, sizeof(buf), fp) && state != Done)
		{
		    if (buf[0] && buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		    string option;
		    switch (state)
		    {
		    case Other:
			if (string(buf).contains("Options:"))
			    state = Options;
			break;

		    case Options:
			option = buf;
			if (option == "")
			    state = Done;
			else
			    options += "  " + option.after(rxwhite) + "\n";
			break;

		    case Done:
			break;
		    }
		}
	    }
	}
	break;
    }

    show_version();
    cout << gdb_version <<
	"This is DDD, the data display debugger, based on "
	<< base << "\n" << 
	"Usage:\n"
	"    " ddd_NAME " [options...]"
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
	"  --trace-dialog     Show dialog with inferior debugger"
	" on standard error.\n"
	"  --trace-shell      Show shell commands on standard error.\n"
	"  --exec-window      Create a window for"
	" running debugged programs.\n"
	"  --no-exec-window   Do not create a window for"
	" running debugged programs.\n"
	"  --attach-windows   Attach data and source windows to"
	" debugger console.\n"
	"  --separate-windows Do not attach data and source windows to"
	" debugger console.\n"
	"  --scrolled-graph   Use Motif scrollbars"
	" for moving the data display.\n"
	"  --panned-graph     Use an Athena panner"
	" for moving the data display.\n"
        "  --tty              Use controlling tty"
	" as additional debugger console.\n"
	"  --version          Show the DDD version and exit.\n"
	"  --configuration    Show the DDD configuration flags and exit.\n"
	"  --manual           Show the DDD manual and exit.\n"
	"  --license          Show the DDD license and exit.\n"
	"  --vsl-help         Show VSL options and exit.\n"
	"\n"
	"Standard X options are also accepted, such as:\n"
	"  -display DISPLAY   Run on X server DISPLAY.\n"
	"  -geometry GEOMETRY Specify initial size and location.\n"
	"  -iconic            Start-up in iconic mode.\n"
	"  -foreground COLOR  Use COLOR as foreground color.\n"
	"  -background COLOR  Use COLOR as background color.\n"
	"  -xrm RESOURCE      Specify a resource name and value.\n"
	"\n"
	"For more information, consult the " DDD_NAME " `Help' menu,"
	" type `help' from\n"
	"within " DDD_NAME ", "
	"and see the " DDD_NAME " and " << title << " documentation.\n";
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
	"Compiled with "
#ifdef __GNUC__
	"GCC " stringize(__GNUC__)
#ifdef __GNUC_MINOR__
        "." stringize(__GNUC_MINOR__)
#endif
#else
	"CC"
#endif
#ifdef _G_LIB_VERSION
	", libg++ " _G_LIB_VERSION
#endif
#if defined(__GLIBC__)
	", GNU libc " stringize(__GLIBC__) "." stringize(__GLIBC_MINOR__)
#elif defined(_LINUX_C_LIB_VERSION)
        ", Linux libc " _LINUX_C_LIB_VERSION
#endif
	"\n"
	"Using X" stringize(X_PROTOCOL) "R" stringize(XlibSpecificationRelease)
	 ", Xt" stringize(X_PROTOCOL) "R" stringize(XtSpecificationRelease)
	 ", Motif " stringize(XmVERSION) "." stringize(XmREVISION)
#ifdef XmVERSION_STRING
	 " (" XmVERSION_STRING ")"
#endif
#ifdef XpmFormat
         ", XPM " stringize(XpmFormat) "." stringize(XpmVersion) 
	"." stringize(XpmRevision)
#endif
	"\n";
    cout << config_info;
}


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

static int uncompress(ostream& os, const char *text, int size)
{
    string tempfile = tmpnam(0);
    FILE *fp = fopen(tempfile, "w");
    if (fp == 0)
    {
	os << tempfile << ": " << strerror(errno);
	return -1;
    }

    int i;
    for (i = 0; i < size; i++)
    {
	if (i % 100 == 0)
	    process_pending_events();
	putc(text[i], fp);
    }
    fclose(fp);

    string cmd = string(app_data.uncompress_command) + " < " + tempfile;

    fp = popen(sh_command(cmd, true) + " 2>&1", "r");
    if (fp == 0)
    {
	os << app_data.uncompress_command << ": " << strerror(errno);
	return -1;
    }

    int c;
    i = 0;
    while ((c = getc(fp)) != EOF)
    {
	if (i % 100 == 0)
	    process_pending_events();
	os << (char)c;
	i++;
    }
    pclose(fp);

    unlink(tempfile);
    return 0;
}

static void show(int (*formatter)(ostream& os))
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
	pager = popen(sh_command(cmd), "w");
    }

    if (pager == 0)
    {
	formatter(cout);
	cout << flush;
    }
    else
    {
	ostrstream text;
	formatter(text);
	string s(text);

	fputs(s.chars(), pager);
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
// License
//-----------------------------------------------------------------------------

int ddd_license(ostream& os)
{
    static const char COPYING[] =
#include "COPYING.gz.C"
	;

    return uncompress(os, COPYING, sizeof(COPYING) - 1);
}

void show_license()
{
    show(ddd_license);
}


//-----------------------------------------------------------------------------
// Show License
//-----------------------------------------------------------------------------

void DDDLicenseCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusMsg msg("Invoking " DDD_NAME " license browser");

    ostrstream license;
    int ret = ddd_license(license);
    string s(license);
    TextHelpCB(w, XtPointer((char *)s), call_data);

    if (ret != 0 || !s.contains("GNU"))
	post_error("The license could not be uncompressed.", 
		   "no_license_error", w);
}


//-----------------------------------------------------------------------------
// Manual
//-----------------------------------------------------------------------------

int ddd_man(ostream& os)
{
    static const char MANUAL[] =
#include "ddd.man.txt.gz.C"
	;

    return uncompress(os, MANUAL, sizeof(MANUAL) - 1);
}

void show_manual()
{
    show(ddd_man);
}


//-----------------------------------------------------------------------------
// Show Manual Page
//-----------------------------------------------------------------------------

void DDDManualCB(Widget w, XtPointer, XtPointer)
{
    StatusMsg msg("Invoking " DDD_NAME " manual browser");
    
    ostrstream man;
    int ret = ddd_man(man);
    string s(man);

    MString title(DDD_NAME " Manual");
    ManualStringHelpCB(w, title, s);

    if (ret != 0 || !s.contains(DDD_NAME))
	post_error("The manual could not be uncompressed.", 
		   "no_ddd_manual_error", w);
}

void GDBManualCB(Widget w, XtPointer, XtPointer)
{
    StatusMsg msg("Invoking " + gdb->title() + " manual browser");

    string cmd = "man " + downcase(gdb->title());

    if (gdb->type() == GDB)
    {
	// Try `info' first
	cmd.prepend("info --subnodes -o - -f " 
		    + downcase(gdb->title()) + " 2> /dev/null || ");
    }

    FILE *fp = popen(sh_command(cmd), "r");
    if (fp != 0)
    {
	ostrstream man;
	int c;
	int i = 0;
	while ((c = getc(fp)) != EOF)
	{
	    if (i % 100 == 0)
		process_pending_events();
	    man << char(c);
	    i++;
	}
	
	string s(man);
	bool info = s.contains("File: ", 0);

	MString title(gdb->title() + (info ? " Info" : " Manual"));
	ManualStringHelpCB(w, title, s);

	pclose(fp);
    }
}
