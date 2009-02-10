// $Id$ -*- C++ -*-
// DDD info functions

// Copyright (C) 1996-2000 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Universitaet des Saarlandes, Germany.
// Copyright (C) 2001-2004, 2005 Free Software Foundation, Inc.
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

char show_rcsid[] = 
    "$Id$";

#include "show.h"

#include "AppData.h"
#include "LiterateA.h"
#include "SmartC.h"
#include "build.h"
#include "config.h"
#include "configinfo.h"
#include "cook.h"
#include "ddd.h"
#include "gdbinit.h"
#include "host.h"
#include "post.h"
#include "regexps.h"
#include "resolveP.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "tempfile.h"
#include "version.h"
#include "filetype.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "Xpm.h"
#if HAVE_ATHENA
#include <X11/Xaw/XawInit.h>
#endif
#include "HelpCB.h"

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif

#if WITH_READLINE
extern "C" {
#include "readline/readline.h"
}
#endif

//-----------------------------------------------------------------------------
// Show invocation
//-----------------------------------------------------------------------------

void show_invocation(const string& gdb_command, std::ostream& os)
{
    string command     = gdb_command;
    string gdb_version = "";
    string title       = "";
    string base        = "";
    StringArray options;

    DebuggerType type;
    bool type_ok = get_debugger_type(command, type);
    if (!type_ok)
	type = GDB;
    if (command.empty())
	command = default_debugger(command, type);

    string gdb_get_help    = sh_command(command + " -h");
    //string gdb_get_version = sh_command(command + " -v");

    string args = "executable-file [core-file | process-id]";

    // Set up DDD options
    static const char *options_string[] = {
	"  --bash             Invoke Bash as inferior debugger.",
	"  --dbg              Invoke DBG as inferior debugger.",
	"  --dbx              Invoke DBX as inferior debugger.",
	"  --gdb              Invoke GDB as inferior debugger.",
	"  --ladebug          Invoke Ladebug as inferior debugger.",
	"  --jdb              Invoke JDB as inferior debugger.",
	"  --make             Invoke remake (GNU Make) inferior debugger.",
	"  --perl             Invoke Perl as inferior debugger.",
	"  --pydb             Invoke PYDB as inferior debugger.",
	"  --wdb              Invoke WDB as inferior debugger.",
	"  --xdb              Invoke XDB as inferior debugger.",
	"  --debugger CMD     Invoke inferior debugger as CMD.",
	"  --host USER@HOST   Run inferior debugger on HOST.",
	"  --rhost USER@HOST  Like --host, but use a rlogin connection.",
	"  --trace            Show interaction with inferior debugger"
	" on standard error.",
        "  --tty              Use controlling tty"
	" as additional debugger console.",
	"  --version          Show the DDD version and exit.",
	"  --configuration    Show the DDD configuration flags and exit.",
	"  --manual           Show the DDD manual and exit.",
	"  --license          Show the DDD license and exit.",
	"  --news             Show the DDD news and exit.",
	0
    };

    int i = 0;
    while (options_string[i] != 0)
    	options += options_string[i++];

    // Set up debugger-specific options
    switch (type)
    {
    case BASH:
    {
	title = "BASH";
	base  = "the BASH debugger.";
	options += "  [bash options]     Pass option to Bash.";
	args = "program-file [args]";
    }
    break;

    case DBG:
    {
	title = "DBG";
	base  = "DBG, the PHP debugger.";
	options += "  [PHP options]      Pass option to DBG.";
    }
    break;

    case DBX:
    {
	title = "DBX";
	base  = "DBX, the UNIX debugger.";
	options += "  [DBX options]      Pass option to DBX.";
    }
    break;

    case GDB:
    {
	title = "GDB";
	base  = "GDB, the GNU debugger.";

	Agent help(gdb_get_help);
	help.start();

	FILE *fp = help.inputfp();
	if (fp)
	{
	    enum { Options, Other, Done } state = Other;
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
		    if (option.contains("For more information"))
			state = Done;
		    else if (!option.empty())
			options += option;
		    break;

		case Done:
		    break;
		}
	    }
	}
	break;
    }

    case JDB:
    {
	title = "JDB";
	base  = "JDB, the Java debugger.";
	options += "  [JDB options]      Pass option to JDB.";
	args = "[class]";
    }
    break;

    case MAKE:
    {
	title = "MAKE";
	base  = "the GNU Make debugger.";
	options += "  [make options]     Pass option to GNU Make.";
	args = "program-file [args]";
    }
    break;

    case PYDB:
    {
	title = "PYDB";
	base  = "PYDB, the Extended Python debugger.";
	options += "  [PYDB options]     Pass option to PYDB.";
	args = "program-file";
    }
    break;

    case PERL:
    {
	title = "Perl";
	base  = "the Perl debugger.";
	options += "  [Perl options]     Pass option to Perl.";
	args = "program-file [args]";
    }
    break;

    case XDB:
    {
	title = "XDB";
	base  = "XDB, the HP-UX debugger.";
	options += "  [XDB options]      Pass option to XDB.";
    }
    break;
    }

    show_version(os);
    os << gdb_version << "\n"
	"This is " DDD_NAME ", the data display debugger, based on "
	<< base << "\n\n" 
	"Usage:\n\n"
	"    " ddd_NAME " [options...] " << args << "\n\n"
	"Options (including " << title << " options):\n\n";

    smart_sort(options);
    for (i = 0; i < options.size(); i++)
	os << options[i] << '\n';

    os << "\n"
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
// Show Version and Configuration
//-----------------------------------------------------------------------------

