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

#ifndef _DDD_ThemePattern_h
#define _DDD_ThemePattern_h

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream>
#include "StringA.h"

class ThemePattern {
private:
    StringArray _patterns;
    bool _active;

protected:
    // Return true if PATTERN matches EXPR
    virtual bool matches(const string& pattern, const string& expr) const;

public:
    // Create as empty
    ThemePattern()
	: _patterns(), _active(true)
    {}

    // Create from external representation REP
    ThemePattern(const string& rep, bool active = true);

    // Copy constructor
    ThemePattern(const ThemePattern& t)
	: _patterns(t.patterns()), _active(t.active())
    {}

    // Destructor
    virtual ~ThemePattern() {}

    // True if active
    bool  active() const { return _active; }
    bool& active()       { return _active; }

    // Patterns
    const StringArray& patterns() const { return _patterns; }

    // Add pattern
    void add(const string& pattern);

    // Remove pattern
    void remove(const string& pattern);

    // Assignment
    ThemePattern& operator = (const ThemePattern& t)
    {
	_patterns = t.patterns();
	_active   = t.active();
	return *this;
    }

    // Convert into external representation
    friend std::ostream& operator<<(std::ostream& os, const ThemePattern& t);

    // Matching pattern
    string matching_pattern(const string& expr) const;

    // True if EXPR matches
    bool matches(const string& expr) const
    {
	return !matching_pattern(expr).empty();
    }
};

std::ostream& operator<<(std::ostream& os, const ThemePattern& t);

#endif // _DDD_ThemePattern_h
// DON'T ADD ANYTHING BEHIND THIS #endif
