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
// 675 Mass Ave, Cambridge, MA 02139, USA.
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

extern string _cook(CONST_REF(string) raw, bool for_postscript);

// Quote any non-printable character via backslash
inline string cook(CONST_REF(string) raw) 
{ 
    return _cook(raw, false); 
}

// Same, but quote ( and ) as well
inline string pscook(CONST_REF(string) raw) 
{ 
    return _cook(raw, true); 
}

// Unquote any \-quotations
extern string uncook(CONST_REF(string) cooked);

// Strip quotes
inline string _unquote(CONST_REF(string) s)
{
    return ((REF(string))s).at(1, s.length() - 2);
}

// Enclose S in quotes QUOTE
inline string _quote(CONST_REF(string) s, char quote = '\"')
{
    return string(quote) + s + quote;
}

// Strip any quotes and make it raw
inline string unquote(CONST_REF(string) s)
{
    return _unquote(uncook(s));
}

// Same, but only if enclosed in quotes QUOTE
inline string unquote(CONST_REF(string) s, char quote)
{
    if (s.length() > 0 && 
	((REF(string))s)[0] == quote && 
	((REF(string))s)[s.length() - 1] == quote)
	return unquote(s);
    else
	return s;
}
    
// Cook S and add quotes
inline string quote(CONST_REF(string) s, char q = '\"')
{
    return _quote(cook(s), q);
}

#endif // _ICE_cook_h
// DON'T ADD ANYTHING BEHIND THIS #endif
