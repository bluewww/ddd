// $Id$ -*- C++ -*-
// Shorten an expression

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char shorten_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "shorten.h"
#include "cook.h"
#include "regexps.h"
#include "IntArray.h"

static string shorten_replacement = "...";

// Shorten EXPR such that it is at most MAX_LENGTH characters long.
void shorten(string& expr, unsigned max_length)
{
    // Strip excessive whitespace
    if (expr.contains(rxwhite))
	expr.gsub(rxwhite, ' ');
    while (expr.contains(' ', 0))
	expr = expr.after(0);
    while (expr.contains(' ', expr.length() - 1))
	expr = expr.before(int(expr.length()) - 1);

    // Remove text from the middle
    if (expr.length() > max_length)
    {
	int keep = (max_length - shorten_replacement.length()) / 2;
	expr = expr.through(keep) 
	    + shorten_replacement
	    + expr.from(int(expr.length()) - keep);
    }
}
