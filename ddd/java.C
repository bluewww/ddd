// $Id$ -*- C++ -*-
// Java class lookup

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

char java_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "java.h"
#include "filetype.h"
#include "SourceView.h"
#include "ddd.h"
#include "glob.h"

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Don't rely on libiberty basename() because we don't want to depend
// on libiberty include files
static const char *file_basename(const char *name)
{
    const char *base = name;

    while (*name)
    {
	if (*name++ == '/')
	    base = name;
    }

    return base;
}

#define basename file_basename



static void sortClasses(StringArray& a)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= a.size());
    do {
	h /= 3;
	for (int i = h; i < a.size(); i++)
	{
	    string v = a[i];
	    int j;
	    for (j = i; j >= h && a[j - h] > v; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

static bool is_archive(const string& loc)
{
    return loc.contains(".jar", -1) || loc.contains(".zip", -1);
}



//-----------------------------------------------------------------------------
// Main functions
//-----------------------------------------------------------------------------

// Store all classes matching BASE in CLASSES_LIST
void get_java_classes(StringArray& classes_list, const string& base)
{
    string use = source_view->class_path();
    while (use != "")
    {
	string loc;
	if (use.contains(':'))
	    loc = use.before(':');
	else
	    loc = use;
	use = use.after(':');

	if (is_archive(loc))
	{
	    // Archive file.
	    // Should we search this for sources? (FIXME)
	}
	else
	{
	    string mask;

	    if (loc == "" || loc == ".")
	    {
		mask = base;
	    }
	    else
	    {
		if (!loc.contains('/', -1))
		    loc += '/';
		mask = loc + base;
	    }

	    char **files = glob_filename(mask);
	    if (files == (char **)0)
	    {
		cerr << mask << ": glob failed\n";
	    }
	    else if (files == (char **)-1)
	    {
#if 0
		// No *.java in directory
		post_error(string(mask) + ": " + strerror(errno));
#endif
	    }
	    else
	    {
		for (int i = 0; files[i] != 0; i++)
		{
		    string file = files[i];
		    file = basename(file);
		    strip_java_suffix(file);
		    classes_list += file;

		    free(files[i]);
		}
		free((char *)files);
	    }
	}
    }

    sortClasses(classes_list);
}

// Remove `.java' suffix from S
void strip_java_suffix(string& s)
{
    if (s.contains(JAVA_SUFFIX, -1))
	s = s.before(int(int(s.length()) - strlen(JAVA_SUFFIX)));
}

// Return source file of CLASS_NAME; "" if none
string java_class_file(const string& class_name)
{
    string base = class_name;
    strip_java_suffix(base);
    base.gsub(".", "/");
    base += JAVA_SUFFIX;

    string use = source_view->class_path();
    while (use != "")
    {
	string loc;
	if (use.contains(':'))
	    loc = use.before(':');
	else
	    loc = use;
	use = use.after(':');

	if (is_archive(loc))
	{
	    // Archive file.
	    // Should we search this for sources? (FIXME)
	}
	else
	{
	    string file_name;

	    if (loc == "" || loc == ".")
	    {
		file_name = base;
	    }
	    else
	    {
		if (!loc.contains('/', -1))
		    loc += '/';
		file_name = loc + base;
	    }

	    if (is_source_file(file_name))
		return file_name;
	}
    }

    return "";			// Not found
}
