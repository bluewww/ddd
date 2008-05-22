// $Id$
// Turn a string into C representation and back again

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_cook_h
#define _DDD_cook_h

#include "strclass.h"
#include "bool.h"

extern string _cook(const string& raw, bool for_postscript);

// Quote any non-printable character via backslash
inline string cook(const string& raw) 
{ 
    return _cook(raw, false); 
}

// Same, but quote ( and ) as well
inline string pscook(const string& raw) 
{ 
    return _cook(raw, true); 
}

// Unquote any \-quotations
extern string uncook(const string& cooked);

// Strip quotes
inline string _unquote(const string& s)
{
    return s.at(1, s.length() - 2);
}

// Enclose S in quotes QUOTE
inline string _quote(const string& s, char quote = '\"')
{
    return string(quote) + s + quote;
}

// Strip any quotes and make it raw
inline string unquote(const string& s)
{
    return _unquote(uncook(s));
}

// Same, but only if enclosed in quotes QUOTE
inline string unquote(const string& s, char quote)
{
    if (s.length() > 0 && 
	s[0] == quote && 
	s[s.length() - 1] == quote)
	return unquote(s);
    else
	return s;
}
    
// Cook S and add quotes
inline string quote(const string& s, char q = '\"')
{
    return _quote(cook(s), q);
}

#endif // _DDD_cook_h
// DON'T ADD ANYTHING BEHIND THIS #endif