#define _stringize(x) #x
#define stringize(x) _stringize(x)

static void show_configuration(std::ostream& os, bool version_only)
{
    // Storing this as a string literal would create an SCCS entry
    const string sccs = "@(" + string("#)");

    string s;

    // Version info
    s = string("@(#)GNU " DDD_NAME " " DDD_VERSION " (" DDD_HOST ")\n") +
	"@(#)Copyright (C) 1995-1999 "
	"Technische Universit\344t Braunschweig, Germany.\n"
        "@(#)Copyright (C) 1999-2001 "
	"Universit\344t Passau, Germany.\n"
        "@(#)Copyright (C) 2001 "
	"Universit\344t des Saarlandes, Germany.\n"
        "@(#)Copyright (C) 2001-2009 "
        "Free Software Foundation, Inc.\n";
    s.gsub(sccs, string(""));
    os << s;

    if (version_only)
	return;

    // Compilation stuff
    s = "\n@(#)Compiled with "
#ifdef __GNUC__
	"GCC "
# ifdef __VERSION__
	__VERSION__
# else  // !defined(__VERSION__)
	stringize(__GNUC__)
#  ifdef __GNUC_MINOR__
        "." stringize(__GNUC_MINOR__)
#  endif
# endif // !defined(__VERSION__)

#elif defined(__SUNPRO_CC)
	"SunPRO CC " stringize(__SUNPRO_CC)

#elif defined(__sgi) && defined (_COMPILER_VERSION)
	"MIPSPro CC " stringize(_COMPILER_VERSION)

#elif defined(__DECCXX) && defined (__DECCXX_VER)
	"Compaq cxx " stringize(__DECCXX_VER)

#elif defined (__HP_aCC)
	"HP aCC " stringize(__HP_aCC)

#elif defined (__xlC__)
# if defined (__IBMCPP__)
	"IBM Visual Age " stringize(__IBMCPP__) " (" stringize(__xlC__) ")"
# else
	"IBM xlC " stringize(__xlC__)
# endif

#elif defined(_MSC_VER)
	"MS VC++ " stringize(_MSC_VER)
  
#elif defined(__BORLANDC__)
	"Borland C++ " stringize(__BORLANDC__)

#elif defined(__INTEL_COMPILER)
        "Intel C++ " stringize(__INTEL_COMPILER)

#elif defined(CXX_NAME)
	CXX_NAME
#else  // Anything else
	"some C++ compiler"
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

    // Optional stuff
    s = "@(#)Includes "
#ifdef XpmFormat
	"XPM " stringize(XpmFormat) "." stringize(XpmVersion) 
	"." stringize(XpmRevision) ", "
#endif
#if HAVE_ATHENA
	"Athena Panner"
#if defined(XawVersion)
        " (" stringize(XawVersion) ")"
#endif	
	", "
#endif // HAVE_ATHENA
#if WITH_BUILTIN_VSLLIB
	"VSL library, "
#endif
#if WITH_BUILTIN_MANUAL
	"Manual, "
#endif
#if WITH_BUILTIN_LICENSE
	"License, "
#endif
#if WITH_BUILTIN_NEWS
	"News, "
#endif
#if WITH_BUILTIN_APP_DEFAULTS
	"App defaults, "
#endif
#if defined(WITH_VALGRIND)
        "Self Valgrind leak check, "
#endif
	 DDD_NAME " core\n";
    s.gsub(sccs, string(""));

#if WITH_READLINE
    s = s.through("core") + ", Readline " + rl_library_version + 
	s.after("core");
#endif
    os << s;

    string cinfo = config_info;
    cinfo = cinfo.before("\n");
    strip_space(cinfo);

    os << cinfo << "\n";
}

