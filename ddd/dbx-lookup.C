// $Id$ -*- C++ -*-
// Lookup DBX cache

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char dbx_lookup_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "dbx-lookup.h"

#include "StringSA.h"
#include "ddd.h"
#include "post.h"
#include "misc.h"
#include "question.h"
#include "regexps.h"
#include "index.h"
#include "string-fun.h"
#include "GDBAgent.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Lookup location of FUNC using DBX
//-----------------------------------------------------------------------------

static StringStringAssoc pos_cache;

string dbx_lookup(const string& func_name, bool silent)
{
    if (pos_cache.has(func_name))
    {
	string pos = pos_cache[func_name];
	if (silent || pos != "")
	    return pos;
    }

    string reply;
    switch (gdb->type())
    {
    case GDB:
	reply = gdb_question("info line " + func_name, 0, true);
	break;

    case DBX:
	reply = gdb_question("list " + func_name, 0, true);
	break;

    case XDB:
	reply = gdb_question("v " + func_name, 0, true);
	break;
    }

    if (reply == NO_GDB_ANSWER)
    {
	// post_gdb_busy();
	return "";
    }

    string file = "";
    int line    = 0;
    switch (gdb->type())
    {
    case GDB:
    {
	file = reply.from('\"');
	file = file.before('\"');
	string line_s = reply.after("Line ");
	line = atoi(line_s);
	break;
    }

    case DBX:
	line = line_of_listing(reply, silent);
	if (line > 0)
	{
	    file = gdb_question("file");
	    strip_final_blanks(file);
	}
	break;

    case XDB:
	{
#if RUNTIME_REGEX
	    static regex rxcolons("[^:]*:[^:]*: *[0-9][0-9]*.*");
#endif
	    if (reply.matches(rxcolons))
	    {
		file = reply.before(':');
		reply = reply.after(':'); // Skip file
		reply = reply.after(':'); // Skip function
		read_leading_blanks(reply);
		string line_s = reply.before(':');
		line = atoi(line_s);
	    }
	    else
	    {
		// post_gdb_message(reply);
		return "";
	    }
	}
    }

    string pos = "";
    if (line > 0)
	pos = file + ":" + itostring(line);

    pos_cache[func_name] = pos;
    return pos;
}

// Find path of source file SOURCE
string dbx_path(const string& source)
{
    string path;
    if (gdb->has_setenv_command() && gdb->has_edit_command())
    {
	// The DBX `file' command issues only the base name of the
	// current file.  The `edit' command, however, invokes an
	// editor with the entire path.  So, we misuse the `edit'
	// command such that it reports the entire path.
	gdb_question("setenv EDITOR echo");
	path = gdb_question("edit " + source);
	gdb_question(string("setenv EDITOR ") + 
		     (getenv("EDITOR") ? getenv("EDITOR") : "vi"));
    }
    else
    {
	// No `setenv' and no `edit' command - revert to SOURCE.
	path = source;
    }

    strip_final_blanks(path);
    return path;
}

void clear_dbx_lookup_cache()
{
    static StringStringAssoc empty;
    pos_cache = empty;
}

int line_of_listing(string& listing, bool silent)
{
    string message;
    while (listing != ""
	   && !listing.contains('>', 0) 
	   && !listing.contains('*', 1) 
	   && atoi(listing) == 0)
    {
	message += listing.through('\n');
	listing = listing.after('\n');
    }

    if (message != "" && !silent)
	post_gdb_message(message);

    int idx = -1;

    if (idx < 0)
    {
	// SGI DBX issues `*' in column 2 before the `list'ed line.
	// Quite useful.
#if RUNTIME_REGEX
	static regex rxnlstar("\n.[*]");
#endif
	int idx = index(listing, rxnlstar, "\n");
	if (idx < 0 && listing.contains('*', 1))
	    idx = 1;
    }

    if (idx < 0)
    {
	// DEC and SGI DBX issue `>' in column 1 before the current
	// execution line.  Quite useful.
	int idx = listing.index("\n>");
	if (idx < 0 && listing.contains('>', 0))
	    idx = 0;
    }

    // Use the first number found.
    int num_idx = listing.index(rxint, max(idx, 0));
    int line = 0;
    if (num_idx >= 0)
	line = atoi(listing.chars() + num_idx);

    if (line == 0)
    {
	// Weird.  No source?
    }
    else if (idx < 0)
    {
	// No indicator found - 
	// the position we are looking for is in the middle
	line += (listing.freq('\n') + 1) / 2 - 1;
    }

    return line;
}
