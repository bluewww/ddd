// $Id$
// GraphEdit Widget

// Copyright (C) 1995, 1996 Technische Universitaet Braunschweig, Germany.
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

char GraphEdit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "VarArray.h"
#pragma implementation "DynArray.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>

#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "events.h"
#include "Graph.h"
#include "GraphEditP.h"
#include "GraphEdit.h"
#include "HintGraphN.h"
#include "LineGraphE.h"
#include "VarArray.h"
#include "layout.h"


static BoxRegion EVERYWHERE(BoxPoint(0,0), BoxSize(INT_MAX, INT_MAX));


// Compute default foreground

static void defaultForeground(Widget w, int, XrmValue *value)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    value->addr = caddr_t(&_w->primitive.foreground);
}


// Resource list

static XtResource resources[] = {
#define offset(field) XtOffsetOf(GraphEditRec, graphEdit.field)
    // {name, class, type, size, offset, default_type, default_addr}
    { XtNgraph, XtCGraph, XtRPointer, sizeof(Graph *),
	offset(graph), XtRImmediate, XtPointer(NULL) },
    { XtNmoveDelta, XtCMoveDelta, XtRDimension, sizeof(Dimension),
	offset(moveDelta), XtRImmediate, XtPointer(4) },
    { XtNrubberEdges, XtCRubberEdges, XtRBoolean, sizeof(Boolean),
	offset(rubberEdges), XtRImmediate, XtPointer(true) },
    { XtNrubberArrows, XtCRubberEdges, XtRBoolean, sizeof(Boolean),
	offset(rubberArrows), XtRImmediate, XtPointer(false) },
    { XtNopaqueMove, XtCOpaqueMove, XtRBoolean, sizeof(Boolean),
	offset(opaqueMove), XtRImmediate, XtPointer(false) },

    { XtNshowHints, XtCShowHints, XtRBoolean, sizeof(Boolean),
	offset(showHints), XtRImmediate, XtPointer(false) },
    { XtNhintSize, XtCHintSize, XtRDimension, sizeof(Dimension),
	offset(hintSize), XtRImmediate, XtPointer(6) },

    { XtNgridWidth, XtCGridWidth, XtRDimension, sizeof(Dimension),
	offset(gridWidth), XtRImmediate, XtPointer(16) },
    { XtNgridHeight, XtCGridHeight, XtRDimension, sizeof(Dimension),
	offset(gridHeight), XtRImmediate, XtPointer(16) },
    { XtNshowGrid, XtCShowGrid, XtRBoolean, sizeof(Boolean),
	offset(showGrid), XtRImmediate, XtPointer(false) },
    { XtNsnapToGrid, XtCSnapToGrid, XtRBoolean, sizeof(Boolean),
	offset(snapToGrid), XtRImmediate, XtPointer(false) },

    { XtNautoLayout, XtCAutoLayout, XtRBoolean, sizeof(Boolean),
	offset(autoLayout), XtRImmediate, XtPointer(false) },

    { XtNrotation, XtCRotation, XtRCardinal, sizeof(Cardinal),
	offset(rotation), XtRImmediate, XtPointer(0)},

    { XtNedgeWidth, XtCEdgeWidth, XtRDimension, sizeof(Dimension),
	offset(edgeWidth), XtRImmediate, XtPointer(0) },
    { XtNarrowAngle, XtCArrowAngle, XtRDimension, sizeof(Dimension),
	offset(arrowAngle), XtRImmediate, XtPointer(30) },
    { XtNarrowLength, XtCArrowLength, XtRDimension, sizeof(Dimension),
	offset(arrowLength), XtRImmediate, XtPointer(10) },

    { XtNextraWidth, XtCExtraWidth, XtRDimension, sizeof(Dimension),
	offset(extraWidth), XtRImmediate, XtPointer(0) },
    { XtNextraHeight, XtCExtraHeight, XtRDimension, sizeof(Dimension),
	offset(extraHeight), XtRImmediate, XtPointer(0) },

    { XtNselectTile, XtCBitmap, XtRBitmap, sizeof(Pixmap),
	offset(selectTile), XtRImmediate, XtPointer(0)},

    { XtNedgeAttachMode, XtCEdgeAttachMode, XtREdgeAttachMode,
	sizeof(EdgeAttachMode), offset(edgeAttachMode), 
        XtRImmediate, XtPointer(Straight) },
    { XtNlayoutMode, XtCLayoutMode, XtRLayoutMode,
	sizeof(LayoutMode), offset(layoutMode), 
        XtRImmediate, XtPointer(RegularLayoutMode) },

    { XtNdefaultCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(defaultCursor), XtRImmediate, XtPointer(0)},
    { XtNmoveCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(moveCursor), XtRImmediate, XtPointer(0)},
    { XtNselectCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(selectCursor), XtRImmediate, XtPointer(0)},
    { XtNselectBottomLeftCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(selectBottomLeftCursor), XtRImmediate, XtPointer(0)},
    { XtNselectBottomRightCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(selectBottomRightCursor), XtRImmediate, XtPointer(0)},
    { XtNselectTopLeftCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(selectTopLeftCursor), XtRImmediate, XtPointer(0)},
    { XtNselectTopRightCursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(selectTopRightCursor), XtRImmediate, XtPointer(0)},

    { XtNnodeColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(nodeColor), XtRCallProc, XtPointer(defaultForeground) },
    { XtNedgeColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(edgeColor), XtRCallProc, XtPointer(defaultForeground) },
    { XtNframeColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(frameColor), XtRCallProc, XtPointer(defaultForeground) },
    { XtNoutlineColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(outlineColor), XtRCallProc, XtPointer(defaultForeground) },
    { XtNgridColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(gridColor), XtRCallProc, XtPointer(defaultForeground) },
    { XtNselectColor, XtCColor, XtRPixel, sizeof(Pixel),
	offset(selectColor), XtRCallProc, XtPointer(defaultForeground) },

    { XtNpositionChangedCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(positionChangedProc), XtRCallback, XtPointer(0) },
    { XtNselectionChangedCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(selectionChangedProc), XtRCallback, XtPointer(0) },
    { XtNsizeChangedCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(sizeChangedProc), XtRCallback, XtPointer(0) },
    { XtNcompareNodesCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(compareNodesProc), XtRCallback, XtPointer(0) },

#undef offset
};


// Action function declarations

static void Select      (Widget, XEvent *, String *, Cardinal *);
static void Extend      (Widget, XEvent *, String *, Cardinal *);
static void SelectOrMove(Widget, XEvent *, String *, Cardinal *);
static void ExtendOrMove(Widget, XEvent *, String *, Cardinal *);
static void MoveSelected(Widget, XEvent *, String *, Cardinal *);
static void Follow      (Widget, XEvent *, String *, Cardinal *);
static void End         (Widget, XEvent *, String *, Cardinal *);
static void ShowEdges   (Widget, XEvent *, String *, Cardinal *);
static void HideEdges   (Widget, XEvent *, String *, Cardinal *);
static void SelectAll   (Widget, XEvent *, String *, Cardinal *);
static void SelectFirst (Widget, XEvent *, String *, Cardinal *);
static void SelectNext  (Widget, XEvent *, String *, Cardinal *);
static void SelectPrev  (Widget, XEvent *, String *, Cardinal *);
static void UnselectAll (Widget, XEvent *, String *, Cardinal *);
static void SnapToGrid  (Widget, XEvent *, String *, Cardinal *);
static void _SnapToGrid (Widget, XEvent *, String *, Cardinal *);
static void Rotate      (Widget, XEvent *, String *, Cardinal *);
static void _Rotate     (Widget, XEvent *, String *, Cardinal *);
static void DoLayout    (Widget, XEvent *, String *, Cardinal *);
static void _Layout     (Widget, XEvent *, String *, Cardinal *);
static void Normalize   (Widget, XEvent *, String *, Cardinal *);
static void _Normalize  (Widget, XEvent *, String *, Cardinal *);


// Actions table

