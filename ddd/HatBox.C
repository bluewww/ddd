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

// $Log$
// Revision 1.1  1995/05/01 15:47:23  zeller
// Initial revision
//
// Revision 10.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.12  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.11  1994/07/19  21:12:13  zeller
// New: printing functions const-corrected
//
// Revision 10.10  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:14:50  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  10:51:51  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:11:02  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:31:42  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1993/02/23  16:17:43  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.3  1992/06/01  16:34:12  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:12  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:47  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:18  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:33  zeller
// Installierte Version
// 
// Revision 1.10  1991/06/09  17:37:55  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.9  1991/05/09  15:06:12  zeller
// Warnungen vermieden
// 
// Revision 1.8  1991/02/01  14:50:35  zeller
// FixBoxen nach FixBox.h ausgelagert
// 
// Revision 1.7  1991/02/01  13:28:16  zeller
// Umstellung: Extend sind jetzt Integer-Zahlen
// 
// Revision 1.6  1990/09/16  13:01:05  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.5  1990/09/11  11:18:32  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.4  1990/08/31  16:06:07  zeller
// Kleine Funktionen inline gemacht
// 
// Revision 1.3  1990/08/29  13:32:07  zeller
// size_debug eingefuehrt
// 
// Revision 1.2  1990/08/28  14:23:42  zeller
// Umstellung auf display() mit Region's
// 
// Revision 1.1  1990/08/25  17:09:05  zeller
// Initial revision
// 

char HatBox_rcsid[] = "$Id$";

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