void show_version(std::ostream& os)
{
    show_configuration(os, true);
}

void show_configuration(std::ostream& os)
{    
    show_configuration(os, false);
}

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

static int uncompress(std::ostream& os, const char *text, int size)
{
    string tmpfile = tempfile();
    FILE *fp = fopen(tmpfile.chars(), "w");
    if (fp == 0)
    {
	os << tmpfile << ": " << strerror(errno);
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

    string cmd = string(app_data.uncompress_command) + " < " + tmpfile;

    const string s1 = sh_command(cmd, true) + " 2>&1";  
    fp = popen(s1.chars(), "r");
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

    unlink(tmpfile.chars());
    return 0;
}

void show(int (*formatter)(std::ostream& os))
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

	const char *env_pager = getenv("PAGER");
	if (env_pager != 0)
	    cmd = string(env_pager) + " || " + cmd;
	cmd = "( " + cmd + " )";
	const string s1 = sh_command(cmd);
	pager = popen(s1.chars(), "w");
    }

    if (pager == 0)
    {
	formatter(std::cout);
	std::cout << std::flush;
    }
    else
    {
	std::ostringstream text;
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
    system(cmd.chars());
}



//-----------------------------------------------------------------------------
// License
//-----------------------------------------------------------------------------

int ddd_license(std::ostream& os)
{
    (void) uncompress;		// Use it

#if WITH_BUILTIN_LICENSE
    static const char COPYING[] =
#include "COPYING.gz.C"
	;

    return uncompress(os, COPYING, sizeof(COPYING) - 1);
#else
    const string s1 = resolvePath("COPYING"); 
    std::ifstream is(s1.chars());
    if (is.bad())
	return 1;

    int c;
    while ((c = is.get()) != EOF)
	os << (char)c;

    return 0;
#endif
}

void DDDLicenseCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusMsg msg("Invoking " DDD_NAME " license browser");

    std::ostringstream license;
    int ret = ddd_license(license);
    string s(license);
    s.prepend("@license@");

    TextHelpCB(w, XtPointer(s.chars()), call_data);

    if (ret != 0 || !s.contains("GNU"))
	post_error("The " DDD_NAME " license could not be uncompressed.", 
		   "no_license_error", w);
}

//-----------------------------------------------------------------------------
// News
//-----------------------------------------------------------------------------

int ddd_news(std::ostream& os)
{
#if WITH_BUILTIN_NEWS
    static const char NEWS[] =
#include "NEWS.gz.C"
	;

    return uncompress(os, NEWS, sizeof(NEWS) - 1);
#else
    const string s1 = resolvePath("NEWS"); 
    std::ifstream is(s1.chars());
    if (is.bad())
	return 1;

    int c;
    while ((c = is.get()) != EOF)
	os << (char)c;
    return 0;
#endif
}

void DDDNewsCB(Widget w, XtPointer, XtPointer call_data)
{
    StatusMsg msg("Invoking " DDD_NAME " news browser");

    std::ostringstream news;
    int ret = ddd_news(news);
    string s(news);
    s.prepend("@news@");

    TextHelpCB(w, XtPointer(s.chars()), call_data);

    if (ret != 0 || !s.contains(DDD_NAME))
	post_error("The " DDD_NAME " news could not be uncompressed.", 
		   "no_news_error", w);
}



//-----------------------------------------------------------------------------
// Manual
//-----------------------------------------------------------------------------

int ddd_man(std::ostream& os)
{
#if WITH_BUILTIN_MANUAL
    static const char MANUAL[] =
#include "ddd.info.txt.gz.C"
	;

    return uncompress(os, MANUAL, sizeof(MANUAL) - 1);
#else
    // Try `info ddd', `man ddd' and `man xddd'.
    string cmd = 
	"info --subnodes -o - -f " ddd_NAME " 2> /dev/null || "
	"man " ddd_NAME " || man x" ddd_NAME;

    const string s1 = sh_command(cmd);
    FILE *fp = popen(s1.chars(), "r");
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
    
    std::ostringstream man;
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

    string key = downcase(gdb->title());
    if (gdb->type() == PERL)
	key = "perldebug";

    // Ordinary way: try `man dbx', etc.
    string cmd = "man " + key + " 2>&1";

    if (gdb->type() == GDB)
    {
	// Try `info gdb' first
	cmd.prepend("info --subnodes -o - -f " + key + " 2> /dev/null || ");
    }

    const string s1 = sh_command(cmd); 
    FILE *fp = popen(s1.chars(), "r");
    if (fp != 0)
    {
	std::ostringstream man;
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
