// $Id$ -*- C++ -*-
// Tabulator functions

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char tabs_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "tabs.h"

//-----------------------------------------------------------------------
// Tabulator stuff
//-----------------------------------------------------------------------

// Replace first '\t' by filling up spaces until POS is reached
void tabto(string& s, int pos)
{
    for (int i = 0; unsigned(i) < s.length() && i < pos; i++)
    {
	if (s[i] == '\t')
	{
	    int offset = pos - i;
	    s(i, 1) = replicate(' ', offset);
	    return;
	}
    }
}
    
// Replace all '\t' by filling up spaces until multiple of TAB_WIDTH is reached
void untabify(string& str, int tab_width)
{
    char *buffer = 
	new char[str.length() + str.freq('\t') * (tab_width - 1) + 1];
    char *b = buffer;

    int column = 0;
    int spaces;
    const char *s = str.chars();
    while (*s != '\0')
    {
	switch (*s)
	{
	case '\t':
	    spaces = tab_width - (column % tab_width);
	    while (spaces-- > 0)
	    {
		*b++ = ' ';
		column++;
	    }
	    break;

	case '\n':
	case '\r':
	    *b++   = *s;
	    column = 0;
	    break;

	default:
	    *b++ = *s;
	    column++;
	    break;
	}
	s++;
    }
    *b++ = '\0';

    str = buffer;
    delete[] buffer;
}
