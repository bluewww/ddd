// $Id$ -*- C++ -*-
// Dereference expression

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

char deref_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "deref.h"

#include "GDBAgent.h"
#include "assert.h"
#include "ddd.h"
#include "buttons.h"
#include "disp-read.h"
#include "question.h"
#include "regexps.h"
#include "string-fun.h"

// Return dereferenced EXPR.  Only useful in Perl.
string deref(const string& expr, const string& sym)
{
    const string& symbol = (sym == "" ? expr : sym);

    if (gdb->program_language() != LANGUAGE_PERL)
	return gdb->dereferenced_expr(symbol);

    assert(gdb->program_language() == LANGUAGE_PERL);
    if (!expr.contains('$', 0))
	return symbol;		// Only scalars can be dereferenced

    string ref = NO_GDB_ANSWER;

    if (ref == NO_GDB_ANSWER)
    {
	// Try value directly.  Must have the form `REF(ADDRESS)'.
	string val = gdbValue(expr);
	if (val.matches(rxperlref))
	    ref = val.through(rxidentifier);
    }

#if 0
    if (ref == NO_GDB_ANSWER)
    {
	// Try `ref' operator
	ref = gdbValue("ref(" + expr + ")");
    }
#endif

    if (ref == NO_GDB_ANSWER)
	return symbol;		// Cannot access debugger

    strip_space(ref);
    string prefix = "";
    if (ref == "REF")
	prefix = "$";
    else if (ref == "SCALAR")
	prefix = "$";
    else if (ref == "ARRAY")
	prefix = "@";
    else if (ref == "HASH")
	prefix = "%";
    else if (ref == "CODE")
	prefix = "&";
    else if (ref == "GLOB")
	prefix = "*";
    else
	return symbol;		// Unknown type

    if (symbol == "()" || symbol.matches(rxidentifier))
	return prefix + symbol;
    else
	return prefix + "{" + symbol + "}";
}
