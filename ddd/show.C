// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#include "Xpm.h"
#include "HelpCB.h"

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif

//-----------------------------------------------------------------------------
// Show version
//-----------------------------------------------------------------------------

void show_version(ostream& os)
{
    os << "@(#)" DDD_NAME " " DDD_VERSION " (" DDD_HOST "), "
	"Copyright (C) 1998 TU Braunschweig.\n" + 4;
}

//-----------------------------------------------------------------------------
// Show invocation
//-----------------------------------------------------------------------------

void show_invocation(DebuggerType type, ostream& os)
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

    case JDB:
	{
	    title = "JDB";
	    base  = "JDB, the Java debugger.";
	    options = "  [JDB options]      Pass option to JDB.\n";
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

    show_version(os);
    os << gdb_version <<
	"This is " DDD_NAME ", the data display debugger, based on "
	<< base << "\n" << 
	"Usage:\n"
	"    " ddd_NAME " [options...]"
	" executable-file [core-file or process-id]]\n"
	"Options:\n"
	<< options <<
	"  --dbx              Invoke DBX as inferior debugger.\n"
	"  --gdb              Invoke GDB as inferior debugger.\n"
	"  --xdb              Invoke XDB as inferior debugger.\n"
	"  --jdb              Invoke JDB as inferior debugger.\n"
	"  --debugger NAME    Invoke inferior debugger as NAME.\n"
	"  --host USER@HOST   Run inferior debugger on HOST.\n"
	"  --rhost USER@HOST  Like --host, but use a rlogin connection.\n"
	"  --vsl-library LIB  Load VSL library LIB.\n"
	"  --vsl-path PATH    Look for VSL libraries in PATH.\n"
	"  --font FONT        Use font FONT as default.\n"
	"  --fontsize SIZE    Use fonts sized SIZE 1/10 points.\n"
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
	"  --news             Show the DDD news and exit.\n"
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

void show_configuration(ostream& os)
{    
    string cinfo = string(config_info).before("\n");
    while (cinfo.contains(' ', -1))
	cinfo = cinfo.before(int(cinfo.length()) - 1);

    show_version(os);
    string sccs = "@(" + string("#)");

    string s;

    // Compilation stuff
    s = "@(#)Compiled with "
#ifdef __GNUC__
	"GCC " stringize(__GNUC__)
#ifdef __GNUC_MINOR__
        "." stringize(__GNUC_MINOR__)
#endif
#else
	"CC"
#endif
#ifdef _G_LIB_VERSION
	", libstdc++ " _G_LIB_VERSION
#endif
#if defined(__GLIBC__)
	", GNU libc " stringize(__GLIBC__) "." stringize(__GLIBC_MINOR__)
#elif defined(_LINUX_C_LIB_VERSION)
        ", Linux libc " _LINUX_C_LIB_VERSION
#endif
	"\n";
    s.gsub(sccs, string(""));
    os << s;

    // X stuff
    s = "@(#)Requires X" stringize(X_PROTOCOL) 
	"R" stringize(XlibSpecificationRelease)
	", Xt" stringize(X_PROTOCOL) "R" stringize(XtSpecificationRelease)
	", Motif " stringize(XmVERSION) "." stringize(XmREVISION)
#if XmUPDATE_LEVEL
	"." stringize(XmUPDATE_LEVEL)
#endif
#ifdef XmVERSION_STRING
        " (" XmVERSION_STRING ")"
#endif
        "\n";
    s.gsub(sccs, string(""));
    s.gsub("( ", "(");
    s.gsub(" )", ")");
    os << s;

#if HAVE_XMUSEVERSION
    if (xmUseVersion != XmVersion)
    {
	os << "(Note: Compiled using Motif " 
	   << XmVersion / 1000 << "." << XmVersion % 1000 
	   << ", but linked against Motif " 
	   << xmUseVersion / 1000 << "." << xmUseVersion % 1000 
	   << ")\n";
    }
#endif

    // Optional stuff
    s = "@(#)Includes " DDD_NAME " core"
#if WITH_BUILTIN_MANUAL
	", manual"
#endif
#if WITH_BUILTIN_APP_DEFAULTS
	", app-defaults"
#endif
#ifdef XpmFormat
	", XPM " stringize(XpmFormat) "." stringize(XpmVersion) 
	"." stringize(XpmRevision)
#endif
#if HAVE_ATHENA
	", Athena Panner"
#endif
	"\n";
    s.gsub(sccs, string(""));
    os << s << cinfo << "\n";
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

void show(int (*formatter)(ostream& os))
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

void DDDLicenseCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusMsg msg("Invoking " DDD_NAME " license browser");

    ostrstream license;
    int ret = ddd_license(license);
    string s(license);
    s.prepend("@license@");

    TextHelpCB(w, XtPointer((char *)s), call_data);

    if (ret != 0 || !s.contains("GNU"))
	post_error("The " DDD_NAME " license could not be uncompressed.", 
		   "no_license_error", w);
}

//-----------------------------------------------------------------------------
// News
//-----------------------------------------------------------------------------

int ddd_news(ostream& os)
{
    static const char NEWS[] =
#include "NEWS.gz.C"
	;

    return uncompress(os, NEWS, sizeof(NEWS) - 1);
}

void DDDNewsCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusMsg msg("Invoking " DDD_NAME " news browser");

    ostrstream news;
    int ret = ddd_news(news);
    string s(news);
    s.prepend("@news@");

    TextHelpCB(w, XtPointer((char *)s), call_data);

    if (ret != 0 || !s.contains(DDD_NAME))
	post_error("The " DDD_NAME " news could not be uncompressed.", 
		   "no_news_error", w);
}



//-----------------------------------------------------------------------------
// Manual
//-----------------------------------------------------------------------------

int ddd_man(ostream& os)
{
#if WITH_BUILTIN_MANUAL
    static const char MANUAL[] =
#include "ddd.man.txt.gz.C"
	;

    return uncompress(os, MANUAL, sizeof(MANUAL) - 1);
#else
    // Try `man ddd' and `man xddd'.
    FILE *fp = popen(sh_command("man " ddd_NAME " || man x" ddd_NAME), "r");
    if (fp == 0)
	return -1;

    int c;
    int i = 0;
    while ((c = getc(fp)) != EOF)
    {
	if (i % 100 == 0)
	    process_pending_events();
	os << char(c);
	i++;
    }
    pclose(fp);
    return 0;
#endif
}

void DDDManualCB(Widget w, XtPointer, XtPointer)
{
    StatusMsg msg("Invoking " DDD_NAME " manual browser");
    
    ostrstream man;
    int ret = ddd_man(man);
    string s(man);

    MString title(DDD_NAME " Reference");
    ManualStringHelpCB(w, title, s);

    if (ret != 0 || !s.contains(DDD_NAME))
    {
#if WITH_BUILTIN_MANUAL
	post_error("The " DDD_NAME " manual could not be uncompressed.", 
		   "no_ddd_manual_error", w);
#else
	post_error("The " DDD_NAME " manual could not be accessed.",
		   "no_ddd_man_page_error", w);
#endif
    }
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
