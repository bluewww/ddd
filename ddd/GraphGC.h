// $Id$
// Graph graphics context

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

#ifndef _DDD_GraphGC_h
#define _DDD_GraphGC_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Xlib.h>
#include "BoxPoint.h"
#include "Box.h"
#include "TypeInfo.h"
#include "explicit.h"

// Where to attach edges
enum EdgeAttachMode {
    Straight,	// attach edge on nearest edge point
    Circle,     // ...but assume nodes to be displayed as circles
    Centered    // attach edge on center of node edge
};

// Where to attach edges pointing at self
enum SelfEdgePosition {
    NorthEast,			// Upper right corner
    SouthEast,			// Lower right corner
    NorthWest,			// Upper left corner
    SouthWest			// Lower left corner
};

// How to align edges pointing at self
enum SelfEdgeDirection { Clockwise, Counterclockwise };

struct GraphGC {
    DECLARE_TYPE_INFO

    // Drawing stuff
    bool           redraw;	     // Redraw only marked nodes?

    GC             nodeGC;           // X Graphics context for nodes
    GC             hintGC;           // X Graphics context for hints
    GC             edgeGC;           // X Graphics context for edges
    GC             invertGC;         // X Graphics context for inverting
    GC             clearGC;          // X Graphics context for clearing

    BoxPoint       offsetIfSelected; // Offset to apply if nodes are selected
    EdgeAttachMode edgeAttachMode;   // Where to attach edges
    bool           drawArrowHeads;   // Flag: draw arrow heads?
    bool           drawHints;        // Flag: draw hints?
    bool	   drawAnnotations;  // Flag: draw annotations?
    BoxCoordinate  hintSize;         // Hint size (in pixels)
    unsigned       arrowAngle;       // Arrow angle (in degrees)
    unsigned       arrowLength;      // Arrow length (in pixels)

    BoxCoordinate     selfEdgeDiameter;  // Diameter of edge pointing at self
    SelfEdgePosition  selfEdgePosition;  // Its position relative to the node
    SelfEdgeDirection selfEdgeDirection; // Its direction

    // Printing stuff
    BoxPrintGC     *printGC;               // Graphics context for printing
    bool        printSelectedNodesOnly; // Flag: print selected nodes only?

    static BoxPostScriptGC defaultPrintGC;

    // Default Constructor
    explicit GraphGC(GC n = 0, GC e = 0, GC i = 0, GC c = 0):
	redraw(false),
        nodeGC(n),
        hintGC(n),
        edgeGC(e),
        invertGC(i),
        clearGC(c),
        offsetIfSelected(0,0),
	edgeAttachMode(Straight),
        drawArrowHeads(true),
        drawHints(false),
        drawAnnotations(true),
        hintSize(8),
        arrowAngle(30),
        arrowLength(10),
	selfEdgeDiameter(32),
	selfEdgePosition(NorthEast),
	selfEdgeDirection(Counterclockwise),
	printGC(&defaultPrintGC),
	printSelectedNodesOnly(false)
    {}

    // Copy Constructor
    GraphGC(const GraphGC& g):
	redraw(g.redraw),
        nodeGC(g.nodeGC),
        hintGC(g.hintGC),
        edgeGC(g.edgeGC),
        invertGC(g.invertGC),
        clearGC(g.clearGC),
        offsetIfSelected(g.offsetIfSelected),
	edgeAttachMode(g.edgeAttachMode),
        drawArrowHeads(g.drawArrowHeads),
        drawHints(g.drawHints),
        drawAnnotations(g.drawAnnotations),
        hintSize(g.hintSize),
        arrowAngle(g.arrowAngle),
        arrowLength(g.arrowLength),
	selfEdgeDiameter(g.selfEdgeDiameter),
	selfEdgePosition(g.selfEdgePosition),
	selfEdgeDirection(g.selfEdgeDirection),
	printGC(g.printGC),
	printSelectedNodesOnly(g.printSelectedNodesOnly)
    {}

    // Assignment
    GraphGC& operator = (const GraphGC& g)
    {
	if (&g != this)
	{
	    redraw                 = g.redraw;
	    nodeGC                 = g.nodeGC;
	    hintGC                 = g.hintGC;
	    edgeGC                 = g.edgeGC;
	    invertGC               = g.invertGC;
	    clearGC                = g.clearGC;
	    offsetIfSelected       = g.offsetIfSelected;
	    edgeAttachMode         = g.edgeAttachMode;
	    drawArrowHeads         = g.drawArrowHeads;
	    drawHints              = g.drawHints;
	    drawAnnotations        = g.drawAnnotations;
	    hintSize               = g.hintSize;
	    arrowAngle             = g.arrowAngle;
	    arrowLength            = g.arrowLength;
	    selfEdgeDiameter       = g.selfEdgeDiameter;
	    selfEdgePosition       = g.selfEdgePosition;
	    selfEdgeDirection      = g.selfEdgeDirection;
	    printGC                = g.printGC;
	    printSelectedNodesOnly = g.printSelectedNodesOnly;
	}
	return *this;
    }
};

#endif // _DDD_GraphGC_h
// DON'T ADD ANYTHING BEHIND THIS #endif
