// $Id$
// A String Box contains a string

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _Nora_StringBox_h
#define _Nora_StringBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "strclass.h"
#include "Box.h"
#include "PrimitiveB.h"
#include "FontTable.h"


// StringBox

class StringBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

private:
    string _string;
    string _fontname;
    XFontStruct *_font;
    BoxCoordinate _ascent;

protected:
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const;

    StringBox(const StringBox& box):
	PrimitiveBox(box), _string(box._string), _fontname(box._fontname),
	_font(box._font), _ascent(box._ascent)
    {}

    void dump(ostream& s) const;

    bool matches (const Box &b, const Box * = 0) const
    {
	return PrimitiveBox::matches(b) &&
	    _string == ((StringBox *)&b)->_string;  // dirty trick
    }

public:
    static FontTable* fontTable;    // Font table
    static bool quoted;          // Flag: insert \ before quotes?

    // Constructor
    StringBox(const string& s = "", const string& fontname = "fixed",
	char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(false, false), t),
	_string(s), _fontname(fontname), _font(0), _ascent(0)
    {
	newFont(fontname);
    }

    StringBox(const string& s, XFontStruct *fnt,
	char *t = "StringBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(false, false), t),
	_string(s), _fontname("?"), _font(fnt), _ascent(0)
    {
	resize();
    }

    Box *dup() const { return new StringBox(*this); }

    void _newFont(XFontStruct *newfont)
    {
	// If this is a new font, resize
	if (newfont != _font)
	{
	    _font = newfont;
	    resize();
	}
    }

    // Assign a new font
    void newFont(const string& fontname);

    void _print(ostream& os, 
		const BoxRegion& region, 
		const BoxPrintGC& gc) const;

    // Resources
    const XFontStruct *font() const { return _font; }
    const string& fontName() const  { return _fontname; }
    virtual string str() const { return _string; }

    Box *resize();

    bool isStringBox() const { return true; }
};

#endif
