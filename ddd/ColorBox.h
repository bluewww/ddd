// $Id$ -*- C++ -*-
// Colored Boxes

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_ColorBox_h
#define _DDD_ColorBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "THatBox.h"
#include "TypeInfo.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

// Associate a color with a box
class ColorBox: public TransparentHatBox {
public:
    DECLARE_TYPE_INFO

    static bool use_color;	// Set to false to disable color

private:
    string _color_name;		// The color name, as string
    Pixel _color;		// The color itself, as pixel
    bool _color_valid;		// True if COLOR is valid
    bool _color_failed;		// True if conversion failed

protected:
    // Copy Constructor
    ColorBox(const ColorBox& box):
	TransparentHatBox(box),
	_color_name(box._color_name),
	_color(box._color),
	_color_valid(box._color_valid),
	_color_failed(box._color_failed)
    {}

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

public:
    // Constructor
    ColorBox(Box *box, const string& name):
	TransparentHatBox(box), _color_name(name),
	_color_valid(false), _color_failed(false)
    {}

    // Resources
    const string& color_name() const { return _color_name; }
    bool color_valid() const         { return _color_valid; }
    bool color_failed() const        { return _color_failed; }
    Pixel color() const              { assert(color_valid()); return _color; }
};

// Draw box using associated color as foreground
class ForegroundColorBox: public ColorBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Draw
    virtual void color_draw(Widget w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GC gc,
			    bool context_selected) const;

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
};

// Draw box using associated color as background
class BackgroundColorBox: public ColorBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Draw
    virtual void color_draw(Widget w, 
			    const BoxRegion& region, 
			    const BoxRegion& exposed, 
			    GC gc,
			    bool context_selected) const;

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
};

#endif // _DDD_ColorBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
