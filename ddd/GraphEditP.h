// $Id$
// GraphEdit Widget -- private interface

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
// Revision 1.1  1995/05/01 15:47:42  zeller
// Initial revision
//
// Revision 1.34  1995/04/22  16:20:15  zeller
// New: selectCursor
//
// Revision 1.33  1995/04/16  09:48:31  zeller
// New: compact layout
//
// Revision 1.32  1995/04/14  16:29:07  zeller
// New: extraWidth, extraHeight (for panner)
//
// Revision 1.31  1995/04/10  08:26:54  zeller
// New: optional compare function
//
// Revision 1.30  1995/04/09  00:00:15  zeller
// New: color resources
//
// Revision 1.29  1995/04/04  13:27:36  zeller
// New: selectionChanged callback
//
// Revision 1.28  1995/03/30  13:08:33  zeller
// Fixes for later Motif revisions
//
// Revision 1.27  1995/03/29  08:25:47  zeller
// New: automatic layout
// New: layout() and rotate() accept relative and absolute rotation specs
//
// Revision 1.26  1995/03/28  16:36:14  zeller
// New: rotate graph
//
// Revision 1.25  1993/05/22  20:11:03  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.24  1993/04/22  11:00:21  zeller
// Lizenz verbessert
// 
// Revision 1.23  1993/04/20  23:00:58  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.22  1993/04/16  11:38:35  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.21  1992/11/28  13:38:12  zeller
// - Neue Ressource: defaultCursor
// 
// Revision 1.20  1992/11/13  19:09:08  zeller
// Neu: hints optional anzeigen; Felder besser dokumentiert
// 
// Revision 1.19  1992/11/11  14:12:25  zeller
// Neu: SetValues() prueft, ob sich Groesse veraendert hat
// 
// Revision 1.18  1992/11/11  09:42:52  zeller
// Neu: Grid-Anzeige
// 
// Revision 1.17  1992/11/05  19:03:44  zeller
// Neue Graphik-Kontexte -- vermeiden allzuhaeufiges Umschalten;
// Neu: selectTile -- Bitmap beim Selektieren
// 
// Revision 1.16  1992/11/04  14:26:08  zeller
// Neu: positionChangedCallback
// 
// Revision 1.15  1992/06/02  00:55:54  zeller
// Anpassung an gcc 2.1
// 
// Revision 1.14  1992/06/01  16:59:05  zeller
// Neu: #pragma interface
// 
// Revision 1.13  1992/06/01  16:55:33  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.12  1992/03/16  17:13:03  zeller
// Neu: Umwandlung String nach EdgeAttachMode
// 
// Revision 1.11  1992/03/16  16:01:28  zeller
// Neue Verfahren, um Verbindungspunkte der Kanten zu bestimmen
// 
// Revision 1.10  1992/02/28  00:01:48  zeller
// Neue Ressourcen: rubberEdges, rubberArrows, edgeWidth,
// arrowAngle, arrowLength
// 
// Revision 1.9  1992/02/27  20:10:42  zeller
// Neu: grid
// 
// Revision 1.8  1992/02/27  15:26:12  zeller
// Neu: minimalOffset
// 
// Revision 1.7  1992/02/27  13:22:41  zeller
// Neu: Custom-Cursors
// 
// Revision 1.6  1992/02/27  11:24:03  zeller
// Neu: moveDelta, rubberBanding, opaqueMove;
// somethingSelected entfernt
// 
// Revision 1.5  1992/02/27  11:01:11  zeller
// MoveDelta eingefuehrt
// 
// Revision 1.4  1992/02/27  10:40:07  zeller
// Neue Aktionen: Select und Move mit einem einzigen Knopf
// 
// Revision 1.3  1992/02/26  16:42:46  zeller
// Neu: Move mit Rubberbanding
// 
// Revision 1.2  1992/02/26  13:25:51  zeller
// Neu: Selektion vorbereitet
// 
// Revision 1.1  1992/02/26  11:52:22  zeller
// Initial revision
// 

#ifndef _Nora_GraphEditP_h
#define _Nora_GraphEditP_h

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
    NopState,		// something completely different
};

// The GraphEdit instance record

typedef struct _GraphEditPart {
    // resources

    // Graph stuff
    Graph *graph;	       // Graph to be displayed

    // Movement stuff
    Dimension moveDelta;       // Start move after movement greater than this
    Boolean rubberEdges;       // Draw edges  when moving
    Boolean rubberArrows;      // Draw arrows when moving
    Boolean opaqueMove;	       // Draw nodes  when moving

    // Hint stuff
    Boolean showHints;         // Display hints?

    // Grid stuff
    Boolean showGrid;          // Display grid?
    Dimension gridWidth;       // grid width (0 if none)
    Dimension gridHeight;      // grid height (0 if none)
    Boolean snapToGrid;        // Make sure nodes are on grid?

    // Layout stuff
    Boolean autoLayout;        // Layout graph after each change?

    // Rotation stuff
    Cardinal rotation;	       // Rotation in degrees (starting with zero)

    // Sizes
    Dimension hintSize;        // hint size (in pixels)
    Dimension edgeWidth;       // edge width (in pixels)
    Dimension arrowAngle;      // arrow angle (in degrees)
    Dimension arrowLength;     // arrow length (in pixels)

    // Extra size to be added to the graph size; for panner usage only
    Dimension extraWidth;      // additional width (in pixels)
    Dimension extraHeight;     // additional height (in pixels)

    // Pixmaps
    Pixmap selectTile;         // Tile to apply when node is selected

    // Modes
    EdgeAttachMode edgeAttachMode;      // how to attach edges on nodes
    LayoutMode layoutMode;              // how to layout graphs

    // Cursors
    Cursor defaultCursor;		// Standard cursor to use
    Cursor moveCursor;			// Cursor to use when moving
    Cursor selectCursor;	        // Cursors to use when selecting
    Cursor selectBottomLeftCursor;
    Cursor selectBottomRightCursor;
    Cursor selectTopLeftCursor;
    Cursor selectTopRightCursor;

    // Colors
    Pixel nodeColor;		          // Color to use for nodes
    Pixel edgeColor;		          // Color to use for edges
    Pixel frameColor;		          // Color to use for frames
    Pixel gridColor;		          // Color to use for grid
    Pixel selectColor;	                  // Color to use for selected nodes

    // Callbacks
    XtCallbackList positionChangedProc;   // Position changed
    XtCallbackList selectionChangedProc;  // Selection changed
    XtCallbackList sizeChangedProc;       // Size changed
    XtCallbackList compareNodesProc;      // Compare nodes (for layouting)

    // Private state
    GraphEditState state;	// Current state

    BoxPoint startAction;	// starting point action
    BoxPoint endAction;		// end point action
    BoxPoint minimalOffset;	// minimal offset for moving
    BoxPoint lastOffset;	// last offset for moving

    Time lastSelectTime;	// last selection time (for multi-clicks)

    GC nodeGC;			// graphic context for nodes
    GC edgeGC;			// graphic context for edges
    GC invertGC;		// graphic context for inverting
    GC clearGC;		        // graphic context for clearing
    GC frameGC;			// graphic context for frames

    GraphGC graphGC;		// Graph graphic context

    Pixmap gridPixmap;		// Grid pixmap

    Boolean sizeChanged;	// Flag: size changed in SetValues

} GraphEditPart;

typedef struct _GraphEditRec {
    CorePart        core;
    XmPrimitivePart primitive;
    GraphEditPart   graphEdit;
} GraphEditRec;

#endif
