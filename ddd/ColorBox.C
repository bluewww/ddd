// $Id$ -*- C++ -*-
// Colored Boxes

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

const char ColorBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ColorBox.h"
#include <X11/StringDefs.h>

DEFINE_TYPE_INFO_1(ColorBox, TransparentHatBox);

bool ColorBox::use_color = true;

void ColorBox::_draw(Widget w, 
		     const BoxRegion& region, 
		     const BoxRegion& exposed, 
		     GC gc,
		     bool context_selected) const
{
    if (use_color && !color_valid())
	((ColorBox *)this)->convert_color(w);

    if (use_color && color_valid())
    {
	XGCValues gc_values;
	if (XGetGCValues(XtDisplay(w), gc, GCForeground, &gc_values))
	{
	    // Draw with new foreground color
	    XSetForeground(XtDisplay(w), gc, color());
	    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
	    XSetForeground(XtDisplay(w), gc, gc_values.foreground);
	    return;
	}
    }
	    
    // No color - proceed as if this box was not there
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    return;
}


// Create color, using the colormap of W
void ColorBox::convert_color(Widget w)
{
    if (color_valid() || color_failed())
	return;

    XrmValue from, to;
    from.size = color_name().length();
    from.addr = (String)color_name();
    to.size   = sizeof(_color);
    to.addr   = (String)&_color;

    if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to))
    {
	XtDisplayStringConversionWarning(XtDisplay(w), color_name(), XtCColor);
	_color_failed = true;
	return;
    }

    _color_valid = true;
}
