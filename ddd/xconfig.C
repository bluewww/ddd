// $Id$ -*- C++ -*-
// Setup X defaults and determine common problem causes.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char xconfig_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "xconfig.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "filetype.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <Xm/VirtKeys.h>
#include <Xm/SelectioB.h>

inline string sh_quote(string s)
{
    s.gsub('\'', "'\\''");
    return string('\'') + s + '\'';
}

inline string dirname(string file)
{
    if (file.contains('/'))
	return file.before('/', -1);
    else
	return ".";
}

static bool is_file(string file)
{
    FILE *fp = fopen(file, "r");
    if (fp == 0)
	return false;

    fclose(fp);
    return true;
}

// Fetch the X library directory, using xmkmf(1)
static String xlibdir(Display *display, bool verbose = false)
{
    static bool tried = false;
    static String dir = 0;

    if (tried)
	return dir;
    tried = true;

    if (!is_cmd_file(cmd_file("xmkmf")))
	return dir;		// No `xmkmf' in PATH
    if (!is_cmd_file(cmd_file("make")))
	return dir;		// No `make' in PATH

    static const char shell_command[] =
	""
#include "xlibdir.C"
	"";

    String me, my_class;
    XtGetApplicationNameAndClass(display, &me, &my_class);

    if (verbose)
    {
	cout << "Checking for X11 library directory... ";
	cout.flush();
    }

    FILE *fp = popen("/bin/sh -c " + sh_quote(shell_command), "r");
    if (fp == 0)
    {
	if (verbose)
	{
	    cout << strerror(errno) << "\n";
	    cout.flush();
	}
	return dir;
    }

    char buffer[BUFSIZ];
    buffer[0] = '\0';
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);

    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
	buffer[len - 1] = '\0';

    if (buffer[0] == '/')	// Sanity check
	dir = (String)XtNewString(buffer);

    if (verbose)
    {
	if (dir)
	    cout << dir << "\n";
	else
	    cout << "(not found)\n";
	cout.flush();
    }

    return dir;
}

static int check_xkeysymdb(Display *display, bool verbose)
{
    if (verbose)
    {
	(void) xlibdir(display, verbose);
    }

    if (verbose)
    {
	cout << "Checking for XKeysymDB... ";
	cout.flush();
    }

    String me, my_class;
    XtGetApplicationNameAndClass(display, &me, &my_class);

    String xkeysymdb = 0;

    if (xkeysymdb == 0)
	xkeysymdb = getenv("XKEYSYMDB");
    if (xkeysymdb == 0)
	xkeysymdb = XtResolvePathname(display, "", "XKeysymDB", "",
				      NULL, NULL, 0, NULL);

    if (xkeysymdb)
    {
	if (verbose)
	{
	    cout << xkeysymdb << "\n";
	    cout.flush();
	}

	// Fix it now
	static string env;
	env = "XKEYSYMDB=" + string(xkeysymdb);
	putenv(env);
	return 0;			// Okay
    }
    

    if (xlibdir(display) != 0)
    {
	string path = string(xlibdir(display)) + "/XKeysymDB";
	if (is_file(path))
	{
	    if (verbose)
	    {
		cout << path << "\n"
		     << "Note: this is not the default path compiled into " 
		     << me << ".\n"
		     << "    To avoid having " << me 
		     << " determine this setting each time it is started,\n"
		     << "    please set the XKEYSYMDB "
		     << "environment variable to\n"
		     << "    " << quote(path) << ".\n";
		cout.flush();
	    }

	    // Fix it
	    static string env;
	    env = "XKEYSYMDB=" + path;
	    putenv(env);
	    return 0;
	}
    }

    cout << "(none)\n";
    cout.flush();

    if (verbose)
    {
	cout << "Warning: The `XKeysymDB' file is not in the "
	     << "default X file search path.\n"
	     << "    If " << me << " was not compiled on this machine, " 
	     << me << "\n" 
	     << "    may not run properly "
	     << "(lots of warnings for unknown keysym names).\n"
	     << "    Please get the `XKeysymDB' file from\n"
	     << "    `ftp://ftp.ips.cs.tu-bs.de/pub/local/softech/ddd/bin/'\n";

	if (xlibdir(display) != 0)
	    cout << "    and install it into `" << xlibdir(display, verbose) 
	     << "'\n";
	else
	    cout << "    and install it into your X project root "
		 << "(typically `/usr/lib/X11')\n";

	cout << "    or have the XKEYSYMDB environment variable "
	     << "point at it.\n";

	cout.flush();
    }

    return 1;
}


static int check_xnlspath(Display *display, bool verbose)
{
    if (verbose)
    {
	(void) xlibdir(display, verbose);
    }

    if (verbose)
    {
	cout << "Checking for nls directory... ";
	cout.flush();
    }

    String me, my_class;
    XtGetApplicationNameAndClass(display, &me, &my_class);
    String nls_file = "nls.dir";

    String xnlspath = 0;

    if (xnlspath == 0)
	xnlspath = getenv("XNLSPATH");
    if (xnlspath == 0)
    {
	xnlspath = XtResolvePathname(display, "nls", nls_file, "",
				     NULL, NULL, 0, NULL);
	if (xnlspath)
	{
	    static string name;
	    name = dirname(xnlspath);
	    xnlspath = name;
	}
    }

    if (xnlspath)
    {
	if (verbose)
	{
	    cout << xnlspath << "\n";
	    cout.flush();
	}

	// Fix it now
	static string env;
	env = string("XNLSPATH=") + xnlspath;
	putenv(env);
	return 0;		// Okay
    }

    if (xlibdir(display, verbose) != 0)
    {
	string path;
	path = xlibdir(display, verbose);
	path += "/nls";
	if (is_file(path + "/" + nls_file))
	{
	    if (verbose)
	    {
		cout << path << "\n"
		     << "Note: this is not the default path compiled into " 
		     << me << ".\n"
		     << "    To avoid having " << me 
		     << " determine this setting each time it is started,\n"
		     << "    please set the XNLSPATH "
		     << "environment variable to\n"
		     << "    " << quote(path) << ".\n\n";
		cout.flush();
	    }

	    // Fix it now
	    static string env;
	    env = "XNLSPATH=" + path;
	    putenv(env);
	    return 0;
	}
    }

    if (verbose)
    {
	cout << "Warning: cannot locate the X11 `nls' directory!\n"
	     << "    If " << me << " was not compiled on this machine, " 
	     << me << "\n" 
	     << "    may not run properly (`cut and paste' fails).\n"
	     << "    Please get the `nls' directory from\n"
	     << "    `ftp://ftp.ips.cs.tu-bs.de/pub/local/softech/ddd/bin/'\n";

	if (xlibdir(display) != 0)
	    cout << "    and install it into `" << xlibdir(display) << "'\n";
	else
	    cout << "    and install it into your X project root "
		 << "(typically `/usr/lib/X11')\n";
	cout << "    or have the XNLSPATH environment variable "
	     << "point at it.\n\n";
	cout.flush();
    }

    return 1;
}


// Set up the X11 library directory
int check_x_configuration(Widget toplevel, bool verbose)
{
    Display *display = XtDisplay(toplevel);

    int ret = 0;

    // This is required for all Motif programs.
    ret = check_xkeysymdb(display, verbose) || ret;

#if XtSpecificationRelease >= 5 || XlibSpecificationRelease >= 5
    // This is required for executables linked against X11R5 or later.
    ret = check_xnlspath(display, verbose) || ret;
#endif

    if (verbose)
    {
	if (ret == 0)
	{
	    cout << "No configuration problems found.\n";
	    cout.flush();
	}
    }

    return ret;
}
