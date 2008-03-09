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

char ThemeManager_rcsid[] = 
    "$Id$";

#include "ThemeM.h"
#include "string-fun.h"

#include <iostream>

#ifndef LOG_THEMES
#define LOG_THEMES 0
#endif

ThemeManager::ThemeManager(const string& rep)
    : map()
{
    int count    = rep.freq('\n') + 1;
    string *subs = new string[count];

    split(rep, subs, count, '\n');

    for (int i = 0; i < count; i++)
    {
	string& s = subs[i];
	strip_space(s);
	string theme;

	if (s.contains('\t'))
	{
	    theme = s.before('\t');
	    s = s.after('\t');
	}
	else if (s.contains(' '))
	{
	    theme = s.before(' ');
	    s = s.after(' ');
	}
	else
	{
	    theme = s;
	    s = "";
	}

	strip_space(theme);
	strip_space(s);

	if (theme.empty())
	    continue;
	if (s.empty())
	    s = "*";

	bool active = true;
	if (theme.contains("!", 0))
	{
	    active = false;
	    theme = theme.after("!");
	    strip_space(theme);
	}

	map[theme] = ThemePattern(s, active);
    }

    delete[] subs;
}

std::ostream& operator<<(std::ostream& os, const ThemeManager& t)
{
    for (StringThemePatternAssocIter i(t.map); i.ok(); i = i.next())
    {
	if (!i.value().active())
	    os << "! ";
	os << i.key() << "\t" << i.value() << "\n";
    }

    return os;
}

// Get list of themes for an expression
StringArray ThemeManager::themes(const string& expr) const
{
    StringArray ret;

    for (StringThemePatternAssocIter i(map); i.ok(); i = i.next())
    {
	if (i.value().active() && i.value().matches(expr))
	    ret += i.key();
    }

#if LOG_THEMES
    std::clog << "Themes of " << expr << ":";
    for (int j = 0; j < ret.size(); j++)
	std::clog << " " << ret[j];
    std::clog << "\n";
#endif

    return ret;
}

// Get all themes
StringArray ThemeManager::themes() const
{
    StringArray ret;

    for (StringThemePatternAssocIter i(map); i.ok(); i = i.next())
	ret += i.key();

    return ret;
}
