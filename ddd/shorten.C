// $Id$ -*- C++ -*-
// Shorten an expression

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char shorten_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "shorten.h"
#include "cook.h"
#include "IntArray.h"

#ifdef MAIN
#include <iostream.h>
#include <stdlib.h>
#ifndef DEBUG
#define DEBUG 1
#endif
#endif

static string shorten_replacement = "..";

static regex RXdbx_scope("[a-zA-Z_0-9]*`");

// Shorten EXPR such that it is at most MAX_LENGTH characters long.
void shorten(string& expr, unsigned max_length)
{
#if DEBUG
    clog << quote(expr) << "\n";
#endif

    // Strip excessive whitespace
    if (expr.contains(rxwhite))
    {
	expr.gsub(rxwhite, ' ');
#if DEBUG
	clog << quote(expr) << "\n";
#endif
    }
    while (expr.contains(' ', 0))
	expr = expr.after(0);
    while (expr.contains(' ', expr.length() - 1))
	expr = expr.before(int(expr.length()) - 1);

    if (expr.length() > max_length * 3)
    {
	// Bring EXPR to a reasonable size first: cut in the middle.
	int keep = (max_length * 3 - shorten_replacement.length()) / 2;
	expr = expr.through(keep) 
	    + shorten_replacement
	    + expr.from(int(expr.length()) - keep);
    }

    // Strip DBX scope information
    while (expr.length() > max_length && expr.contains(RXdbx_scope))
    {
	string postfix = expr.after(RXdbx_scope);
	expr = expr.before(RXdbx_scope);
	expr += postfix;
#if DEBUG
	clog << quote(expr) << "\n";
#endif
    }

    if (expr.length() > max_length)
    {
	// Strip expressions '{..}', '[..]', '(..)', and `<..>'.
	const char opening[] = "{[<(";
	const char closing[] = "}]>)";
	int i = 0;
	for (i = 0; i < int(sizeof(opening) / sizeof(opening[0])) - 1; i++)
	{
	    char open_paren  = opening[i];
	    char close_paren = closing[i];
	
	    while (expr.length() > max_length && expr.contains(open_paren))
	    {
		// Find last `(' not followed by replacement string
		int last_opening = expr.index(open_paren, -1);
		while (last_opening >= 0 
		       && expr.contains(shorten_replacement, last_opening + 1))
		{
		    last_opening = 
			expr.index(open_paren, 
				   last_opening - expr.length() - 1);
		}
		if (last_opening < 0)
		    break;

		int next_closing = expr.index(close_paren, last_opening + 1);
		if (next_closing < 0)
		    break;

		expr.at(last_opening + 1, next_closing - last_opening - 1) 
		    = shorten_replacement;
#if DEBUG
		clog << quote(expr) << "\n";
#endif
	    }
	}
    }

    if (expr.length() > max_length)
    {
	// Still too long?  Remove tokens!
	const regex *rxs[] = { &rxdouble, &rxint, &rxidentifier };

	for (int i = 0; i < int(sizeof(rxs) / sizeof(rxs[0])); i++)
	{
	    const regex& rx = *rxs[i];

	    // Replace all tokens, starting in the middle
	    int number_of_rxs = 0;
	    IntArray positions;	// all occurrences of rx
	    IntArray matchlens;	// the strings matched
	    int rxpos = 0;
	    while (rxpos >= 0)
	    {
		int matchlen = 0;
		rxpos = 
		    rx.search(expr.chars(), expr.length(), matchlen, rxpos);
		if (rxpos < 0)
		    break;
		if (matchlen > int(shorten_replacement.length()))
		{
		    number_of_rxs++;
		    positions += rxpos;
		    matchlens += matchlen;
		}
		rxpos += matchlen;
	    }

	    while (expr.length() > max_length)
	    {
		if (&rx == &rxidentifier && number_of_rxs <= 2)
		    break;
		if (number_of_rxs == 0)
		    break;

		int kill = number_of_rxs / 2;
		
		expr.at(positions[kill], matchlens[kill]) =
		    shorten_replacement;
#if DEBUG
		clog << quote(expr) << "\n";
#endif

		int diff = matchlens[kill] - shorten_replacement.length();
		for (int k = kill; k < number_of_rxs - 1; k++)
		{
		    positions[k] = positions[k + 1] - diff;
		    matchlens[k] = matchlens[k + 1];
		}

		number_of_rxs--;
	    }
	}
    }

    if (expr.length() > max_length)
    {
	// Still too long?  Cut in the middle!
	int keep = (max_length - shorten_replacement.length()) / 2;
	expr = expr.through(keep) 
	    + shorten_replacement
	    + expr.from(int(expr.length()) - keep);
    }
}

#if MAIN
int main(int argc, char *argv[])
{
    int max_length = 20;
    if (argc > 1)
	max_length = atoi(argv[1]);

    char buf[1024];
    while (cin)
    {
	cin.getline(buf, sizeof buf);
	string expr(buf);
	shorten(expr, max_length);
	cout << expr << "\n";
    }

    return 0;
}
#endif
