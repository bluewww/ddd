// $Id$
// GraphEdit Widget -- public interface

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_GraphEdit_h
#define _DDD_GraphEdit_h

#ifdef IF_MOTIF

#include <X11/Intrinsic.h>

#else // NOT IF_MOTIF

#include "gtk_wrapper.h"
#include <gtkmm/drawingarea.h>

#endif // IF_MOTIF

#include "BoxPoint.h"

class Graph;
class GraphGC;
class GraphNode;


// define any special resource names here that are not in <X11/StringDefs.h>

// Resource names

#define XtNgraph     	            "graph"
#define XtNmoveDelta                "moveDelta"
#define XtNrubberEdges              "rubberEdges"
#define XtNrubberArrows             "rubberArrows"
#define XtNrubberAnnotations        "rubberAnnotations"
#define XtNshowHints                "showHints"
#define XtNshowAnnotations          "showAnnotations"
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
#define XtNnodePrintColor           "nodePrintColor"
#define XtNedgePrintColor           "edgePrintColor"
#define XtNselfEdgeDiameter         "selfEdgeDiameter"
#define XtNselfEdgePosition         "selfEdgePosition"
#define XtNselfEdgeDirection        "selfEdgeDirection"
#define XtNdashedLines              "dashedLines"



// Class types

#define XtCGraph     	     "Graph"
#define XtCMoveDelta         "MoveDelta"
#define XtCRubberEdges       "RubberEdges"
#define XtCRubberArrows      "RubberArrows"
#define XtCRubberAnnotations "RubberAnnotations"
#define XtCShowHints         "ShowHints"
#define XtCShowAnnotations   "ShowAnnotations"
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
#define XtCDashedLines       "DashedLines"


// Representation types

#define XtREdgeAttachMode    "EdgeAttachMode"
#define XtRLayoutMode        "LayoutMode"
#define XtRSelfEdgePosition  "SelfEdgePosition"
#define XtRSelfEdgeDirection "SelfEdgeDirection"


// Declare specific GraphEdit class and instance datatypes
#ifdef IF_MOTIF
typedef struct _GraphEditClassRec *GraphEditWidgetClass;
typedef struct _GraphEditRec      *GraphEditWidget;
#endif // IF_MOTIF


// Modes
enum LayoutMode {
    RegularLayoutMode,		// Use regular Sugiyama/Misue layout method
    CompactLayoutMode		// Use a more compact alternative
};

enum ShowHideMode {
    NopeShowHideMode = 0,
    BothShowHideMode = 1,
    FromShowHideMode = 2,
    ToShowHideMode = 3,
    AnyShowHideMode = 4
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
        if (this != &info) {
	  graph        = info.graph;
	  node         = info.node;
	  old_position = info.old_position;
	  new_position = info.new_position;
	  is_last      = info.is_last;
	}
	return *this;
    }
};

struct GraphEditPreSelectionInfo {
    Graph     *graph;		// Graph this node is in
    GraphNode *node;		// Selected node
    XEvent    *event;		// Event
    Boolean   double_click;	// Double-click?
    Boolean   doit;		// Flag: do default action?

    GraphEditPreSelectionInfo():
        graph(0), node(0), event(0), double_click(False), doit(True)
    {}
    GraphEditPreSelectionInfo(const GraphEditPreSelectionInfo& info):
        graph(info.graph), node(info.node), event(info.event),
	double_click(info.double_click), doit(info.doit)
    {}
    GraphEditPreSelectionInfo& 
        operator = (const GraphEditPreSelectionInfo& info)
    {
        if (this != &info) {
	  graph        = info.graph;
	  node         = info.node;
	  event        = info.event;
	  double_click = info.double_click;
	  doit         = info.doit;
	}
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
        if (this != &info) {
	  graph    = info.graph;
	  mode     = info.mode;
	  rotation = info.rotation;
	}
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
        if (this != &info) {
	  graph  = info.graph;
	  node1  = info.node1;
	  node2  = info.node2;
	  result = info.result;
	}
	return *this;
    }
};


#ifdef IF_MOTIF

// Declare the class constant
extern WidgetClass graphEditWidgetClass;

typedef Widget GRAPH_EDIT_P;

#else // NOT IF_MOTIF

#include "GraphGC.h"
#include <gtkmm/drawingarea.h>
#include <gdkmm/color.h>

enum GraphEditState {
    SelectState,	// draw select frame
    DeltaState,		// wait for movement
    MoveState,		// draw move frames
    NopState		// something completely different
};