static XtActionsRec actions[] = {
    { "select",         Select },        // select()
    { "extend",         Extend },        // extend()
    { "select-or-move", SelectOrMove },	 // select-or-move()
    { "extend-or-move", ExtendOrMove },	 // extend-or-move()
    { "move-selected",  MoveSelected },	 // move-selected(X, Y)
    { "follow",		Follow },        // follow()
    { "end",		End },           // end()
    { "show-edges",	ShowEdges },     // show-edges([any|both|from|to])
    { "hide-edges", 	HideEdges },     // hide-edges([any|both|from|to])
    { "select-all", 	SelectAll },     // select-all()
    { "select-first", 	SelectFirst },   // select-first()
    { "select-next", 	SelectNext },    // select-next()
    { "select-prev", 	SelectPrev },    // select-prev()
    { "unselect-all", 	UnselectAll },   // unselect-all()
    { "snap-to-grid",	SnapToGrid },    // snap-to-grid()
    { "_snap-to-grid",	_SnapToGrid },
    { "rotate",		Rotate },        // rotate([[+|-]DEGREES])
    { "_rotate",	_Rotate },
    { "layout",		DoLayout },      // layout([regular|compact],
    { "_layout",	_Layout },       //         [[+|-]DEGREES]])
    { "normalize",	Normalize },     // normalize()
    { "_normalize",	_Normalize },
};


// Default translation table

static char defaultTranslations[] =
    "Shift<Btn1Down>:	extend-or-move()\n"
    "<Btn1Down>:	select-or-move()\n"
    "<Btn1Motion>:	follow()\n"
    "<Btn1Up>:		end()\n"
    "<Btn2Down>:	extend-or-move()\n"
    "<Btn2Motion>:	follow()\n"
    "<Btn2Up>:		end()\n"
    "Ctrl<Key>KP_1:     move-selected(-grid, +grid)\n"
    "Ctrl<Key>KP_2:     move-selected(    0, +grid)\n"
    "Ctrl<Key>KP_3:     move-selected(+grid, +grid)\n"
    "Ctrl<Key>KP_4:     move-selected(-grid,     0)\n"
    "Ctrl<Key>KP_6:     move-selected(+grid,     0)\n"
    "Ctrl<Key>KP_7:     move-selected(-grid, -grid)\n"
    "Ctrl<Key>KP_8:     move-selected(    0, -grid)\n"
    "Ctrl<Key>KP_9:     move-selected(+grid, -grid)\n"
    "Ctrl<Key>B:        move-selected(-grid,     0)\n"
    "Ctrl<Key>F:        move-selected(+grid,     0)\n"
    "Ctrl<Key>P:        move-selected(    0, -grid)\n"
    "Ctrl<Key>N:        move-selected(    0, +grid)\n"
    "Shift<Key>KP_1:    move-selected(-1, +1)\n"
    "Shift<Key>KP_2:    move-selected( 0, +1)\n"
    "Shift<Key>KP_3:    move-selected(+1, +1)\n"
    "Shift<Key>KP_4:    move-selected(-1,  0)\n"
    "Shift<Key>KP_6:    move-selected(+1,  0)\n"
    "Shift<Key>KP_7:    move-selected(-1, -1)\n"
    "Shift<Key>KP_8:    move-selected( 0, -1)\n"
    "Shift<Key>KP_9:    move-selected(+1, -1)\n"
    "Shift<Key>B:       move-selected(-1,  0)\n"
    "Shift<Key>F:       move-selected(+1,  0)\n"
    "Shift<Key>P:       move-selected( 0, -1)\n"
    "Shift<Key>N:       move-selected( 0, +1)\n"
    "~Meta ~Alt<Key>KP_4: select-prev()\n"
    "~Meta ~Alt<Key>KP_6: select-next()\n"
    "~Meta ~Alt<Key>KP_7: select-first()\n"
    "~Meta ~Alt<Key>KP_0: select-first()\n"
    "~Meta ~Alt<Key>B:  select-prev()\n"
    "~Meta ~Alt<Key>F:  select-next()\n"
    "~Meta ~Alt<Key>P:  select-prev()\n"
    "~Meta ~Alt<Key>N:  select-next()\n"
    "~Meta ~Alt<Key>A:  select-first()\n"
    ;

// These translations override the XmPrimitive base translations
static char extraTranslations[] =
    "Ctrl<Key>osfLeft:  move-selected(-grid,     0)\n"
    "Ctrl<Key>osfRight: move-selected(+grid,     0)\n"
    "Ctrl<Key>osfUp:    move-selected(    0, -grid)\n"
    "Ctrl<Key>osfDown:  move-selected(    0, +grid)\n"
    "Ctrl<Key>osfBeginLine: move-selected(-grid, -grid)\n"
    "Shift<Key>osfLeft: move-selected(-1,  0)\n"
    "Shift<Key>osfRight:move-selected(+1,  0)\n"
    "Shift<Key>osfUp:   move-selected( 0, -1)\n"
    "Shift<Key>osfDown: move-selected( 0, +1)\n"
    "Shift<Key>osfBeginLine: move-selected(-1, -1)\n"
    "~Meta ~Alt ~Shift ~Ctrl<Key>osfLeft:      select-prev()\n"
    "~Meta ~Alt ~Shift ~Ctrl<Key>osfRight:     select-next()\n"
    "~Meta ~Alt ~Shift ~Ctrl<Key>osfUp:        select-prev()\n"
    "~Meta ~Alt ~Shift ~Ctrl<Key>osfDown:      select-next()\n"
    "~Meta ~Alt ~Shift ~Ctrl<Key>osfBeginLine: select-first()\n"
;

// Method function declarations

static void ClassInitialize();

static void Initialize(Widget request, 
		       Widget w, 
		       ArgList args,
		       Cardinal *num_args);

static void Redisplay(Widget w, XEvent *event, Region region);

static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes);

static Boolean SetValues(Widget old, 
			 Widget request, 
			 Widget new_w,
			 ArgList args, 
			 Cardinal *num_args);

static void Destroy(Widget w);


// Class record initialization

GraphEditClassRec graphEditClassRec = {
  {	 /* core fields */
    /* superclass               */  (WidgetClass) &xmPrimitiveClassRec,
    /* class_name               */  "GraphEdit",
    /* widget_size              */  sizeof(GraphEditRec),
    /* class_initialize         */  ClassInitialize,
    /* class_part_initialize    */  NULL,
    /* class_inited             */  false,
    /* initialize               */  Initialize,
    /* initialize_hook          */  NULL,
    /* realize                  */  Realize,
    /* actions                  */  actions,
    /* num_actions              */  XtNumber(actions),
    /* resources                */  resources,
    /* num_resources            */  XtNumber(resources),
    /* xrm_class                */  NULLQUARK,
    /* compress_motion          */  true,
    /* compress_exposure        */  true,
    /* compress_enterleave      */  true,
    /* visible_interest         */  false,
    /* destroy                  */  Destroy,
    /* resize                   */  XtInheritResize,
    /* expose                   */  Redisplay,
    /* set_values               */  SetValues,
    /* set_values_hook          */  NULL,
    /* set_values_almost        */  XtInheritSetValuesAlmost,
    /* get_values_hook          */  NULL,
    /* accept_focus             */  NULL,
    /* version                  */  XtVersion,
    /* callback_private         */  NULL,
    /* tm_table                 */  defaultTranslations,
    /* query_geometry           */  XtInheritQueryGeometry,
    /* display_accelerator      */  XtInheritDisplayAccelerator,
    /* extension                */  NULL,
  },
  {	 /* Primitive fields */
    /* border_highlight         */ (XtWidgetProc) _XtInherit,
    /* border_unhighlight       */ (XtWidgetProc) _XtInherit,
    /* translations             */ XtInheritTranslations,
    /* arm_and_activate         */ NULL,
    /* syn_resources            */ NULL,
    /* num_syn_resources        */ 0,
    /* extension                */ NULL,
  },
  {	/* GraphEdit fields */
    /* extension                */ NULL,
  },
};

WidgetClass graphEditWidgetClass = (WidgetClass)&graphEditClassRec;


// Method function definitions

