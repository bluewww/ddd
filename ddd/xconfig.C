// $Id$ -*- C++ -*-
// Setup X defaults and determine common problem causes.

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

char xconfig_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "xconfig.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "Agent.h"

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

    static const char shell_command[] =
	""
#include "xlibdir.C"
	"";

    String me, my_class;
    XtGetApplicationNameAndClass(display, &me, &my_class);

    if (verbose)
	cout << "Checking for X11 library directory... ";

    Agent xmkmf("/bin/sh -c " + sh_quote(shell_command));
    xmkmf.start();
    FILE *fp = xmkmf.inputfp();
    if (fp == 0)
    {
	if (verbose)
	    cout << strerror(errno) << "\n";
	return dir;
    }

    char buffer[BUFSIZ];
    buffer[0] = '\0';
    fgets(buffer, sizeof(buffer), fp);
    xmkmf.wait();

    int len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
	buffer[len - 1] = '\0';

    if (buffer[0] == '/')	// Sanity check
	dir = XtNewString(buffer);

    if (verbose)
    {
	if (dir)
	    cout << dir << "\n";
	else
	    cout << "(not found)\n";
    }

    return dir;
}

static int check_xkeysymdb(Display *display, bool verbose)
{
    if (verbose)
	xlibdir(display, verbose);

    if (verbose)
	cout << "Checking for XKeysymDB... ";

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
	    cout << xkeysymdb << "\n";

	// Fix it now
	static string env;
	env = string("XKEYSYMDB=") + string(xkeysymdb);
	putenv(env);
	return 0;			// Okay
    }
    

    if (xlibdir(display))
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
		     << "    please set the $XKEYSYMDB "
		     << "environment variable to\n"
		     << "    " << quote(path) << ".\n";
	    }

	    // Fix it
	    static string env;
	    env = "XKEYSYMDB=" + path;
	    putenv(env);
	    return 0;
	}
    }

    cout << "(none)\n";

    if (verbose)
    {
	cout << "Warning: The `XKeysymDB' file is not in the "
	     << "default X file search path.\n"
	     << "    If " << me << " was not compiled on this machine, " 
	     << me << "\n" 
	     << "    may not run properly "
	     << "(lots of warnings for unknown keysym names).\n"
	     << "    Please get the `XKeysymDB' file from\n"
	     << "    `ftp://ftp.ips.cs.tu-bs.de/local/softech/ddd/bin/'\n";

	if (xlibdir(display))
	    cout << "    and install it into `" << xlibdir(display, verbose) 
	     << "'\n";
	else
	    cout << "    and install it into your X project root "
		 << "(typically `/usr/lib/X11')\n";

	cout << "    or have the $XKEYSYMDB environment variable "
	     << "point at it.\n";
    }

    return 1;
}


static int check_xnlspath(Display *display, bool verbose)
{
    if (verbose)
	xlibdir(display, verbose);

    if (verbose)
	cout << "Checking for nls directory... ";

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
	    cout << xnlspath << "\n";

	// Fix it now
	static string env;
	env = string("XNLSPATH=") + xnlspath;
	putenv(env);
	return 0;		// Okay
    }

    if (xlibdir(display, verbose))
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
		     << "    please set the $XNLSPATH "
		     << "environment variable to\n"
		     << "    " << quote(path) << ".\n\n";
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
	     << "    `ftp://ftp.ips.cs.tu-bs.de/local/softech/ddd/bin/'\n";

	if (xlibdir(display))
	    cout << "    and install it into `" << xlibdir(display) << "'\n";
	else
	    cout << "    and install it into your X project root "
		 << "(typically `/usr/lib/X11')\n";
	cout << "    or have the $XNLSPATH environment variable "
	     << "point at it.\n\n";
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
	    cout << "No configuration problems found.\n";
    }

    return ret;
}
