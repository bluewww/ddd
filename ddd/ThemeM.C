// $Id$ -*- C++ -*-
// Theme Manager

// Copyright (C) 2000 Universitaet Passau, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char ThemeManager_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ThemeM.h"
#include "cook.h"
#include "glob.h"
#include "value-read.h"
#include "string-fun.h"

string ThemeManager::read_word(string& value)
{
    string token;

    do {
	token = read_token(value);
	if (token == "")
	    return "";
	strip_space(token);
    } while (token == "");

    return token;
}

ThemeManager::ThemeManager(const string& value)
    : map()
{
    string pattern = "*";
    StringArray themes;
    StringStringArrayAssoc a_map;

    string v = value;
    while (v != "")
    {
	string token = read_word(v);

	if (token == "pattern")
	{
	    pattern = unquote(read_word(v));
	}
	else if (token == "theme")
	{
	    themes += unquote(read_word(v));
	    a_map[pattern] = themes;
	}
	else
	{
	    cerr << "themes: syntax error at " << quote(token) << "\n";
	}
    }

    // Be sure all entries are in correct order
    for (StringStringArrayAssocIter i(a_map); i.ok(); i = i.next())
	map[i.key()] = i.value();
}

ostream& operator<<(ostream& os, const ThemeManager& t)
{
    for (StringStringArrayAssocIter i(t.map); i.ok(); i = i.next())
    {
	os << "pattern " << quote(i.key()) << " ";
	for (int j = 0; j < i.value().size(); j++)
	    os << "theme " << quote(i.value()[j]);
	os << "\n";
    }

    return os;
}

// Get list of themes for EXPR.  First pattern matches.
const StringArray& ThemeManager::expr_themes(const string& expr) const
{
    for (StringStringArrayAssocIter i(map); i.ok(); i = i.next())
	if (glob_match (i.key(), expr, 0))
	    return i.value();

    static StringArray empty;
    return empty;
}

// Get list of themes for PATTERN
const StringArray& ThemeManager::pattern_themes(const string& pattern) const
{
    for (StringStringArrayAssocIter i(map); i.ok(); i = i.next())
	if (i.key() == pattern)
	    return i.value();

    static StringArray empty;
    return empty;
}

// Get list of patterns
StringArray ThemeManager::patterns() const
{
    StringArray ret;
    for (StringStringArrayAssocIter i(map); i.ok(); i = i.next())
	ret += i.key();
    return ret;
}

// Set a pattern
void ThemeManager::set_pattern(const string& pattern, 
			       const StringArray& themes)
{
    map[pattern] = themes;
}

// Remove a pattern
void ThemeManager::remove_pattern(const string& pattern)
{
    map.remove(pattern);
}
