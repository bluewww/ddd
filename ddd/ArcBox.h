// $Id$
// Arc boxes

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

#ifndef _DDD_ArcBox_h
#define _DDD_ArcBox_h

#include "LineBox.h"
#include "Widget.h"


// BoxDegrees

typedef int BoxDegrees;  // -360..+360


// ArcBox

class ArcBox: public LineBox {
public:
    DECLARE_TYPE_INFO

private:
    BoxDegrees _start;     // Start of arc
    BoxDegrees _length;    // Length of arc

    void __draw(Widget w, 
		const BoxRegion& region, 
		const BoxRegion& exposed,
		GC gc, 
		bool context_selected) const;

    void _printPS(std::ostream& os, 
		  const BoxRegion& region, 
		  const PrintGC& gc) const;
    void _printFig(std::ostream& os, 
		   const BoxRegion& region, 
		   const PrintGC& gc) const;
	
protected:
    ArcBox(const ArcBox& box):
	LineBox(box), _start(box._start), _length(box._length)
    {}

    void dump(std::ostream& s) const;

    bool matches (const Box &b, const Box * = 0) const
    {
	return LineBox::matches(b) &&
	    _start == ((const ArcBox *)&b)->_start &&
	    _length == ((const ArcBox *)&b)->_length; // dirty trick
    }

public:
    ArcBox(BoxDegrees start, BoxDegrees length,
	   BoxCoordinate linethickness = 1, const char * t = "ArcBox")
	: LineBox(linethickness, t), _start(start), _length(length)
    {}

    Box *dup() const { return new ArcBox(*this); }

    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const;

    // Resources
    BoxDegrees start() const { return _start; }
    BoxDegrees length() const { return _length; }
};

#endif