// Set widget to minimal size
void graphEditSizeChanged(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    Arg args[10];
    int arg;

    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->graphEdit.graph;
    const Dimension highlight_thickness = _w->primitive.highlight_thickness;
    Boolean& sizeChanged                = _w->graphEdit.sizeChanged;
    const GraphGC& graphGC              = _w->graphEdit.graphGC;
    const Dimension extraWidth          = _w->graphEdit.extraWidth;
    const Dimension extraHeight         = _w->graphEdit.extraHeight;

    // Could it be this is invoked without any graph yet?
    if (graph == 0)
	return;

    sizeChanged = false;

    BoxRegion r = graph->region(graphGC);

    Dimension myWidth  = r.origin(X) + r.space(X) + highlight_thickness * 2;
    Dimension myHeight = r.origin(Y) + r.space(Y) + highlight_thickness * 2;

    Dimension parentWidth  = 0;
    Dimension parentHeight = 0;

    Widget parent = XtParent(w);
    if (!XmIsScrolledWindow(parent))
	parent = XtParent(parent);	// skip clipping window
    if (XmIsScrolledWindow(parent))
    {
	arg = 0;
	XtSetArg(args[arg], XtNwidth, &parentWidth); arg++;
	XtSetArg(args[arg], XtNheight, &parentHeight); arg++;
	XtGetValues(parent, args, arg);

	parentWidth  -= 4;	// Don't ask. I really can't imagine 
	parentHeight -= 4;	// why this has to be just 4.
    }

    Dimension width  = max(parentWidth, myWidth);
    Dimension height = max(parentHeight, myHeight);

    width  += extraWidth;
    height += extraHeight;

    arg = 0;
    XtSetArg(args[arg], XtNwidth, width); arg++;
    XtSetArg(args[arg], XtNheight, height); arg++;
    XtSetValues(w, args, arg);
}

// Return the graph's Graphic Context
const GraphGC& graphEditGetGraphGC(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w            = GraphEditWidget(w);
    const GraphGC& graphGC              = _w->graphEdit.graphGC;

    return graphGC;
}

// Return the graph
Graph *graphEditGetGraph(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    GraphEditWidget _w = GraphEditWidget(w);
    return _w->graphEdit.graph;
}


// Set grid pixmap
static void setGrid(Widget w, Boolean reset = false)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Pixel     gridColor  = _w->graphEdit.gridColor;
    const Pixel     background = _w->core.background_pixel;
    const Boolean   showGrid   = _w->graphEdit.showGrid;
    const Dimension gridHeight = _w->graphEdit.gridHeight;
    const Dimension gridWidth  = _w->graphEdit.gridWidth;
    Pixmap& gridPixmap         = _w->graphEdit.gridPixmap;
    
    if (reset && gridPixmap != None)
    {
	// delete old pixmap
	XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), ParentRelative);
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, true);

	XFreePixmap(XtDisplay(w), gridPixmap);
	gridPixmap = None;
    }

    if (gridPixmap == None)
    {
	// Create grid pixmap
	int gridDataSize = ((gridWidth + 7) / 8) * gridHeight + 1;
	char *gridData = new char [gridDataSize];
	for (int i = 0; i < gridDataSize; i++)
	    gridData[i] = 0x00;
	if (showGrid)
	    gridData[0] = 0x01;

	int depth = PlanesOfScreen(XtScreen(w));
	gridPixmap = 
	    XCreatePixmapFromBitmapData(XtDisplay(w), 
					XtWindow(w),
					gridData, 
					gridWidth, gridHeight,
					gridColor, background,
					depth);

 	XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), gridPixmap);
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, true);

	delete[] gridData;
    }
}



// Redraw all
void graphEditRedraw(Widget w)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;
    const Dimension highlight_thickness = _w->primitive.highlight_thickness;

    setGrid(w);

    XClearArea(XtDisplay(w), XtWindow(w),
	       highlight_thickness, highlight_thickness, 
	       _w->core.width  - highlight_thickness * 2, 
	       _w->core.height - highlight_thickness * 2,
	       false);

    if (graph)
	graph->draw(w, EVERYWHERE, graphGC);
}


// Redraw a specific region
void graphEditRedrawNode(Widget w, GraphNode *node)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;

    setGrid(w);

    if (node == 0)
	return;

    BoxRegion r = node->region(graphGC);
    XClearArea(XtDisplay(w), XtWindow(w), r.origin(X), r.origin(Y),
	       r.space(X), r.space(Y), false);

    if (graph)
	graph->draw(w, r, graphGC);
}



// Converters

#define done(type, value) \
    {							\
	if (toVal->addr != NULL) {			\
	    if (toVal->size < sizeof(type)) {		\
		toVal->size = sizeof(type);		\
		return false;				\
	    }						\
	    *(type *)(toVal->addr) = (value);		\
	}						\
	else {						\
	    static type static_val;			\
	    static_val = (value);			\
	    toVal->addr = (caddr_t)&static_val;	        \
	}						\
							\
	toVal->size = sizeof(type);			\
	return true;					\
    }



// Convert String to EdgeAttachMode and vice-versa

static Boolean CvtStringToEdgeAttachMode (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    EdgeAttachMode mode = Straight;

    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtStringToEdgeAttachMode", "wrongParameters",
	    "XtToolkitError",
	    "String to EdgeAttachMode conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);
    
    string s = downcase((char *)fromVal->addr);

    if (s == "straight")
	mode = Straight;
    else if (s == "circle")
	mode = Circle;
    else if (s == "centered")
	mode = Centered;
    else
	XtDisplayStringConversionWarning(display, (String)fromVal->addr,
	    XtREdgeAttachMode);

    done(EdgeAttachMode, mode);
}

static Boolean CvtEdgeAttachModeToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtEdgeAttachModeToString", "wrongParameters",
	    "XtToolkitError",
	    "EdgeAttachMode to String conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);

    EdgeAttachMode mode = *((EdgeAttachMode *)fromVal->addr);

    String s = "unknown";
    switch (mode)
    {
    case Straight:
	s = "straight";
	break;
    case Circle:
	s = "circle";
	break;
    case Centered:
	s = "centered";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(String, s);
}


// Convert String to LayoutMode

static Boolean CvtStringToLayoutMode (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    LayoutMode mode = RegularLayoutMode;

    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtStringToLayoutMode", "wrongParameters",
	    "XtToolkitError",
	    "String to LayoutMode conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);
    
    string s = downcase((char *)fromVal->addr);

    if (s == "regular")
	mode = RegularLayoutMode;
    else if (s == "compact")
	mode = CompactLayoutMode;
    else
	XtDisplayStringConversionWarning(display, (String)fromVal->addr,
	    XtRLayoutMode);

    done(LayoutMode, mode);
}


static Boolean CvtLayoutModeToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtLayoutModeToString", "wrongParameters",
	    "XtToolkitError",
	    "LayoutMode to String conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);

    LayoutMode mode = *((LayoutMode *)fromVal->addr);

    String s = "unknown";
    switch (mode)
    {
    case RegularLayoutMode:
	s = "regular";
	break;
    case CompactLayoutMode:
	s = "compact";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(String, s);
}


// Standard Converters

static Boolean CvtBooleanToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtBooleanToString", "wrongParameters",
	    "XtToolkitError",
	    "Boolean to String conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);

    Boolean mode = *((Boolean *)fromVal->addr);

    String s = "unknown";
    switch (mode)
    {
    case True:
	s = "true";
	break;
    case False:
	s = "false";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(String, s);
}

static Boolean CvtDimensionToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtDimensionToString", "wrongParameters",
	    "XtToolkitError",
	    "Dimension to String conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);

    Dimension d = *((Dimension *)fromVal->addr);

    ostrstream os;
    os << d;
    string os_s(os);
    String s = (String)XtNewString((String)os_s);

    done(String, s);
}

static Boolean CvtCardinalToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtCardinalToString", "wrongParameters",
	    "XtToolkitError",
	    "Cardinal to String conversion needs no extra arguments",
	    (String *)NULL, (Cardinal *)NULL);

    Cardinal d = *((Cardinal *)fromVal->addr);

    ostrstream os;
    os << d;
    string os_s(os);
    String s = (String)XtNewString((String)os_s);

    done(String, s);
}


// Initialize class
static void ClassInitialize()
{
    // Register own converters
    XtSetTypeConverter(XtRString, XtREdgeAttachMode, 
		       CvtStringToEdgeAttachMode,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRString, XtRLayoutMode, 
		       CvtStringToLayoutMode,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtREdgeAttachMode, XtRString, 
		       CvtEdgeAttachModeToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRLayoutMode, XtRString, 
		       CvtLayoutModeToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));

    // Register standard converters
    XtSetTypeConverter(XtRBoolean, XtRString, 
		       CvtBooleanToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRDimension, XtRString, 
		       CvtDimensionToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRCardinal, XtRString, 
		       CvtCardinalToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
}

