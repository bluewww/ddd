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

char ThemePattern_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ThemeP.h"
#include "cook.h"
#include "glob.h"
#include "string-fun.h"

ThemePattern::ThemePattern(const string& rep)
    : patterns()
{
    int count    = rep.freq(';') + 1;
    string *subs = new string[count];

    split(rep, subs, count, ';');

    for (int i = 0; i < count; i++)
    {
	strip_space(subs[i]);
	patterns += subs[i];
    }

    delete[] subs;
}

ostream& operator<<(ostream& os, const ThemePattern& p)
{
    for (int i = 0; i < p.patterns.size(); i++)
    {
	if (i > 0)
	    os << ';';
	os << p.patterns[i];
    }

    return os;
}

bool ThemePattern::matches(const string& expr) const
{
    for (int i = 0; i < patterns.size(); i++)
    {
	if (glob_match(patterns[i], expr, 0))
	    return true;
    }

    return false;
}
