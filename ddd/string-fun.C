// $Id$
// Miscellaneous string functions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
#include <stdio.h>		// sprintf
#include <stdlib.h>		// atoi
#include <ctype.h>		// isspace

regex RXblanks          (" +");
regex RXblanks_or_tabs  ("[ \t]+");

// ***************************************************************************
string itostring (int nr)
{
    char buffer[10];
    sprintf(buffer, "%d", nr);
    return string(buffer);
}

// ***************************************************************************
// gibt den Integer zurueck, der am Anfang des String steht
// 
int get_nr (/*const*/ string& s)
{
    string int_string = s.through(rxint);
    return atoi((const char *)int_string);
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
int get_positive_nr (/*const*/ string& s)
{
    string int_string(s);
    while (int_string.contains('(', 0))
	int_string = int_string.after(0);

    int_string = int_string.through(rxint);
    if (int_string == "")
	return -1;

    int i = atoi((const char *)int_string);
    return i;
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
    while (s != "" && s[0] == '(')
	s = s.after(0);

    if (s == "" || !isdigit(s[0]))
	return "";

    string int_string = s.through(rxint);
    s = s.after(int(int_string.length()));
    return int_string;
}

// ***************************************************************************
int read_positive_nr (string& s)
{
    while (s != "" && s[0] == '(')
	s = s.after(0);

    if (s == "" || !isdigit(s[0]))
	return -1;

    string int_string = s.through(rxint);
    s = s.after(int(int_string.length()));
    return atoi(int_string);
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
