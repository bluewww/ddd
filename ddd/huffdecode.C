// $Id$ -*- C++ -*-
// Huffman-decode a text encoded with `huffencode'

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

static const char huffdecode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "strclass.h"

struct HuffCode { char c; const HuffCode *left; const HuffCode *right; };

#ifdef HUFFTEXT
#include HUFFTEXT
#else
#include "hufftext.C"
#endif

static void huffdecode(ostream& os)
{
    (void) huffdecode_rcsid;	  // Use it

    int p = 0;			  // Run across hufftext
    int i = 7;			  // Run across bits
    const HuffCode *h = huffcode; // Run around in the code

    int n = 0;

    while (n < hufflength)
    {
	if (h->left == 0)
	{
	    // Arrived at character
	    os << h->c;
	    n++;
	    h = huffcode;
	}

	if (h->left != 0)
	{
	    // Find next position
	    if (((unsigned char)(hufftext[p])) & (1 << i))
		h = h->right;
	    else
		h = h->left;
	}

	if (--i < 0)
	{
	    // Find next bit
	    p++;
	    i = 7;
	}
    }
}

#ifdef MAIN
#include <stdlib.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

int main()
{
    huffdecode(cout);
    return EXIT_SUCCESS;
}
#endif
