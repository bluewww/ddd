// $Id$
// Graph graphics context

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_GraphGC_h
#define _DDD_GraphGC_h

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
    Centered    // attach edge on center of node edge
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
    bool           drawArrowHeads;   // Flag: draw arrow heads?
    bool           drawHints;        // Flag: draw hints?
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
