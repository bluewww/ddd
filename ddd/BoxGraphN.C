// $Id$
// BoxGraphNode class: RegionGraphNode with box

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
// Revision 1.1  1995/05/01 15:47:32  zeller
// Initial revision
//
// Revision 1.18  1995/03/17  09:54:48  zeller
// License and RCS IDs added
//
// Revision 1.17  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.16  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.15  1993/05/26  09:13:22  zeller
// Fix: const-Warnung unterdrueckt
//
// Revision 1.14  1993/05/22  20:10:31  zeller
// %Q% added by apply-q-flag.sh 1.5
// 
// Revision 1.13  1993/04/22  11:00:34  zeller
// Lizenz verbessert
// 
// Revision 1.12  1993/04/16  11:38:26  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.11  1993/03/01  10:00:17  zeller
// Neu: XFIG: Knoten als Compound ausgegeben
// 
// Revision 1.10  1993/02/24  12:16:24  zeller
// Neu: Knoten vor Zeichnen loeschen (vorerst nur in PostScript)
// 
// Revision 1.9  1993/02/23  17:31:18  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.8  1992/11/13  19:07:40  zeller
// forceDraw: const eingefuehrt; region() angepasst
// 
// Revision 1.7  1992/11/12  23:46:48  zeller
// 'const' eingefuehrt
// 
// Revision 1.6  1992/11/05  19:02:57  zeller
// Neue Graphik-Kontexte -- vermeiden allzuhaeufiges Umschalten
// 
// Revision 1.5  1992/06/01  17:02:34  zeller
// Neu: #pragma implementation
// 
// Revision 1.4  1992/05/20  15:45:51  zeller
// Dateinamen verkuerzt
// 
// Revision 1.3  1992/02/27  23:02:01  zeller
// Neu: GraphGC
// 
// Revision 1.2  1992/02/27  19:13:33  zeller
// Box jetzt stets als Ganzes anzeigen
// 
// Revision 1.1  1992/02/26  11:52:34  zeller
// Initial revision
// 

#ifdef __GNUG__
#pragma implementation
#endif


#include "BoxGraphN.h"
#include "printBox.h"

DEFINE_TYPE_INFO_1(BoxGraphNode, RegionGraphNode)

// Draw a BoxGraphNode
void BoxGraphNode::forceDraw(Widget w, 
			     const BoxRegion& exposed, 
			     const GraphGC& gc) const
{
    // We do not check for exposures here --
    // boxes are usually small and partial display
    // doesn't work well with scrolling

    box()->draw(w, region(gc), region(gc), gc.nodeGC, false);
}


// mark the following objects as one XFIG compound object
static void startCompound(ostream& os, BoxRegion region)
{
    BoxPoint origin = region.origin();
    BoxPoint width = region.space();

    os << CMPHEAD;
    os << origin[X] + width[X] + 1 << " " << origin[Y] - 1 << " ";
    os << origin[X] - 1 << " " << origin[Y] + width[Y] + 1 << "\n";
}

static void endCompound(ostream& os)
{
    os << CMPTAIL;
}


// Print a BoxGraphNode
void BoxGraphNode::_print(ostream& os, const GraphGC& gc) const
{
    if (gc.printGC->isFig())
	startCompound(os, region(gc));

    RegionGraphNode::_print(os, gc);
    box()->_print(os, (BoxRegion&)region(gc), *gc.printGC);

    if (gc.printGC->isFig())
	endCompound(os);
}