class GtkGraphEdit: public Gtk::Widget {
private:
#if 0
    bool show_grid_;
    bool snap_to_grid_;
    bool show_hints_;
    bool show_annotations_;
    bool auto_layout_;
    LayoutMode layout_mode_;
    int grid_width_;
    int grid_height_;
    bool dashed_lines_;
    Cardinal rotation_;
#endif
public:
    GtkGraphEdit(void);
    ~GtkGraphEdit(void);
    bool on_expose_event(GdkEventExpose* event);
    bool get_show_grid(void);
    void set_show_grid(bool);
    bool get_snap_to_grid(void);
    void set_snap_to_grid(bool);
    bool get_show_hints(void);
    void set_show_hints(bool);
    bool get_show_annotations(void);
    void set_show_annotations(bool);
    bool get_auto_layout(void);
    void set_auto_layout(bool);
    LayoutMode get_layout_mode(void);
    void set_layout_mode(LayoutMode);
    int get_grid_width(void);
    void set_grid_width(int);
    int get_grid_height(void);
    void set_grid_height(int);
    bool get_dashed_lines(void);
    void set_dashed_lines(bool);
    Cardinal get_rotation(void);
    void set_rotation(Cardinal);
    Position get_x(void);
    Position get_y(void);
    Graph *get_graph(void);
    const GraphGC& get_graph_GC(void);
    void set_graph(Graph *);
    // Get node at point
    GraphNode *graphEditGetNodeAtPoint(const BoxPoint&);
    // Redraw a single node
    void graphEditRedrawNode(GraphNode *);
    // Return final position (if snapToGrid is enabled, for example)
    BoxPoint final_position(const BoxPoint&);
    // Disable or enable display
    bool enable_redisplay(bool state);

private:
    
    int highlight_thickness;


    // Graph stuff
    Graph *graph;		// Graph to be displayed

    // Movement stuff
    Dimension moveDelta;	// Start move after movement greater than this
    bool rubberEdges;		// Draw edges  when moving
    bool rubberArrows;		// Draw arrows when moving
    bool rubberAnnotations;	// Draw annotations when moving
    bool opaqueMove;		// Draw nodes  when moving

    // Selection stuff
    bool autoRaise;		// Raise selected nodes?

    // Hint stuff
    bool showHints;		// Display hints?

    // Annotation stuff
    bool showAnnotations;	// Display annotations?

    // Grid stuff
    bool showGrid;		// Display grid?
    Dimension gridWidth;	// Grid width (> 0)
    Dimension gridHeight;	// Grid height (> 0)
    bool snapToGrid;		// Make sure nodes are on grid?

    // Layout stuff
    bool autoLayout;		// Layout graph after each change?

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

    // Requested sizes; for panner and scrollbar creation only
    Dimension requestedWidth;	// Requested width (in pixels)
    Dimension requestedHeight;	// Requested height (in pixels)

    // Pixmaps
    Pixmap selectTile;		// Tile to apply when node is selected

    // Modes
    EdgeAttachMode edgeAttachMode;	 // How to attach edges on nodes
    LayoutMode layoutMode;		 // How to layout graphs
    SelfEdgePosition  selfEdgePosition;	 // Position of self-pointing edges
    SelfEdgeDirection selfEdgeDirection; // Its direction
    bool dashedLines;			 // Whether dashed lines are to be used

    // Cursors
    Cursor defaultCursor;	         // Standard cursor to use
    Cursor moveCursor;			 // Cursor to use when moving
    Cursor selectCursor;	         // Cursors to use when selecting
    Cursor selectBottomLeftCursor;
    Cursor selectBottomRightCursor;
    Cursor selectTopLeftCursor;
    Cursor selectTopRightCursor;

    // Colors
    Gdk::Color nodeColor;		          // Color to use for nodes
    Gdk::Color edgeColor;		          // Color to use for edges
    Gdk::Color frameColor;		          // Color to use for frame
    Gdk::Color outlineColor;		          // Color to use for outlines
    Gdk::Color gridColor;		          // Color to use for grid
    Gdk::Color selectColor;	                  // Color to use for selected nodes

    // Printing colors
    String nodePrintColor;	          // Color for printing nodes
    String edgePrintColor;	          // Color for printing edges

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
    GC clearGC;			// Graphic context for clearing
    GC frameGC;			// Graphic context for frame
    GC outlineGC;		// Graphic context for outlines

    GraphGC graphGC;		// Graph graphic context

    Pixmap gridPixmap;		// Grid pixmap