// Initialize widget

inline void createCursor(Widget w, Cursor& cursor, unsigned int shape)
{
    if (cursor == 0)
	cursor = XCreateFontCursor(XtDisplay(w), shape);
}

static void setGCs(Widget w)
{
    const GraphEditWidget _w        = GraphEditWidget(w);

    // read-only
    const Dimension edgeWidth       = _w->graphEdit.edgeWidth;
    const Pixmap selectTile         = _w->graphEdit.selectTile;
    const Pixel background          = _w->core.background_pixel;
    const Pixel nodeColor           = _w->graphEdit.nodeColor;
    const Pixel edgeColor           = _w->graphEdit.edgeColor;
    const Pixel frameColor          = _w->graphEdit.frameColor;
    const Pixel outlineColor        = _w->graphEdit.outlineColor;
    const Pixel selectColor         = _w->graphEdit.selectColor;


    // write-only
    GC& nodeGC                      = _w->graphEdit.nodeGC;
    GC& edgeGC                      = _w->graphEdit.edgeGC;
    GC& invertGC                    = _w->graphEdit.invertGC;
    GC& clearGC                     = _w->graphEdit.clearGC;
    GC& frameGC                     = _w->graphEdit.frameGC;
    GC& outlineGC                   = _w->graphEdit.outlineGC;

    // set nodeGC
    XGCValues gcv;
    gcv.foreground = nodeColor;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    nodeGC = XtGetGC(w, GCForeground | GCLineWidth | GCLineStyle, &gcv);

    // set edgeGC
    gcv.foreground = edgeColor;
    gcv.line_width = edgeWidth;
    edgeGC = XtGetGC(w, GCForeground | GCLineWidth, &gcv);

    // set invertGC
    if (selectTile)
    {
	gcv.foreground = selectColor;
	gcv.function   = GXcopy;
	gcv.fill_style = FillStippled;
	gcv.stipple    = selectTile;
	invertGC = XtGetGC(w, GCForeground | GCFunction | 
			   GCFillStyle | GCStipple, &gcv);
    }
    else
    {
	gcv.foreground = selectColor;
	gcv.function   = GXinvert;
	gcv.plane_mask = selectColor ^ background;
	invertGC = XtGetGC(w, GCForeground | GCFunction | GCPlaneMask, &gcv);
    }

    // set clearGC
    gcv.foreground = background;
    gcv.function   = GXcopy;
    clearGC = XtGetGC(w, GCForeground | GCFunction, &gcv);

    // set frameGC
    gcv.foreground = frameColor;
    gcv.function   = GXinvert;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.plane_mask = frameColor ^ background;
    frameGC = XtGetGC(w, GCForeground | GCFunction | 
		      GCLineWidth | GCLineStyle | GCPlaneMask, &gcv);

    // set outlineGC
    gcv.foreground = outlineColor;
    gcv.function   = GXinvert;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.plane_mask = outlineColor ^ background;
    outlineGC = XtGetGC(w, GCForeground | GCFunction | 
			GCLineWidth | GCLineStyle | GCPlaneMask, &gcv);
}


static void setGraphGC(Widget w)
{
    const GraphEditWidget _w        = GraphEditWidget(w);

    // read-only
    const Dimension arrowAngle      = _w->graphEdit.arrowAngle;
    const Dimension arrowLength     = _w->graphEdit.arrowLength;
    const Dimension hintSize        = _w->graphEdit.hintSize;
    const Boolean showHints         = _w->graphEdit.showHints;
    const EdgeAttachMode edgeAttachMode = 
	EdgeAttachMode(_w->graphEdit.edgeAttachMode);
    const GC nodeGC                 = _w->graphEdit.nodeGC;
    const GC edgeGC                 = _w->graphEdit.edgeGC;
    const GC invertGC               = _w->graphEdit.invertGC;
    const GC clearGC                = _w->graphEdit.clearGC;

    // write-only
    GraphGC& graphGC                = _w->graphEdit.graphGC;

    // set graphGC
    graphGC = GraphGC(nodeGC, edgeGC, invertGC, clearGC);
    graphGC.arrowAngle     = arrowAngle;
    graphGC.arrowLength    = arrowLength;
    graphGC.edgeAttachMode = EdgeAttachMode(edgeAttachMode);
    graphGC.drawHints      = showHints;
    graphGC.hintSize       = hintSize;
}


static void Initialize(Widget, Widget w, ArgList, Cardinal *)
{
    // read-only
    const GraphEditWidget _w        = GraphEditWidget(w);

    // write-only
    GraphEditState& state           = _w->graphEdit.state;
    Cursor& moveCursor              = _w->graphEdit.moveCursor;
    Cursor& selectCursor            = _w->graphEdit.selectCursor;
    Cursor& selectBottomLeftCursor  = _w->graphEdit.selectBottomLeftCursor;
    Cursor& selectBottomRightCursor = _w->graphEdit.selectBottomRightCursor;
    Cursor& selectTopLeftCursor     = _w->graphEdit.selectTopLeftCursor;
    Cursor& selectTopRightCursor    = _w->graphEdit.selectTopRightCursor;
    Pixmap& gridPixmap              = _w->graphEdit.gridPixmap;
    Boolean& sizeChanged            = _w->graphEdit.sizeChanged;
    Time& lastSelectTime            = _w->graphEdit.lastSelectTime;

    // init state
    state = NopState;

    // init sizeChanged
    sizeChanged = false;

    // init lastSelectTime
    lastSelectTime = 0;

    // set GCs
    setGCs(w);

    // set Graph GC
    setGraphGC(w);

    // set grid pixmap
    gridPixmap = None;

    // create cursors if not already set
    createCursor(w, moveCursor,              XC_fleur);
    createCursor(w, selectCursor,            XC_plus);
    createCursor(w, selectBottomLeftCursor,  XC_ll_angle);
    createCursor(w, selectBottomRightCursor, XC_lr_angle);
    createCursor(w, selectTopLeftCursor,     XC_ul_angle);
    createCursor(w, selectTopRightCursor,    XC_ur_angle);

    // set size
    graphEditSizeChanged(w);

    // Override XmPrimitive translations
    XtOverrideTranslations(w, XtParseTranslationTable(extraTranslations));
}

inline void defineCursor(Widget w, Cursor cursor)
{
    if (cursor != 0)
	XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
    else
	XUndefineCursor(XtDisplay(w), XtWindow(w));
}

// Realize widget
static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    Cursor defaultCursor     = _w->graphEdit.defaultCursor;

    // Call superclass realize method
    graphEditClassRec.core_class.superclass->
	core_class.realize(w, value_mask, attributes);
    
    // Setup default cursor
    defineCursor(w, defaultCursor);
}


// Redisplay widget
static void Redisplay(Widget w, XEvent *event, Region)
{
    const GraphEditWidget _w      = GraphEditWidget(w);
    const Graph* graph            = _w->graphEdit.graph;
    const GraphGC& graphGC        = _w->graphEdit.graphGC;
    const Boolean sizeChanged     = _w->graphEdit.sizeChanged;
    const Boolean highlight_drawn = _w->primitive.highlight_drawn;

    if (sizeChanged)
	graphEditSizeChanged(w);

    setGrid(w);

    // Redraw XmPrimitive border
    if (highlight_drawn)
	graphEditClassRec.primitive_class.border_highlight(w);

    graph->draw(w, BoxRegion(point(event), size(event)), graphGC);
}


