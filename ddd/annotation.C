// $Id$ -*- C++ -*-
// Create an edge annotation

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

char annotation_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "annotation.h"

#include "ddd.h"
#include "GDBAgent.h"
#include "cook.h"

// Remove leading S from A
static void strip_leading(string& a, const string& s)
{
    if (a.length() > s.length() && a.contains(s, 0))
	a = a.from(int(s.length()));
}

// In A, substitute 1st occurrence of S1 by S2
static bool _sub1(string& a, const string& s1, const string& s2)
{
    int index = a.index(s1);
    if (index >= 0)
    {
	a = a.before(index) + s2 + a.from(index + int(s1.length()));
	return true;
    }

    return false;
}

static string c_prefix(string& expr)
{
    string prefix;
    while (expr.contains('*', 0) || expr.contains('&', 0))
    {
	prefix += expr[0];
	expr = expr.after(0);
    }

    return prefix;
}

static bool sub1(string& a, string s1, string s2)
{
    string prefix = c_prefix(s1);

    if (_sub1(a, prefix + "((" + s1 + "))", s2))
	return true;

    if (_sub1(a, "((" + prefix + s1 + "))", s2))
	return true;

    if (_sub1(a, prefix + "(" + s1 + ")", s2))
	return true;

    if (_sub1(a, "(" + prefix + s1 + ")", s2))
	return true;

    if (_sub1(a, prefix + s1, s2))
	return true;

    return false;
}

// Normalize C pointers: `A->B' becomes `(*A).B'.
static void normalize_pointers(string& expr)
{
    if (gdb->program_language() != LANGUAGE_C)
	return;

    string prefix = c_prefix(expr);
    while (expr.contains("->"))
    {
	expr = "(*" + expr.before("->") + ")." + expr.after("->");
    }

    expr = prefix + expr;
}

// Return an annotation for an edge from FROM to TO
string annotation(string from, string to)
{
    // clog << "from = " << quote(from) << "\n";
    // clog << "to   = " << quote(to)   << "\n";

    string annotation = to;
    sub1(annotation, from, "()");

    if (annotation == to)
    {
	// No suitable annotation found
	normalize_pointers(from);
	normalize_pointers(to);

	// clog << "from = " << quote(from) << " (normalized)\n";
	// clog << "to   = " << quote(to)   << " (normalized)\n";

	annotation = to;
	sub1(annotation, from, "()");
    }

    if (annotation == to)
    {
	// No suitable annotation found
	return "";
    }

    while (annotation.contains("(())"))
	annotation.gsub("(())", "()");

    strip_leading(annotation, "*().");
    strip_leading(annotation, "()");

#if 0
    if (annotation.contains("*()", 0))
	annotation = "*" + annotation.after("()");
#endif

    return annotation;
}
