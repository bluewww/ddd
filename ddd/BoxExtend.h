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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _Nora_BoxExtend_h
#define _Nora_BoxExtend_h

#ifdef __GNUG__
#pragma interface
#endif


// BoxExtend beschreibt Ausdehnungsfaehigkeiten (insbesondere von Boxen).
// Eine Ausdehnung von 0 heisst: keine Ausdehnung moeglich.
// Je groesser eine Ausdehnung, um so mehr wird der betreffenden Box
// freier Restplatz zugewiesen. Hat etwa im Ausdruck a & b
// die Box a eine Ausdehnungsfaehigkeit von 1 und die Box b eine von 2, 
// so wird a 1/3 und b 2/3 des freien Restplatzes zugewiesen.

#include "BoxPoint.h"
#include "misc.h" // min(), max()

class BoxExtend: public BoxPoint {
public:
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

    BoxExtend operator & (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(point[X] + e.point[X],
			     min(point[Y],e.point[Y]));
	else
	    return BoxExtend();
    }

    BoxExtend operator | (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(min(point[X], e.point[X]),
			     point[Y] + e.point[Y]);
	else
	    return BoxExtend();
    }

    BoxExtend operator ^ (const BoxExtend& e) 
    {
	if (isValid() && e.isValid())
	    return BoxExtend(max(point[X], e.point[X]),
			     max(point[Y], e.point[Y]));
	else
	    return BoxExtend();
    }

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

    void fix (BoxDimension dimension) 
    {
	if (isValid())
	    point[dimension] = 0;
    }
};


inline ostream& operator << (ostream& stream, const BoxExtend& extend)
{
    const BoxPoint& p = extend;
    return operator << (stream, p);
}

#define NoExtend BoxExtend()

#endif // _Nora_BoxExtend_h
// DON'T ADD ANYTHING BEHIND THIS #endif
