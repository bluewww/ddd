// $Id$ -*- C++ -*-
// Edge annotation with boxes

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char BoxEdgeAnnotation_rcsid[] = 
    "$Id$";

#include "BoxEdgeA.h"

DEFINE_TYPE_INFO_1(BoxEdgeAnnotation, EdgeAnnotation)

BoxRegion BoxEdgeAnnotation::region(const BoxPoint& p, const GraphGC& gc) const
{
    // Center box horizontally 2 pixels above P
    BoxPoint origin(p);
    origin[X] -= size(gc)[X] / 2;
    origin[Y] -= size(gc)[Y] + 2;

    return BoxRegion(origin, size(gc));
}

void BoxEdgeAnnotation::_draw(Widget w, const BoxPoint& p,
			      const BoxRegion& exposed, 
			      const GraphGC& gc) const
{
    box()->draw(w, region(p, gc), exposed, gc.edgeGC);
}


// Print
void BoxEdgeAnnotation::_print(std::ostream& os, const BoxPoint& p,
			       const GraphGC& gc) const
{
    box()->_print(os, region(p, gc), *gc.printGC);
}
