// $Id$
// Klasse PrimitiveBox und folgende (Deklaration)

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_PrimitiveBox_h
#define _DDD_PrimitiveBox_h

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
    virtual void _draw(Widget, 
		       const BoxRegion&, 
		       const BoxRegion&,
		       GC, 
		       bool) const
    {
	// do nothing
    }

    EmptyBox(const EmptyBox& box):
	PrimitiveBox(box)
    {}

public:
    void _print(ostream&, 
		const BoxRegion&, 
		const BoxPrintGC&) const
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
