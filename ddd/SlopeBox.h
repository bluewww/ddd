// $Id$
// Klasse SlopeBox (Deklaration)

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
// Revision 10.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.17  1994/09/05  13:50:07  zeller
// New: destructors provided
//
// Revision 10.16  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.15  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.14  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.13  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.12  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.11  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:15:43  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  10:52:09  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/20  22:57:58  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.7  1993/04/16  11:31:56  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1993/02/23  16:17:51  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.5  1992/07/22  10:52:39  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:56:05  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:54:06  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:09  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:44  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:49:02  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:18  zeller
// Installierte Version
// 
// Revision 1.12  1991/06/09  17:37:48  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.11  1991/06/07  19:17:43  zeller
// _draw() in __draw() umbenannt
// (_draw() wird in LineBox aufgerufen)
// 
// Revision 1.10  1991/05/09  15:17:53  zeller
// Warnungen vermieden
// 
// Revision 1.9  1991/02/08  16:29:53  zeller
// dump() neu definiert
// 
// Revision 1.8  1991/01/28  19:08:57  zeller
// dup() eingefuehrt
// 
// Revision 1.7  1990/09/16  13:01:20  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.6  1990/09/11  10:55:05  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.5  1990/09/07  14:31:19  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.4  1990/08/28  14:22:50  zeller
// Umstellung auf display() mit Regions
// 
// Revision 1.3  1990/08/21  11:51:34  zeller
// dup() eingefuehrt
// 
// Revision 1.2  1990/08/13  11:22:21  zeller
// #pragma once eingefuehrt
// 
// Revision 1.1  1990/08/13  11:18:05  zeller
// Initial revision
// 

#ifndef _Nora_SlopeBox_h
#define _Nora_SlopeBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "LineBox.h"
#include "Widget.h"


// SlopeBox

class SlopeBox: public LineBox {
public:
    DECLARE_TYPE_INFO

protected:
    SlopeBox(const SlopeBox& box):
	LineBox(box)
    {}

public:
    SlopeBox(BoxCoordinate linethickness = 1, char *t = "SlopeBox"):
	LineBox(linethickness, t) 
    {}
    ~SlopeBox()
    {}
};


// RiseBox

class RiseBox: public SlopeBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void __draw(Widget w, 
			const BoxRegion& region, 
			const BoxRegion& exposed,
			GC gc, 
			bool context_selected) const;

    RiseBox(const RiseBox& box):
	SlopeBox(box)
    {}

    void dump(ostream& s) const;

public:
    RiseBox(BoxCoordinate linethickness = 1, char *t = "RiseBox"):
	SlopeBox(linethickness, t)
    {}
    ~RiseBox()
    {}

    Box *dup() const { return new RiseBox(*this); }

    void _print(ostream& os, 
		const BoxRegion& region, 
		const BoxPrintGC& gc) const;
};


// FallBox

class FallBox: public SlopeBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void __draw(Widget w, 
			const BoxRegion& region, 
			const BoxRegion& exposed,
			GC gc, 
			bool context_selected) const;

    FallBox(const FallBox& box):
	SlopeBox(box)
    {}

    void dump(ostream& s) const;

public:
    FallBox(BoxCoordinate linethickness = 1, char *t = "FallBox"):
	SlopeBox(linethickness, t)
    {}
    ~FallBox()
    {}

    Box *dup() const { return new FallBox(*this); }

    void _print(ostream& os, 
		const BoxRegion& region, 
		const BoxPrintGC& gc) const;
};

#endif
