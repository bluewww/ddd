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

// ***************************************************************************
string itostring (int nr)
{
    char buffer[10];
    sprintf(buffer, "%d", nr);
    return string(buffer);
}

static void read_leading_parentheses(string& s)
{
    while (s.length() > 0 && (isspace(s[0]) || s[0] == '(' || s[0] == '['))
	s = s.after(0);
}

static void read_trailing_parentheses(string& s)
{
    while (s.length() > 0 && (isspace(s[0]) || s[0] == ')' || s[0] == ']'))
	s = s.after(0);
}
    
    

// ***************************************************************************
bool has_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    return int_string.contains(rxint, 0);
}

// ***************************************************************************
// gibt den Integer zurueck, der am Anfang des String steht
// 
int get_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    int_string = int_string.through(rxint);
    return atoi(int_string.chars());
}

// ***************************************************************************
// 
int get_positive_nr (const char* s) 
{
    string str = s;
    return get_positive_nr (str);
}

// ***************************************************************************
// 
int get_positive_nr (const string& s)
{
    string int_string(s);
    read_leading_parentheses(int_string);
    int_string = int_string.through(rxint);
    if (int_string == "")
	return -1;

    return atoi(int_string.chars());
}


// ***************************************************************************
void read_leading_blanks (string& s)
{
    int i = 0;
    while (i < int(s.length()) && isspace(s[i]))
	i++;
    s = s.from(i);
}

// ***************************************************************************
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

// ***************************************************************************
int read_positive_nr (string& s)
{
    return atoi(read_nr_str(s));
}


// ***************************************************************************
void strip_final_blanks(string& text)
{
    while (text.length() > 0 &&
	   isspace(text[text.length() - 1]))
	text = text.before(int(text.length() - 1));
}

// ***************************************************************************
void strip_final_newlines(string& text)
{
    while (text.length() > 0 && text[text.length() - 1] == '\n')
	text = text.before(int(text.length() - 1));
}
