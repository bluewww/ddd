// $Id$  -*- C++ -*-
// BoxPoint class

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _Nora_BoxPoint_h
#define _Nora_BoxPoint_h

#ifdef __GNUG__
#pragma interface
#endif

// BoxPoint beschreibt Paare von Koordinaten, die per BoxDimension (X oder Y)
// angesprochen werden. BoxPoint wird typischerweise fuer Positionen
// benutzt, aber auch als Basisklasse fuer Groessen (BoxSize)
// und Ausdehnungsfaehigkeiten (BoxExtend).

#include "BoxC.h"
#include "BoxD.h"
#include "bool.h"
#include <iostream.h>
class BoxRegion;

#define CHECK_VALID(p) { if (!isValid() || !p.isValid()) return Point(); }

class BoxPoint {
protected:
    BoxCoordinate point[NDimensions];
public:
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

    bool isValid() const
    {
	return point[X] != NoCoordinate && point[Y] != NoCoordinate;
    }

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

    const BoxCoordinate& operator [] (const BoxDimension& dimension) const
    {
	return point[dimension];
    }
    BoxCoordinate& operator [] (const BoxDimension& dimension)
    {
	return point[dimension];
    }

    friend ostream& operator << (ostream& stream, 
				 const BoxPoint& point);
};

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

#define NoPoint Point()

#endif // _Nora_BoxPoint_h
// DON'T ADD ANYTHING BEHIND THIS #endif
