// $Id$  -*- C++ -*-
// BoxRegion class

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

#ifndef _Nora_BoxRegion_h
#define _Nora_BoxRegion_h

#ifdef __GNUG__
#pragma interface
#endif

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

    BoxPoint& origin()                    { return _origin; }
    BoxCoordinate& origin(BoxDimension d) { return _origin[d]; }
    BoxSize& space()                      { return _space; }
    BoxCoordinate& space(BoxDimension d)  { return _space[d]; }

    const BoxPoint& origin() const                    { return _origin; }
    const BoxCoordinate& origin(BoxDimension d) const { return _origin[d]; }
    const BoxSize& space() const                      { return _space; }
    const BoxCoordinate& space(BoxDimension d) const  { return _space[d]; }

    // Vergleichsoperatoren
    bool operator == (const BoxRegion& r) const
    { return origin() == r.origin() && space() == r.space(); }
    bool operator != (const BoxRegion& r) const
    { return origin() != r.origin() || space() != r.space(); }

    // Pruefen, ob Punkt p in r enthalten ist
    friend static bool operator <= (const BoxPoint& p, const BoxRegion& r);

    // Pruefen, ob BoxRegion (teilweise oder ganz) in r enthalten ist
    bool operator <= (const BoxRegion& r) const
    {
	for (BoxDimension d = X; d <= Y; d++)
	    if (r.origin(d) >= origin(d) + space(d) ||
		origin(d) >= r.origin(d) + r.space(d))
	    return false;

	return true;
    }

    // Schnittmenge zweier Regionen berechnen
    BoxRegion operator & (const BoxRegion& r) const;

    // Vereinigung zweier Regionen berechnen
    BoxRegion operator | (const BoxRegion& r) const;

    // Pruefen, ob leerer Bereich
    bool isEmpty() const
    {
	return space() <= BoxSize(0,0);
    }

    // Debugging
    friend ostream& operator << (ostream& s, const BoxRegion& r);
};

// Pruefen, ob Punkt p in r enthalten ist
inline bool operator <= (const BoxPoint& p, const BoxRegion& r)
{
    for (BoxDimension d = X; d <= Y; d++)
	if (p[d] < r.origin(d) || p[d] >= r.origin(d) + r.space(d))
	    return false;

    return true;
}

#endif // _Nora_BoxRegion_h
// DON'T ADD ANYTHING BEHIND THIS #endif
