// $Id$
// Hat boxes

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

char HatBox_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "HatBox.h"

DEFINE_TYPE_INFO_1(HatBox, Box)

// HatBox

// Draw
void HatBox::_draw(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed, GC gc,
		   bool context_selected) const
{
    BoxRegion childRegion = r;

    // If not extensible, shrink to minimal size
    if (extend(X) == 0)
	childRegion.space(X) = size(X);
    if (extend(Y) == 0)
	childRegion.space(Y) = size(Y);

    _box->draw(w, childRegion, exposed, gc, context_selected);
}

// Print
void HatBox::_print(std::ostream& os, 
		    const BoxRegion& region, 
		    const PrintGC& gc) const
{
    BoxRegion childRegion = region;

    if (extend(X) == 0)
	childRegion.space(X) = size(X);
    if (extend(Y) == 0)
	childRegion.space(Y) = size(Y);
	
    _box->_print(os, childRegion, gc);
}
