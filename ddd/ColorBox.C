// $Id$ -*- C++ -*-
// Colored Boxes

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char ColorBox_rcsid[] = 
    "$Id$";

#include "ColorBox.h"
#include "PrimitiveB.h"
#include "cook.h"
#include "casts.h"
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
#ifdef IF_MOTIF
    XtVaGetValues(w, XtNcolormap, &colormap, XtPointer(0));
#else // NOT IF_MOTIF
    colormap = w->get_colormap();
#endif // IF_MOTIF

#ifdef IF_MOTIF
    XColor exact_def;
    Status ok = 
	XParseColor(XtDisplay(w), colormap, color_name().chars(), &exact_def);
#else // NOT IF_MOTIF
    Gdk::Color exact_def(Glib::ustring(color_name().chars()));
    bool ok = colormap->alloc_color(exact_def);
#endif // IF_MOTIF

    if (ok)
    {
#ifdef IF_MOTIF
	MUTABLE_THIS(ColorBox *)->_red   = exact_def.red;
	MUTABLE_THIS(ColorBox *)->_green = exact_def.green;
	MUTABLE_THIS(ColorBox *)->_blue  = exact_def.blue;
#else // NOT IF_MOTIF
	_color = exact_def;
#endif // IF_MOTIF
    }
    else
    {
	// Let XtConvertAndStore generate an appropriate message
    }

#ifdef IF_MOTIF
    XrmValue from, to;
    from.size = color_name().length();
    from.addr = CONST_CAST(char*,color_name().chars());
    to.size   = sizeof(_color);
    to.addr   = (XPointer)&_color;

    if (!XtConvertAndStore(w, XtRString, &from, XtRPixel, &to))
    {
	MUTABLE_THIS(ColorBox *)->_color_failed = true;
	return;
    }
#endif // IF_MOTIF

    MUTABLE_THIS(ColorBox *)->_color_valid = true;
}

// Draw using color
void ColorBox::_draw(Widget w, 
		     const BoxRegion& region, 
		     const BoxRegion& exposed, 
		     GC gc,
		     bool context_selected) const
{
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
#ifdef IF_MOTIF
    XGCValues gc_values;
    XGetGCValues(XtDisplay(w), gc, GCBackground | GCForeground, &gc_values);

    // Draw with new foreground color
    XSetForeground(XtDisplay(w), gc, color());
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);

    // Restore old foreground
    XSetForeground(XtDisplay(w), gc, gc_values.foreground);
#else // NOT IF_MOTIF
    gc->set_foreground(color());
#endif // IF_MOTIF
}


// Draw using background color
void BackgroundColorBox::color_draw(Widget w, 
				    const BoxRegion& region, 
				    const BoxRegion& exposed, 
				    GC gc,
				    bool context_selected) const
{
#ifdef IF_MOTIF
    XGCValues gc_values;
    XGetGCValues(XtDisplay(w), gc, GCBackground | GCForeground, &gc_values);
#else // NOT IF_MOTIF
    GdkGCValues gc_values;
    gdk_gc_get_values(gc->gobj(), &gc_values);
#endif // IF_MOTIF

    BoxRegion r = region & exposed;

    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    BoxPoint width(extend(X) ? space[X] : size(X),
		   extend(Y) ? space[Y] : size(Y));

    // Fill child area with background color
#ifdef IF_MOTIF
    XSetForeground(XtDisplay(w), gc, color());
    XFillRectangle(XtDisplay(w), XtWindow(w), gc,
		   origin[X], origin[Y], width[X], width[Y]);
    XSetForeground(XtDisplay(w), gc, gc_values.foreground);
#else // NOT IF_MOTIF
    gc->set_foreground(color());
    w->get_window()->draw_rectangle(gc, true, origin[X], origin[Y], width[X], width[Y]);
    gdk_gc_set_values(gc->gobj(), &gc_values, GDK_GC_FOREGROUND);
#endif // IF_MOTIF

    // Draw child with new background color
#ifdef IF_MOTIF
    XSetBackground(XtDisplay(w), gc, color());
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    XSetBackground(XtDisplay(w), gc, gc_values.background);
#else // NOT IF_MOTIF
    gc->set_background(color());
    TransparentHatBox::_draw(w, region, exposed, gc, context_selected);
    gc->set_background(Glib::wrap(&gc_values.background));
#endif // IF_MOTIF
}


// Print using foreground color
void ForegroundColorBox::_print(std::ostream& os, const BoxRegion& region, 
				const PrintGC& gc) const
{
    // Set foreground color
    if (gc.isPostScript())
    {
	const PostScriptPrintGC &ps = const_ref_cast(PostScriptPrintGC, gc);

	if (ps.color)
	{
	    // Set new color
	    os << double(red())   / 65535.0 << " "
	       << double(green()) / 65535.0 << " "
	       << double(blue())  / 65535.0 << " "
	       << "begincolor*"
	       << " % " << color_name() 
		// << " " << red() << " " << green() << " " << blue()
		// << " " << color_valid() << " " << color_failed()
	       << "\n";
	}
    }

    ColorBox::_print(os, region, gc);

    // Reset color
    if (gc.isPostScript())
    {
	const PostScriptPrintGC &ps = const_ref_cast(PostScriptPrintGC, gc);

	if (ps.color)
	{
	    // Pop color from stack
	    os << "endcolor*\n";
	}
    }
}

// Print using background color
void BackgroundColorBox::_print(std::ostream& os, const BoxRegion& region, 
				const PrintGC& gc) const
{
    if (gc.isPostScript())
    {
	const PostScriptPrintGC &ps = const_ref_cast(PostScriptPrintGC, gc);

	if (ps.color && (red() < 65535 || green() < 65535 || blue() < 65535))
	{
	    // os << "% Background " << color_name() << "\n";

	    // First print a rule box in place (using the background color as
	    // foreground color of the rule)
	    ColorBox *block = 
		new ForegroundColorBox(new RuleBox(size()), color_name());
	    block->set_rgb(red(), green(), blue());

	    block->_print(os, region, gc);

	    block->unlink();

	    // os << "% End Background\n";
	}
    }

    // Now print the box itself
    ColorBox::_print(os, region, gc);
}
