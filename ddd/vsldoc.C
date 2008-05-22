// $Id$ -*- C++ -*-
// Fetch VSL documentation

// Copyright (C) 2000 Universitaet Passau, Germany.
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

char vsldoc_rcsid[] = 
    "$Id$";

#include "vsldoc.h"
#include "string-fun.h"
#include "basename.h"
#include "StringSA.h"

#include <stdio.h>

// Cache
static StringStringAssoc docs;

// Clear cache
void clear_vsldoc_cache()
{
    static StringStringAssoc empty;
    docs = empty;
}

// Return initial comments from VSL file FILE.
string vsldoc(const string& file, const string& path)
{
    if (docs.has(file))
	return docs[file];

    FILE *fp = 0;

    if (file == basename(file.chars()))
    {
	// Search for FILE in PATH
	int n = path.freq(':');
	string *dirs = new string[n + 1];
	split(path, dirs, n + 1, ':');
	for (int i = 0; i < n; i++)
	{
	    const string full_file_name = dirs[i] + "/" + file;
	    fp = fopen(full_file_name.chars(), "r");
	    if (fp != 0)
		break;
	}
	delete[] dirs;
    }

    if (fp == 0)
	fp = fopen(file.chars(), "r");

    if (fp == 0)
	return "";

    std::ostringstream os;
    int c;
    while ((c = getc(fp)) != EOF)
	os << (char)c;
    fclose(fp);

    string contents(os);

    string doc = "";
    while (contents.contains("//", 0))
    {
	string line = contents.after("//");
	line = line.before("\n");
	if (line.contains(" ", 0))  // Strip initial space.
	    line = line.after(0);
	if (!line.contains("$", 0)) // RCS comment
	{
	    if (!doc.empty())
		doc += '\n';
	    doc += line;
	}
	contents = contents.after("\n");
    }

    docs[file] = doc;
    return doc;
}
