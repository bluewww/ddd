// $Id$ -*- C++ -*-
// Convert ^[character] sequences to control characters

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

char ctrl_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ctrl.h"
#include "cook.h"
#include <ctype.h>

// Convert ^[character] sequence to control characters
string ctrl(string name)
{
    if (name.length() >= 2)
    {
	if (name[0] == '^')
	    if (name[1] == '?')
		return '\177';	// ^?
	    else
		return char(toupper(name[1]) - '@'); // ^A...^Z

	if (name[0] == '\\')
	    return uncook(name); // `\003' or similar
    }

    if (name.length() == 1 && isalpha(name[0]))
	return char(toupper(name[0]) - '@'); // A..Z

    return name;
}