// Set widget values
static Boolean SetValues(Widget old, Widget, Widget new_w, 
			 ArgList, Cardinal *)
{
    GraphEditWidget before = GraphEditWidget(old);
    GraphEditWidget after  = GraphEditWidget(new_w);

    bool redisplay = false;

    // redisplay graph if changed
    if (before->graphEdit.graph != after->graphEdit.graph)
    {
	redisplay = true;

	// Re-layout if auto-layout is enabled
	if (after->graphEdit.autoLayout)
	{
	    Cardinal zero = 0;
	    _Layout(new_w, 0, 0, &zero);
	}

	// Snap to grid if enabled
	if (after->graphEdit.snapToGrid)
	{
	    Cardinal zero = 0;
	    _SnapToGrid(new_w, 0, 0, &zero);
	}
    }

    // reset GCs if changed
    if (before->graphEdit.edgeWidth   != after->graphEdit.edgeWidth ||
	before->graphEdit.selectTile  != after->graphEdit.selectTile)
    {    
	setGCs(new_w);
	redisplay = true;
    }

    // reset GraphGC if changed
    if (before->graphEdit.arrowAngle  != after->graphEdit.arrowAngle  ||
	before->graphEdit.arrowLength != after->graphEdit.arrowLength ||
	before->graphEdit.showHints   != after->graphEdit.showHints   ||
	before->graphEdit.hintSize    != after->graphEdit.hintSize    ||
	before->graphEdit.edgeAttachMode != after->graphEdit.edgeAttachMode)
    {
	setGraphGC(new_w);
	redisplay = true;
    }

    // reset grid pixmap if changed
    if (before->graphEdit.gridWidth  != after->graphEdit.gridWidth  ||
	before->graphEdit.gridHeight != after->graphEdit.gridHeight ||
	before->graphEdit.showGrid   != after->graphEdit.showGrid)
    {
	setGrid(new_w, true);
	redisplay = true;
    }

    // Always recompute size
    after->graphEdit.sizeChanged = true;

    return redisplay;
}

// Destroy widget
static void Destroy(Widget)
{
    // delete graph?
}




// Action function definitions

// Helping stuff

// Find node at point
GraphNode *graphEditGetNodeAtPoint(Widget w, BoxPoint p)
{
    const GraphEditWidget _w  = GraphEditWidget(w);
    const Graph* graph        = _w->graphEdit.graph;
    GraphGC& graphGC          = _w->graphEdit.graphGC;
    GraphNode *found = 0;

    // Could it be this is invoked without any graph yet?
    if (graph == 0)
	return 0;

    // note that we return the last matching node in the list;
    // thus on overlapping nodes we select the top one

    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
	if (p <= (BoxRegion&)node->sensitiveRegion(graphGC))
	    found = node;

    return found;
}
    
GraphNode *graphEditGetNodeAtEvent(Widget w, XEvent *event)
{
    return graphEditGetNodeAtPoint(w, point(event));
}


// Get frame region
static BoxRegion frameRegion(Widget w)
{
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEdit.startAction;
    const BoxPoint& endAction   = _w->graphEdit.endAction;

    BoxPoint origin(min(startAction[X], endAction[X]),
	min(startAction[Y], endAction[Y]));

    BoxSize space(abs(startAction[X] - endAction[X]),
	abs(startAction[Y] - endAction[Y]));

    return BoxRegion(origin, space);
}

// Get frame cursor
static void setRegionCursor(Widget w)
{
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEdit.startAction;
    const BoxPoint& endAction   = _w->graphEdit.endAction;

    Cursor selectCursor            = _w->graphEdit.selectCursor;
    Cursor selectBottomLeftCursor  = _w->graphEdit.selectBottomLeftCursor;
    Cursor selectBottomRightCursor = _w->graphEdit.selectBottomRightCursor;
    Cursor selectTopLeftCursor     = _w->graphEdit.selectTopLeftCursor;
    Cursor selectTopRightCursor    = _w->graphEdit.selectTopRightCursor;

    Cursor cursor = selectCursor;

    if (endAction[X] < startAction[X])
    {
	if (endAction[Y] < startAction[Y])
	    cursor = selectTopLeftCursor;
	else if (endAction[Y] > startAction[Y])
	    cursor = selectBottomLeftCursor;
    }
    else if (endAction[X] > startAction[X])
    {
	if (endAction[Y] < startAction[Y])
	    cursor = selectTopRightCursor;
	else if (endAction[Y] > startAction[Y])
	    cursor = selectBottomRightCursor;
    }

    defineCursor(w, cursor);
}


inline void myXDrawLine(Display *display, 
			Drawable d, 
			GC gc,
			const BoxPoint& f, const BoxPoint& t)
{
    if (f != t)
	XDrawLine(display, d, gc, f[X], f[Y], t[X], t[Y]);
}
    

// Redraw line (f0/t1) at (f1/t1)
inline void redrawLine(Display *display,
		       Drawable d,
		       GC gc,
		       const BoxPoint& f0, 
		       const BoxPoint& t0, 
		       const BoxPoint& f1, 
		       const BoxPoint& t1)
{
    // Clear old line (by redrawing it)
    myXDrawLine(display, d, gc, f0, t0);

    // Draw new line
    myXDrawLine(display, d, gc, f1, t1);
}

static void drawSelectFrames(Widget w, 
			     const BoxRegion& r0, 
			     const BoxRegion& r1)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const GC frameGC         = _w->graphEdit.frameGC;

    Display *display = XtDisplay(w);
    Window window    = XtWindow(w);


    // Redraw all four sides, one after the other

    // North
    redrawLine(display, window, frameGC,
	r0.origin(),
	r0.origin() + BoxPoint(r0.space(X), 0),
	r1.origin(),
	r1.origin() + BoxPoint(r1.space(X), 0));

    // South
    redrawLine(display, window, frameGC,
	r0.origin() + BoxPoint(0, r0.space(Y)),
	r0.origin() + r0.space(),
	r1.origin() + BoxPoint(0, r1.space(Y)),
	r1.origin() + r1.space());

    // East
    redrawLine(display, window, frameGC,
	r0.origin(),
	r0.origin() + BoxPoint(0, r0.space(Y)),
	r1.origin(),
	r1.origin() + BoxPoint(0, r1.space(Y)));

    // West
    redrawLine(display, window, frameGC,
	r0.origin() + BoxPoint(r0.space(X), 0),
	r0.origin() + r0.space(),
	r1.origin() + BoxPoint(r1.space(X), 0),
	r1.origin() + r1.space());

    // Set appropriate cursor
    setRegionCursor(w);
}


// Draw the selection frame
inline void drawSelectFrame(Widget w)
{
    drawSelectFrames(w, frameRegion(w),
	BoxRegion(BoxPoint(0, 0), BoxSize(0, 0)));
}


// Redraw selection frame
static void redrawSelectFrame(Widget w, BoxPoint& p)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    BoxPoint& endAction      = _w->graphEdit.endAction;

    BoxRegion r0 = frameRegion(w);
    endAction = p;
    BoxRegion r1 = frameRegion(w);

    drawSelectFrames(w, r0, r1);
}


// Find min possible offset
static void getMinimalOffset(Widget w)
{
    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->graphEdit.graph;
    const Dimension highlight_thickness = _w->primitive.highlight_thickness;
    const GraphGC& graphGC              = _w->graphEdit.graphGC;
    BoxPoint& minimalOffset             = _w->graphEdit.minimalOffset;

    const Dimension min_origin = highlight_thickness + 2;

    bool found[NDimensions];
    found[X] = false;
    found[Y] = false;

    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    BoxRegion r = node->region(graphGC);

	    for (BoxDimension d = X; d <= Y; d++)
	    {
		if (!found[d] || minimalOffset[d] < min_origin - r.origin(d))
		{
		    minimalOffset[d] = min_origin - r.origin(d);
		    found[d] = true;
		}
	    }
	}
    }
}

// Return current offset
static BoxPoint actionOffset(Widget w)
{
    const GraphEditWidget _w      = GraphEditWidget(w);
    const Dimension gridWidth     = _w->graphEdit.gridWidth;
    const Dimension gridHeight    = _w->graphEdit.gridHeight;
    const Boolean snapToGrid      = _w->graphEdit.snapToGrid;
    const BoxPoint& startAction   = _w->graphEdit.startAction;
    const BoxPoint& endAction     = _w->graphEdit.endAction;
    const BoxPoint& minimalOffset = _w->graphEdit.minimalOffset;

    BoxPoint offset = endAction - startAction;
    BoxPoint grid(gridWidth, gridHeight);

    for (BoxDimension d = X; d <= Y; d++)
    {
	// Offset must not move nodes out of area
	if (offset[d] < minimalOffset[d])
	    offset[d] = minimalOffset[d];

	// Offset must be a grid multiple
	if (snapToGrid && grid[d] > 0)
	{
	    offset[d] = ((offset[d] + grid[d] / 2) / grid[d]) * grid[d];
	    if (offset[d] < minimalOffset[d])
		offset[d] += grid[d];
	}
    }

    return offset;
}

