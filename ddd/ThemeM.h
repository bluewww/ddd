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

#ifndef _DDD_ThemeManager_h
#define _DDD_ThemeManager_h

#include "StringTPA.h"
#include "ThemeP.h"
#include <iostream>

class ThemeManager {
private:
    StringThemePatternAssoc map;

protected:
    static string read_word(string& value);

public:
    // Create as empty
    ThemeManager()
	: map()
    {}

    // Create from external representation REP
    ThemeManager(const string& rep);

    // Add pattern
    void add(const string& theme, const ThemePattern& pattern)
    {
	map[theme] = pattern;
    }

    // Copy constructor
    ThemeManager(const ThemeManager& t)
	: map(t.map)
    {}

    // Assignment
    ThemeManager& operator = (const ThemeManager& t)
    {
        if (this != &t)
	  map = t.map;
	return *this;
    }

    // Convert into external representation
    friend std::ostream& operator<<(std::ostream& os, const ThemeManager& t);

    // Get list of themes for an expression
    StringArray themes(const string& expr) const;

    // Get all themes
    StringArray themes() const;

    // Check wether pattern exists
    bool has_pattern(const string& theme) const
    {
	return map.has(theme);
    }

    // Get pattern of theme
    ThemePattern& pattern(const string& theme)
    {
	return map[theme];
    }

    const ThemePattern& pattern(const string& theme) const
    {
	return map[theme];
    }
};

std::ostream& operator<<(std::ostream& os, const ThemeManager& t);

#endif // _DDD_ThemeManager_h
// DON'T ADD ANYTHING BEHIND THIS #endif
