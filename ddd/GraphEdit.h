// $Id$
// GraphEdit Widget -- public interface

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
// Revision 1.37  1995/04/22  16:20:22  zeller
// New: selectCursor
//
// Revision 1.36  1995/04/16  09:48:19  zeller
// New: compact layout
//
// Revision 1.35  1995/04/14  16:29:13  zeller
// New: extraWidth, extraHeight (for panner)
//
// Revision 1.34  1995/04/10  08:26:25  zeller
// New: optional compare function
//
// Revision 1.33  1995/04/09  00:00:32  zeller
// New: color resources
//
// Revision 1.32  1995/04/04  13:27:45  zeller
// New: selectionChanged callback
//
// Revision 1.31  1995/03/29  08:25:53  zeller
// New: automatic layout
// New: layout() and rotate() accept relative and absolute rotation specs
//
// Revision 1.30  1995/03/28  16:36:08  zeller
// New: rotate graph
//
// Revision 1.29  1995/03/21  22:13:19  zeller
// New: redrawing of single nodes (better than redrawing regions)
//
// Revision 1.28  1995/03/21  21:05:29  zeller
// New: public redraw functions
//

#ifndef _Nora_GraphEdit_h
#define _Nora_GraphEdit_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Graph.h"
#include <X11/Intrinsic.h>


// define any special resource names here that are not in <X11/StringDefs.h>

// Resource names

#define XtNgraph     	            "graph"
#define XtNmoveDelta                "moveDelta"
#define XtNrubberEdges              "rubberEdges"
#define XtNrubberArrows             "rubberArrows"
#define XtNshowHints                "showHints"
#define XtNhintSize                 "hintSize"
#define XtNopaqueMove               "opaqueMove"
#define XtNgridWidth                "gridWidth"
#define XtNgridHeight               "gridHeight"
#define XtNextraWidth               "extraWidth"
#define XtNextraHeight              "extraHeight"
#define XtNshowGrid                 "showGrid"
#define XtNsnapToGrid               "snapToGrid"
#define XtNedgeWidth                "edgeWidth"
#define XtNedgeAttachMode	    "edgeAttachMode"
#define XtNlayoutMode	            "layoutMode"
#define XtNarrowAngle		    "arrowAngle"
#define XtNarrowLength		    "arrowLength"
#define XtNdefaultCursor            "defaultCursor"
#define XtNmoveCursor               "moveCursor"
#define XtNselectCursor             "selectCursor"
#define XtNselectBottomLeftCursor   "selectBottomLeftCursor"
#define XtNselectBottomRightCursor  "selectBottomRightCursor"
#define XtNselectTopLeftCursor      "selectTopLeftCursor"
#define XtNselectTopRightCursor     "selectTopRightCursor"
#define XtNpositionChangedCallback  "positionChanged"
#define XtNselectionChangedCallback "selectionChanged"
#define XtNsizeChangedCallback      "sizeChanged"
#define XtNcompareNodesCallback     "compareNodes"
#define XtNselectTile               "selectTile"
#define XtNrotation		    "rotation"
#define XtNautoLayout		    "autoLayout"
#define XtNnodeColor                "nodeColor"
#define XtNedgeColor                "edgeColor"
#define XtNframeColor               "frameColor"
#define XtNgridColor                "gridColor"
#define XtNselectColor              "selectColor"



// Class types

#define XtCGraph     	   "Graph"
#define XtCMoveDelta       "MoveDelta"
#define XtCRubberEdges     "RubberEdges"
#define XtCRubberArrows    "RubberArrows"
#define XtCShowHints       "ShowHints"
#define XtCHintSize        "HintSize"
#define XtCOpaqueMove      "OpaqueMove"
#define XtCGridWidth       "GridWidth"
#define XtCGridHeight      "GridHeight"
#define XtCExtraWidth      "ExtraWidth"
#define XtCExtraHeight     "ExtraHeight"
#define XtCShowGrid        "ShowGrid"
#define XtCSnapToGrid      "SnapToGrid"
#define XtCEdgeWidth       "EdgeWidth"
#define XtCEdgeAttachMode  "EdgeAttachMode"
#define XtCLayoutMode      "LayoutMode"
#define XtCArrowAngle	   "ArrowAngle"
#define XtCArrowLength	   "ArrowLength"
#define XtCRotation	   "Rotation"
#define XtCAutoLayout	   "AutoLayout"


// Representation types

#define XtREdgeAttachMode  "EdgeAttachMode"
#define XtRLayoutMode      "LayoutMode"


// declare specific GraphEdit class and instance datatypes
typedef struct _GraphEditClassRec *GraphEditWidgetClass;
typedef struct _GraphEditRec      *GraphEditWidget;


// Callback Infos
struct GraphEditPositionChangedInfo {
    Graph     *graph;          // Graph this node is in
    GraphNode *node;           // Node that changed position
    BoxPoint  old_position;    // Old node position
    BoxPoint  new_position;    // New node position
    Boolean   is_last;         // True iff no more changes follow

    GraphEditPositionChangedInfo():
        graph(0), node(0), is_last(False)
    {}
    GraphEditPositionChangedInfo(const GraphEditPositionChangedInfo& info):
        graph(info.graph), node(info.node), 
        old_position(info.old_position),
        new_position(info.new_position),
        is_last(info.is_last)
    {}
};

struct GraphEditSelectionChangedInfo {
    Graph     *graph;          // Graph this node is in

    GraphEditSelectionChangedInfo():
        graph(0)
    {}
    GraphEditSelectionChangedInfo(const GraphEditSelectionChangedInfo& info):
        graph(info.graph)
    {}
};

struct GraphEditCompareNodesInfo {
    Graph     *graph;          // Graph these nodes are in
    GraphNode *node1;	       // Nodes to compare
    GraphNode *node2;
    int       result;          // Compare result (read/write; < 0, 0, > 0)

    GraphEditCompareNodesInfo():
        graph(0),
	node1(0),
	node2(0),
	result(0)
    {}
    GraphEditCompareNodesInfo(const GraphEditCompareNodesInfo& info):
        graph(info.graph),
	node1(info.node1),
	node2(info.node2),
	result(info.result)
    {}
};


// Modes
enum LayoutMode {
    RegularLayoutMode,	// use regular Sugiyama/Misue layout method
    CompactLayoutMode,	// use a more compact alternative
};

// declare the class constant
extern WidgetClass graphEditWidgetClass;

// declare special access functions

// Inform widget that its size has changed
extern void graphEditSizeChanged(Widget w);

// Get the graph's Graphic Context
extern const GraphGC& graphEditGetGraphGC(Widget w);

// Get the graph
extern Graph *graphEditGetGraph(Widget w);

// Get node at point
extern GraphNode *graphEditGetNodeAtPoint(Widget w, BoxPoint p);

// Get node at event
extern GraphNode *graphEditGetNodeAtEvent(Widget w, XEvent *e);

// Redraw all
extern void graphEditRedraw(Widget w);

// Redraw a single node
extern void graphEditRedrawNode(Widget w, GraphNode *node);

#endif
