// $Id$
// Graph graphics context

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
// Revision 1.1  1995/05/01 15:47:43  zeller
// Initial revision
//
// Revision 1.20  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.19  1994/11/14  19:31:27  zeller
// Fixes for gcc 2.6.1
//
// Revision 1.18  1993/12/06  16:39:08  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.17  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.16  1993/05/22  20:11:07  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.15  1993/04/22  11:00:30  zeller
// Lizenz verbessert
// 
// Revision 1.14  1993/04/21  00:18:14  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.13  1993/04/20  23:00:59  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.12  1993/04/16  11:38:35  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.11  1993/02/24  15:29:42  zeller
// Neu: printSelectedNodesOnly
// 
// Revision 1.10  1993/02/23  17:31:22  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.9  1992/11/13  19:09:21  zeller
// Neu: hints optional anzeigen
// 
// Revision 1.8  1992/11/12  23:48:00  zeller
// Neu: Unterstuetzung HintGraphNodes (hintGC)
// 
// Revision 1.7  1992/11/05  19:03:57  zeller
// Neue Graphik-Kontexte -- vermeiden allzuhaeufiges Umschalten
// 
// Revision 1.6  1992/11/04  15:47:24  zeller
// Copy-Constructor eingefuehrt (gcc will es so)
// 
// Revision 1.5  1992/06/01  16:59:06  zeller
// Neu: #pragma interface
// 
// Revision 1.4  1992/06/01  16:55:34  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.3  1992/03/16  16:01:34  zeller
// Neue Verfahren, um Verbindungspunkte der Kanten zu bestimmen
// 
// Revision 1.2  1992/02/28  00:03:51  zeller
// Groessen Pfeilspitzen aus GraphEdit uebernommen
// 
// Revision 1.1  1992/02/27  23:02:26  zeller
// Initial revision
// 

#ifndef _Nora_GraphGC_h
#define _Nora_GraphGC_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Xlib.h>
#include "BoxPoint.h"
#include "Box.h"
#include "TypeInfo.h"


enum EdgeAttachMode {
    Straight,	// attach edge on nearest edge point
    Circle,     // ...but assume nodes to be displayed as circles
    Centered,   // attach edge on center of node edge
};

struct GraphGC {
    DECLARE_TYPE_INFO

    // Drawing stuff
    GC             nodeGC;           // X Graphics context for nodes
    GC             hintGC;           // X Graphics context for hints
    GC             edgeGC;           // X Graphics context for edges
    GC             invertGC;         // X Graphics context for inverting
    GC             clearGC;          // X Graphics context for clearing

    BoxPoint       offsetIfSelected; // Offset to apply if nodes are selected
    EdgeAttachMode edgeAttachMode;   // Where to attach edges
    bool        drawArrowHeads;   // Flag: draw arrow heads?
    bool        drawHints;        // Flag: draw hints?
    BoxCoordinate  hintSize;         // Hint size (in pixels)
    unsigned       arrowAngle;       // Arrow angle (in degrees)
    unsigned       arrowLength;      // Arrow length (in pixels)

    // Printing stuff
    BoxPrintGC     *printGC;               // Graphics context for printing
    bool        printSelectedNodesOnly; // Flag: print selected nodes only?

    static BoxPostScriptGC defaultPrintGC;

    // Default Constructor
    GraphGC(GC n = 0, GC e = 0, GC i = 0, GC c = 0):
        nodeGC(n),
        hintGC(n),
        edgeGC(e),
        invertGC(i),
        clearGC(c),
        offsetIfSelected(0,0),
	edgeAttachMode(Straight),
        drawArrowHeads(true),
        drawHints(false),
        hintSize(8),
        arrowAngle(30),
        arrowLength(10),
	printGC(&defaultPrintGC),
	printSelectedNodesOnly(false)
    {}

    // Copy Constructor
    GraphGC(const GraphGC& g):
        nodeGC(g.nodeGC),
        hintGC(g.hintGC),
        edgeGC(g.edgeGC),
        invertGC(g.invertGC),
        clearGC(g.clearGC),
        offsetIfSelected(g.offsetIfSelected),
	edgeAttachMode(g.edgeAttachMode),
        drawArrowHeads(g.drawArrowHeads),
        drawHints(g.drawHints),
        hintSize(g.hintSize),
        arrowAngle(g.arrowAngle),
        arrowLength(g.arrowLength),
	printGC(g.printGC),
	printSelectedNodesOnly(g.printSelectedNodesOnly)
    {}
};

#endif
