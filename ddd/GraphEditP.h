// $Id$
// GraphEdit Widget -- private interface

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_GraphEditP_h
#define _DDD_GraphEditP_h

// Superclasses
extern "C" {
#define new new_w
#define class class_w
#include <X11/CoreP.h>
#include <Xm/XmP.h>
#if XmVersion >= 1002
#include <Xm/PrimitiveP.h>
#endif
#undef new
#undef class
}

// This class
#include "GraphEdit.h"
#include "Graph.h"


// The GraphEdit class record

typedef struct {
    caddr_t extension;
} GraphEditClassPart;

typedef struct _GraphEditClassRec {
    CoreClassPart        core_class;
    XmPrimitiveClassPart primitive_class;
    GraphEditClassPart   graphEdit_class;
} GraphEditClassRec;

extern GraphEditClassRec graphEditClassRec;


// Possible states

enum GraphEditState {
    SelectState,	// draw select frame
    DeltaState,		// wait for movement
    MoveState,		// draw move frames
    NopState		// something completely different
};

// The GraphEdit instance record

typedef struct _GraphEditPart {
    // Resources

    // Graph stuff
    Graph *graph;		// Graph to be displayed

    // Movement stuff
    Dimension moveDelta;	// Start move after movement greater than this
    Boolean rubberEdges;	// Draw edges  when moving
    Boolean rubberArrows;	// Draw arrows when moving
    Boolean opaqueMove;		// Draw nodes  when moving

    // Selection stuff
    Boolean autoRaise;		// Raise selected nodes?

    // Hint stuff
    Boolean showHints;		// Display hints?

    // Grid stuff
    Boolean showGrid;		// Display grid?
    Dimension gridWidth;	// Grid width (> 0)
    Dimension gridHeight;	// Grid height (> 0)
    Boolean snapToGrid;		// Make sure nodes are on grid?

    // Layout stuff
    Boolean autoLayout;		// Layout graph after each change?

    // Rotation stuff
    Cardinal rotation;		// Rotation in degrees (starting with zero)

    // Sizes
    Dimension hintSize;		// Hint size (in pixels)
    Dimension edgeWidth;	// Edge width (in pixels)
    Dimension arrowAngle;	// Arrow angle (in degrees)
    Dimension arrowLength;	// Arrow length (in pixels)
    Dimension selfEdgeDiameter; // Diameter of edge pointing at self

    // Extra size to be added to the graph size; for panner usage only
    Dimension extraWidth;	// Additional width (in pixels)
    Dimension extraHeight;	// Additional height (in pixels)

    // Pixmaps
    Pixmap selectTile;		// Tile to apply when node is selected

    // Modes
    EdgeAttachMode edgeAttachMode;       // How to attach edges on nodes
    LayoutMode layoutMode;	         // How to layout graphs
    SelfEdgePosition  selfEdgePosition;  // Position of self-pointing edges
    SelfEdgeDirection selfEdgeDirection; // Its direction

    // Cursors
    Cursor defaultCursor;	         // Standard cursor to use
    Cursor moveCursor;			 // Cursor to use when moving
    Cursor selectCursor;	         // Cursors to use when selecting
    Cursor selectBottomLeftCursor;
    Cursor selectBottomRightCursor;
    Cursor selectTopLeftCursor;
    Cursor selectTopRightCursor;

    // Colors
    Pixel nodeColor;		          // Color to use for nodes
    Pixel edgeColor;		          // Color to use for edges
    Pixel frameColor;		          // Color to use for frame
    Pixel outlineColor;		          // Color to use for outlines
    Pixel gridColor;		          // Color to use for grid
    Pixel selectColor;	                  // Color to use for selected nodes

    // Callbacks
    XtCallbackList positionChangedProc;   // Position changed
    XtCallbackList selectionChangedProc;  // Selection changed
    XtCallbackList sizeChangedProc;       // Size changed
    XtCallbackList compareNodesProc;      // Compare nodes (for layouting)
    XtCallbackList preLayoutProc;         // Called before layouting
    XtCallbackList postLayoutProc;        // Called after layouting

    // Private state
    GraphEditState state;	// Current state

    BoxPoint startAction;	// Starting point action
    BoxPoint endAction;		// End point action
    BoxPoint minimalOffset;	// Minimal offset for moving
    BoxPoint lastOffset;	// Last offset for moving

    Time lastSelectTime;	// Last selection time (for multi-clicks)
    XtIntervalId redrawTimer;	// Timer used for redrawing

    GC nodeGC;			// Graphic context for nodes
    GC edgeGC;			// Graphic context for edges
    GC invertGC;		// Graphic context for inverting
    GC clearGC;		        // Graphic context for clearing
    GC frameGC;			// Graphic context for frame
    GC outlineGC;		// Graphic context for outlines

    GraphGC graphGC;		// Graph graphic context

    Pixmap gridPixmap;		// Grid pixmap

    Boolean sizeChanged;	// Flag: size changed in SetValues
    Boolean redisplayEnabled;	// Flag: redisplay is enabled

} GraphEditPart;

typedef struct _GraphEditRec {
    CorePart        core;
    XmPrimitivePart primitive;
    GraphEditPart   graphEdit;
} GraphEditRec;

#endif // _DDD_GraphEditP_h
// DON'T ADD ANYTHING BEHIND THIS #endif
