// $Id$ -*- C++ -*-
// Shorten file names to unique base names

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char uniquify_rcsid[] = 
    "$Id$";

#include "uniquify.h"
#include "SmartC.h"
#include "basename.h"
#include "bool.h"

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


// Make each item in LABELS a base name of FILES, such that all
// items have unique names.
void uniquify(StringArray& files, StringArray& labels, char sep)
{
    // Start with base names
    static const StringArray empty;
    labels = empty;
    int i;
    for (i = 0; i < files.size(); i++)
	labels += basename(files[i].chars(), sep);

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
}