// Draw moving frames and edges for nodes at (endAction - startAction)
static void drawOutlines(Widget w, const BoxPoint& offset)
{
    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->graphEdit.graph;
    const Boolean rubberArrows          = _w->graphEdit.rubberArrows;
    const Boolean rubberEdges           = _w->graphEdit.rubberEdges;
    const GraphGC& graphGC              = _w->graphEdit.graphGC;
    const GC& outlineGC                 = _w->graphEdit.outlineGC;

    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    // this should also handle opaqueMove (FIXME)...
	    BoxRegion r = node->region(graphGC);
	    XDrawRectangle(XtDisplay(w), XtWindow(w), outlineGC,
		r.origin(X) + offset[X], r.origin(Y) + offset[Y],
		r.space(X), r.space(Y));
	}
    }

    if (rubberEdges)
    {
	GraphGC gc(graphGC);
	gc.edgeGC           = outlineGC;
	gc.offsetIfSelected = offset;
	gc.drawArrowHeads   = rubberArrows;

	for (GraphEdge *edge = graph->firstEdge(); edge != 0;
	    edge = graph->nextEdge(edge))
	{
	    if (edge->from()->selected() || edge->to()->selected())
		edge->draw(w, EVERYWHERE, gc);
	}
    }
}

// Move Node to specified position and call callbacks
static void moveTo(Widget w, 
		   GraphNode *node, 
		   const BoxPoint& newPos,
		   Boolean isLast)
{
    const GraphEditWidget _w  = GraphEditWidget(w);
    Graph* graph              = _w->graphEdit.graph;

    if (node->pos() != newPos)
    {
	GraphEditPositionChangedInfo info;

	info.graph        = graph;
	info.node         = node;
	info.old_position = node->pos();
	info.new_position = newPos;
	info.is_last      = isLast;

	XtCallCallbacks(w, XtNpositionChangedCallback, caddr_t(&info));

	node->moveTo(newPos);
    }
}

// Call ``selection changed'' callbacks
static void selectionChanged(Widget w)
{
    const GraphEditWidget _w  = GraphEditWidget(w);
    Graph* graph              = _w->graphEdit.graph;

    GraphEditSelectionChangedInfo info;
    info.graph = graph;

    XtCallCallbacks(w, XtNselectionChangedCallback, caddr_t(&info));
}


// Action functions

// Select all nodes
static bool _SelectAll(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->graphEdit.graph;
    const GraphGC& graphGC   = _w->graphEdit.graphGC;

    bool changed = false;
    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	if (!node->selected())
	{
	    changed = true;
	    node->selected() = true;
	    node->draw(w, EVERYWHERE, graphGC);
	}
    }

    return changed;
}

static void SelectAll(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    if (_SelectAll(w, event, params, num_params))
	selectionChanged(w);
}


// Unselect all nodes
static bool _UnselectAll(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->graphEdit.graph;
    const GraphGC& graphGC   = _w->graphEdit.graphGC;

    bool changed = false;
    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    changed = true;
	    node->selected() = false;
	    node->draw(w, EVERYWHERE, graphGC);
	}
    }

    return changed;
}

static void UnselectAll(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    if (_UnselectAll(w, event, params, num_params))
	selectionChanged(w);
}

// Select an entire subgraph
bool select_graph(Widget w, GraphNode *node)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const GraphGC& graphGC   = _w->graphEdit.graphGC;

    bool changed = false;

    if (!node->selected())
    {
	node->selected() = true;
	node->draw(w, EVERYWHERE, graphGC);
	changed = true;
    }

    GraphEdge *edge;
    for (edge = node->firstFrom(); edge != 0; edge = node->nextFrom(edge))
	if (!edge->to()->selected())
	    changed = select_graph(w, edge->to()) || changed;

    for (edge = node->firstTo(); edge != 0; edge = node->nextTo(edge))
	if (!edge->from()->selected())
	    changed = select_graph(w, edge->from()) || changed;

    return changed;
}

// Begin selecting or moving
static void _SelectOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params, bool select, bool follow)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const GraphGC& graphGC   = _w->graphEdit.graphGC;
    Cursor moveCursor        = _w->graphEdit.moveCursor;

    GraphEditState& state    = _w->graphEdit.state;
    BoxPoint& startAction    = _w->graphEdit.startAction;
    BoxPoint& endAction      = _w->graphEdit.endAction;
    Time& lastSelectTime     = _w->graphEdit.lastSelectTime;

    // Get the input focus
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

    BoxPoint p = point(event);
    startAction = p;
    endAction   = p;

    Time t = time(event);
    bool double_click = 
	(Time(t - lastSelectTime) <= Time(XtGetMultiClickTime(XtDisplay(w))));
    lastSelectTime = t;

    GraphNode *node = graphEditGetNodeAtPoint(w, p);

    if (node == 0)
    {
	// On the background

	if (select)
	{
	    if (double_click)
		SelectAll(w, event, params, num_params);
	    else
		UnselectAll(w, event, params, num_params);
	}

	if (follow)
	{
	    // Not on a node: draw a frame
	    state = SelectState;

	    // start drawing a frame
	    drawSelectFrame(w);
	}
    }
    else
    {
	// On a node
	bool changed = false;

	// Create new selection
	if (!node->selected() && select)
	    changed = _UnselectAll(w, event, params, num_params);

	if (double_click)
	{
	    // Select all connected nodes
	    changed = select_graph(w, node);
	}
	else
	{
	    // Select single node
	    if (!node->selected())
	    {		
		node->selected() = true;
		node->draw(w, EVERYWHERE, graphGC);
		changed = true;
	    }
	}

	if (changed)
	    selectionChanged(w);

	if (follow)
	{
	    // Wait for movement
	    state = DeltaState;

	    // Set moving cursor
	    defineCursor(w, moveCursor);
	}
    }
}

static void SelectOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SelectOrMove(w, event, params, num_params, true, true);
}

static void ExtendOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SelectOrMove(w, event, params, num_params, false, true);
}

static void Select(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SelectOrMove(w, event, params, num_params, true, false);
}

static void Extend(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SelectOrMove(w, event, params, num_params, false, false);
}

// Keep on acting...
static void Follow(Widget w, XEvent *event, String *, Cardinal *)
{
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEdit.startAction;
    BoxPoint& endAction         = _w->graphEdit.endAction;
    GraphEditState& state       = _w->graphEdit.state;
    BoxPoint& lastOffset        = _w->graphEdit.lastOffset;
    const Dimension moveDelta   = _w->graphEdit.moveDelta;

    BoxPoint p = point(event);

    switch(state)
    {
	case SelectState:
	    // Draw new select frame
	    redrawSelectFrame(w, p);
	    break;

	case MoveState:
	{
	    // Draw new move frames
	    endAction = p;
	    BoxPoint newOffset = actionOffset(w);
	    if (newOffset != lastOffset)
	    {
		drawOutlines(w, lastOffset);
		drawOutlines(w, lastOffset = newOffset);
	    }
	    break;
	}

	case DeltaState:
	    // Wait for movement
	    if (abs(p[X] - startAction[X]) > moveDelta ||
		abs(p[Y] - startAction[Y]) > moveDelta)
	    {
		// start moving
		endAction = p;
		getMinimalOffset(w);
		graphEditSizeChanged(w);
		drawOutlines(w, lastOffset = actionOffset(w));
		state = MoveState;
	    }
	    break;

	default:
	    // Do nothing
	    break;
    }
}

// Now, all is done.

static void move_selected_nodes(Widget w, const BoxPoint& offset)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;

    if (offset == BoxPoint(0, 0))
	return;

    // Clear graph area
    BoxRegion r = graph->region(graphGC);
    XClearArea(XtDisplay(w), XtWindow(w), r.origin(X), r.origin(Y),
	       r.space(X), r.space(Y), false);

    // Move selected nodes
    GraphNode *lastNode = 0;
    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    if (lastNode)
		moveTo(w, lastNode, lastNode->pos() + offset, false);
	    lastNode = node;
	}
    }
    if (lastNode)
	moveTo(w, lastNode, lastNode->pos() + offset, true);

    // resize widget to graph size and redraw graph
    graphEditSizeChanged(w);
    graphEditRedraw(w);
}

