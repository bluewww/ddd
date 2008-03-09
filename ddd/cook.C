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

char cook_rcsid[] =
    "$Id$";

#include <ctype.h>
#include <sstream>
#include <stdio.h>

#include "bool.h"
#include "cook.h"

// Transform RAW into C string
string _cook(const string& raw, bool for_postscript)
{
    std::ostringstream cooked;
    const char *raw_s = raw.chars();

    for (unsigned i = 0; i < raw.length(); i++)
    {
	char cc = raw_s[i];
	unsigned char c = (unsigned char)cc;

	switch(c) 
	{
	case '\a':
	    cooked << "\\a";
	    break;

	case '\b':
	    cooked << "\\b";
	    break;

#if 0				// This encoding is not ISO C
	case '\033':
	    cooked << "\\e";
	    break;
#endif

	case '\f':
	    cooked << "\\f";
	    break;

	case '\n':
	    cooked << "\\n";
	    break;

	case '\r':
	    cooked << "\\r";
	    break;

	case '\t':
	    cooked << "\\t";
	    break;

	case '\v':
	    cooked << "\\v";
	    break;

	case '\0':
	    cooked << "\\0";
	    break;
	    
	case '(':
	case ')':
	    if (!for_postscript)
		goto standard;
	    goto quote;

	quote:
	case '\"':
	case '\'':
	case '\\':
	    cooked << "\\";
	    cooked << c;
	    break;

	standard:
	default:
	    if (isascii(c) && isprint(c))
		cooked << c;
	    else
	    {
		char buffer[256];
		sprintf(buffer, "\\%03o", int(c));
		cooked << buffer;
	    }
	    break;
	}
    }

    return string(cooked);
}

// Return the digit represented by C
static int digit(char c)
{
    switch(c)
    {
    case '0':
	return 0;

    case '1':
	return 1;

    case '2':
	return 2;

    case '3':
	return 3;

    case '4':
	return 4;

    case '5':
	return 5;

    case '6':
	return 6;

    case '7':
	return 7;

    case '8':
	return 8;

    case '9':
	return 9;

    case 'a':
    case 'A':
	return 10;

    case 'b':
    case 'B':
	return 11;

    case 'c':
    case 'C':
	return 12;

    case 'd':
    case 'D':
	return 13;

    case 'e':
    case 'E':
	return 14;

    case 'f':
    case 'F':
	return 15;

    default:
	return -1;
    }
}

// Transform COOKED into C string
string uncook(const string& cooked)
{
    std::ostringstream uncooked;
    int n;
    int count;

    const char *i = cooked.chars();
    while (*i != '\0')
    {
	if (*i == '\\')
	{
	    switch (*++i)
	    {
	    case '\n':
		i++;
		break;

	    case 'a':
		uncooked << '\a';
		i++;
		break;

	    case 'b':
		uncooked << '\b';
		i++;
		break;

	    case 'e':		// GNU C extension
		uncooked << '\033';
		i++;
		break;

	    case 'f':
		uncooked << '\f';
		i++;
		break;

	    case 'n':
		uncooked << '\n';
		i++;
		break;

	    case 'r':
		uncooked << '\r';
		i++;
		break;

	    case 't':
		uncooked << '\t';
		i++;
		break;

	    case 'v':
		uncooked << '\v';
		i++;
		break;

	    case '0':
		if (*(i + 1) == 'x')
		{
		    i++;
		    goto hex;
		}
		// FALL THROUGH

	    case '1': 
	    case '2': 
	    case '3':
	    case '4': 
	    case '5': 
	    case '6': 
	    case '7':
		n = 0;
		count = 0;
		while (count++ < 3)
		{
		    int d = digit(*i);
		    if (d < 0 || d >= 8)
			break;

		    n = (n << 3) + d;
		    i++;
		}
		uncooked << char(n);
		break;

	    hex:
	    case 'x':
		n = 0;
		count = 0;
		i++;
		while (count++ < 2)
		{
		    int d = digit(*i);
		    if (d < 0 || d >= 16)
			break;

		    n = (n << 4) + d;
		    i++;
		}
		uncooked << char(n);
		break;

	    case '\"':
	    case '\'':
	    case '\\':
	    default:
		uncooked << *i++;
		break;
	    }
	}
	else
	    uncooked << *i++;
    }

    return string(uncooked);
}
