// $Id$ -*- C++ -*-
// Regular expression class, based on POSIX regcomp()/regexec() interface

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char regex_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "bool.h"
#include "strclass.h"
#include "assert.h"

#include <iostream.h>

regex::regex(const char* t, int flags)
{
    int errcode = regcomp(&compiled, t, flags);
    if (errcode)
    {
	size_t length = regerror(errcode, &compiled, NULL, 0);
	char *buffer = new char[length];
	regerror(errcode, &compiled, buffer, length);

	cerr << "regex: " << buffer << "\n";
	delete[] buffer;

	abort();
	return;
    }

    matches = new regmatch_t[nmatches()];
}

regex::~regex()
{
    regfree(&compiled);
    delete[] matches;
}

int regex::match_info(regoff_t& start, regoff_t& length, int nth) const
{
    if ((unsigned)(nth) >= nmatches())
	return 0;
    else
    {
	start  = matches[nth].rm_so;
	length = matches[nth].rm_eo - start;
	return start >= 0 && length >= 0;
    }
}

int regex::search(const char* s, int len, int& matchlen, int startpos) const
{
    bool rightmost = false;

    if (startpos < 0)
    {
	startpos += len;
	rightmost = true;
    }
    if (startpos < 0 || startpos > len)
	return -1;

    if (s[len] != '\0')
    {
	static string substr;
	substr = string(s, len);
	s = (char *)substr;
    }
    assert(s[len] == '\0');

    int errcode = 0;

    do {
	errcode = regexec((regex_t *)&compiled, (char *)s + startpos,
			  nmatches(), matches, 0);
    } while (rightmost && errcode && --startpos >= 0);

    if (errcode || startpos < 0)
	return -1;

    int matchpos;
    if (matches[0].rm_so >= 0)
    {
	matchpos = matches[0].rm_so + startpos;
	matchlen = matches[0].rm_eo - matches[0].rm_so;
    }
    else
    {
	matchpos = -1;
	matchlen = 0;
    }

    return matchpos;
}

int regex::match(const char *s, int len, int startpos) const
{
    if (startpos < 0)
	startpos += len;
    if (startpos > len)
	return -1;

    int matchlen;
    int ret = search(s, len, matchlen, startpos);
    if (ret < 0)
	return ret;

    return matchlen;
}

int regex::OK() const
{
    assert(matches != 0);
    return 1;
}


// Built-in regular expressions

const regex rxwhite("[ \n\t\r\v\f]+");
const regex rxint("-?[0-9]+");
const regex rxdouble("-?(([0-9]+\\.[0-9]*)|([0-9]+)|(\\.[0-9]+))([eE][---+]?[0-9]+)?");
const regex rxalpha("[A-Za-z]+");
const regex rxlowercase("[a-z]+");
const regex rxuppercase("[A-Z]+");
const regex rxalphanum("[0-9A-Za-z]+");
const regex rxidentifier("[A-Za-z_][A-Za-z0-9_]*");
