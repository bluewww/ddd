// $Id$ -*- C++ -*-
// Theme Pattern

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

char ThemePattern_rcsid[] = 
    "$Id$";

#include "ThemeP.h"
#include "cook.h"
#include "glob.h"
#include "string-fun.h"

#include <iostream>

#ifndef LOG_THEME_PATTERNS
#define LOG_THEME_PATTERNS 0
#endif

ThemePattern::ThemePattern(const string& rep, bool active)
    : _patterns(), _active(active)
{
    int count    = rep.freq(';') + 1;
    string *subs = new string[count];

    split(rep, subs, count, ';');

    for (int i = 0; i < count; i++)
    {
	strip_space(subs[i]);
	_patterns += subs[i];
    }

    delete[] subs;
}

std::ostream& operator<<(std::ostream& os, const ThemePattern& p)
{
    for (int i = 0; i < p.patterns().size(); i++)
    {
	if (i > 0)
	    os << ';';
	os << p.patterns()[i];
    }

    return os;
}

bool ThemePattern::matches(const string& pattern, const string& expr) const
{
    if (pattern.contains('"', 0) || pattern.contains("'", 0))
	return expr == unquote(pattern);

    const int dot_special = 0;
    return glob_match(pattern.chars(), expr.chars(), dot_special);
}

string ThemePattern::matching_pattern(const string& expr) const
{
    for (int i = 0; i < patterns().size(); i++)
    {
	if (matches(patterns()[i], expr))
	{
#if LOG_THEME_PATTERNS
	    std::clog << quote(patterns()[i]) << " matches " << quote(expr) << "\n";
#endif
	    return patterns()[i];
	}
    }

    return "";
}

void ThemePattern::add(const string& pattern)
{
    for (int i = 0; i < patterns().size(); i++)
	if (patterns()[i] == pattern)
	    return;		// PATTERN is already there

    _patterns += pattern;
}

void ThemePattern::remove(const string& pattern)
{
    StringArray new_patterns;
    for (int i = 0; i < patterns().size(); i++)
	if (patterns()[i] != pattern)
	    new_patterns += patterns()[i];

    _patterns = new_patterns;
}
