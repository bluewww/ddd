// $Id$
// Line boxes

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

#ifndef _DDD_LineBox_h
#define _DDD_LineBox_h

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

    bool matches (const Box &b, const Box * = 0) const
    {
	return PrimitiveBox::matches(b) &&
	    _linethickness == ((const LineBox *)&b)->_linethickness;  // dirty trick
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
    LineBox(BoxCoordinate linethickness = 1, const char *t = "LineBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(1, 1), t),
	_linethickness(linethickness)
    {}
    
    BoxCoordinate linethickness() const { return _linethickness; }
};


#endif
