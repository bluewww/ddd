// $Id$
// Klasse LineBox (Deklaration)

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
// Revision 1.1  1995/05/01 15:47:31  zeller
// Initial revision
//
// Revision 10.16  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.15  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.14  1994/07/19  20:57:14  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.13  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.12  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.11  1993/05/22  20:14:58  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.10  1993/04/22  10:52:03  zeller
// Lizenz verbessert
// 
// Revision 10.9  1993/04/20  22:57:51  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.8  1993/04/16  11:31:44  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.7  1993/01/18  13:30:17  zeller
// Neu: linethickness()
// 
// Revision 10.6  1992/07/22  10:52:34  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/02  02:55:56  zeller
// Neu: #pragma interface
// 
// Revision 10.4  1992/06/01  12:53:51  zeller
// #pragma once entfernt
// 
// Revision 10.3  1992/05/20  15:36:48  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:10  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:45  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:42  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:40  zeller
// Installierte Version
// 
// Revision 1.10  1991/06/09  17:37:50  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.9  1991/06/09  16:00:49  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.8  1991/06/07  19:18:08  zeller
// Neu: _draw() setzt Liniendicke vor Aufruf __draw()
// 
// Revision 1.7  1991/05/09  15:05:56  zeller
// Warnungen vermieden
// 
// Revision 1.6  1991/02/20  19:29:58  zeller
// *** empty log message ***
// 
// Revision 1.5  1991/01/28  19:09:02  zeller
// dup() eingefuehrt
// 
// Revision 1.4  1990/09/07  14:31:37  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.3  1990/09/01  13:31:12  zeller
// Umstellung auf lange Dateinamen
// 
// Revision 1.2  1990/08/13  11:22:11  zeller
// #pragma once eingefuehrt
// 
// Revision 1.1  1990/08/13  11:18:06  zeller
// Initial revision
// 

#ifndef _Nora_LineBox_h
#define _Nora_LineBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "PrimitiveB.h"

// LineBox

class LineBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

private:
    BoxCoordinate _linethickness;

protected:
    LineBox(const LineBox& box):
	PrimitiveBox(box), _linethickness(box._linethickness)
    {}

    bool matches (const Box &b, const Box *callbackArg = 0) const
    {
	return PrimitiveBox::matches(b) &&
	    _linethickness == ((LineBox *)&b)->_linethickness;  // dirty trick
    }

    // Draw this box
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const;

    // Called by _draw() after line thickness is set in GC
    virtual void __draw(Widget w, 
			const BoxRegion& region, 
			const BoxRegion& exposed,
			GC gc, 
			bool context_selected) const = 0;

public:
    LineBox(BoxCoordinate linethickness = 1, char *t = "LineBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(true, true), t),
	_linethickness(linethickness)
    {}
    
    BoxCoordinate linethickness() const { return _linethickness; }
};


#endif
