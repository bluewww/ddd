// $Id$ -*- C++ -*-
// Lookup DBX cache

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
#include "string-fun.h"
#include "GDBAgent.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Lookup location of FUNC using DBX
//-----------------------------------------------------------------------------

static StringStringAssoc pos_cache;

string dbx_lookup(const string& func_name)
{
    if (pos_cache.has(func_name))
	return pos_cache[func_name];

    string reply;
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	reply = gdb_question(string("list ") + func_name);
	break;

    case XDB:
	reply = gdb_question(string("v ") + func_name);
	break;
    }

    if (reply == NO_GDB_ANSWER)
    {
	post_gdb_busy();
	return "";
    }

    string file;
    string line;
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	{
	    line = itostring(line_of_listing(reply));
	    file = gdb_question("file");
	    strip_final_blanks(file);
	}
	break;

    case XDB:
	{
	    static regex RXcolons("[^:]*:[^:]*: *[0-9][0-9]*.*");
	    if (reply.matches(RXcolons))
	    {
		file = reply.before(':');
		reply = reply.after(':'); // Skip file
		reply = reply.after(':'); // Skip function
		read_leading_blanks(reply);
		line = reply.before(':');
	    }
	    else
	    {
		post_gdb_message(reply);
		return "";
	    }
	}
    }

    if (atoi(line) == 0)
	return "";

    string pos = file + ":" + line;

    pos_cache[func_name] = pos;
    return pos;
}

void clear_dbx_lookup_cache()
{
    static StringStringAssoc empty;
    pos_cache = empty;
}

int line_of_listing(string& listing)
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

    if (message != "")
	post_gdb_message(message);

    int idx = -1;

    if (idx < 0)
    {
	// SGI DBX issues `*' in column 2 before the `list'ed line.
	// Quite useful.
	static regex nlstar("\n.[*]");
	int idx = listing.index(nlstar);
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
