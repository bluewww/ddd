// $Id$
// Primitive boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_PrimitiveBox_h
#define _DDD_PrimitiveBox_h

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
	const char *t = "PrimitiveBox"):
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

    void dump(std::ostream& s) const;

public:
    RuleBox(BoxSize s = BoxSize(0, 0), BoxExtend e = BoxExtend(1, 1),
	const char *t = "RuleBox"): 
	PrimitiveBox(s, e, t) 
    {}

    Box *dup() const { return new RuleBox(*this); }
    void _print(std::ostream& os, 
		const BoxRegion& r, 
		const PrintGC& gc) const;
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
    void _print(std::ostream&, 
		const BoxRegion&, 
		const PrintGC&) const
    {
	// do nothing
    }

    EmptyBox(BoxSize s = BoxSize(0,0), BoxExtend e = BoxExtend(0, 0),
	const char *t = "EmptyBox"):
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

    void dump(std::ostream& s) const;

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

    void dump(std::ostream& s) const;

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

    void dump(std::ostream& s) const;

public:
    SquareBox(BoxCoordinate s): 
	SpaceBox(BoxSize(s, s)) 
    {}

    Box *dup() const { return new SquareBox(*this); }
};

#endif
