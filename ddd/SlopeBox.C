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

// $Log$
// Revision 1.1  1995/05/01 15:47:50  zeller
// Initial revision
//
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/09/06  06:15:13  zeller
// Fix: #pragma implementation must come *before* any include
//
// Revision 10.13  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.12  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:15:40  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  10:52:13  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/16  11:31:55  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1993/02/23  16:17:50  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.5  1992/06/02  03:04:24  zeller
// #pragma besser formatiert
// 
// Revision 10.4  1992/06/01  16:34:33  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.3  1992/06/01  14:13:48  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/02/18  10:57:09  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:43  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:48:51  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:16  zeller
// Installierte Version
// 
// Revision 1.13  1991/06/09  17:37:46  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.12  1991/06/07  19:17:36  zeller
// _draw() in __draw() umbenannt
// (_draw() wird in LineBox aufgerufen)
// 
// Revision 1.11  1991/05/09  15:19:35  zeller
// Warnungen vermieden
// 
// Revision 1.10  1991/05/09  15:05:47  zeller
// Warnungen vermieden
// 
// Revision 1.9  1991/02/08  16:29:46  zeller
// dump() neu definiert
// 
// Revision 1.8  1991/02/01  15:31:26  zeller
// Ausgabe RiseBox korrigiert
// 
// Revision 1.7  1990/10/04  18:19:45  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.6  1990/09/16  13:01:18  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.5  1990/09/11  11:18:22  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.4  1990/08/29  13:32:03  zeller
// size_debug eingefuehrt
// 
// Revision 1.3  1990/08/28  14:23:48  zeller
// Umstellung auf display() mit Region's
// 
// Revision 1.2  1990/08/13  12:55:06  zeller
// Sccs-ID koorigiert
// 
// Revision 1.1  1990/08/13  11:18:03  zeller
// Initial revision
// 

char SlopeBox_rcsid[] = "$Id$";

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
