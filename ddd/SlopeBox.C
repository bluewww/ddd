// $Id$
// Klasse SlopeBox (Implementation)

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

char SlopeBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SlopeBox.h"
#include "printBox.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

DEFINE_TYPE_INFO_1(SlopeBox, LineBox)
DEFINE_TYPE_INFO_1(FallBox, SlopeBox)
DEFINE_TYPE_INFO_1(RiseBox, SlopeBox)


// FallBox

// FallBox anzeigen
void FallBox::__draw(Widget w, 
		     const BoxRegion& r, 
		     const BoxRegion& exposed, 
		     GC gc,
		     bool context_selected) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    XDrawLine(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y],
	origin[X] + space[X], origin[Y] + space[Y]);
}

// FallBox drucken
void FallBox::_print(ostream& os, 
		     const BoxRegion& region, 
		     const BoxPrintGC& gc) const
{
    BoxPoint origin = region.origin() ;
    BoxPoint space = region.space();

    if (gc.isFig()) {
	os << LINEHEAD1 ;
	os << linethickness() << LINEHEAD2 ;
	os << origin[X] << " " << origin[Y] << " " ;
	os << origin[X] + space[X] << " " ;
	os << origin[Y] + space[Y] << " " ;
	os << "9999 9999\n" ;
    } else if (gc.isPostScript()) {
	os << origin[X] << " " << origin[Y] << " " ;
	os << origin[X] + space[X] << " " ;
	os << origin[Y] + space[Y] << " " ;
	os << linethickness() << " line*\n";
    }
}



void FallBox::dump(ostream& s) const
{
    s << "fall()";
}



// RiseBox

// RiseBox anzeigen
void RiseBox::__draw(Widget w, 
		     const BoxRegion& r, 
		     const BoxRegion& exposed, 
		     GC gc,
		     bool context_selected) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    XDrawLine(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y] + space[Y],
	origin[X] + space[X], origin[Y]);
}

// RiseBox drucken
void RiseBox::_print(ostream& os, 
		     const BoxRegion& region, 
		     const BoxPrintGC& gc) const
{
    BoxPoint origin = region.origin();
    BoxPoint space  = region.space();
	
    if (gc.isFig()) {
	os << LINEHEAD1 ;
	os << linethickness() << LINEHEAD2 ;
	os << origin[X] << " " << origin[Y] + space[Y] << " " ;
	os << origin[X] + space[X] << " " << origin[Y] << " " ;
	os << "9999 9999\n" ;
    } else if (gc.isPostScript()) {
	os << origin[X] << " " << origin[Y] + space[Y] << " " ;
	os << origin[X] + space[X] << " " << origin[Y] << " " ;
	os << linethickness() << " line*\n";
    }
}
    
void RiseBox::dump(ostream& s) const
{
    s << "rise()";
}