static void End(Widget w, XEvent *event, String *, Cardinal *)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;
    const BoxPoint& lastOffset = _w->graphEdit.lastOffset;
    Cursor defaultCursor       = _w->graphEdit.defaultCursor;

    BoxPoint& endAction        = _w->graphEdit.endAction;
    GraphEditState& state      = _w->graphEdit.state;

    switch(state)
    {
	case SelectState:
	{
	    drawSelectFrame(w);
	    endAction = point(event);

	    BoxRegion selected = frameRegion(w);

	    // Find all nodes in frame and select them
	    for (GraphNode *node = graph->firstNode(); node != 0;
		node = graph->nextNode(node))
	    {
		if (!node->selected())
		{
		    // both corners must be inside frame
		    BoxPoint nw = node->region(graphGC).origin();
		    BoxPoint se = nw + node->region(graphGC).space() - 1;

		    if (nw <= selected && se <= selected)
		    {
			node->selected()  = true;
			node->draw(w, EVERYWHERE, graphGC);
			selectionChanged(w);
		    }
		}
	    }

	    state = NopState;
	    break;
	}

        case MoveState:
	{
	    // Move all selected nodes to new positions
		   
	    // clear graph area
	    drawOutlines(w, lastOffset);

            // move nodes
	    endAction = point(event);
	    BoxPoint offset = actionOffset(w);
	    move_selected_nodes(w, offset);

	    state = NopState;
	    break;
	}

	default:
	    // Do nothing
	    break;
    }

    defineCursor(w, defaultCursor);
}

// Key movement action
static void MoveSelected(Widget w, XEvent *, String *params, 
			 Cardinal *num_params)
{
    const GraphEditWidget _w      = GraphEditWidget(w);
    const Dimension gridWidth     = _w->graphEdit.gridWidth;
    const Dimension gridHeight    = _w->graphEdit.gridHeight;
    const BoxPoint& minimalOffset = _w->graphEdit.minimalOffset;

    BoxPoint grid(gridWidth, gridHeight);

    if (num_params == 0 || *num_params != 2)
    {
	cerr << "move-selected: usage: move-selected(X, Y)\n";
	return;
    }

    BoxPoint offset;

    string offset_s[2];
    offset_s[X] = params[0];
    offset_s[Y] = params[1];

    BoxDimension d;
    for (d = X; d <= Y; d++)
    {
	BoxCoordinate& c = offset[d];
	string& s = offset_s[d];

	if (s == "+grid" || s == "grid")
	    c = +grid[d];
	else if (s == "-grid")
	    c = -grid[d];
	else
	{
	    char *ptr;
	    char *str = (char *)s;
	    c = strtol(str, &ptr, 0);
	    if (ptr == str)
	    {
		cerr << "move-selected: illegal argument " << str << "\n";
		return;
	    }
	}
    }

    getMinimalOffset(w);
    
    for (d = X; d <= Y; d++)
    {
	// Offset must not move nodes out of area
	if (offset[d] < minimalOffset[d])
	    offset[d] = minimalOffset[d];
    }

    if (offset != BoxPoint(0, 0))
    {
	move_selected_nodes(w, offset);
	graphEditSizeChanged(w);
    }
}


// Select single node
static void select_single_node(Widget w, GraphNode *selectNode)
{
    if (selectNode == 0)
	return;

    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;

    bool changed = false;

    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (node != selectNode && node->selected())
	{
	    node->selected() = false;
	    changed = true;
	}
    }

    if (!selectNode->selected())
    {
	selectNode->selected() = true;
	changed = true;
    }

    if (changed)
    {
	selectionChanged(w);
	graphEditRedraw(w);
    }
}

// Select first node
static void SelectFirst(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;

    select_single_node(w, graph->firstNode());
}

// Select next node
static void SelectNext(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;

    GraphNode *selectNode = 0;
    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    selectNode = graph->nextNode(node);
	    break;
	}
    }

    if (selectNode == 0)
	selectNode = graph->firstNode();

    select_single_node(w, selectNode);
}

// Select previous node
static void SelectPrev(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->graphEdit.graph;

    GraphNode *lastNode = 0;
    GraphNode *selectNode = 0;
    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (node->selected())
	    selectNode = lastNode;
	lastNode = node;
    }

    if (selectNode == 0)
	selectNode = lastNode;

    select_single_node(w, selectNode);
}



// Snap nodes to grid
static void _SnapToGrid(Widget w, XEvent *, String *params, 
			Cardinal *num_params)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const Dimension gridWidth  = _w->graphEdit.gridWidth;
    const Dimension gridHeight = _w->graphEdit.gridHeight;

    BoxPoint grid(gridWidth, gridHeight);

    if (*num_params >= 1)
	grid[X] = atoi(params[0]);
    if (*num_params >= 2)
	grid[Y] = atoi(params[1]);

    bool redraw = false;

    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	BoxPoint pos = node->pos();

	for (BoxDimension d = X; d <= Y; d++)
	    if (grid[d] > 0)
		pos[d] = ((pos[d] + grid[d] / 2) / grid[d]) * grid[d];

	if (pos != node->pos())
	{
            // set new node position
	    moveTo(w, node, pos, graph->nextNode(node) == 0);
	    redraw = true;
	}
    }
}

static void SnapToGrid(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SnapToGrid(w, event, params, num_params);
    graphEditRedraw(w);
}

static int get_new_rotation(Widget w, String *params, Cardinal *num_params,
			    String name, String default_param, 
			    String extra_args = "")
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Cardinal rotation    = _w->graphEdit.rotation;

    string param = "";
    if (num_params && *num_params >= 1)
	param = params[0];
    if (param == "")
	param = default_param;

    int new_rotation = atoi(param);
    new_rotation = (new_rotation % 360 + 360) % 360;

    if (new_rotation % 90 != 0)
    {
	cerr << name << ": usage: " << name << "(" 
	     << extra_args << "[[+-]DEGREES]), "
	    "where DEGREES is a multiple of 90\n";
	return -1;
    }

    if (param[0] == '+')
	new_rotation = (rotation + new_rotation + 360) % 360;
    else if (param[0] == '-')
	new_rotation = (rotation - new_rotation + 360) % 360;

    return new_rotation;
}


// Rotate nodes
static void _Rotate(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    Cardinal& rotation         = _w->graphEdit.rotation;

    int new_rotation = 
	get_new_rotation(w, params, num_params, "rotate", "+90");
    if (new_rotation < 0)
	return;

    int width  = _w->core.width;
    int height = _w->core.height;

    for (int offset  = (rotation - new_rotation + 360) % 360;
	 offset > 0; offset -= 90)
    {
	for (GraphNode *node = graph->firstNode(); 
	     node != 0;
	     node = graph->nextNode(node))
	{
	    BoxPoint pos = node->pos();
	    pos[X] = width - node->pos()[Y];
	    pos[Y] = node->pos()[X];

	    if (pos != node->pos())
	    {
		moveTo(w, node, pos, 
		       graph->nextNode(node) == 0 && new_rotation <= 90);
	    }
	}

	int tmp = width;
	width = height;
	height = tmp;
    }

    rotation = new_rotation;

    Cardinal zero = 0;
    _Normalize(w, event, 0, &zero);
    _SnapToGrid(w, event, 0, &zero);
}

static void Rotate(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Boolean autoLayout   = _w->graphEdit.autoLayout;

    _Rotate(w, event, params, num_params);
    if (autoLayout)
    {
	Cardinal zero = 0;
	_Layout(w, event, 0, &zero);
    }
    graphEditRedraw(w);
}



// Layout nodes

static Graph *layout_graph = 0;
static Widget layout_widget = 0;

static void LayoutNodeCB(char *node_name, int x, int y)
{
    GraphNode *node = (GraphNode *)strtol(node_name, 0, 16);
    assert(node != 0);

    node->moveTo(BoxPoint(x, y));
}

