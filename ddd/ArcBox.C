// $Id$
// Klasse ArcBox (Implementation)

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

char ArcBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ArcBox.h"
#include "printBox.h"

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

// Some systems define these values in <values.h> and re-define them 
// DIFFERENTLY in <math.h>.  Prefer the <math.h> definitions.
#undef M_LN2
#undef M_PI
#undef M_SQRT2
#include <math.h>

DEFINE_TYPE_INFO_1(ArcBox, LineBox)

// ArcBox

// ArcBox anzeigen
void ArcBox::__draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion& , GC gc, 
		    bool) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    // Bei Teilboegen: Koordinaten anpassen

    /*
	An sich muesste hier eine vollstaendige Behandlung
	fuer alle moeglichen Ellipsensegmente stattfinden.
	Dies ist aber verhaeltnismaessig aufwendig und
	wegen der in diesem Punkt obskuren X11-Dokumentation
	eher langwierig zu implementieren. Deshalb werden
	hier nur Viertelkreise behandelt.
    */

    switch (_length) {
	case 360:   // do nothing
	    break;

	case 180:   // Groesse der Anzeigeflaeche verdoppeln
	    switch (_start) {
		case 0:
		    space[Y] *= 2;
		    break;
		case 90:
		    space[X] *= 2;
		    break;
		case 180:
		    origin[Y] -= space[Y];
		    space[Y] *= 2;
		    break;
		case 270:
		    origin[X] -= space[X];
		    space[X] *= 2;
		    break;
		default:
		    cerr << "ArcBox::_draw(): illegal start\n";
	    }
	    break;

	case 90:    // Groesse der Anzeigeflaeche vervierfachen
	    switch (_start) {
		case 0:
		    origin[X] -= space[X];
		    break;
		case 90:
		    break;
		case 180:
		    origin[Y] -= space[Y];
		    break;
		case 270:
		    origin[X] -= space[X];
		    origin[Y] -= space[Y];
		    break;
		default:
		    cerr << "ArcBox::_draw(): illegal start\n";
	    }
	    space *= BoxPoint(2,2);
	    break;

	default:
	    cerr << "ArcBox::_draw(): illegal length\n";
    }

    if (space[X] > 0 && space[Y] > 0 && _length > 0)
	XDrawArc(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y],
	    space[X], space[Y], _start << 6, _length << 6); // <<6 is *64
}

void ArcBox::dump(ostream& s) const
{
    s << "arc(" << _start << "," << _length << ")";
}


// ArcBox drucken

#define POINT(i) (os << points[i][X] << " " << points[i][Y] << " ")

//
// constants for drawing Arcs
//

const float rad = 0.70710678118654752440;
const float pi  = 3.14159265358979323846;


//
// drawFigArc
//

