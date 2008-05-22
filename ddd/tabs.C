// $Id$ -*- C++ -*-
// Tabulator functions

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char tabs_rcsid[] = 
    "$Id$";

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
	    s.at(i, 1) = replicate(' ', offset);
	    return;
	}
    }
}
    
// Replace all '\t' by filling up spaces until multiple of TAB_WIDTH
// is reached; ignore first INDENT characters.
void untabify(string& str, int tab_width, int indent)
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
	    if (column >= indent)
		spaces = tab_width - ((column - indent) % tab_width);
	    else
		spaces = 1;
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
