// $Id$
// Klasse LineBox (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_LineBox_h
#define _DDD_LineBox_h

#ifdef __GNUG__
#pragma interface
#endif


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
	    _linethickness == ((LineBox *)&b)->_linethickness;  // dirty trick
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
    LineBox(BoxCoordinate linethickness = 1, char *t = "LineBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(true, true), t),
	_linethickness(linethickness)
    {}
    
    BoxCoordinate linethickness() const { return _linethickness; }
};


#endif
