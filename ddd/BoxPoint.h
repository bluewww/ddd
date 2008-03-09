// $Id$  -*- C++ -*-
// BoxPoint class

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

#ifndef _DDD_BoxPoint_h
#define _DDD_BoxPoint_h

// A BoxPoint is a pair of coordinates accessed by a BoxDimension (X
// or Y).  A BoxPoint is typically used for positions, but is also a
// base class for sizes (BoxSize) and extents (BoxExtend)

#include "BoxC.h"
#include "BoxD.h"
#include "bool.h"
#include <iostream>
class BoxRegion;

#define CHECK_VALID(p) { if (!isValid() || !p.isValid()) return Point(); }

class BoxPoint {
protected:
    BoxCoordinate point[NDimensions];
public:

    // Constructors
    BoxPoint(BoxCoordinate x, BoxCoordinate y) 
    {
	point[X] = x;
	point[Y] = y;
    }
    BoxPoint(BoxCoordinate c) 
    {
	point[X] = point[Y] = c;
    }
    BoxPoint()
    {
	point[X] = point[Y] = NoCoordinate;
    }
    BoxPoint(const BoxPoint& p) 
    {
	point[X] = p.point[X];
	point[Y] = p.point[Y];
    }

    // Check for validity
    bool isValid() const
    {
	return point[X] != NoCoordinate && point[Y] != NoCoordinate;
    }

    // Operators
    BoxPoint operator + (const BoxPoint& p) const
    {
	if (isValid() && p.isValid())
	    return BoxPoint(point[X] + p.point[X], point[Y] + p.point[Y]);
	else
	    return BoxPoint();
    }
    BoxPoint operator - (const BoxPoint& p) const
    {
	if (isValid() && p.isValid())
	    return BoxPoint(point[X] - p.point[X], point[Y] - p.point[Y]);
	else
	    return BoxPoint();
    }
    BoxPoint operator - () const
    {
	if (isValid())
	    return BoxPoint(-point[X], -point[Y]);
	else
	    return BoxPoint();
    }
    BoxPoint operator * (const BoxPoint& p) const
    {
	if (isValid() && p.isValid())
	    return BoxPoint(point[X] * p.point[X], point[Y] * p.point[Y]);
	else
	    return BoxPoint();
    }
    BoxPoint operator / (const BoxPoint& p) const
    {
	if (isValid() && p.isValid())
	    return BoxPoint(point[X] / p.point[X], point[Y] / p.point[Y]);
	else
	    return BoxPoint();
    }
    BoxPoint operator % (const BoxPoint& p) const
    {
	if (isValid() && p.isValid())
	    return BoxPoint(point[X] % p.point[X], point[Y] % p.point[Y]);
	else
	    return BoxPoint();
    }

    void operator += (const BoxPoint& p) 
    {
	if (isValid() && p.isValid())
	{
	    point[X] += p.point[X];
	    point[Y] += p.point[Y];
	}
    }
    void operator -= (const BoxPoint& p) 
    {
	if (isValid() && p.isValid())
	{
	    point[X] -= p.point[X];
	    point[Y] -= p.point[Y];
	}
    }
    void operator *= (const BoxPoint& p) 
    {
	if (isValid() && p.isValid())
	{
	    point[X] *= p.point[X];
	    point[Y] *= p.point[Y];
	}
    }
    void operator /= (const BoxPoint& p) 
    {
	if (isValid() && p.isValid())
	{
	    point[X] /= p.point[X];
	    point[Y] /= p.point[Y];
	}
    }
    void operator %= (const BoxPoint& p) 
    {
	if (isValid() && p.isValid())
	{
	    point[X] %= p.point[X];
	    point[Y] %= p.point[Y];
	}
    }

    // Resources
    const BoxCoordinate& operator [] (const BoxDimension& dimension) const
    {
	return point[dimension];
    }
    BoxCoordinate& operator [] (const BoxDimension& dimension)
    {
	return point[dimension];
    }

    // I/O
    friend std::ostream& operator << (std::ostream& stream, 
				 const BoxPoint& point);
};

// Comparisons
inline bool operator == (const BoxPoint& p1, const BoxPoint& p2)
{
    return p1[X] == p2[X] && p1[Y] == p2[Y];
}

inline bool operator != (const BoxPoint& p1, const BoxPoint& p2)
{
    return p1[X] != p2[X] || p1[Y] != p2[Y];
}

inline bool operator > (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1[X] > p2[X] || p1[Y] > p2[Y];
    else
	return false;
}

inline bool operator < (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1[X] < p2[X] || p1[Y] < p2[Y];
    else
	return false;
}

inline bool operator >= (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1[X] >= p2[X] || p1[Y] >= p2[Y];
    else
	return false;
}

inline bool operator <= (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1[X] <= p2[X] || p1[Y] <= p2[Y];
    else
	return false;
}

#endif // _DDD_BoxPoint_h
// DON'T ADD ANYTHING BEHIND THIS #endif
