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
    if (s.contains(JAVA_SRC_SUFFIX, -1))
	s = s.before(int(int(s.length()) - strlen(JAVA_SRC_SUFFIX)));
    if (s.contains(JAVA_CLASS_SUFFIX, -1))
	s = s.before(int(int(s.length()) - strlen(JAVA_CLASS_SUFFIX)));
}

// Read FILE into S
static void slurp_file(const string& file, string& s)
{
    s = "";

    FILE *fp = fopen(file, "r");
    if (fp == NULL)
	return;

    while (!feof(fp))
    {
	char buffer[BUFSIZ];
	int nitems = fread(buffer, sizeof(char), BUFSIZ, fp);
	s += string(buffer, nitems);
    }

    fclose(fp);
}

// Return the position of CLASS_NAME definition in TEXT; -1 if none
int java_class_start(const string& text, const string& class_name, 
		     bool first_line)
{
    // We search for `class [whitespace] CLASS_NAME [whitespace]'.
    int i = -1;
    while ((i = text.index("class", i + 1)) >= 0)
    {
	while (i < int(text.length()) && !isspace(text[i]))
	    i++;
	while (i < int(text.length()) && isspace(text[i]))
	    i++;

	if (text.contains(class_name, i))
	{
	    int start = i;
	    i += class_name.length();
	    if (i < int(text.length()) && isspace(text[i]))
	    {
		// Be sure the class name is followed by `{'
		while (i < int(text.length()) && 
		       text[i] != ';' && text[i] != '{')
		    i++;
		if (i < int(text.length()) && text[i] == '{')
		{
		    // Okay, we got it.
		    if (first_line)
		    {
			// Return first line after `{'
			while (i < int(text.length()) && 
			       text[i - 1] != '\n' && text[i - 1] != '}')
			    i++;
			return i;
		    }
		    else
		    {
			// Return start of class definition.
			return start;
		    }
		}
	    }
	}
    }

    return -1;
}

// Check if FILE_NAME has a definition of CLASS_NAME
static bool has_class(const string& file_name, const string& class_name)
{
    string src_file;
    slurp_file(file_name, src_file);
    return java_class_start(src_file, class_name) > 0;
}

// Return source file of CLASS_NAME; "" if none.
string java_class_file(const string& class_name, bool search_classes)
{
    // We use 4 iterations:
    // Trial 0.  Search for CLASS_NAME.java; make sure it defines CLASS_NAME.
    // Trial 1.  Search for CLASS_NAME.class and its .java file, as in 0.
    // Trial 2.  Search for CLASS_NAME.java, even if CLASS_NAME is not def'd.
    // Trial 3.  Search for CLASS_NAME.class and its .java file, as in 2.
    for (int trial = 0; trial < 4; trial++)
    {
	string base = class_name;
	strip_java_suffix(base);
	base.gsub(".", "/");

	switch (trial)
	{
	case 0:
	case 2:
	    base += JAVA_SRC_SUFFIX;
	    break;

	case 1:
	case 3:
	    if (!search_classes)
		continue;
	    base += JAVA_CLASS_SUFFIX;
	    break;

	default:
	    return "";		// No such trial
	}

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
		continue;
	    }

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

	    switch (trial)
	    {
	    case 0:
	    case 2:
		// Look for CLASS_NAME.java 
		if (is_source_file(file_name) && 
		    (trial > 0 || has_class(file_name, class_name)))
		    return file_name;
		break;

	    case 1:
	    case 3:
	    {
		// Look for CLASS_NAME.class and scan it for
		// `SRC.java' occurrences.
		if (is_regular_file(file_name))
		{
		    string class_file;
		    slurp_file(file_name, class_file);

		    // We begin searching at the end of the file,
		    // because that's where debugging information is
		    // usually placed.
		    int i = class_file.length();
		    while ((i = 
			    class_file.index(JAVA_SRC_SUFFIX, 
					     i - class_file.length() - 1)) > 0)
		    {
			// The class file contains `.java' at I
			string src_class;
			string c;

			// Find start of name
			int start = i;
			while (start > 0 && isalnum(class_file[start - 1]))
			    start--;
			src_class = class_file(start, i - start);

			// Search for this class file instead.
			c = java_class_file(src_class, trial);
			if (c != "")
			    return c;

			// JDK javac places source file names as \001
			// LENGTH NAME, where LENGTH is the length of
			// NAME in 2-byte (lo/hi) format.  Try this.

			while (start >= 0 && class_file[start] != '\001')
			    start--;
			start += 3;	// Skip \001 LENGTH
			src_class = class_file(start, i - start);
			    
			c = java_class_file(src_class, false);
			if (c != "")
			    return c;
		    }
		}
	    }
	    }
	}
    }

    return "";			// Not found
}
