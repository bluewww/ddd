// $Id$
// Miscellaneous string functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

//-----------------------------------------------------------------------------
// Miscellaneous string functions
//-----------------------------------------------------------------------------

#ifndef _DDD_string_fun_h
#define _DDD_string_fun_h

// Misc includes
#include "strclass.h"

//-----------------------------------------------------------------------------

#if RUNTIME_REGEX
extern const regex rxblanks;
extern const regex rxblanks_or_tabs;
#endif

// Convert NR to a string
string itostring (int nr);

// Return true iff S begins with an integer
bool has_nr (const string& s);

// Return the integer at beginning of S, or 0 if none
int get_nr (const string& s);

// Return the integer at beginning of S, or -1 if none
int get_positive_nr (const char* s);
int get_positive_nr (const string& s);

// Remove leading whitespace
void strip_leading_space (string& value);

// Remove and return a leading integer from S, or "" if none
string read_nr_str (string& s);

// Remove and return a leading integer from S, or 0 if none
int read_positive_nr (string& s);

// Strip trailing whitespace
void strip_trailing_space(string& text);
void strip_trailing_newlines(string& text);

// Strip leading & trailing whitespace
inline void strip_space(string& text)
{
    strip_leading_space(text);
    strip_trailing_space(text);
}

#endif // _DDD_string_fun_h
// DON'T ADD ANYTHING BEHIND THIS #endif
