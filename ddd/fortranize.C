// $Id$ -*- C++ -*-
// Convert identifiers to FORTRAN format

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char fortranize_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "fortranize.h"
#include "ddd.h"
#include "GDBAgent.h"
#include "buttons.h"
#include "regexps.h"
#include "disp-read.h"		// is_invalid()



// Return ID in `fortranized' form -- that is, in lower case and with
// `_' appended.    If GLOBALS_FIRST is set, try global symbols first.
string _fortranize(const string& id, bool globals_first)
{
    if (globals_first && !id.contains('_', -1))
    {
	// Try global id first
	if (!is_invalid(gdbValue(downcase(id) + "_")))
	    return downcase(id) + "_";
    }

    // Try identifier as given.
    if (!is_invalid(gdbValue(id)))
	return id;

    // Try in lower-case.  g77 does this for local objects.
    if (!is_invalid(gdbValue(downcase(id))))
	return downcase(id);

    // Try with `_' postfix.  g77 does this for global objects.
    if (!id.contains('_', -1) &&
	!is_invalid(gdbValue(downcase(id) + "_")))
	return downcase(id) + "_";

    // Don't know what to try next - use as given
    return id;
}

// Return ID in `fortranized' form -- that is, in lower case and with
// `_' appended.    If GLOBALS_FIRST is set, try global symbols first.
// In case of pointers, dereference automatically.
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
    if (!is_invalid(value) && 
	downcase(value).contains("ptr to") && 
	!value.contains("0x0", -1))
	return gdb->dereferenced_expr(name);
    else
	return name;
}
