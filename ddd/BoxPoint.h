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

// $Log$
// Revision 1.1  1995/05/01 15:47:33  zeller
// Initial revision
//
// Revision 1.6  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.5  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.4  1993/05/22  20:13:37  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.3  1993/04/21  00:25:39  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.2  1993/04/21  00:10:54  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.1  1993/04/20  23:35:25  zeller
// Initial revision
// 

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

    // true, wenn beide Dimensionen die Bedingung erfuellen
    friend static bool operator == (const BoxPoint& p1, const BoxPoint& p2);

    // true, wenn eine BoxDimension die Bedingung erfuellt
    friend static bool operator != (const BoxPoint& p1, const BoxPoint& p2);
    friend static bool operator > (const BoxPoint& p1, const BoxPoint& p2);
    friend static bool operator < (const BoxPoint& p1, const BoxPoint& p2);
    friend static bool operator >= (const BoxPoint& p1, const BoxPoint& p2);
    friend static bool operator <= (const BoxPoint& p1, const BoxPoint& p2);

    friend ostream& operator << (ostream& stream, 
				 const BoxPoint& point);
};

inline bool operator == (const BoxPoint& p1, const BoxPoint& p2)
{
    return p1.point[X] == p2.point[X] && p1.point[Y] == p2.point[Y];
}

inline bool operator != (const BoxPoint& p1, const BoxPoint& p2)
{
    return p1.point[X] != p2.point[X] || p1.point[Y] != p2.point[Y];
}

inline bool operator > (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1.point[X] > p2.point[X] || p1.point[Y] > p2.point[Y];
    else
	return false;
}

inline bool operator < (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1.point[X] < p2.point[X] || p1.point[Y] < p2.point[Y];
    else
	return false;
}

inline bool operator >= (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1.point[X] >= p2.point[X] || p1.point[Y] >= p2.point[Y];
    else
	return false;
}

inline bool operator <= (const BoxPoint& p1, const BoxPoint& p2)
{
    if (p1.isValid() && p2.isValid())
	return p1.point[X] <= p2.point[X] || p1.point[Y] <= p2.point[Y];
    else
	return false;
}

#define NoPoint Point()

#endif // _Nora_BoxPoint_h
// DON'T ADD ANYTHING BEHIND THIS #endif
