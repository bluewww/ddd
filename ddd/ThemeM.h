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

#ifndef _DDD_ThemeManager_h
#define _DDD_ThemeManager_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringSAA.h"
#include <iostream.h>

class ThemeManager {
private:
    StringStringArrayAssoc map;

protected:
    static string read_word(string& value);

public:
    // Create from external representation REP
    ThemeManager(const string& rep);

    // Copy constructor
    ThemeManager(const ThemeManager& t)
	: map(t.map)
    {}

    // Assignment
    ThemeManager& operator = (const ThemeManager& t)
    {
	map = t.map;
	return *this;
    }

    // Convert into external representation
    friend ostream& operator<<(ostream& os, const ThemeManager& t);

    // Get list of themes for EXPR.  First pattern matches.
    const StringArray& expr_themes(const string& expr) const;

    // Get list of themes for PATTERN
    const StringArray& pattern_themes(const string& pattern) const;

    // Get list of patterns
    StringArray patterns() const;

    // Set a pattern
    void set_pattern(const string& pattern, const StringArray& themes);

    // Remove a pattern
    void remove_pattern(const string& pattern);
};

ostream& operator<<(ostream& os, const ThemeManager& t);

#endif // _DDD_ThemeManager_h
// DON'T ADD ANYTHING BEHIND THIS #endif
