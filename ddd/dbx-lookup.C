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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char dbx_lookup_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "dbx-lookup.h"

#include "Assoc.h"
#include "ddd.h"
#include "post.h"
#include "question.h"
#include "string-fun.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Lookup location of FUNC using DBX
//-----------------------------------------------------------------------------

static Assoc<string, string> pos_cache;

string dbx_lookup(const string& func_name)
{
    if (pos_cache.has(func_name))
	return pos_cache[func_name];

    string reply = gdb_question("func " + func_name);
    if (reply == string(-1))
    {
	post_gdb_busy();
	return "";
    }
    if (reply != "")
    {
	post_gdb_message(reply);
	return "";
    }

    string file    = gdb_question("file");
    strip_final_blanks(file);
    string listing = gdb_question("list");

    // DBX lists 10 lines; the current line is the 5th one.
    string pos = file + ":" + itostring(atoi(listing) + 5);

    pos_cache[func_name] = pos;
    return pos;
}

void clear_dbx_lookup_cache()
{
    static Assoc<string, string> empty;
    pos_cache = empty;
}