void ArcBox::_printFig(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC &) const
{
    BoxPoint origin = region.origin() ;
    BoxPoint space = region.space();
    BoxDegrees s = start();
    BoxDegrees l = length() ;
    BoxCoordinate thickness = linethickness() ;
    BoxPoint points[12] ;
    BoxCoordinate middle ;

    switch (l) {
    case 0:
    case 360:
    case 270:
	break ;
    case 180:
	switch (s) {
	case 0:
	    space[Y] *= 2;
	    break ;
	case 90:
	    space[X] *= 2;
	    break;
	case 180:
	    origin[Y] -= space[Y] ;
	    space[Y] *= 2;
	    break ;
	case 270:
	    origin[X] -= space[X] ;
	    space[X] *= 2;
	    break;
	default:
	    cerr << "illegal start" ;
	}
	
	break ;
    case 90:
	switch (s) {
	case 0:
	    origin[X] -= space[X] ;
	    break ;
	case 90:
	    break ;
	case 180:
	    origin[Y] -= space[Y] ;
	    break ;
	case 270:
	    origin[X] -= space[X] ;
	    origin[Y] -= space[Y] ;
	    break ;
	default:
	    cerr << "illegal start" ;
	}
	space[X] *= 2;
	space[Y] *= 2;
	break ;
    default:
	cerr << "illegal length" ;
    }
    

    if (space[X] < space[Y]) {
	middle = (int) ((float)space[X]/2 * rad) ;
	
	points[0][X] = origin[X] + space[X]/2 ;
	points[0][Y] = origin[Y] ;
	points[1][X] = origin[X] + space[X]/2 + middle ;
	points[1][Y] = origin[Y] + space[X]/2 - middle ;
	points[2][X] = origin[X] + space[X] ;
	points[2][Y] = origin[Y] + space[X]/2 ;
	points[3][X] = origin[X] + space[X] ;
	points[3][Y] = origin[Y] + space[Y]/2 ;
	
	points[4][X] = origin[X] + space[X] ;
	points[4][Y] = origin[Y] + space[Y] - space[X]/2;
	points[5][X] = origin[X] + space[X]/2 + middle ;
	points[5][Y] = origin[Y] + space[Y] - space[X]/2 + middle ;
	points[6][X] = origin[X] + space[X]/2 ;
	points[6][Y] = origin[Y] + space[Y]  ;
	points[7][X] = origin[X] + space[X]/2 - middle ;
	points[7][Y] = origin[Y] + space[Y] - space[X]/2 + middle;
	
	points[8][X] = origin[X] ;
	points[8][Y] = origin[Y] + space[Y] - space[X]/2;
	points[9][X] = origin[X] ;
	points[9][Y] = origin[Y] + space[Y]/2 ;
	points[10][X] = origin[X] ;
	points[10][Y] = origin[Y] + space[X]/2 ;
	points[11][X] = origin[X] + space[X]/2 - middle ;
	points[11][Y] = origin[Y] + space[X]/2 - middle ;
	
	while (l) {
	    switch (s) {
	    case 360:
	    case 0:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(0) ;
		POINT(1) ;
		POINT(2) ;
		POINT(3) ;
		os << "9999 9999\n" ;
		
		break;
	    case 270:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(3) ;
		POINT(4) ;
		POINT(5) ;
		POINT(6) ;
		os << "9999 9999\n" ;
		
		break;
	    case 180:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(6) ;
		POINT(7) ;
		POINT(8) ;
		POINT(9) ;
		os << "9999 9999\n" ;
		
		break;
	    case 90:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(9) ;
		POINT(10) ;
		POINT(11) ;
		POINT(0) ;
		os << "9999 9999\n" ;
		
		break;
		
	    default:
		cerr << "drawArc: illegal start\n";
	    }
	    l -= 90 ;
	    s = (s + 90) % 360 ;
	}
    } else  {
	middle = (int) ((float)space[Y]/2 * rad) ;
	int equal = (space[X] == space[Y]) ;
	
	points[0][Y] = origin[Y] + space[Y]/2 ;
	points[0][X] = origin[X] ;
	points[1][Y] = origin[Y] + space[Y]/2 + middle ;
	points[1][X] = origin[X] + space[Y]/2 - middle ;
	points[2][Y] = origin[Y] + space[Y] ;
	points[2][X] = origin[X] + space[Y]/2 ;
	points[3][Y] = origin[Y] + space[Y] ;
	points[3][X] = origin[X] + space[X]/2 ;
	
	points[4][Y] = origin[Y] + space[Y] ;
	points[4][X] = origin[X] + space[X] - space[Y]/2;
	points[5][Y] = origin[Y] + space[Y]/2 + middle ;
	points[5][X] = origin[X] + space[X] - space[Y]/2 + middle ;
	points[6][Y] = origin[Y] + space[Y]/2 ;
	points[6][X] = origin[X] + space[X]  ;
	points[7][Y] = origin[Y] + space[Y]/2 - middle ;
	points[7][X] = origin[X] + space[X] - space[Y]/2 + middle;
	
	points[8][Y] = origin[Y] ;
	points[8][X] = origin[X] + space[X] - space[Y]/2;
	points[9][Y] = origin[Y] ;
	points[9][X] = origin[X] + space[X]/2 ;
	points[10][Y] = origin[Y] ;
	points[10][X] = origin[X] + space[Y]/2 ;
	points[11][Y] = origin[Y] + space[Y]/2 - middle ;
	points[11][X] = origin[X] + space[Y]/2 - middle ;

	while (l) {
	    switch (s) {
	    case 180:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(0) ;
		POINT(1) ;
		POINT(2) ;
		if (!equal)
		    POINT(3) ;
		os << "9999 9999\n" ;
		
		break;
	    case 270:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		if (!equal)
		    POINT(3) ;
		POINT(4) ;
		POINT(5) ;
		POINT(6) ;
		os << "9999 9999\n" ;
		
		break;
	    case 360:
	    case 0:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		POINT(6) ;
		POINT(7) ;
		POINT(8) ;
		if (!equal)
		    POINT(9) ;
		os << "9999 9999\n" ;
		
		break;
	    case 90:
		os << LINEHEAD1 ;
		os << thickness ;
		os << LINEHEAD2 ;
		if (!equal)
		    POINT(9) ;
		POINT(10) ;
		POINT(11) ;
		POINT(0) ;
		os << "9999 9999\n" ;
		
		break;
		
	    default:
		cerr << "drawArc: illegal start\n";
	    }
	    l -= 90 ;
	    s = (s + 90) % 360 ;
	}
    }
}


//
// drawPSArc
//

void ArcBox::_printPS(ostream& os, 
		      const BoxRegion& region, 
		      const BoxPrintGC &) const
{
    BoxPoint origin = region.origin() ;
    BoxPoint space = region.space();
    BoxDegrees s = start();
    BoxDegrees end;
    BoxDegrees l = length() ;
    BoxCoordinate thickness = linethickness() ;
    
    switch (l) {
    case 0:
    case 360:
    case 270:
	break ;
    case 180:
	switch (s) {
	case 0:
	    space[Y] *= 2;
	    break ;
	case 90:
	    space[X] *= 2;
	    break;
	case 180:
	    origin[Y] -= space[Y] ;
	    space[Y] *= 2;
	    break ;
	case 270:
	    origin[X] -= space[X] ;
	    space[X] *= 2;
	    break;
	default:
	    cerr << "illegal start" ;
	}
	
	break ;
    case 90:
	switch (s) {
	case 0:
	    origin[X] -= space[X] ;
	    break ;
	case 90:
	    break ;
	case 180:
	    origin[Y] -= space[Y] ;
	    break ;
	case 270:
	    origin[X] -= space[X] ;
	    origin[Y] -= space[Y] ;
	    break ;
	default:
	    cerr << "illegal start" ;
	}
	space[X] *= 2;
	space[Y] *= 2;
	break ;
    default:
	cerr << "illegal length" ;
    }
    
    end = (720 - s) % 360 ;
    s   = (720 - s - l) % 360 ;
    
    
    os << s << " " << end << " " ;
    os << space[X]/2 << " " << space[Y]/2 << " ";
    os << origin[X] + space[X]/2 << " " << origin[Y] + space[Y]/2;
    os << " " << thickness << " arc*\n";
    
}

//
// drawArc
//

void ArcBox::_print(ostream& os, 
		    const BoxRegion& region, 
		    const BoxPrintGC& gc) const
{
    if (gc.isFig()) {
	_printFig(os, region, gc);
    } else if (gc.isPostScript()) {
	_printPS(os, region, gc);
    }

}
