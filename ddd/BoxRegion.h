// $Id$  -*- C++ -*-
// BoxRegion class

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_BoxRegion_h
#define _DDD_BoxRegion_h

#ifdef __GNUG__
#pragma interface
#endif


// A BoxRegion is a rectangular area which has an ORIGIN and a SPACE.

#include <iostream.h>
#include "misc.h"
#include "BoxPoint.h"
#include "BoxSize.h"
#include "bool.h"
#include "BoxD.h"
#include "BoxC.h"

class BoxRegion {
private:
    BoxPoint _origin;
    BoxSize  _space;

public:
    // Constructors
    BoxRegion(const BoxPoint& o, const BoxSize& s):
	_origin(o), _space(s)
    {}

    BoxRegion(const BoxPoint& o):
	_origin(o)
    {}

    BoxRegion()
    {}

    BoxRegion(const BoxRegion& r):
	_origin(r._origin), _space(r._space)
    {}

    // Resources
    BoxPoint& origin()                    { return _origin; }
    BoxCoordinate& origin(BoxDimension d) { return _origin[d]; }
    BoxSize& space()                      { return _space; }
    BoxCoordinate& space(BoxDimension d)  { return _space[d]; }

    const BoxPoint& origin() const                    { return _origin; }
    const BoxCoordinate& origin(BoxDimension d) const { return _origin[d]; }
    const BoxSize& space() const                      { return _space; }
    const BoxCoordinate& space(BoxDimension d) const  { return _space[d]; }

    // Comparison
    bool operator == (const BoxRegion& r) const
    { return origin() == r.origin() && space() == r.space(); }
    bool operator != (const BoxRegion& r) const
    { return origin() != r.origin() || space() != r.space(); }

    // Check if BoxRegion is (partially) contained in R
    bool operator <= (const BoxRegion& r) const
    {
	for (BoxDimension d = X; d <= Y; d++)
	    if (r.origin(d) >= origin(d) + space(d) ||
		origin(d) >= r.origin(d) + r.space(d))
	    return false;

	return true;
    }

    // Rectangular intersection of two regions
    BoxRegion operator & (const BoxRegion& r) const;

    // Rectangular union of two regions
    BoxRegion operator | (const BoxRegion& r) const;

    // Check if region is empty
    bool isEmpty() const
    {
	return space() <= BoxSize(0,0);
    }

    // Debugging
    friend ostream& operator << (ostream& s, const BoxRegion& r);
};

// Check if P is contained in R
inline bool operator <= (const BoxPoint& p, const BoxRegion& r)
{
    for (BoxDimension d = X; d <= Y; d++)
	if (p[d] < r.origin(d) || p[d] >= r.origin(d) + r.space(d))
	    return false;

    return true;
}

#endif // _DDD_BoxRegion_h
// DON'T ADD ANYTHING BEHIND THIS #endif
