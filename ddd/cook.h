// $Id$
// Turn a string into C representation and back again

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

#ifndef _ICE_cook_h
#define _ICE_cook_h

#ifdef __GNUG__
#pragma interface
#endif

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
    return ((string&)s).at(1, s.length() - 2);
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
	((string&)s)[0] == quote && 
	((string&)s)[s.length() - 1] == quote)
	return unquote(s);
    else
	return s;
}
    
// Cook S and add quotes
inline string quote(const string& s, char q = '\"')
{
    return _quote(cook(s), q);
}

#endif // _ICE_cook_h
// DON'T ADD ANYTHING BEHIND THIS #endif
