// $Id$
// Miscellaneous string functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// Verschiedene Funktionen auf string
//-----------------------------------------------------------------------------

#ifndef _DDD_string_fun_h
#define _DDD_string_fun_h

#ifdef __GNUG__
#pragma interface
#endif

// Misc includes
#include "strclass.h"

//-----------------------------------------------------------------------------

extern regex RXblanks;
extern regex RXblanks_or_tabs;

// Convert NR to a string
string itostring (int nr);

// Return true iff S begins with an integer
bool has_nr (const string& s);

// Return the integer at beginning of S, or 0 if none
int get_nr (const string& s);

// Return the integer at beginning of S, or -1 if none
int get_positive_nr (const char* s);
int get_positive_nr (const string& s);

// Remove leading blanks from S
void read_leading_blanks (string& value);

// Remove and return a leading integer from S, or "" if none
string read_nr_str (string& s);

// Remove and return a leading integer from S, or 0 if none
int read_positive_nr (string& s);

// Strip final characters
void strip_final_blanks(string& text);
void strip_final_newlines(string& text);

#endif // _DDD_string_fun_h
// DON'T ADD ANYTHING BEHIND THIS #endif
