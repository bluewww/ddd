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
