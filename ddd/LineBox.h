// $Id$
// Klasse LineBox (Deklaration)

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

#ifndef _Nora_LineBox_h
#define _Nora_LineBox_h

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

    bool matches (const Box &b, const Box *callbackArg = 0) const
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
