// $Id$
// Implementation Klasse HatBox

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char HatBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "HatBox.h"

DEFINE_TYPE_INFO_1(HatBox, Box)

// HatBox

// HatBox ausgeben
void HatBox::_draw(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed, GC gc,
		   bool context_selected) const
{
    BoxRegion sonRegion = r;

    // Wenn nicht erweiterbar, Platz auf minimalen Platz reduzieren
    if (extend(X) == 0)
	sonRegion.space(X) = size(X);
    if (extend(Y) == 0)
	sonRegion.space(Y) = size(Y);

    _box->draw(w, sonRegion, exposed, gc, context_selected);
}

// print HatBox
void HatBox::_print(ostream& os, 
		    const BoxRegion& region, 
		    const PrintGC& gc) const
{
    BoxRegion sonRegion = region;

    if (extend(X) == 0) {
	sonRegion.space(X) = size(X) ;
    }
    if (extend(Y) == 0) {
	sonRegion.space(Y) = size(Y) ;
    }
	
    _box->_print(os, sonRegion, gc);
}
