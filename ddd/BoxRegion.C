// $Id$  -*- C++ -*-
// BoxRegion class

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

char BoxRegion_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BoxRegion.h"

// Schnittmenge zweier Regionen berechnen
BoxRegion BoxRegion::operator & (const BoxRegion& r) const
{
    BoxPoint nw_corner(max(origin(X), r.origin(X)),
		    max(origin(Y), r.origin(Y)));
    BoxPoint se_corner(min(origin(X) + space(X), r.origin(X) + r.space(X)),
		    min(origin(Y) + space(Y), r.origin(Y) + r.space(Y)));

    return BoxRegion(nw_corner, BoxSize(se_corner - nw_corner));
}

// Vereinigung zweier Regionen berechnen
BoxRegion BoxRegion::operator | (const BoxRegion& r) const
{
    BoxPoint nw_corner(min(origin(X), r.origin(X)),
		    min(origin(Y), r.origin(Y)));
    BoxPoint se_corner(max(origin(X) + space(X), r.origin(X) + r.space(X)),
		    max(origin(Y) + space(Y), r.origin(Y) + r.space(Y)));

    return BoxRegion(nw_corner, BoxSize(se_corner - nw_corner));
}

// BoxRegion ausgeben
ostream& operator << (ostream& s, const BoxRegion& r)
{
    return s << r.origin() << r.space();
}
