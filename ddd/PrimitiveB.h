// $Id$
// Klasse PrimitiveBox und folgende (Deklaration)

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
// Revision 1.1  1995/05/01 15:47:48  zeller
// Initial revision
//
// Revision 10.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.17  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.16  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.15  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.14  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.13  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.12  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.11  1993/05/22  20:15:28  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.10  1993/04/22  10:51:50  zeller
// Lizenz verbessert
// 
// Revision 10.9  1993/04/20  22:57:55  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.8  1993/04/16  11:31:51  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.7  1993/02/23  16:17:49  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.6  1993/01/05  13:40:09  zeller
// Anpassung an GCC 2.3.3
// 
// Revision 10.5  1992/07/22  10:52:37  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:56:02  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:53:56  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:03  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:38  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:47:58  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:03  zeller
// Installierte Version
// 
// Revision 1.22  1991/06/09  17:37:38  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.21  1991/05/09  15:05:12  zeller
// Warnungen vermieden
// 
// Revision 1.20  1991/02/08  16:29:11  zeller
// dump() neu definiert
// 
// Revision 1.19  1991/02/01  13:28:00  zeller
// Umstellung: Extend sind jetzt Integer-Zahlen
// 
// Revision 1.18  1991/01/28  19:08:31  zeller
// dup() eingefuehrt
// 
// Revision 1.17  1990/09/16  13:01:15  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.16  1990/09/11  10:55:00  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.15  1990/09/09  16:41:24  zeller
// Neue Klasse "EmptyBox" eingefuehrt
// 
// Revision 1.14  1990/09/07  14:30:14  zeller
// StringBox, DiagBox ausgelagert;
// Neu: Unterstuetzung == und !=
// 
// Revision 1.13  1990/09/01  13:30:17  zeller
// 'PrimBox' durch 'PrimitiveBox' ersetzt
// 
// Revision 1.12  1990/08/28  14:22:47  zeller
// Umstellung auf display() mit Regions
// 
// Revision 1.11  1990/08/25  14:20:29  zeller
// dup() entfernt
// 
// Revision 1.10  1990/08/21  11:51:26  zeller
// Unbenutzte Klassen entfernt;
// dup() eingefuehrt
// 
// Revision 1.9  1990/08/13  11:22:16  zeller
// #pragma once eingefuehrt
// 
// Revision 1.8  1990/08/13  10:14:25  zeller
// Typennamen ueberarbeitet
// 
// Revision 1.7  1990/08/10  12:37:50  zeller
// Intrinsic.h eingebunden (fuer XFontStruct)
// 
// Revision 1.6  1990/08/01  21:17:23  zeller
// Neufassung mit neuen Boxen
// 
// Revision 1.5  1990/08/01  14:46:01  zeller
// Umbennennung von _text nach _string
// 
// Revision 1.4  1990/08/01  10:21:34  zeller
// 'String' eingefuehrt
// 
// Revision 1.3  1990/08/01  09:46:21  zeller
// Ausgabe beruecksichtigt jetzt den Ascent eines Fonts
// 
// Revision 1.2  1990/07/25  18:39:56  zeller
// ID korrigiert
// 
// Revision 1.1  1990/07/25  18:36:04  zeller
// Initial revision
// 

#ifndef _Nora_PrimitiveBox_h
#define _Nora_PrimitiveBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Box.h"

// PrimitiveBox

class PrimitiveBox: public Box {
public:
    DECLARE_TYPE_INFO

protected:
    PrimitiveBox(const PrimitiveBox& box):
	Box(box)
    {}

public:
    PrimitiveBox(BoxSize s = BoxSize(0,0), BoxExtend e = BoxExtend(0, 0),
	char *t = "PrimitiveBox"):
	Box(s, e, t)
    {}
};


// RuleBox

class RuleBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const;

    RuleBox(const RuleBox& box):
	PrimitiveBox(box)
    {}

    void dump(ostream& s) const;

public:
    RuleBox(BoxSize s = BoxSize(0, 0), BoxExtend e = BoxExtend(1, 1),
	char *t = "RuleBox"): 
	PrimitiveBox(s, e, t) 
    {}

    Box *dup() const { return new RuleBox(*this); }
    void _print(ostream& os, 
		const BoxRegion& r, 
		const BoxPrintGC& gc) const;
};


// EmptyBox

class EmptyBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const
    {
	// do nothing
    }

    EmptyBox(const EmptyBox& box):
	PrimitiveBox(box)
    {}

public:
    void _print(ostream& os, 
		const BoxRegion& r, 
		const BoxPrintGC& gc) const
    {
	// do nothing
    }

    EmptyBox(BoxSize s = BoxSize(0,0), BoxExtend e = BoxExtend(0, 0),
	char *t = "EmptyBox"):
	PrimitiveBox(s, e, t)
    {}
};


// FillBox

class FillBox: public EmptyBox {
public:
    DECLARE_TYPE_INFO

protected:
    FillBox(const FillBox& box):
	EmptyBox(box)
    {}

    void dump(ostream& s) const;

public:
    FillBox(BoxSize s = BoxSize(0, 0), BoxExtend e = BoxExtend(1, 1)):
	EmptyBox(s, e) 
    {}

    Box *dup() const { return new FillBox(*this); }
};


// SpaceBox

class SpaceBox: public EmptyBox {
public:
    DECLARE_TYPE_INFO

protected:
    SpaceBox(const SpaceBox& box):
	EmptyBox(box)
    {}

    void dump(ostream& s) const;

public:
    SpaceBox(BoxSize s = BoxSize(0, 0)):
	EmptyBox(s, BoxExtend(0, 0)) 
    {}

    Box *dup() const { return new SpaceBox(*this); }
};


// SquareBox

class SquareBox: public SpaceBox {
public:
    DECLARE_TYPE_INFO

protected:
    SquareBox(const SquareBox& box):
	SpaceBox(box)
    {}

    void dump(ostream& s) const;

public:
#ifdef GCC_OK
    SquareBox(BoxCoordinate s): 
	SpaceBox(BoxSize(s, s)) 
    {}
#else
    // Patch for GCC Version 2.3.3
    SquareBox(BoxCoordinate s)
    {
	thesize() = s;
    }
#endif

    Box *dup() const { return new SquareBox(*this); }
};

#endif
