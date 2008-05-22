// $Id$ -*- C++ -*-
// Create an edge annotation

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

char annotation_rcsid[] = 
    "$Id$";

#include "annotation.h"

#include "ddd.h"
#include "GDBAgent.h"
#include "cook.h"
#include "isid.h"

// Remove leading S from A
static void strip_leading(string& a, const string& s)
{
    if (a.length() > s.length() && a.contains(s, 0))
	a = a.from(int(s.length()));
}

static bool my_equal(char c1, char c2)
{
    if (is_perl_prefix(c1) && is_perl_prefix(c2))
	return true;
    else
	return c1 == c2;
}

// Return 1st occurrence of B in A
static int my_index(const string& a, const string& b)
{
    if (gdb->program_language() != LANGUAGE_PERL)
	return a.index(b);

    // Compare with all Perl prefix keys considered equal
    for (int i = 0; i < int(a.length()) - int(b.length()) + 1; i++)
    {
	int j;
	for (j = 0; j < int(b.length()) && my_equal(a[i + j], b[j]); j++)
	    ;
	if (j >= int(b.length()))
	    return i;
    }

    return -1;
}

// In A, substitute 1st occurrence of S1 by S2
static bool _sub1(string& a, const string& s1, const char *s2)
{
    int index = my_index(a, s1);
    if (index >= 0)
    {
	a = a.before(index) + s2 + a.from(index + int(s1.length()));
	return true;
    }

    return false;
}

static string c_prefix(string& expr)
{
    if (gdb->program_language() != LANGUAGE_C)
	return "";

    string prefix;

    while (expr.contains('*', 0) || expr.contains('&', 0))
    {
	prefix += expr[0];
	expr = expr.after(0);
    }

    return prefix;
}

static string perl_prefix(string& expr)
{
    if (gdb->program_language() != LANGUAGE_PERL)
	return "";

    string prefix;

    while (!expr.empty() && is_perl_prefix(expr[0]))
    {
	prefix += expr[0];
	expr = expr.after(0);
    }

    return prefix;
}

static bool sub1(string& a, string s1, const char *s2)
{
    const string prefix = c_prefix(s1);

    for (int depth = 2; depth >= 0; depth--)
    {
	for (int p = 0; p < 2; p++)
	{
	    const string open  = replicate("([{"[p], depth);
	    const string close = replicate(")]}"[p], depth);

	    if (_sub1(a, prefix + open + s1 + close, s2))
		return true;

	    if (_sub1(a, open + prefix + s1 + close, s2))
		return true;
	}
    }

    return false;
}

// Normalize pointers: In C, `A->B' becomes `(*A).B'.
static void normalize_pointers(string& expr)
{
    if (gdb->program_language() == LANGUAGE_C)
    {
	const string prefix = c_prefix(expr);
	while (expr.contains("->"))
	    expr = "(*" + expr.before("->") + ")." + expr.after("->");

	expr = prefix + expr;
    }
    else if (gdb->program_language() == LANGUAGE_PERL)
    {
	// Re-insert optional arrows between brackets subscripts
	int i;
	for (i = 0; i < 3; i++)
	{
	    for (int j = 0; j < 3; j++)
	    {
	        const string open  = "([{"[j];
		const string close = ")]}"[i];
		expr.gsub(close + open, close + "->" + open);
	    }
	}

	// Replace $A->[B] by ${$A}[B]
	const string prefix = perl_prefix(expr);
	while (expr.contains("->"))
	    expr = "{$" + expr.before("->") + "}" + expr.after("->");

	// Replace $$A by ${$A}
	for (i = prefix.length() - 1; i > 0; i--)
	    expr = string("{") + prefix[i] + expr + "}";
	if (prefix.length() > 0)
	    expr.prepend(prefix[0]);
    }
}

static string _annotation(string from, string to)
{
    string annotation = to;
    sub1(annotation, from, "()");

    if (annotation == to)
    {
	string to0 = to;

	// No suitable annotation found -- try normalized
	normalize_pointers(from);
	normalize_pointers(to);

	annotation = to;
	sub1(annotation, from, "()");
	if (annotation == to)
	    annotation = to0;
    }

    return annotation;
}

// Return an annotation for an edge from FROM to TO
string annotation(const string& from, const string& to)
{
    string annotation = _annotation(from, to);
    if (annotation == to)
	return "";		// Nothing found

    // Strip surrounding braces
    for (int i = 0; i < 3; i++)
    {
	const string open  = "([{"[i];
	const string close = ")]}"[i];

	while (annotation.contains(open + "()" + close))
	    annotation.gsub(open + "()" + close, string("()"));

	while (annotation.length() > 2 && 
	       annotation.contains(open, 0) && annotation.contains(close, -1))
	{
	    annotation = unquote(annotation);
	}
    }

    if (gdb->program_language() == LANGUAGE_C)
    {
	// Display pointers using the base name only
	strip_leading(annotation, "*().");
	strip_leading(annotation, "()->");
    }

    // Omit the source name from any other expressions
    strip_leading(annotation, "()");

    return annotation;
}
