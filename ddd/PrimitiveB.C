// $Id$
// Klasse PrimitiveBox (Implementation)

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

// $Log$
// Revision 1.1  1995/05/01 15:47:48  zeller
// Initial revision
//
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.13  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.12  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:15:24  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  10:52:02  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/16  11:31:51  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.7  1993/02/23  16:17:48  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.6  1992/10/13  20:49:07  zeller
// strclass.h statt String.h
// 
// Revision 10.5  1992/06/02  03:04:21  zeller
// #pragma besser formatiert
// 
// Revision 10.4  1992/06/01  16:34:26  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.3  1992/05/20  15:36:51  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:56:58  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:33  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:47:46  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:59  zeller
// Installierte Version
// 
// Revision 1.25  1991/06/09  17:37:36  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.24  1991/05/09  15:04:41  zeller
// Warnungen vermieden
// 
// Revision 1.23  1991/02/08  16:28:39  zeller
// dump() neu definiert
// 
// Revision 1.22  1990/10/04  18:19:35  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.21  1990/09/16  13:01:12  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.20  1990/09/11  11:18:05  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.19  1990/09/07  14:29:16  zeller
// StringBox, DiagBox ausgelagert;
// Neu: Unterstuetzung == und !=
// 
// Revision 1.18  1990/09/01  13:30:14  zeller
// 'PrimBox' durch 'PrimitiveBox' ersetzt
// 
// Revision 1.17  1990/08/29  13:32:00  zeller
// size_debug eingefuehrt
// 
// Revision 1.16  1990/08/28  14:23:45  zeller
// Umstellung auf display() mit Region's
// 
// Revision 1.15  1990/08/25  17:08:21  zeller
// unlink() eingefuehrt
// 
// Revision 1.14  1990/08/21  23:00:23  zeller
// Ausgabe String: umgebendes Rechteck entfernt
// 
// Revision 1.13  1990/08/21  11:50:56  zeller
// Unbenutzte Klassen entfernt
// 
// Revision 1.12  1990/08/20  11:00:03  zeller
// Ausgabe *this bei allen Boxen entfernt
// 
// Revision 1.11  1990/08/13  10:31:17  zeller
// dump(): Ausgabe *this bei Konstanten Boxen entfernt
// 
// Revision 1.10  1990/08/13  10:14:22  zeller
// Typennamen ueberarbeitet
// 
// Revision 1.9  1990/08/10  13:05:50  zeller
// DiagBox() enthaelt jetzt Platz-Information
// 
// Revision 1.8  1990/08/10  12:21:02  zeller
// String fuer Groesse korrekt aufgebaut
// 
// Revision 1.7  1990/08/01  21:17:09  zeller
// Neufassung mit neuer Ausgabe und neuen Boxen
// 
// Revision 1.6  1990/08/01  14:45:48  zeller
// Umstellung auf private Box-Members
// 
// Revision 1.5  1990/08/01  10:21:21  zeller
// 'String' eingefuehrt
// 
// Revision 1.4  1990/08/01  09:46:19  zeller
// Ausgabe beruecksichtigt jetzt den Ascent eines Fonts
// 
// Revision 1.3  1990/07/25  18:54:41  zeller
// _sccsid durch _sccsid[] ersetzt
// 
// Revision 1.2  1990/07/25  18:45:13  zeller
// _sccsid[] eingefuehrt
// 
// Revision 1.1  1990/07/25  18:36:02  zeller
// Initial revision
// 

char PrimitiveBox_rcsid[] = "$Id$";

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
		    const BoxRegion& exposed, 
		    GC gc,
		    bool context_selected) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    BoxPoint width(extend(X) ? space[X] : size(X),
		extend(Y) ? space[Y] : size(Y));

    XFillRectangle(XtDisplay(w), XtWindow(w), gc, origin[X], origin[Y],
	width[X], width[Y]);
}

void RuleBox::dump(ostream& s) const
{
    s << "rule" << size();
}

void RuleBox::_print(ostream& os, 
		     const BoxRegion& region, 
		     const BoxPrintGC& gc) const
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
