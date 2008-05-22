// $Id$ -*- C++ -*-
// Convert ^[character] sequences to control characters

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char ctrl_rcsid[] = 
    "$Id$";

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
