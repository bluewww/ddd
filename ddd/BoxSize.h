// $Id$  -*- C++ -*-
// BoxSize class

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

#ifndef _DDD_BoxSize_h
#define _DDD_BoxSize_h

#ifdef __GNUG__
#pragma interface
#endif


// A BoxSize denotes a two-dimensional size (especially box sizes).

#include "BoxPoint.h"
#include "misc.h" // max()

class BoxSize: public BoxPoint {
public:
    // Constructors
    BoxSize(BoxCoordinate x_size, BoxCoordinate y_size): 
	BoxPoint(x_size, y_size) 
    {}
    BoxSize(BoxCoordinate c): 
	BoxPoint(c) 
    {}
    BoxSize():
	BoxPoint()
    {}
    BoxSize(const BoxPoint& p): 
	BoxPoint(p) 
    {}

    // Operators
    // Horizontal concatenation
    BoxSize operator & (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(point[X] + s.point[X],
			   max(point[Y], s.point[Y]));
	else
	    return BoxSize();
    }

    // Vertical concatenation
    BoxSize operator | (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(max(point[X], s.point[X]),
			   point[Y] + s.point[Y]);
	else
	    return BoxSize();
    }

    // Stacked concatenation (maximum size)
    BoxSize operator ^ (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(max(point[X], s.point[X]),
			   max(point[Y], s.point[Y]));
	else
	    return BoxSize();
    }

    // Assignment versions
    void operator &= (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	{
	    point[X] += s.point[X];
	    point[Y] = max(point[Y], s.point[Y]);
	}
    }
    void operator |= (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	{
	    point[X] = max(point[X], s.point[X]);
	    point[Y] += s.point[Y];
	}
    }
    void operator ^= (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	{
	    point[X] = max(point[X], s.point[X]);
	    point[Y] = max(point[Y], s.point[Y]);
	}
    }
};

// I/O
inline ostream& operator << (ostream& stream, const BoxSize& size)
{
    const BoxPoint& p = size;
    return operator << (stream, p);
}

#endif // _DDD_BoxSize_h
// DON'T ADD ANYTHING BEHIND THIS #endif
