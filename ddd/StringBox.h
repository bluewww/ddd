// $Id$
// A String Box contains a string

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_StringBox_h
#define _DDD_StringBox_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "strclass.h"
#include "Box.h"
#include "PrimitiveB.h"
#include "FontTable.h"

#if !defined(IF_XM)
#include <GUI/Widget.h>
#endif

// StringBox

class StringBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

private:
    string _string;
    string _fontname;
#if defined(IF_XM)
    XFontStruct *_font;
#else
    GUI::RefPtr<GUI::Font> _font;
#endif
    BoxCoordinate _ascent;

    StringBox& operator = (const StringBox&);

    void newFont();

protected:
#if defined(IF_XM)
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const;
#else
    virtual void _draw(GUI::Widget *w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GUI::RefPtr<GUI::GC> gc, 
		       bool context_selected) const;
#endif

    StringBox(const StringBox& box):
	PrimitiveBox(box), _string(box._string), _fontname(box._fontname),
	_font(box._font), _ascent(box._ascent)
    {}

    void dump(std::ostream& s) const;

    bool matches (const Box &b, const Box * = 0) const
    {
	return PrimitiveBox::matches(b) &&
	    _string == ((const StringBox *)&b)->_string;  // dirty trick
    }

public:
    static FontTable* fontTable;    // Font table
    static bool quoted;          // Flag: insert \ before quotes?

    // Constructor
    StringBox(const string& s, const char *fontname = DEFAULT_FONT_NAME,
	const char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(0, 0), t),
	_string(s), _fontname(fontname), _font(0), _ascent(0)
    {
	newFont();
    }

    StringBox(const char *s = "", const char *fontname = DEFAULT_FONT_NAME,
	const char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(0, 0), t),
	_string(s), _fontname(fontname), _font(0), _ascent(0)
    {
	newFont();
    }

#if defined(IF_XM)
    StringBox(const string& s, XFontStruct *fnt,
	      const char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(0, 0), t),
	_string(s), _fontname("?"), _font(fnt), _ascent(0)
    {
	resize();
    }
#else
    StringBox(const string& s, GUI::RefPtr<GUI::Font> fnt,
	      const char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(0, 0), t),
	_string(s), _fontname("?"), _font(fnt), _ascent(0)
    {
	resize();
    }
#endif

    Box *dup() const { return new StringBox(*this); }

#if defined(IF_XM)
    void _newFont(XFontStruct *newfont)
    {
	// If this is a new font, resize
	if (newfont != _font)
	{
	    _font = newfont;
	    resize();
	}
    }
#else
    void _newFont(GUI::RefPtr<GUI::Font> newfont)
    {
	// If this is a new font, resize
	if (newfont != _font)
	{
	    _font = newfont;
	    resize();
	}
    }
#endif

    // Assign a new font
    void newFont(const string& fontname);

    void _print(std::ostream& os, 
		const BoxRegion& region, 
		const PrintGC& gc) const;

    // Resources
#if defined(IF_XM)
    const XFontStruct *font() const { return _font; }
#else
    const GUI::RefPtr<GUI::Font> font() const { return _font; }
#endif
    const string& fontName() const  { return _fontname; }
    const char *fontName_c() const { return _fontname.chars(); }
    virtual string str() const { return _string; }

    Box *resize();

    bool isStringBox() const { return true; }
};

#endif
