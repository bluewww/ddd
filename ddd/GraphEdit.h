// $Id$
// GraphEdit Widget -- public interface

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

#ifndef _DDD_GraphEdit_h
#define _DDD_GraphEdit_h

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
#define XtNautoRaise                "autoRaise"
#define XtNgridWidth                "gridWidth"
#define XtNgridHeight               "gridHeight"
#define XtNextraWidth               "extraWidth"
#define XtNextraHeight              "extraHeight"
#define XtNrequestedWidth           "requestedWidth"
#define XtNrequestedHeight          "requestedHeight"
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
#define XtNpreLayoutCallback        "preLayout"
#define XtNpostLayoutCallback       "postLayout"
#define XtNpreSelectionCallback     "preSelection"
#define XtNselectTile               "selectTile"
#define XtNrotation		    "rotation"
#define XtNautoLayout		    "autoLayout"
#define XtNnodeColor                "nodeColor"
#define XtNedgeColor                "edgeColor"
#define XtNframeColor               "frameColor"
#define XtNoutlineColor             "outlineColor"
#define XtNgridColor                "gridColor"
#define XtNselectColor              "selectColor"
#define XtNselfEdgeDiameter         "selfEdgeDiameter"
#define XtNselfEdgePosition         "selfEdgePosition"
#define XtNselfEdgeDirection        "selfEdgeDirection"



// Class types

#define XtCGraph     	     "Graph"
#define XtCMoveDelta         "MoveDelta"
#define XtCRubberEdges       "RubberEdges"
#define XtCRubberArrows      "RubberArrows"
#define XtCShowHints         "ShowHints"
#define XtCHintSize          "HintSize"
#define XtCOpaqueMove        "OpaqueMove"
#define XtCAutoRaise         "AutoRaise"
#define XtCGridSize          "GridSize"
#define XtCExtraSize         "ExtraSize"
#define XtCRequestedSize     "RequestedSize"
#define XtCShowGrid          "ShowGrid"
#define XtCSnapToGrid        "SnapToGrid"
#define XtCEdgeWidth         "EdgeWidth"
#define XtCEdgeAttachMode    "EdgeAttachMode"
#define XtCLayoutMode        "LayoutMode"
#define XtCArrowAngle	     "ArrowAngle"
#define XtCArrowLength	     "ArrowLength"
#define XtCRotation	     "Rotation"
#define XtCAutoLayout	     "AutoLayout"
#define XtCSelfEdgeDiameter  "SelfEdgeDiameter"
#define XtCSelfEdgePosition  "SelfEdgePosition"
#define XtCSelfEdgeDirection "SelfEdgeDirection"


// Representation types

#define XtREdgeAttachMode    "EdgeAttachMode"
#define XtRLayoutMode        "LayoutMode"
#define XtRSelfEdgePosition  "SelfEdgePosition"
#define XtRSelfEdgeDirection "SelfEdgeDirection"


// Declare specific GraphEdit class and instance datatypes
typedef struct _GraphEditClassRec *GraphEditWidgetClass;
typedef struct _GraphEditRec      *GraphEditWidget;


// Modes
enum LayoutMode {
    RegularLayoutMode,		// Use regular Sugiyama/Misue layout method
    CompactLayoutMode		// Use a more compact alternative
};

enum SelectionMode { 
    SetSelection,		// Set the selection
    ExtendSelection,		// Extend the selection
    ToggleSelection		// Toggle the selection
};


// Callback Infos
struct GraphEditPositionChangedInfo {
    Graph     *graph;          // Graph this node is in
    GraphNode *node;           // Node that changed position
    BoxPoint  old_position;    // Old node position
    BoxPoint  new_position;    // New node position
    Boolean   is_last;         // True iff no more changes follow

    GraphEditPositionChangedInfo():
        graph(0), node(0), 
	old_position(),
	new_position(),
	is_last(False)
    {}
    GraphEditPositionChangedInfo(const GraphEditPositionChangedInfo& info):
        graph(info.graph), node(info.node), 
        old_position(info.old_position),
        new_position(info.new_position),
        is_last(info.is_last)
    {}
    GraphEditPositionChangedInfo& 
        operator = (const GraphEditPositionChangedInfo& info)
    {
	graph        = info.graph;
	node         = info.node;
	old_position = info.old_position;
	new_position = info.new_position;
	is_last      = info.is_last;
	return *this;
    }
};

struct GraphEditPreSelectionInfo {
    Graph     *graph;		// Graph this node is in
    GraphNode *node;		// Selected node
    Boolean   double_click;	// Double-click?
    Boolean   doit;		// Flag: do default action?

    GraphEditPreSelectionInfo():
        graph(0), node(0), double_click(False), doit(True)
    {}
    GraphEditPreSelectionInfo(const GraphEditPreSelectionInfo& info):
        graph(info.graph), node(info.node), double_click(info.double_click),
	doit(info.doit)
    {}
    GraphEditPreSelectionInfo& 
        operator = (const GraphEditPreSelectionInfo& info)
    {
	graph        = info.graph;
	node         = info.node;
	double_click = info.double_click;
	doit         = info.doit;
	return *this;
    }
};

typedef GraphEditPreSelectionInfo GraphEditSelectionChangedInfo;

struct GraphEditLayoutInfo {
    Graph     *graph;		// Graph this node is in
    LayoutMode mode;		// Current layout mode
    int rotation;		// Rotation (in degrees)

    GraphEditLayoutInfo():
        graph(0), mode(RegularLayoutMode), rotation(0)
    {}
    GraphEditLayoutInfo(const GraphEditLayoutInfo& info):
        graph(info.graph), mode(info.mode), rotation(info.rotation)
    {}
    GraphEditLayoutInfo& 
        operator = (const GraphEditLayoutInfo& info)
    {
	graph    = info.graph;
	mode     = info.mode;
	rotation = info.rotation;
	return *this;
    }
};

struct GraphEditCompareNodesInfo {
    Graph     *graph;		// Graph these nodes are in
    GraphNode *node1;		// Nodes to compare
    GraphNode *node2;
    int       result;		// Compare result (read/write; < 0, 0, > 0)

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
    GraphEditCompareNodesInfo& 
        operator = (const GraphEditCompareNodesInfo& info)
    {
        graph  = info.graph;
	node1  = info.node1;
	node2  = info.node2;
	result = info.result;
	return *this;
    }
};


// Declare the class constant
extern WidgetClass graphEditWidgetClass;

// Declare special access functions

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

// Raise a single node
extern void graphEditRaiseNode(Widget w, GraphNode *node);

// Return final position (if snapToGrid is enabled, for example)
extern BoxPoint graphEditFinalPosition(Widget w, const BoxPoint& p);

// Disable or enable display
extern Boolean graphEditEnableRedisplay(Widget w, Boolean state);

#endif // _DDD_GraphEdit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
