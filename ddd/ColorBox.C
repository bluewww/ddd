// $Id$ -*- C++ -*-
// Colored Boxes

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

    Colormap colormap;
    XtVaGetValues(w, XtNcolormap, &colormap, NULL);

    XColor exact_def;
    Status ok = 
	XParseColor(XtDisplay(w), colormap, (String)color_name(), &exact_def);

    if (ok)
    {
	((ColorBox *)this)->_red   = exact_def.red;
	((ColorBox *)this)->_green = exact_def.green;
	((ColorBox *)this)->_blue  = exact_def.blue;
    }
    else
    {
	// Let XtConvertAndStore generate an appropriate message
    }

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

    // Restore old foreground
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


// Print using foreground color
void ForegroundColorBox::_print(ostream& os, const BoxRegion& region, 
				const PrintGC& gc) const
{
    // Set foreground color
    if (gc.isPostScript())
    {
	// Rainer Keller <Rainer.Keller@studbox.uni-stuttgart.de> says
	// egcs 1.1 bails out if we use ref_cast() here.
	const PostScriptPrintGC &ps = 
	    *ptr_cast(PostScriptPrintGC, (PrintGC *)&gc);

	if (ps.color)
	{
	    // Set new color
	    os << double(red())   / 65535.0 << " "
	       << double(green()) / 65535.0 << " "
	       << double(blue())  / 65535.0 << " "
	       << "begincolor*\n";
	}
    }

    ColorBox::_print(os, region, gc);

    // Reset color
    if (gc.isPostScript())
    {
	const PostScriptPrintGC &ps = 
	    ref_cast(PostScriptPrintGC, (PrintGC&) gc);

	if (ps.color)
	{
	    // Pop color from stack
	    os << "endcolor*\n";
	}
    }
}
