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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char string_functions_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Verschiedene Funktionen auf string
//-----------------------------------------------------------------------------

#include "assert.h"
#include "string-fun.h"
#include "regexps.h"

#include <stdio.h>		// sprintf
#include <stdlib.h>		// atoi
#include <ctype.h>		// isspace

#if RUNTIME_REGEX
const regex rxblanks          (" +");
const regex rxblanks_or_tabs  ("[ \t]+");
#endif


// Convert NR to a string
string itostring (int nr)
{
    char buffer[10];
    sprintf(buffer, "%d", nr);
    return string(buffer);
}

// Remove leading parentheses
static void read_leading_parentheses(string& s)
{
    while (s.length() > 0 && (isspace(s[0]) || s[0] == '(' || s[0] == '['))
	s = s.after(0);
}

// Remove trainling parentheses
static void read_trailing_parentheses(string& s)
{
    while (s.length() > 0 && (isspace(s[0]) || s[0] == ')' || s[0] == ']'))
	s = s.after(0);
}
    
    

// Return true iff S begins with an integer
bool has_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    return int_string.contains(rxint, 0);
}

// Return the integer at beginning of S, or 0 if none
int get_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    int_string = int_string.through(rxint);
    return atoi(int_string.chars());
}

// Return the integer at beginning of S, or -1 if none
int get_positive_nr (const char* s) 
{
    string str = s;
    return get_positive_nr (str);
}

// Return the integer at beginning of S, or -1 if none
int get_positive_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    int_string = int_string.through(rxint);
    if (int_string == "")
	return -1;

    return atoi(int_string.chars());
}

// Remove leading blanks from S
void strip_leading_space (string& s)
{
    int i = 0;
    while (i < int(s.length()) && isspace(s[i]))
	i++;
    s = s.from(i);
}

// Remove and return a leading integer from S, or "" if none
string read_nr_str (string& s)
{
    string s0 = s;
    read_leading_parentheses(s);

    string int_string = s.through(rxint);
    if (int_string == "")
    {
	s = s0;
	return "";
    }

    s = s.from(int(int_string.length()));
    read_trailing_parentheses(s);

    return int_string;
}

// Remove and return a leading integer from S, or 0 if none
int read_positive_nr (string& s)
{
    return atoi(read_nr_str(s));
}

// Strip final characters
void strip_trailing_space(string& text)
{
    int index = text.length() - 1;
    while (index >= 0 && isspace(text[index]))
	index--;

    if (index < 0)
	text = "";
    else
	text.after(index) = "";
}

// Strip final characters
void strip_trailing_newlines(string& text)
{
    int index = text.length() - 1;
    while (index >= 0 && text[index] == '\n')
	index--;

    if (index < 0)
	text = "";
    else
	text.after(index) = "";
}
