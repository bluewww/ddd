// $Id$  -*- C++ -*-
// BoxSize class

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
// Revision 1.5  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.4  1993/12/07  10:44:02  zeller
// Fix: Anpassung ANSI-Overloading
//
// Revision 1.3  1993/05/22  20:13:46  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.2  1993/04/21  00:10:55  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.1  1993/04/20  23:31:18  zeller
// Initial revision
// 

#ifndef _Nora_BoxSize_h
#define _Nora_BoxSize_h

#ifdef __GNUG__
#pragma interface
#endif


// BoxSize beschreibt Groessen (insbesondere von Boxen).

#include "BoxPoint.h"
#include "misc.h" // max()

class BoxSize: public BoxPoint {
public:
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

    BoxSize operator & (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(point[X] + s.point[X],
			   max(point[Y], s.point[Y]));
	else
	    return BoxSize();
    }
    BoxSize operator | (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(max(point[X], s.point[X]),
			   point[Y] + s.point[Y]);
	else
	    return BoxSize();
    }
    BoxSize operator ^ (const BoxSize& s) 
    {
	if (isValid() && s.isValid())
	    return BoxSize(max(point[X], s.point[X]),
			   max(point[Y], s.point[Y]));
	else
	    return BoxSize();
    }

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

inline ostream& operator << (ostream& stream, const BoxSize& size)
{
    const BoxPoint& p = size;
    return operator << (stream, p);
}

#define NoSize BoxSize()

#endif // _Nora_BoxSize_h
// DON'T ADD ANYTHING BEHIND THIS #endif
