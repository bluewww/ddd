// $Id$ -*- C++ -*-
// Simple interface to Motif composite strings

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char MString_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MString.h"

Boolean MString::OK() const
{
    if (xmstring() == 0)
	return True;		// Null pointer

    // All of these must return some result.
    (void) isEmpty();
    (void) length();
    (void) lineCount();

    return True;
}

// Return all characters in M
string MString::str() const
{
    string s = "";

    XmString m = xmstring();
    XmStringContext c;
    XmStringInitContext(&c, m);
    XmStringComponentType t = XmSTRING_COMPONENT_UNKNOWN;

    while (t != XmSTRING_COMPONENT_END)
    {
	char *s_text            = 0;
	XmStringCharSet s_cs    = 0;
	XmStringDirection d     = XmSTRING_DIRECTION_DEFAULT;
	XmStringComponentType u = XmSTRING_COMPONENT_UNKNOWN;
	unsigned short ul       = 0;
	unsigned char *s_uv     = 0;
	
	t = XmStringGetNextComponent(c, &s_text, &s_cs, &d, &u, &ul, &s_uv);

	// Upon EOF in LessTif 0.82, XmStringGetNextComponent()
	// returns XmSTRING_COMPONENT_UNKNOWN instead of
	// XmSTRING_COMPONENT_END.  Work around this.
	if (t == XmSTRING_COMPONENT_UNKNOWN && s_uv == 0)
	    t = XmSTRING_COMPONENT_END;

	// Place string values in strings
	string text(s_text == 0 ? "" : s_text);
	string cs(s_cs == 0 ? "" : s_cs);
	string uv;
	if (s_uv != 0)
	    uv = string((char *)s_uv, ul);

	// Free unused memory
	XtFree(s_text);
	XtFree(s_cs);
	XtFree((char *)s_uv);

	switch (t)
	{
	case XmSTRING_COMPONENT_TEXT:
#if XmVersion >= 1002
	case XmSTRING_COMPONENT_LOCALE_TEXT:
#endif
#if XmVersion >= 2000
	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
#endif
	    s += text;
	    break;

	case XmSTRING_COMPONENT_SEPARATOR:
	    s += "\n";
	    break;

	default:
	    break;
	}
    }

    XmStringFreeContext(c);

    return s;
}
