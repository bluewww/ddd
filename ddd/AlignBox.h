// $Id$
// Klasse AlignBox (Deklaration)

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
// Revision 1.1  1995/05/01 15:47:29  zeller
// Initial revision
//
// Revision 10.21  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.20  1994/09/05  06:31:37  zeller
// Fix: copy constructors made const correct
//
// Revision 10.19  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.18  1994/07/19  21:59:50  zeller
// New: draw() const-corrected
//
// Revision 10.17  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.16  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.15  1994/02/19  20:19:44  zeller
// Neu: nchild() statt nson() (PC schlaegt zu)
//
// Revision 10.14  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.13  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.12  1993/05/22  19:53:30  zeller
// %Q% added
//
// Revision 10.11  1993/04/21  00:10:48  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.10  1993/04/20  22:57:38  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.9  1993/04/16  11:31:20  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.8  1993/02/23  16:17:39  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.7  1992/07/22  10:52:19  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.6  1992/06/02  02:55:42  zeller
// Neu: #pragma interface
// 
// Revision 10.5  1992/06/01  13:08:12  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.4  1992/06/01  12:53:25  zeller
// #pragma once entfernt
// 
// Revision 10.3  1992/05/20  15:36:52  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:27  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:01  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:41:55  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:20  zeller
// Installierte Version
// 
// Revision 1.9  1991/06/09  17:38:05  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.8  1991/05/09  15:07:40  zeller
// Warnungen vermieden
// 
// Revision 1.7  1991/05/05  16:43:57  zeller
// Ausgabe vertikales Alignment mit "|"
// 
// Revision 1.6  1991/04/30  16:00:30  zeller
// corner(), TAlignBox wieder eingefuehrt
// 
// Revision 1.5  1991/02/24  15:10:43  zeller
// corner, TAlignBox entfernt
// 
// Revision 1.4  1991/02/21  12:07:08  zeller
// Default-Ausgabe VAlignBox auf Operator ! umgestellt
// 
// Revision 1.3  1991/02/17  14:56:46  zeller
// TAlignBox::dump() korrigiert
// 
// Revision 1.2  1991/02/08  16:31:42  zeller
// dump() neu definiert
// 
// Revision 1.1  1991/02/01  14:56:38  zeller
// Initial revision
// 

#ifndef _Nora_AlignBox_h
#define _Nora_AlignBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine AlignBox ist ein Behaelter fuer eine Anordnung von Boxen.
// Eine HAlignBox ordnet Boxen nebeneinander ("horizontal") an (in VSL: &)
// eine VAlignbox uebereinander ("vertikal") (in VSL: |),
// eine UAlignBox an der gleichen Stelle (in VSL: ^) und
// eine TAlignBox textuell (in VSL: ~), d.h. wie Buchstaben
// aneinandergereiht.


#include "assert.h"
#include <values.h>
#include "CompositeB.h"
#include "Widget.h"


// AlignBox

class AlignBox: public CompositeBox {
public:
    DECLARE_TYPE_INFO

protected:
    BoxSize _corner; // Platz in unterer rechter Ecke

    AlignBox(const AlignBox& box):
	CompositeBox(box), _corner(box._corner)
    {}

    // Groesse uebernehmen
    void setSize(Box *b)
    {
	thesize()   = b->size();
	theextend() = b->extend();
	_corner     = b->corner();
    }

    // Groesse hinzufuegen
    virtual void addSize(Box *b) = 0;

    // Groesse neu berechnen
    Box *resize();

    void addChild(Box *b)
    // Box als Sohn hinzufuegen
    {
	// Sohn in CompositeBox eintragen
	CompositeBox::addChild(b);

	// Groesse hinzufuegen (bzw. setzen, wenn erster)
	if (nsons() == 1)
	    setSize(b);     // erster Sohn: Groesse uebernehmen
	else
	    addSize(b);     // spaeterer Sohn: Groesse hinzufuegen
    }

    // Alignment (X/Y) anzeigen
    void drawAlign(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed,
		   GC gc, 
		   bool context_selected, 
		   BoxDimension dimen) const;

public:
    AlignBox(unsigned initialSize = 2, 
	char *t = "AlignBox"):
	CompositeBox(initialSize, t), _corner(0,0)
    {}

    BoxSize corner() const { return _corner; }
};


// HAlignBox

class HAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

protected:
    HAlignBox(const HAlignBox& box):
	AlignBox(box)
    {}

    void dump(ostream& s) const { dumpComposite(s, " & "); }

    virtual void _draw(Widget w, 
		       const BoxRegion& r, 
		       const BoxRegion& exposed,
		       GC gc, bool context_selected) const
    { 
	drawAlign(w, r, exposed, gc, context_selected, X); 
    }

public:
    HAlignBox(unsigned initialSize = 2, char *t = "HAlignBox"):
	AlignBox(initialSize, t) 
    {}

    Box *dup() const { return new HAlignBox(*this); }

    void addSize(Box *b);

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    HAlignBox& operator &= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// VAlignBox

class VAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& r, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const
    { 
	drawAlign(w, r, exposed, gc, context_selected, Y); 
    }

protected:
    VAlignBox(const VAlignBox& box):
	AlignBox(box)
    {}

    void dump(ostream& s) const { dumpComposite(s, " | "); }

public:
    VAlignBox(unsigned initialSize = 2, char *t = "VAlignBox"):
	    AlignBox(initialSize, t)
    {}

    Box *dup() const { return new VAlignBox(*this); }

    void addSize(Box *b);

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    VAlignBox& operator |= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// UAlignBox

class UAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& region, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const;

protected:
    UAlignBox(const UAlignBox& box):
	AlignBox(box)
    {}

    void dump(ostream& s) const { dumpComposite(s, " ^ "); }

public:
    UAlignBox(unsigned initialSize = 2, char *t = "UAlignBox"):
	AlignBox(initialSize, t)
    {}

    Box *dup() const { return new UAlignBox(*this); }

    void addSize(Box *b);

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    UAlignBox& operator ^= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// TAlignBox

class TAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& region, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const;

protected:
    TAlignBox(const TAlignBox& box):
	AlignBox(box)
    {}

    void dump(ostream& s) const { dumpComposite(s, " ~ "); }

public:
    TAlignBox(unsigned initialSize = 2, char *t = "TAlignBox"):
	AlignBox(initialSize, t)
    {}

    Box *dup() const { return new TAlignBox(*this); }

    void addSize(Box *b);

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    TAlignBox& operator &= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


#endif
