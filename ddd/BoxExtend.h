// $Id$  -*- C++ -*-
// BoxExtend class

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

#ifndef _DDD_BoxExtend_h
#define _DDD_BoxExtend_h

#ifdef __GNUG__
#pragma interface
#endif

// A `BoxExtend' denotes the extensibility of a box - that is, how
// much the box extends to fill up available space.  An extensibility
// of 0 means that the box will not extend at all.  The bigger the
// extensibility, the more remaining space is assigned to the box.
// For example, let A and B be two boxes; A has a horizontal
// extensibility of 1, B has a horizontal extensibility of 2.  If A
// and B are horizontally concatenated, A gets 1/3 of the remaining
// space and B gets 2/3 of the remaining space.

#include "BoxPoint.h"
#include "misc.h" // min(), max()

class BoxExtend: public BoxPoint {
public:
    // Constructors
    BoxExtend(BoxCoordinate x_extend, BoxCoordinate y_extend):
	BoxPoint(x_extend, y_extend)
    {}
    BoxExtend(BoxCoordinate c): 
	BoxPoint(c)
    {}
    BoxExtend():
	BoxPoint()
    {}
    BoxExtend(const BoxPoint& p):
	BoxPoint(p)
    {}

    // Horizontal concatenation
    BoxExtend operator & (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(point[X] + e.point[X],
			     min(point[Y],e.point[Y]));
	else
	    return BoxExtend();
    }

    // Vertical concatenation
    BoxExtend operator | (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(min(point[X], e.point[X]),
			     point[Y] + e.point[Y]);
	else
	    return BoxExtend();
    }

    // Stacked concatenation
    BoxExtend operator ^ (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(max(point[X], e.point[X]),
			     max(point[Y], e.point[Y]));
	else
	    return BoxExtend();
    }

    // Assignment versions
    void operator &= (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	{
	    point[X] += e.point[X];
	    point[Y] = min(point[Y], e.point[Y]);
	}
    }
    void operator |= (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	{
	    point[X] = min(point[X], e.point[X]);
	    point[Y] += e.point[Y];
	}
    }
    void operator ^= (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	{
	    point[X] = max(point[X], e.point[X]);
	    point[Y] = max(point[Y], e.point[Y]);
	}
    }

    // Set extensibility to zero
    void fix (BoxDimension dimension) 
    {
	if (isValid())
	    point[dimension] = 0;
    }
};

// I/O
inline ostream& operator << (ostream& stream, const BoxExtend& extend)
{
    const BoxPoint& p = extend;
    return operator << (stream, p);
}

#endif // _DDD_BoxExtend_h
// DON'T ADD ANYTHING BEHIND THIS #endif
