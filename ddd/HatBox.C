// $Id$
// Implementation Klasse HatBox

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
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

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
		    const BoxPrintGC& gc) const
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
