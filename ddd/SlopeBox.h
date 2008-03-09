// $Id$
// Slope boxes

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

#ifndef _DDD_SlopeBox_h
#define _DDD_SlopeBox_h

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
    SlopeBox(BoxCoordinate linethickness = 1, const char *t = "SlopeBox"):
	LineBox(linethickness, t) 
    {}
    ~SlopeBox()
    {}
};


// RiseBox (`/')

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

    void dump(std::ostream& s) const;

public:
    RiseBox(BoxCoordinate linethickness = 1, const char *t = "RiseBox"):
	SlopeBox(linethickness, t)
    {}
    ~RiseBox()
    {}

    Box *dup() const { return new RiseBox(*this); }

    void _print(std::ostream& os, 
		const BoxRegion& region, 
		const PrintGC& gc) const;
};


// FallBox (`\')

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

    void dump(std::ostream& s) const;

public:
    FallBox(BoxCoordinate linethickness = 1, const char *t = "FallBox"):
	SlopeBox(linethickness, t)
    {}
    ~FallBox()
    {}

    Box *dup() const { return new FallBox(*this); }

    void _print(std::ostream& os, 
		const BoxRegion& region, 
		const PrintGC& gc) const;
};

#endif
