// $Id$
// Turn a string into C representation and back again

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char cook_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <ctype.h>
#include <strstream.h>
#include <iomanip.h>

#include "bool.h"
#include "cook.h"
#include "return.h"

// Transform RAW into C string
string _cook(const string& raw, bool for_postscript) RETURNS(cooked)
{
    RETURN_OBJECT(string, cooked);
    cooked = "";
    const char *raw_s = (char *)raw;

    for (unsigned i = 0; i < raw.length(); i++)
    {
	char cc = raw_s[i];
	unsigned char c = (unsigned char)cc;

	switch(c) 
	{
	case '\a':
	    cooked += "\\a";
	    break;

	case '\b':
	    cooked += "\\b";
	    break;

#if defined(__GNUG__) && !defined(__STRICT_ANSI__)
	case '\e':
	    cooked += "\\e";
	    break;
#endif

	case '\f':
	    cooked += "\\f";
	    break;

	case '\n':
	    cooked += "\\n";
	    break;

	case '\r':
	    cooked += "\\r";
	    break;

	case '\t':
	    cooked += "\\t";
	    break;

	case '\v':
	    cooked += "\\v";
	    break;

	case '\0':
	    cooked += "\\0";
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
	    cooked += "\\";
	    cooked += c;
	    break;

	standard:
	default:
	    if (isascii(c) && isprint(c))
		cooked += c;
	    else
	    {
		ostrstream os;
		os << "\\" << oct << setw(3) << setfill('0') << int(c);
		cooked += string(os);
	    }
	    break;
	}
    }

    RETURN(cooked);
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
string uncook(const string& cooked) RETURNS(ret)
{
    RETURN_OBJECT(string, ret);
    ret = "";

    int n;
    int count;

    char *i = cooked;
    while (*i != '\0')
    {
	if (*i == '\\')
	{
	    switch (*++i)
	    {
	    case 'a':
		ret += '\a';
		i++;
		break;

	    case 'b':
		ret += '\b';
		i++;
		break;

#if defined(__GNUG__) && !defined(__STRICT_ANSI__)
	    case 'e':
		ret += '\e';
		i++;
		break;
#endif

	    case 'f':
		ret += '\f';
		i++;
		break;

	    case 'n':
		ret += '\n';
		i++;
		break;

	    case 'r':
		ret += '\r';
		i++;
		break;

	    case 't':
		ret += '\t';
		i++;
		break;

	    case 'v':
		ret += '\v';
		i++;
		break;

	    case '0':
		if (*++i == 'x')
		    goto hex;
		else
		    --i;
	    case '1': 
	    case '2': 
	    case '3':
	    case '4': 
	    case '5': 
	    case '6': 
	    case '7':
		n = 0;
		count = 0;
		while (i++, count++ < 3)
		{
		    int d = digit(*i);
		    if (d >= 0 && d < 8)
			n = (n << 3) + d;
		    else
			break;
		}
		ret += char(n);
		break;

	    hex:
	    case 'x':
		n = 0;
		count = 0;
		while (i++, count++ < 2)
		{
		    int d = digit(*i);
		    if (d >= 0 && d < 16)
			n = (n << 4) + d;
		    else
			break;
		}
		ret += char(n);
		break;

	    case '\"':
	    case '\'':
	    case '\\':
	    default:
		ret += *i++;
		break;
	    }
	}
	else
	    ret += *i++;
    }

    RETURN(ret);
}
