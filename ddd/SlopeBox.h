// $Id$
// Klasse SlopeBox (Deklaration)

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

#ifndef _DDD_SlopeBox_h
#define _DDD_SlopeBox_h

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