    bool sizeChanged;		// Flag: size changed in SetValues
    bool redisplayEnabled;	// Flag: redisplay is enabled

    // Inform widget that its size has changed
    void graphEditSizeChanged(void);

    // Get node at event
    GraphNode *graphEditGetNodeAtEvent(XEvent *e);

    // Redraw all
    void graphEditRedraw(void);

    // Raise a single node
    void graphEditRaiseNode(GraphNode *node);

    // PRIVATE methods (were STATIC functions)
    void defineCursor(Cursor cursor);

    void setGrid(bool reset = false);
    void RedrawCB(void);
    void StartRedraw(void);
    void setGCs(void);
    void setGraphGC(void);
    BoxRegion frameRegion(void);
    void setRegionCursor(void);
    void redrawSelectFrame(const BoxRegion& r);
    void drawSelectFrames(const BoxRegion& r0, 
			  const BoxRegion& r1);
    void drawSelectFrame(void);
    void redrawSelectFrame(const BoxPoint& p);
    void getMinimalOffset(void);
    BoxPoint actionOffset(void);
    void drawOutlines(const BoxPoint& offset);
    void moveTo(GraphNode *node, 
		const BoxPoint& newPos,
		bool isLast);
    Boolean select_graph(GraphNode *root, Boolean set = True);
    Boolean unselect_graph(GraphNode *root);
    void MoveSelected(XEvent *, string movx, string movy);

    // Callbacks
    void selectionChanged(XEvent *event, Boolean double_click);

    // Actions
    Boolean _SelectAll(void);
    void SelectAll(void);
    Boolean _UnselectAll(void);
    void UnselectAll(void);

    void raise_node(GraphNode *node);
    void _SelectOrMove(XEvent *event,
		       SelectionMode mode, Boolean follow);

    void SelectOrMove(XEvent *event);
    void ExtendOrMove(XEvent *event);
    void ToggleOrMove(XEvent *event);
    void Select(XEvent *event);
    void Extend(XEvent *event);
    void Toggle(XEvent *event);
    void Follow(XEvent *event);
    void move_selected_nodes(const BoxPoint& offset);
    void End(XEvent *event);
    void select_single_node(XEvent *event, GraphNode *selectNode);
    void SelectFirst(XEvent *event);
    void SelectNext(XEvent *event);
    void SelectPrev(XEvent *event);
    void _Layout(LayoutMode mode=RegularLayoutMode);
    void DoLayout(LayoutMode mode=RegularLayoutMode);
    void _Normalize(void);
    void Normalize(void);
    void considerEdges(ShowHideMode how, Boolean shallBeHidden);
    void HideEdges(ShowHideMode themode=NopeShowHideMode);
    void ShowEdges(ShowHideMode themode=NopeShowHideMode);
protected:
    sigc::signal<void, GraphEditPositionChangedInfo *> signal_position_changed_;
    sigc::signal<void, GraphEditSelectionChangedInfo *> signal_selection_changed_;
    sigc::signal<void, GraphEditPreSelectionInfo *> signal_pre_selection_;
    sigc::signal<void, GraphEditCompareNodesInfo *> signal_compare_nodes_;
    sigc::signal<void, GraphEditLayoutInfo *> signal_pre_layout_;
    sigc::signal<void, GraphEditLayoutInfo *> signal_post_layout_;
public:
    // Signals:
    sigc::signal<void, GraphEditPositionChangedInfo *> signal_position_changed();
    sigc::signal<void, GraphEditSelectionChangedInfo *> signal_selection_changed();
    sigc::signal<void, GraphEditPreSelectionInfo *> signal_pre_selection();
    sigc::signal<void, GraphEditCompareNodesInfo *> signal_compare_nodes();
    sigc::signal<void, GraphEditLayoutInfo *> signal_pre_layout();
    sigc::signal<void, GraphEditLayoutInfo *> signal_post_layout();
};

typedef GtkGraphEdit *GRAPH_EDIT_P;
typedef GtkGraphEdit *GraphEditWidget;

#endif // IF_MOTIF

#ifdef IF_MOTIF
// Declare special access functions

// Inform widget that its size has changed
extern void graphEditSizeChanged(Widget w);

// Get the graph's Graphic Context
extern const GraphGC& graphEditGetGraphGC(Widget w);

// Get the graph
extern Graph *graphEditGetGraph(Widget w);

// Get node at point
extern GraphNode *graphEditGetNodeAtPoint(Widget w, const BoxPoint& p);

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
#endif // IF_MOTIF

#endif // _DDD_GraphEdit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
