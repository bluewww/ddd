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

char ColorBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ColorBox.h"
#include <X11/StringDefs.h>

DEFINE_TYPE_INFO_1(ColorBox, TransparentHatBox);
DEFINE_TYPE_INFO_1(BackgroundColorBox, ColorBox);
DEFINE_TYPE_INFO_1(ForegroundColorBox, ColorBox);

bool ColorBox::use_color = true;

// Create color, using the colormap of W
void ColorBox::convert_color(Widget w) const
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
	((ColorBox *)this)->_color_failed = true;
	return;
    }

    ((ColorBox *)this)->_color_valid = true;
}

// Draw using color
void ColorBox::_draw(Widget w, 
		     const BoxRegion& region, 
		     const BoxRegion& exposed, 
		     GC gc,
		     bool context_selected) const
{
    if (use_color && !color_valid())
	convert_color(w);

    if (use_color && color_valid())
    {
	// Draw in color
	color_draw(w, region, exposed, gc, context_selected);
    }
    else
    {
	// Draw child without special color settings
	TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    }
}

// Draw using foreground color
void ForegroundColorBox::color_draw(Widget w, 
				    const BoxRegion& region, 
				    const BoxRegion& exposed, 
				    GC gc,
				    bool context_selected) const
{
    XGCValues gc_values;
    XGetGCValues(XtDisplay(w), gc, GCBackground | GCForeground, &gc_values);

    // Draw with new foreground color
    XSetForeground(XtDisplay(w), gc, color());
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    XSetForeground(XtDisplay(w), gc, gc_values.foreground);
}

// Draw using background color
void BackgroundColorBox::color_draw(Widget w, 
				    const BoxRegion& region, 
				    const BoxRegion& exposed, 
				    GC gc,
				    bool context_selected) const
{
    XGCValues gc_values;
    XGetGCValues(XtDisplay(w), gc, GCBackground | GCForeground, &gc_values);

    BoxRegion r = region & exposed;

    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    BoxPoint width(extend(X) ? space[X] : size(X),
		   extend(Y) ? space[Y] : size(Y));

    // Fill child area with background color
    XSetForeground(XtDisplay(w), gc, color());
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
		   origin[X], origin[Y], width[X], width[Y]);
    XSetForeground(XtDisplay(w), gc, gc_values.foreground);

    // Draw child with new background color
    XSetBackground(XtDisplay(w), gc, color());
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    XSetBackground(XtDisplay(w), gc, gc_values.background);
}
