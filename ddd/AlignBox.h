// $Id$
// Klasse AlignBox (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_AlignBox_h
#define _DDD_AlignBox_h

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
	if (nchildren() == 1)
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

    AlignBox& operator += (Box *b)
    {
	addChild(b);
	return *this;
    }
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

    Box *dup() const  { return new HAlignBox(*this); }
    Box *dup0() const { return new HAlignBox; }

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

    Box *dup() const  { return new VAlignBox(*this); }
    Box *dup0() const { return new VAlignBox; }

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

    Box *dup() const  { return new UAlignBox(*this); }
    Box *dup0() const { return new UAlignBox; }

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

    Box *dup() const  { return new TAlignBox(*this); }
    Box *dup0() const { return new TAlignBox; }

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
