// $Id$
// Klasse ArcBox (Deklaration)

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

#ifndef _DDD_ArcBox_h
#define _DDD_ArcBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "LineBox.h"
#include "Widget.h"


// BoxDegrees

typedef int BoxDegrees;  // -360..+360


// ArcBox

class ArcBox: public LineBox {
public:
    DECLARE_TYPE_INFO

private:
    BoxDegrees _start;     // Beginn Bogen
    BoxDegrees _length;    // Laenge Bogen

    void __draw(Widget w, 
		const BoxRegion& region, 
		const BoxRegion& exposed,
		GC gc, 
		bool context_selected) const;

    void _printPS(ostream& os, 
		  const BoxRegion& region, 
		  const BoxPrintGC& gc) const;
    void _printFig(ostream& os, 
		   const BoxRegion& region, 
		   const BoxPrintGC& gc) const;
	
protected:
    ArcBox(const ArcBox& box):
	LineBox(box), _start(box._start), _length(box._length)
    {}

    void dump(ostream& s) const;

    bool matches (const Box &b, const Box * = 0) const
    {
	return LineBox::matches(b) &&
	    _start == ((ArcBox *)&b)->_start &&
	    _length == ((ArcBox *)&b)->_length; // dirty trick
    }

public:
    ArcBox(BoxDegrees start, BoxDegrees length, BoxCoordinate linethickness = 1, 
	char * t = "ArcBox"):
	LineBox(linethickness, t), _start(start), _length(length)
    {}

    Box *dup() const { return new ArcBox(*this); }

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const;

    // Ressourcen
    BoxDegrees start() const { return _start; }
    BoxDegrees length() const { return _length; }
};

#endif
