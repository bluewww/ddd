// $Id$ -*- C++ -*-
// Convert identifiers to FORTRAN format

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

char fortranize_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "fortranize.h"
#include "ddd.h"
#include "deref.h"
#include "GDBAgent.h"
#include "buttons.h"
#include "regexps.h"
#include "disp-read.h"		// is_invalid()

// Return ID in `fortranized' form -- that is, in lower/upper case and
// with `_' suffix.
static string _fortranize_globals(const string& id)
{
    if (!id.contains('_', -1))
    {
	// 1. Try `_'  postfix, lower-case.  Global objects in g77.
	// 2. Try `__' postfix, lower-case.  In g77, if the name contains `_'.
	// 3. Try `_'  postfix, upper-case.  Global objects in f77.
	// 4. Try `__' postfix, upper-case.  In f77?

	for (int caps = 0; caps < 2; caps++)
	{
	    string new_id = caps ? upcase(id) : downcase(id);
	    
	    for (int suffix_length = 1; suffix_length < 3; suffix_length++)
	    {
		new_id += '_';
		if (is_valid(gdbValue(new_id), gdb))
		    return new_id;
	    }
	}
    }

    // Don't know what to try next - use as given
    return id;
}


// Return ID in `fortranized' form -- that is, in lower/upper case and
// with appended `_'.  If GLOBALS_FIRST is set, try global symbols first.
string _fortranize(const string& id, bool globals_first)
{
    if (globals_first)
    {
	// Try global identifier first.
	string global_id = _fortranize_globals(id);
	if (global_id != id)
	    return global_id;
    }

    // Try identifier as given.
    if (is_valid(gdbValue(id), gdb))
	return id;

    // Try in lower-case.  g77 does this for local objects.
    if (is_valid(gdbValue(downcase(id)), gdb))
	return downcase(id);

    // Try in upper-case.  f77 does this for local objects.
    if (is_valid(gdbValue(upcase(id)), gdb))
	return upcase(id);

    // Try global objects.
    return _fortranize_globals(id);
}

// Return ID in `fortranized' form -- that is, in lower/upper case and
// with `_' appended.  If GLOBALS_FIRST is set, try global symbols
// first.  In case of pointers, dereference automatically.
string fortranize(const string& id, bool globals_first)
{
    if (gdb->program_language() != LANGUAGE_FORTRAN)
	return id;
    if (!id.matches(rxidentifier))
	return id;

    string name  = _fortranize(id, globals_first);
    string value = gdbValue(name);

    // Dereference pointers automatically.
    // GDB issues `(PTR TO -> ( integer )) 0x2290'
    if (is_valid(value, gdb) && 
	downcase(value).contains("ptr to") && 
	!value.contains("0x0", -1))
	return deref(name);
    else
	return name;
}
