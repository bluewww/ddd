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

#ifndef _DDD_ColorBox_h
#define _DDD_ColorBox_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "THatBox.h"
#include "TypeInfo.h"
#include "mutable.h"

#if defined(IF_XM)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#endif

// Associate a color with a box
class ColorBox: public TransparentHatBox {
public:
    DECLARE_TYPE_INFO

    static bool use_color;	// Set to false to disable color

private:
    string _color_name;		// The color name, as string
    mutable bool _color_valid;		// True if COLOR is valid
    mutable bool _color_failed;		// True if conversion failed
#if defined(IF_XM)
    Pixel _color;		// The color itself, as pixel
    mutable unsigned short _red;	// Exact color values, scaled 0..65535
    mutable unsigned short _green;
    mutable unsigned short _blue;
#else
    mutable GUI::Color _color;
#endif

protected:
    // Copy Constructor
    ColorBox(const ColorBox& box):
	TransparentHatBox(box),
	_color_name(box._color_name),
	_color(box._color),
#if defined(IF_XM)
	_red(box._red),
	_green(box._green),
	_blue(box._blue),
#endif
	_color_valid(box._color_valid),
	_color_failed(box._color_failed)
    {}

#if defined(IF_XM)
    // Just draw the child
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc,
		       bool context_selected) const;

    // Draw the child in color
    virtual void color_draw(Widget w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GC gc,
			    bool context_selected) const = 0;

    // Create color, using the colormap of W
    void convert_color(Widget w) const;
#else
    // Just draw the child
    virtual void _draw(GUI::Widget *w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GUI::RefPtr<GUI::GC> gc,
		       bool context_selected) const;

    // Draw the child in color
    virtual void color_draw(GUI::Widget *w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GUI::RefPtr<GUI::GC> gc,
			    bool context_selected) const = 0;

    // Create color, using the colormap of W
    void convert_color(GUI::Widget *w) const;
#endif

public:
    // Constructor
    ColorBox(Box *box, const string& name)
	: TransparentHatBox(box), _color_name(name),
	  _color_valid(false), _color_failed(false),
#if defined(IF_XM)
	  _color(0), _red(0), _green(0), _blue(0)
#else
	  _color(0, 0, 0)
#endif
    {}

    // Resources
    const string& color_name() const { return _color_name; }
    bool color_valid() const         { return _color_valid; }
    bool color_failed() const        { return _color_failed; }
#if defined(IF_XM)
    Pixel color() const              { assert(color_valid()); return _color; }
    unsigned short red()   const     { return _red; }
    unsigned short green() const     { return _green; }
    unsigned short blue()  const     { return _blue; }
#else
    GUI::Color color() const         { return _color; }
    unsigned short red()   const     { return (unsigned short)(USHRT_MAX*_color.r); }
    unsigned short green() const     { return (unsigned short)(USHRT_MAX*_color.g); }
    unsigned short blue()  const     { return (unsigned short)(USHRT_MAX*_color.b); }
#endif

    // Set RGB color directly
    void set_rgb(unsigned short red, unsigned short green, unsigned short blue)
    {
#if defined(IF_XM)
	_red = red; _green = green; _blue = blue;
#else
	_color.r = red; _color.g = green; _color.b = blue;
#endif
	_color_valid = true;

    }
};

// Draw box using associated color as foreground
class ForegroundColorBox: public ColorBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Draw
#if defined(IF_XM)
    virtual void color_draw(Widget w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GC gc,
			    bool context_selected) const;
#else
    virtual void color_draw(GUI::Widget *w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GUI::RefPtr<GUI::GC> gc,
			    bool context_selected) const;
#endif

    // Copy Constructor
    ForegroundColorBox(const ForegroundColorBox& box):
	ColorBox(box)
    {}

public:
    // Constructor
    ForegroundColorBox(Box *box, const string& name):
	ColorBox(box, name)
    {}

    Box *dup() const { return new ForegroundColorBox(*this); }

    void _print(std::ostream& os, const BoxRegion& region, const PrintGC& gc) const;
};

// Draw box using associated color as background
class BackgroundColorBox: public ColorBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Draw
#if defined(IF_XM)
    virtual void color_draw(Widget w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GC gc,
			    bool context_selected) const;
#else
    virtual void color_draw(GUI::Widget *w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GUI::RefPtr<GUI::GC> gc,
			    bool context_selected) const;
#endif

    // Copy Constructor
    BackgroundColorBox(const BackgroundColorBox& box):
	ColorBox(box)
    {}

public:
    // Constructor
    BackgroundColorBox(Box *box, const string& name):
	ColorBox(box, name)
    {}

    Box *dup() const { return new BackgroundColorBox(*this); }

    void _print(std::ostream& os, const BoxRegion& region, const PrintGC& gc) const;
};

#endif // _DDD_ColorBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
