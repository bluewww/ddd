// $Id$ -*- C++ -*-
// Regular expression class, based on POSIX regcomp()/regexec() interface

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char regex_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "bool.h"
#include "strclass.h"
#include "assert.h"
#include "misc.h"
#include "cook.h"

#include <stdlib.h>
#include <iostream.h>
#include <ctype.h>
#include <string.h>		// strncmp()

// Get a prefix character from T; let T point at the next prefix character.
char regex::get_prefix(const char *& t, int flags)
{
    if (flags & REG_EXTENDED)
    {
	switch (*t++)
	{
	case '.':
	case '(':
	case '^':
	case '$':
	    return '\0';

	case '[':
	    if (*t != ']' && *t != '\0' && *t != '^' && t[1] == ']')
	    {
		char ret = *t;
		t += 2;
		return ret;
	    }
	    return '\0';

	case '\\':
	    return *t++;

	default:
	    // Ordinary character
	    switch (*t)
	    {
	    case '+':
	    case '*':
	    case '?':
	    case '|':
		return '\0';
	    
	    default:
		return t[-1];
	    }
	}
    }
    else
    {
	// FIXME: give some rules for ordinary regexps
	return '\0';
    }
}

void regex::fatal(int errcode, const char *src)
{
    if (errcode == 0)
	return;

    size_t length = regerror(errcode, &compiled, (char *)0, 0);
    char *buffer = new char[length];
    regerror(errcode, &compiled, buffer, length);

    cerr << "regex ";
    if (src)
	 cerr << quote(src) << ": ";
    cerr << "error " << errcode;
    if (buffer[0] != '\0')
	cerr << " - " << buffer;
    cerr << "\n";
#if !defined(REGCOMP_BROKEN) && !defined(GNU_LIBRX_USED)
    cerr << "As a workaround, link with GNU librx - "
	"in `config.h', #define REGCOMP_BROKEN.\n";
#endif
    delete[] buffer;

    abort();
}

regex::regex(const char* t, int flags)
{
    string rx = "^" + string(t);
    int errcode = regcomp(&compiled, rx, flags);
    if (errcode)
	fatal(errcode, rx.chars());

    exprs = new regmatch_t[nexprs()];

    unsigned int i = 0;
    const char *s = t;
    while ((prefix[i++] = get_prefix(s, flags)) != '\0'
	   && i < sizeof(prefix) - 1)
	;
    prefix[i] = '\0';
}

regex::~regex()
{
    regfree(&compiled);
    delete[] exprs;
}

// Search T in S; return position of first occurrence.
// If STARTPOS is positive, start search from that position.
// If STARTPOS is negative, perform reverse search from that position 
// and return last occurrence.
// MATCHLEN contains the length of the matched expression.
// If T is not found, return -1.
int regex::search(const char* s, int len, int& matchlen, int startpos) const
{
    string substr;
    int direction = +1;

    if (startpos < 0)
    {
	startpos += len;
	direction = -1;
    }
    if (startpos < 0 || startpos > len)
	return -1;

    if (s[len] != '\0')
    {
	substr = string(s, len);
	s = (char *)substr;
    }
    assert(s[len] == '\0');

    int errcode = 0;
    int prefix_len = strlen(prefix);

    for (; startpos >= 0 && startpos < len; startpos += direction)
    {
	char *t = (char *)s + startpos;
	if (strncmp(t, prefix, min(prefix_len, len - startpos)) == 0)
	{
	    errcode = regexec((regex_t *)&compiled, t, nexprs(), exprs, 0);
	    if (errcode == 0)
		break;
	}
    }

    if (startpos < 0 || startpos >= len)
	return -1;

    int matchpos;
    if (exprs[0].rm_so >= 0)
    {
	matchpos = exprs[0].rm_so + startpos;
	matchlen = exprs[0].rm_eo - exprs[0].rm_so;
    }
    else
    {
	matchpos = -1;
	matchlen = 0;
    }

    return matchpos;
}

// Return length of matched string iff T matches S at POS, 
// -1 otherwise.  LEN is the length of S.
int regex::match(const char *s, int len, int pos) const
{
    string substr;
    if (pos < 0)
	pos += len;
    if (pos > len)
	return -1;

    if (s[len] != '\0')
    {
	substr = string(s, len);
	s = (char *)substr;
    }
    assert(s[len] == '\0');

    int errcode = regexec((regex_t *)&compiled, (char *)s + pos, 
			  nexprs(), exprs, 0);

    if (errcode == 0 && exprs[0].rm_so >= 0)
	return exprs[0].rm_eo - exprs[0].rm_so;

    return -1;
}

bool regex::match_info(int& start, int& length, int nth) const
{
    if ((unsigned)(nth) >= nexprs())
	return false;
    else
    {
	start  = exprs[nth].rm_so;
	length = exprs[nth].rm_eo - start;
	return start >= 0 && length >= 0;
    }
}

bool regex::OK() const
{
    assert(exprs != 0);
    return true;
}


// Built-in regular expressions

const regex rxwhite("[ \n\t\r\v\f]+");
const regex rxint("-?[0-9]+");
const regex rxdouble("-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))"
		     "([eE][---+]?[0-9]+)?");
const regex rxalpha("[A-Za-z]+");
const regex rxlowercase("[a-z]+");
const regex rxuppercase("[A-Z]+");
const regex rxalphanum("[0-9A-Za-z]+");
const regex rxidentifier("[A-Za-z_][A-Za-z0-9_]*");
