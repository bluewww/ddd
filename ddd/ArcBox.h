// $Id$
// Klasse ArcBox (Deklaration)

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
// $Log$
// Revision 1.1  1995/05/01 15:47:10  zeller
// Initial revision
//
// Revision 10.16  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.15  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.14  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.13  1994/07/19  20:57:44  zeller
// New: printing functions const-corrected
// New: start(), length() now const
//
// Revision 10.12  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  19:53:35  zeller
// %Q% added
//
// Revision 10.9  1993/04/20  22:57:39  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.8  1993/04/16  11:31:22  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.7  1993/02/23  16:17:41  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.6  1993/01/18  13:41:01  zeller
// Neu: start(), length()
// 
// Revision 10.5  1992/07/22  10:52:20  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:55:44  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:53:26  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:12  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:46  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:42:18  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:26  zeller
// Installierte Version
// 
// Revision 1.16  1991/06/09  17:37:54  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.15  1991/06/09  16:00:50  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.14  1991/06/07  19:18:22  zeller
// _draw() in __draw() umbenannt
// (_draw() wird in LineBox aufgerufen)
// 
// Revision 1.13  1991/05/09  15:06:07  zeller
// Warnungen vermieden
// 
// Revision 1.12  1991/02/20  19:30:01  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.11  1991/02/08  16:30:09  zeller
// dump() neu definiert
// 
// Revision 1.10  1991/01/28  19:09:07  zeller
// dup() eingefuehrt
// 
// Revision 1.9  1990/09/16  13:00:46  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.8  1990/09/11  10:54:31  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.7  1990/09/07  16:14:50  zeller
// Shortcut bei == eingefuehrt
// 
// Revision 1.6  1990/09/07  14:31:48  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.5  1990/08/28  14:22:36  zeller
// Umstellung auf display() mit Regions
// 
// Revision 1.4  1990/08/26  15:39:20  zeller
// Umstellung von _end auf _length (neue Semantik)
// dup() entfernt
// 
// Revision 1.3  1990/08/21  11:45:56  zeller
// dup() eingefuehrt
// 
// Revision 1.2  1990/08/13  11:21:52  zeller
// #pragma once eingefuehrt
// 
// Revision 1.1  1990/08/13  11:18:08  zeller
// Initial revision
// 

#ifndef _Nora_ArcBox_h
#define _Nora_ArcBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "LineBox.h"
#include "Widget.h"


// BoxDegrees

typedef int BoxDegrees;  // -360..+360


// ArcBox

class ArcBox: public LineBox {
public:
    DECLARE_TYPE_INFO

private:
    BoxDegrees _start;     // Beginn Bogen
    BoxDegrees _length;    // Laenge Bogen

    void __draw(Widget w, 
		const BoxRegion& region, 
		const BoxRegion& exposed,
		GC gc, 
		bool context_selected) const;

    void _printPS(ostream& os, 
		  const BoxRegion& region, 
		  const BoxPrintGC& gc) const;
    void _printFig(ostream& os, 
		   const BoxRegion& region, 
		   const BoxPrintGC& gc) const;
	
protected:
    ArcBox(const ArcBox& box):
	LineBox(box), _start(box._start), _length(box._length)
    {}

    void dump(ostream& s) const;

    bool matches (const Box &b, const Box *callbackArg = 0) const
    {
	return LineBox::matches(b) &&
	    _start == ((ArcBox *)&b)->_start &&
	    _length == ((ArcBox *)&b)->_length; // dirty trick
    }

public:
    ArcBox(BoxDegrees start, BoxDegrees length, BoxCoordinate linethickness = 1, 
	char * t = "ArcBox"):
	LineBox(linethickness, t), _start(start), _length(length)
    {}

    Box *dup() const { return new ArcBox(*this); }

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    // Ressourcen
    BoxDegrees start() const { return _start; }
    BoxDegrees length() const { return _length; }
};

#endif
