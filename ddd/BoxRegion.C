// $Id$  -*- C++ -*-
// BoxRegion class

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

char BoxRegion_rcsid[] = 
    "$Id$";

#include "BoxRegion.h"

// Compute intersection of two regions
BoxRegion BoxRegion::operator & (const BoxRegion& r) const
{
    if (!origin().isValid())
	return r;
    if (!r.origin().isValid())
	return *this;

    BoxPoint nw_corner(max(origin(X), r.origin(X)),
		    max(origin(Y), r.origin(Y)));
    BoxPoint se_corner(min(origin(X) + space(X), r.origin(X) + r.space(X)),
		    min(origin(Y) + space(Y), r.origin(Y) + r.space(Y)));

    return BoxRegion(nw_corner, BoxSize(se_corner - nw_corner));
}

// Compute union of two regions
BoxRegion BoxRegion::operator | (const BoxRegion& r) const
{
    if (!origin().isValid())
	return r;
    if (!r.origin().isValid())
	return *this;

    BoxPoint nw_corner(min(origin(X), r.origin(X)),
		    min(origin(Y), r.origin(Y)));
    BoxPoint se_corner(max(origin(X) + space(X), r.origin(X) + r.space(X)),
		    max(origin(Y) + space(Y), r.origin(Y) + r.space(Y)));

    return BoxRegion(nw_corner, BoxSize(se_corner - nw_corner));
}

// Dump
std::ostream& operator << (std::ostream& s, const BoxRegion& r)
{
    return s << r.origin() << r.space();
}
