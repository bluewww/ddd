// $Id$ -*- C++ -*-
// Fast index function

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char index_rcsid[] = 
    "$Id$";

#include "index.h"
#include "misc.h"
#include <string.h>


// Return index of R in S, starting with POS; PREFIX is the constant
// prefix of R.
int index(const string& s, const regex& r, const char *prefix, int startpos)
{
    int direction = +1;

    if (startpos < 0)
    {
	startpos += s.length();
	direction = -1;
    }
    if (startpos < 0 || startpos > int(s.length()))
	return -1;

    for (; startpos >= 0 && startpos < int(s.length()); startpos += direction)
    {
	const char *t = s.chars() + startpos;
	if (strncmp(t, prefix, min(strlen(prefix),
			       s.length() - startpos)) == 0
	    && r.match(s.chars(), s.length(), startpos) >= 0)
	    return startpos;
    }

    return -1;			// not found
}

