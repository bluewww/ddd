// $Id$
// Klasse PrimitiveBox (Implementation)

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

char PrimitiveBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "PrimitiveB.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "strclass.h"
#include "printBox.h"

DEFINE_TYPE_INFO_1(PrimitiveBox, Box)
DEFINE_TYPE_INFO_1(RuleBox, PrimitiveBox)
DEFINE_TYPE_INFO_1(EmptyBox, PrimitiveBox)
DEFINE_TYPE_INFO_1(FillBox, EmptyBox)
DEFINE_TYPE_INFO_1(SpaceBox, EmptyBox)
DEFINE_TYPE_INFO_1(SquareBox, SpaceBox)

// RuleBox

// RuleBox anzeigen
void RuleBox::_draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion&, 
		    GC gc,
		    bool) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    BoxPoint width(extend(X) ? space[X] : size(X),
		   extend(Y) ? space[Y] : size(Y));

    if (width[Y] == 1)
    {
	// Horizontal line
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
		  origin[X], origin[Y], origin[X] + width[X], origin[Y]);
    }
    else if (width[X] == 1)
    {
	// Vertical line
	XDrawLine(XtDisplay(w), XtWindow(w), gc,
		  origin[X], origin[Y], origin[X], origin[Y] + width[Y]);
    }
    else
    {
	// Rectangle
	XFillRectangle(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y],
		       width[X], width[Y]);
    }
}

void RuleBox::dump(ostream& s) const
{
    s << "rule" << size();
}

void RuleBox::_print(ostream& os, 
		     const BoxRegion& region, 
		     const PrintGC& gc) const
{
    BoxSize space = region.space();
    BoxPoint origin = region.origin();
    BoxPoint width ;
    
    width = BoxPoint ( extend(X) ? space[X] : size(X) ,
		       extend(Y) ? space[Y] : size(Y) );

    if (width == BoxPoint (0,1) || width == BoxPoint (1,0)) {
	return ;
    }
    if (width[X] && width[X] < 3 && gc.isFig()) {
	// 
	// vertical Line
	//
	
	os << LINEHEAD1 ;
	os << width[X] << LINEHEAD2 ;
	os << origin[X] + width[X]/2 << " " << origin[Y] ;
	os << " " ;
	os << origin[X] + width[X]/2 << " " ;
	os << origin[Y] + width[Y] << " " ;
	os << "9999 9999\n" ;

    } else if (width[Y] && width[Y] < 3 && gc.isFig()) {
	//
	// horizontal line
	//
	
	os << LINEHEAD1 ;
	os << width[Y] << LINEHEAD2 ;
	os << origin[X] << " " << origin[Y]+width[Y]/2 ;
	os << " " ;
	os << origin[X] + width[X] << " " ;
	os << origin[Y] + width[Y]/2 << " " ;
	os << "9999 9999\n" ;
	
    } else {
	//
	// filled rectangle
	//
	if (gc.isFig()) {
	    os << RULEHEAD ;
	    os << origin[X] << " " << origin[Y] << " " ;
	    os << origin[X] + width[X] << " " << origin[Y] ;
	    os << " " ;
	    os << origin[X] + width[X] << " " ;
	    os << origin[Y] + width[Y] << " ";
	    os << origin[X] << " " << origin[Y] + width[Y] ; 
	    os << " " ;
	    os << origin[X] << " "<< origin[Y] << " 9999 9999\n" ;

	} else if (gc.isPostScript()) {

	    os << origin[X] << " " << origin[Y] << " " ;
	    os << origin[X] + width[X] << " " << origin[Y] ;
	    os << " " ;
	    os << origin[X] + width[X] << " " ;
	    os << origin[Y] + width[Y] << " ";
	    os << origin[X] << " " << origin[Y] + width[Y] ; 
	    os << " box*" << " %" <<  region << "\n"; ;
	}
    }
}



// FillBox

void FillBox::dump(ostream& s) const
{
    s << "fill" << size();
}


// SpaceBox

void SpaceBox::dump(ostream& s) const
{
    s << "space" << size();
}


// SquareBox

void SquareBox::dump(ostream& s) const
{
    s << size(X);
}
