// $Id$ -*- C++ -*-
// Shorten file names to unique base names

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

char uniquify_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "uniquify.h"
#include "SmartC.h"
#include "bool.h"

static const char *file_basename(const char *name, char sep)
{
    const char *base = name;

    while (*name)
    {
	if (*name++ == sep)
	    base = name;
    }

    return base;
}

// FULL_PATH is /X/Y/Z/NAME; expand NAME to Z/NAME.
static bool expand_label(string& name, const string& full_path, char sep)
{
    // Set index to the last SEP before NAME
    int index = full_path.length() - name.length();
    index = full_path.index(sep, index - full_path.length() - 1);

    if (index >= 1)
    {
	// Set index to the last SEP before Z
	index--;
	index = full_path.index(sep, index - full_path.length() - 1);
    }

    if (index >= 1)
    {
	name = full_path.after(index);
	return true;
    }
    else
    {
	name = full_path;
	return false;
    }
}


// Remove adjacent duplicates in A1
static void uniq(StringArray& a1, StringArray& a2)
{
    StringArray b1;
    StringArray b2;

    for (int i = 0; i < a1.size(); i++)
    {
	if (i == 0 || a1[i - 1] != a1[i])
	{
	    b1 += a1[i];
	    b2 += a2[i];
	}
    }
    
    a1 = b1;
    a2 = b2;
}

// Sort A1 and A2 according to the values in A1
static void sort(StringArray& a1, StringArray& a2)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= a1.size());
    do {
	h /= 3;
	for (int i = h; i < a1.size(); i++)
	{
	    string v1 = a1[i];
	    string v2 = a2[i];
	    int j;
	    for (j = i; j >= h && smart_compare(a1[j - h], v1) > 0; j -= h)
	    {
		a1[j] = a1[j - h];
		a2[j] = a2[j - h];
	    }
	    if (i != j)
	    {
		a1[j] = v1;
		a2[j] = v2;
	    }
	}
    } while (h != 1);
}

// Make each item in LABELS a base name of FILES, such that all
// items have unique names.
extern void uniquify(StringArray& files, StringArray& labels, char sep)
{
    // Start with base names
    static StringArray empty;
    labels = empty;
    int i;
    for (i = 0; i < files.size(); i++)
	labels += file_basename(files[i], sep);

    // While there are any duplicate labels, add the directory names
    i = 0;
    while (i < labels.size())
    {
	bool expanded = false;
	for (int j = i + 1; j < labels.size(); j++)
	{
	    if (labels[i] == labels[j])
	    {
		if (expand_label(labels[j], files[j], sep))
		    expanded = true;
	    }
	}

	if (expanded && expand_label(labels[i], files[i], sep))
	{
	    // Try again with expanded labels
	}
	else
	{
	    i++;
	}
    }

    sort(labels, files);
    uniq(labels, files);
}
