// $Id$
// RegionGraphNode class

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
// Revision 1.1  1995/05/01 15:47:49  zeller
// Initial revision
//
// Revision 1.21  1994/07/20  07:55:29  zeller
// Fix: const inserted
//
// Revision 1.20  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.19  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.18  1993/05/22  20:11:35  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.17  1993/04/22  11:00:17  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/21  00:18:17  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.15  1993/04/16  11:38:44  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.14  1993/03/01  10:10:36  zeller
// Fix: Auch in XFig Knotenbereich loeschen
// 
// Revision 1.13  1993/02/24  12:16:13  zeller
// Neu: Knoten vor Zeichnen loeschen (vorerst nur in PostScript)
// 
// Revision 1.12  1992/11/13  19:11:18  zeller
// GraphGC jetzt ueberall const;
// Fix: Aufruf forceDraw() korrigiert;
// Bei unsichtbarer Region sofort umkehren
// 
// Revision 1.11  1992/11/12  23:48:47  zeller
// 'const' eingefuehrt
// 
// Revision 1.10  1992/11/05  19:04:11  zeller
// Graphik-Kontexte -- vermeiden allzuhaeufiges UmschaltenNeue Graphik-Kontexte -- vermeiden allzuhaeufiges Umschalten
// 
// Revision 1.9  1992/11/05  16:49:17  zeller
// Neu: Highlight-Box kann jetzt gesondert angegeben werden
// 
// Revision 1.8  1992/06/02  00:56:32  zeller
// Anpassung an gcc 2.1
// 
// Revision 1.7  1992/06/01  17:02:41  zeller
// Neu: #pragma implementation
// 
// Revision 1.6  1992/05/20  15:45:49  zeller
// Dateinamen verkuerzt
// 
// Revision 1.5  1992/02/27  23:01:11  zeller
// Neu: GraphGC
// 
// Revision 1.4  1992/02/27  19:12:48  zeller
// Box jetzt stets als Ganzes anzeigen
// 
// Revision 1.3  1992/02/26  11:22:10  zeller
// Neu: Zeichnen von selektierten Knoten
// 
// Revision 1.2  1992/02/25  18:27:55  zeller
// Debugging-Ausgaben entfernt
// 
// Revision 1.1  1992/02/25  18:26:28  zeller
// Initial revision
// 

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "RegionGN.h"
#include "printBox.h"

DEFINE_TYPE_INFO_1(RegionGraphNode, PosGraphNode)

// center around position
void RegionGraphNode::center()
{
    _region.origin() = pos() - (_region.space() / 2);
}


// Draw a RegionGraphNode
void RegionGraphNode::draw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const
{
    const BoxRegion& r = region(gc);

    // if not exposed or invisible, return
    if (!(r <= exposed) || r.space(X) == 0 || r.space(Y) == 0)
	return;

    Display *display = XtDisplay(w);
    Window window = XtWindow(w);

    // clear the area
    XFillRectangle(display, window, gc.clearGC,
		   r.origin(X), r.origin(Y),
		   r.space(X), r.space(Y));

    // draw contents
    forceDraw(w, exposed, gc);

    // if selected, invert area
    if (__selected())
    {
	const BoxRegion& h = highlightRegion(gc);

	XFillRectangle(display, window, gc.invertGC,
		       h.origin(X), h.origin(Y),
		       h.space(X), h.space(Y));
    }
};



// cleanRegion
// clean a region with white ink
//
static void cleanRegion (ostream& os, const GraphGC& gc, BoxRegion region)
{
      BoxPoint origin = region.origin();
      BoxPoint width = region.space();

      if (gc.printGC->isPostScript())
      {
	  os << origin[X] << " " << origin[Y] << " ";
	  os << origin[X] + width[X] << " " << origin[Y];
	  os << " ";
	  os << origin[X] + width[X] << " ";
	  os << origin[Y] + width[Y] << " ";
	  os << origin[X] << " " << origin[Y] + width[Y];
	  os << " clean*\n";
      }
      else if (gc.printGC->isFig())
      {
	  os << CLEANHEAD;
	  os << origin[X] << " " << origin[Y] << " ";
	  os << origin[X] + width[X] << " " << origin[Y];
	  os << " ";
	  os << origin[X] + width[X] << " ";
	  os << origin[Y] + width[Y] << " ";
	  os << origin[X] << " " << origin[Y] + width[Y];
	  os << " ";
	  os << origin[X] << " "<< origin[Y] << " 9999 9999\n";
      }	  
}

// Print a RegionGraphNode
void RegionGraphNode::_print(ostream& os, const GraphGC& gc) const
{
    cleanRegion(os, gc, region(gc));
}
