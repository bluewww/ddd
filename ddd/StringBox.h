// $Id$
// A String Box contains a string

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:51  zeller
// Initial revision
//
// Revision 10.22  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.21  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.20  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.19  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.18  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.17  1994/02/19  20:55:20  zeller
// Neu: Kommentare auf englisch
//
// Revision 10.16  1993/12/06  13:17:35  zeller
// Fix: const string& statt string&
//
// Revision 10.15  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.14  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.13  1993/05/22  20:15:49  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.12  1993/04/22  10:51:58  zeller
// Lizenz verbessert
// 
// Revision 10.11  1993/04/20  22:57:58  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.10  1993/04/16  11:31:58  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.9  1993/02/23  16:17:52  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.8  1993/01/26  17:20:29  zeller
// Neu: fontName()
// 
// Revision 10.7  1992/10/13  20:49:09  zeller
// strclass.h statt String.h
// 
// Revision 10.6  1992/07/22  10:52:40  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/02  02:56:06  zeller
// Neu: #pragma interface
// 
// Revision 10.4  1992/06/01  12:54:08  zeller
// #pragma once entfernt
// 
// Revision 10.3  1992/05/20  15:36:47  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:07  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:42  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:49:19  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:24  zeller
// Installierte Version
// 
// Revision 1.18  1991/07/02  12:56:02  zeller
// Neu: Flag `quoted'
// 
// Revision 1.17  1991/06/09  17:37:44  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.16  1991/06/09  16:00:47  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.15  1991/05/09  15:05:40  zeller
// Warnungen vermieden
// 
// Revision 1.14  1991/04/27  16:30:06  zeller
// newFont(String&) in StringBox.C ausgelagert
// 
// Revision 1.13  1991/02/20  19:29:49  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.12  1991/02/08  16:29:39  zeller
// dump() neu definiert
// 
// Revision 1.11  1991/01/31  14:13:30  zeller
// Neu: isStringBox(), font(),
// Konstruktor StringBox mit XFontStruct*
// 
// Revision 1.10  1991/01/28  21:10:56  zeller
// Neu: newFont() hat String als Parameter;
// Unterstuetzung FontTable
// 
// Revision 1.9  1991/01/28  19:08:52  zeller
// dup() eingefuehrt
// 
// Revision 1.8  1991/01/27  15:29:05  zeller
// Neu: string()
// 
// Revision 1.7  1991/01/27  15:14:05  zeller
// Unterstuetzung wechselnde Fonts eingefuehrt
// 
// Revision 1.6  1991/01/27  12:49:32  zeller
// resize() eingefuehrt
// 
// Revision 1.5  1990/10/04  18:19:43  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.4  1990/09/16  13:01:24  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.3  1990/09/11  10:55:08  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.2  1990/09/07  16:14:43  zeller
// Shortcut bei == eingefuehrt
// 
// Revision 1.1  1990/09/07  14:32:59  zeller
// Initial revision
// 

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

    bool matches (const Box &b, const Box *callbackArg = 0) const
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
