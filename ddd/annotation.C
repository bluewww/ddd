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

// Remove leading S from A
static void strip_leading(string& a, const string& s)
{
    if (a.length() > s.length() && a.contains(s, 0))
	a = a.from(int(s.length()));
}

// Remove trailing S from A
static void strip_trailing(string& a, const string& s)
{
    if (a.length() > s.length() && a.contains(s, -1))
	a = a.before(int(a.length() - s.length()));
}

// In A, substitute 1st occurrence of S1 by S2
static void _sub1(string& a, const string& s1, const string& s2)
{
    int index = a.index(s1);
    if (index >= 0)
    {
	a = a.before(index) + s2 + a.from(index + int(s1.length()));
    }
}

static void sub1(string& a, const string& s1, const string& s2)
{
    _sub1(a, "(" + s1 + ")", s2);
    _sub1(a, s1, s2);
}

// Return an annotation for an edge from FROM to TO
string annotation(const string& from, const string& to)
{
    string annotation = to;
    if (from.contains('*', 0))
    {
	// Special case: `*FROM' => `FROM->TO' gets `()->TO'.
	string f = from.after('*');
	sub1(annotation, f + "->", "()->");
    }
    else
    {
	sub1(annotation, from, "()");
    }

    if (annotation == to)
	return "";		// No need to annotate this

    strip_leading(annotation, "()");
    strip_trailing(annotation, "()");

    strip_leading(annotation, "*()");
    strip_leading(annotation, ".");
    strip_leading(annotation, "->");

    return annotation;
}
