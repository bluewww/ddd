// $Id$ -*- C++ -*-
// Machine Code Cache

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_CodeCache_h
#define _DDD_CodeCache_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "VarArray.h"
#include <ctype.h>

// A single entry in the code cache
struct CodeCacheEntry {
    string start;		// First location
    string end;			// Last location
    string code;		// Actual code

    CodeCacheEntry() {};
    CodeCacheEntry(const string& s, const string& e, const string& c)
	: start(s), end(e), code(c)
    {}
    CodeCacheEntry(const CodeCacheEntry& cce)
	: start(cce.start), end(cce.end), code(cce.code)
    {}
    CodeCacheEntry& operator=(const CodeCacheEntry& cce)
    {
	start = cce.start;
	end   = cce.end;
	code  = cce.code;
	return *this;
    }
    bool operator == (const CodeCacheEntry& cce)
    {
	return start == cce.start
	    && end == cce.end
	    && code == cce.code;
    }
    bool operator != (const CodeCacheEntry& cce)
    {
	return !operator==(cce);
    }
};

typedef VarArray<CodeCacheEntry> CodeCache;


// Compare addresses: 
// return Z such that Z < 0 if X < Y, Z == 0 if X == Y, Z > 0 if X > Y
int compare_address(const string& x, const string& y);

// Return true if C is a leading zero character
inline bool is_leading_zero(char c)
{
    return c == '0' || c == 'x' || c == 'X' || isspace(c);
}

#endif // _DDD_CodeCache_h
// DON'T ADD ANYTHING BEHIND THIS #endif