static void LayoutHintCB(char *from_name, char *to_name, int x, int y)
{
    GraphNode *from = (GraphNode *)strtol(from_name, 0, 16);
    assert(from != 0);
    GraphNode *to   = (GraphNode *)strtol(to_name, 0, 16);
    assert(to != 0);

    for (GraphEdge *edge = from->firstFrom();
	 edge != 0;
	 edge = from->nextFrom(edge))
    {
	if (edge->hidden())
	    continue;

	GraphNode *n = edge->to();
	while (n->isHint())
	    n = n->firstFrom()->to();
	if (n == to)
	{
	    // We hide the original edge...
	    edge->hidden() = true;

	    // ... and insert an edge hint at the end
	    // of the path between FROM and TO.
	    HintGraphNode *hint = new HintGraphNode(BoxPoint(x, y));
	 
	    *layout_graph += hint;
	    *layout_graph += new LineGraphEdge(edge->from(), hint);
	    *layout_graph += new LineGraphEdge(hint, edge->to());

	    return;
	}
    }
}

static int LayoutCompareCB(char *name1, char *name2)
{
    GraphNode *node1 = (GraphNode *)strtol(name1, 0, 16);
    assert(node1 != 0);
    GraphNode *node2 = (GraphNode *)strtol(name2, 0, 16);
    assert(node2 != 0);

    GraphEditCompareNodesInfo info;

    info.graph = layout_graph;
    info.node1 = node1;
    info.node2 = node2;

    XtCallCallbacks(layout_widget, XtNcompareNodesCallback, caddr_t(&info));

    return info.result;
}

string node_name(GraphNode *node)
{
    ostrstream os;
    os << "0x" << setbase(16) << (unsigned long)node;
    return string(os);
}

static void remove_all_hints(Graph *graph)
{
    // Find all hint nodes
    VarArray<GraphNode *> hints;

    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (node->isHint())
	    hints += node;
    }

    // Remove hint nodes
    for (int i = 0; i < hints.size(); i++)
	*graph -= hints[i];

    // Enable remaining edges
    for (GraphEdge *edge = graph->firstEdge(); 
	 edge != 0;
	 edge = graph->nextEdge(edge))
    {
	edge->hidden() = false;
    }
}

// Replace all paths A -> HINT_1 -> HINT_2 -> ... -> HINT_N -> B
// by A -> B, where B is placed at the position of HINT_1.

static void compact_layouted_graph(Graph *graph)
{
    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	if (!node->isHint())
	{
	    for (GraphEdge *edge = node->firstFrom();
		 edge != 0; edge = node->nextFrom(edge))
	    {
		if (edge->to()->isHint())
		{
		    GraphNode *hint = edge->to();
		    const BoxPoint& hintPos = hint->pos();

		    GraphEdge *hint_edge = hint->firstFrom();
		    while (hint_edge && hint_edge->to()->isHint())
			hint_edge = hint_edge->to()->firstFrom();

		    if (hint_edge)
		    {
			// Move non-hint node at first hint position
			hint_edge->to()->moveTo(hintPos);
		    }

		    // Remove edge to hint such that we are not visited again
		    *graph -= edge;
		}
	    }
	}
    }

    remove_all_hints(graph);
}    

static void _Layout(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    static char graph_name[] = "graph";

    const GraphEditWidget _w   = GraphEditWidget(w);
    Graph* graph               = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;
    Cardinal& rotation         = _w->graphEdit.rotation;
    LayoutMode mode            = _w->graphEdit.layoutMode;

    if (num_params && *num_params > 0 && params[0][0] != '\0')
    {
	LayoutMode mode_param;
	XrmValue v1, v2;
	v1.addr = caddr_t(params[0]);
	v1.size = sizeof(String);
	v2.addr = caddr_t(&mode_param);
	v2.size = sizeof(LayoutMode);

	Boolean ok = 
	    XtConvertAndStore(w, XtRString, &v1, XtRLayoutMode, &v2);
	if (ok)
	    mode = mode_param;
    }

    Cardinal new_num_params = 
	(num_params && *num_params > 0 ? *num_params - 1 : 0);
    int new_rotation = 
	get_new_rotation(w, params - 1, &new_num_params, 
			 "layout", "+0", "MODE, ");
    if (new_rotation < 0)
	return;

    // Remove all hint nodes
    remove_all_hints(graph);

    // Send graph to layouter
    Layout::add_graph(graph_name);

    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	BoxRegion r = node->region(graphGC);
	int width  = r.space(X);
	int height = r.space(Y);

	if ((new_rotation % 180) / 90 > 0)
	{
	    int tmp = width;
	    width = height;
	    height = tmp;
	}

	string name = node_name(node);
	Layout::add_node(graph_name, name);
	Layout::set_node_width(graph_name, name, width);
	Layout::set_node_height(graph_name, name, height);
	Layout::set_node_position(graph_name, name, -1, -1);
    }

    for (GraphEdge *edge = graph->firstEdge(); 
	 edge != 0;
	 edge = graph->nextEdge(edge))
    {
	Layout::add_edge(graph_name, 
			 node_name(edge->from()), node_name(edge->to()));
    }

    // Layout the graph
    layout_widget = w;
    layout_graph  = graph;
    Layout::node_callback    = LayoutNodeCB;
    Layout::hint_callback    = LayoutHintCB;
    Layout::compare_callback = LayoutCompareCB;
    Layout::layout(graph_name);

    // Post-process graph for compact representation
    if (mode == CompactLayoutMode)
	compact_layouted_graph(graph);

    // Clear the graph...
    Layout::remove_graph(graph_name);

    // ... and re-rotate it.
    ostrstream os;
    os << new_rotation;

    string rotation_s = os;
    Cardinal rotate_num_params = 1;
    String rotate_params[1];
    rotate_params[0] = rotation_s;

    rotation = 0;
    _Rotate(w, event, rotate_params, &rotate_num_params);
}

// DoLayout() should be named Layout(), but this conflicts with the
// `Layout' class on some pre-ARM C++ compilers :-(
static void DoLayout(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _Layout(w, event, params, num_params);
    graphEditRedraw(w);
}


// Normalize graph
static void _Normalize(Widget w, XEvent *, String *, Cardinal *)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEdit.graphGC;
    const Dimension gridHeight = _w->graphEdit.gridHeight;
    const Dimension gridWidth  = _w->graphEdit.gridWidth;

    BoxRegion r = graph->region(graphGC);

    bool redraw = false;

    for (GraphNode *node = graph->firstNode(); 
	 node != 0;
	 node = graph->nextNode(node))
    {
	BoxPoint pos = node->pos() - r.origin() 
	    + BoxPoint(gridHeight, gridWidth);

	if (pos != node->pos())
	{
            // set new node position
	    moveTo(w, node, pos, graph->nextNode(node) == 0);
	    redraw = true;
	}
    }
}

static void Normalize(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _Normalize(w, event, params, num_params);
    graphEditRedraw(w);
}


// Show and hide edges

static void considerEdges(Widget w, XEvent *, String *params,
			  Cardinal *num_params, bool shallBeHidden)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->graphEdit.graph;

    // get the mode
    enum { Nope = 0, Both = 1, From = 2, To = 3, Any = 4 } themode = Nope;
    bool changedSomething = false;

    string p = "all";
    if (*num_params >= 1)
	p = params[0];

    if (p == "from")
	themode = From;
    else if (p == "to")
	themode = To;
    else if (p == "any")
	themode = Any;
    else if (p == "both")
	themode = Both;
    else
	cerr << "show-edges(" << themode << "): bad mode \"" << themode << "\"\n";

    for (GraphEdge *edge = graph->firstEdge(); edge != 0;
	edge = graph->nextEdge(edge))
    {
	bool set = false;

	switch (themode)
	{
	    // there should be a better way of coding this,
	    // but I don't know it...

	    case From:
		set = edge->from()->selected();
		break;

	    case To:
		set = edge->to()->selected();
		break;

	    case Any:
		set = edge->to()->selected() || edge->from()->selected();
		break;

	    case Both:
		set = edge->to()->selected() && edge->from()->selected();
		break;

	    case Nope:
		set = false;
		break;
	}

	if (set)
	{
	    if (edge->hidden() != shallBeHidden)
	    {
		changedSomething = true;
		edge->hidden() = shallBeHidden;
	    }
	}
    }

    if (changedSomething)
	graphEditRedraw(w);
}

static void ShowEdges(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    considerEdges(w, event, params, num_params, false);
}

static void HideEdges(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    considerEdges(w, event, params, num_params, true);
}
