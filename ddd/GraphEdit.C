// $Id$
// GraphEdit Widget

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
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

char GraphEdit_rcsid[] = 
    "$Id$";

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#ifdef IF_MOTIF
#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#endif // IF_MOTIF

#include "events.h"
#include "Graph.h"
#include "GraphEditP.h"
#include "GraphEdit.h"
#include "HintGraphN.h"
#include "LineGraphE.h"
#include "layout.h"
#include "misc.h"
#include "cook.h"
#include "strtoul.h"
#include "TimeOut.h"
#include "EdgeAPA.h"
#include "GraphNPA.h"
#include "casts.h"

#if XmVersion < 1002
// Motif 1.1 backwards compatibility
#ifndef XmInheritBorderHighlight
#define XmInheritBorderHighlight (XtWidgetProc)_XtInherit
#endif

#ifndef XmInheritBorderUnhighlight
#define XmInheritBorderUnhighlight (XtWidgetProc)_XtInherit
#endif

#ifndef XmInheritArmAndActivate
#define XmInheritArmAndActivate (XmArmAndActivate)_XtInherit
#endif
#endif // XmVersion


static BoxRegion EVERYWHERE(BoxPoint(0,0), BoxSize(INT_MAX, INT_MAX));


// Compute default foreground

#ifdef IF_MOTIF
static void defaultForeground(Widget w, int, XrmValue *value)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    value->addr = XPointer(&_w->res_.primitive.foreground);
}
#endif // IF_MOTIF


// Resource list

#ifdef IF_MOTIF
static XtResource resources[] = {
#define offset(field) XtOffsetOf(_GraphEditRec::Res, graphEdit.field)
    // {name, class, type, size, offset, default_type, default_addr}
    { XTRESSTR(XtNgraph), XTRESSTR(XtCGraph), XtRPointer, sizeof(Graph *),
        offset(graph), XtRImmediate, XtPointer(0) },
    { XTRESSTR(XtNmoveDelta), XTRESSTR(XtCMoveDelta), XtRDimension, sizeof(Dimension),
	offset(moveDelta), XtRImmediate, XtPointer(4) },
    { XTRESSTR(XtNrubberEdges), XTRESSTR(XtCRubberEdges), XtRBoolean, sizeof(Boolean),
	offset(rubberEdges), XtRImmediate, XtPointer(True) },
    { XTRESSTR(XtNrubberArrows), XTRESSTR(XtCRubberEdges), XtRBoolean, sizeof(Boolean),
	offset(rubberArrows), XtRImmediate, XtPointer(False) },
    { XTRESSTR(XtNrubberAnnotations), XTRESSTR(XtCRubberAnnotations), XtRBoolean, sizeof(Boolean),
	offset(rubberAnnotations), XtRImmediate, XtPointer(False) },
    { XTRESSTR(XtNopaqueMove), XTRESSTR(XtCOpaqueMove), XtRBoolean, sizeof(Boolean),
	offset(opaqueMove), XtRImmediate, XtPointer(False) },

    { XTRESSTR(XtNautoRaise), XTRESSTR(XtCAutoRaise), XtRBoolean, sizeof(Boolean),
	offset(autoRaise), XtRImmediate, XtPointer(True) },

    {XTRESSTR( XtNshowHints), XTRESSTR(XtCShowHints), XtRBoolean, sizeof(Boolean),
	offset(showHints), XtRImmediate, XtPointer(False) },
    { XTRESSTR(XtNhintSize), XTRESSTR(XtCHintSize), XtRDimension, sizeof(Dimension),
	offset(hintSize), XtRImmediate, XtPointer(6) },

    { XTRESSTR(XtNshowAnnotations), XTRESSTR(XtCShowAnnotations), XtRBoolean, sizeof(Boolean),
	offset(showAnnotations), XtRImmediate, XtPointer(True) },

    { XTRESSTR(XtNgridWidth), XTRESSTR(XtCGridSize), XtRDimension, sizeof(Dimension),
	offset(gridWidth), XtRImmediate, XtPointer(16) },
    { XTRESSTR(XtNgridHeight), XTRESSTR(XtCGridSize), XtRDimension, sizeof(Dimension),
	offset(gridHeight), XtRImmediate, XtPointer(16) },

    { XTRESSTR(XtNshowGrid), XTRESSTR(XtCShowGrid), XtRBoolean, sizeof(Boolean),
	offset(showGrid), XtRImmediate, XtPointer(False) },
    { XTRESSTR(XtNsnapToGrid), XTRESSTR(XtCSnapToGrid), XtRBoolean, sizeof(Boolean),
	offset(snapToGrid), XtRImmediate, XtPointer(False) },

    { XTRESSTR(XtNautoLayout), XTRESSTR(XtCAutoLayout), XtRBoolean, sizeof(Boolean),
	offset(autoLayout), XtRImmediate, XtPointer(False) },

    { XTRESSTR(XtNrotation), XTRESSTR(XtCRotation), XtRCardinal, sizeof(Cardinal),
	offset(rotation), XtRImmediate, XtPointer(0)},

    { XTRESSTR(XtNedgeWidth), XTRESSTR(XtCEdgeWidth), XtRDimension, sizeof(Dimension),
	offset(edgeWidth), XtRImmediate, XtPointer(0) },
    { XTRESSTR(XtNarrowAngle), XTRESSTR(XtCArrowAngle), XtRDimension, sizeof(Dimension),
	offset(arrowAngle), XtRImmediate, XtPointer(30) },
    { XTRESSTR(XtNarrowLength), XTRESSTR(XtCArrowLength), XtRDimension, sizeof(Dimension),
	offset(arrowLength), XtRImmediate, XtPointer(10) },
    { XTRESSTR(XtNselfEdgeDiameter), XTRESSTR(XtCSelfEdgeDiameter), XtRDimension, 
        sizeof(Dimension), offset(selfEdgeDiameter), 
        XtRImmediate, XtPointer(32) },

    { XTRESSTR(XtNextraWidth), XTRESSTR(XtCExtraSize), XtRDimension, sizeof(Dimension),
	offset(extraWidth), XtRImmediate, XtPointer(0) },
    { XTRESSTR(XtNextraHeight), XTRESSTR(XtCExtraSize), XtRDimension, sizeof(Dimension),
	offset(extraHeight), XtRImmediate, XtPointer(0) },

    { XTRESSTR(XtNrequestedWidth), XTRESSTR(XtCRequestedSize), XtRDimension, sizeof(Dimension),
	offset(requestedWidth), XtRImmediate, XtPointer(0) },
    { XTRESSTR(XtNrequestedHeight), XTRESSTR(XtCRequestedSize), XtRDimension, sizeof(Dimension),
	offset(requestedHeight), XtRImmediate, XtPointer(0) },

    { XTRESSTR(XtNselectTile), XTRESSTR(XtCBitmap), XtRBitmap, sizeof(Pixmap),
	offset(selectTile), XtRImmediate, XtPointer(0)},

    { XTRESSTR(XtNedgeAttachMode), XTRESSTR(XtCEdgeAttachMode), XTRESSTR(XtREdgeAttachMode),
	sizeof(EdgeAttachMode), offset(edgeAttachMode), 
        XtRImmediate, XtPointer(Straight) },
    { XTRESSTR(XtNlayoutMode), XTRESSTR(XtCLayoutMode), XTRESSTR(XtRLayoutMode),
	sizeof(LayoutMode), offset(layoutMode), 
        XtRImmediate, XtPointer(RegularLayoutMode) },
    { XTRESSTR(XtNselfEdgePosition), XTRESSTR(XtCSelfEdgePosition), XTRESSTR(XtRSelfEdgePosition),
	sizeof(SelfEdgePosition), offset(selfEdgePosition), 
        XtRImmediate, XtPointer(NorthEast) },
    { XTRESSTR(XtNselfEdgeDirection), XTRESSTR(XtCSelfEdgeDirection), XTRESSTR(XtRSelfEdgeDirection),
	sizeof(SelfEdgeDirection), offset(selfEdgeDirection), 
        XtRImmediate, XtPointer(Counterclockwise) },
    { XTRESSTR(XtNdashedLines), XTRESSTR(XtCDashedLines), XtRBoolean,
	sizeof(Boolean), offset(dashedLines), 
        XtRImmediate, XtPointer(False) },

    { XTRESSTR(XtNdefaultCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(defaultCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNmoveCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(moveCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNselectCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(selectCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNselectBottomLeftCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(selectBottomLeftCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNselectBottomRightCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(selectBottomRightCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNselectTopLeftCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(selectTopLeftCursor), XtRImmediate, XtPointer(0)},
    { XTRESSTR(XtNselectTopRightCursor), XTRESSTR(XtCCursor), XtRCursor, sizeof(Cursor),
	offset(selectTopRightCursor), XtRImmediate, XtPointer(0)},

    { XTRESSTR(XtNnodeColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(nodeColor), XtRCallProc, XtPointer(defaultForeground) },
    { XTRESSTR(XtNedgeColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(edgeColor), XtRCallProc, XtPointer(defaultForeground) },
    { XTRESSTR(XtNframeColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(frameColor), XtRCallProc, XtPointer(defaultForeground) },
    { XTRESSTR(XtNoutlineColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(outlineColor), XtRCallProc, XtPointer(defaultForeground) },
    { XTRESSTR(XtNgridColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(gridColor), XtRCallProc, XtPointer(defaultForeground) },
    { XTRESSTR(XtNselectColor), XTRESSTR(XtCColor), XtRPixel, sizeof(Pixel),
	offset(selectColor), XtRCallProc, XtPointer(defaultForeground) },

    { XTRESSTR(XtNnodePrintColor), XTRESSTR(XtCColor), XtRString, sizeof(String),
	offset(nodePrintColor), XtRImmediate, 0 },
    { XTRESSTR(XtNedgePrintColor), XTRESSTR(XtCColor), XtRString, sizeof(String),
	offset(edgePrintColor), XtRImmediate, 0 },

    { XTRESSTR(XtNpositionChangedCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(positionChangedProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNselectionChangedCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(selectionChangedProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNsizeChangedCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(sizeChangedProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNcompareNodesCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(compareNodesProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNpreLayoutCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(preLayoutProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNpostLayoutCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(postLayoutProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNpreSelectionCallback), XTRESSTR(XtCCallback), XtRCallback, sizeof(XtPointer),
	offset(preSelectionProc), XtRCallback, XtPointer(0) },

#undef offset
};
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No implementation of Xt resources
#endif
#endif // IF_MOTIF


// Action function declarations

#ifdef IF_MOTIF
static void Select      (Widget, XEvent *, String *, Cardinal *);
static void Extend      (Widget, XEvent *, String *, Cardinal *);
static void Toggle      (Widget, XEvent *, String *, Cardinal *);
static void SelectOrMove(Widget, XEvent *, String *, Cardinal *);
static void ExtendOrMove(Widget, XEvent *, String *, Cardinal *);
static void ToggleOrMove(Widget, XEvent *, String *, Cardinal *);
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
#endif // IF_MOTIF


// Actions table

#ifdef IF_MOTIF
static XtActionsRec actions[] = {
    { XTARECSTR("select"),         Select },        // select()
    { XTARECSTR("extend"),         Extend },        // extend()
    { XTARECSTR("toggle"),         Toggle },        // toggle()
    { XTARECSTR("select-or-move"), SelectOrMove },	 // select-or-move()
    { XTARECSTR("extend-or-move"), ExtendOrMove },	 // extend-or-move()
    { XTARECSTR("toggle-or-move"), ToggleOrMove },	 // toggle-or-move()
    { XTARECSTR("move-selected"),  MoveSelected },	 // move-selected(X, Y)
    { XTARECSTR("follow"),		Follow },        // follow()
    { XTARECSTR("end"),		End },           // end()
    { XTARECSTR("show-edges"),	ShowEdges },     // show-edges([any|both|from|to])
    { XTARECSTR("hide-edges"), 	HideEdges },     // hide-edges([any|both|from|to])
    { XTARECSTR("select-all"), 	SelectAll },     // select-all()
    { XTARECSTR("select-first"), 	SelectFirst },   // select-first()
    { XTARECSTR("select-next"), 	SelectNext },    // select-next()
    { XTARECSTR("select-prev"), 	SelectPrev },    // select-prev()
    { XTARECSTR("unselect-all"), 	UnselectAll },   // unselect-all()
    { XTARECSTR("snap-to-grid"),	SnapToGrid },    // snap-to-grid()
    { XTARECSTR("_snap-to-grid"),	_SnapToGrid },
    { XTARECSTR("rotate"),		Rotate },        // rotate([[+|-]DEGREES])
    { XTARECSTR("_rotate"),	_Rotate },
    { XTARECSTR("layout"),		DoLayout },      // layout([regular|compact],
    { XTARECSTR("_layout"),	_Layout },       //         [[+|-]DEGREES]])
    { XTARECSTR("normalize"),	Normalize },     // normalize()
    { XTARECSTR("_normalize"),	_Normalize },
};


// Default translation table

static const char *defaultTranslations =
    "Shift<Btn1Down>:	       toggle()\n"
    "~Shift<Btn1Down>:	       select-or-move()\n"
    "<Btn1Motion>:	       follow()\n"
    "<Btn1Up>:		       end()\n"
    "<Btn2Down>:	       toggle()\n"
    "<Btn2Motion>:	       follow()\n"
    "<Btn2Up>:		       end()\n"
    "~Shift Ctrl<Key>KP_1:     move-selected(-grid, +grid)\n"
    "~Shift Ctrl<Key>KP_2:     move-selected(    0, +grid)\n"
    "~Shift Ctrl<Key>KP_3:     move-selected(+grid, +grid)\n"
    "~Shift Ctrl<Key>KP_4:     move-selected(-grid,     0)\n"
    "~Shift Ctrl<Key>KP_6:     move-selected(+grid,     0)\n"
    "~Shift Ctrl<Key>KP_7:     move-selected(-grid, -grid)\n"
    "~Shift Ctrl<Key>KP_8:     move-selected(    0, -grid)\n"
    "~Shift Ctrl<Key>KP_9:     move-selected(+grid, -grid)\n"
    "~Shift Ctrl<Key>B:        move-selected(-grid,     0)\n"
    "~Shift Ctrl<Key>F:        move-selected(+grid,     0)\n"
    "~Shift Ctrl<Key>P:        move-selected(    0, -grid)\n"
    "~Shift Ctrl<Key>N:        move-selected(    0, +grid)\n"
    "~Ctrl Shift<Key>KP_1:     move-selected(-1, +1)\n"
    "~Ctrl Shift<Key>KP_2:     move-selected( 0, +1)\n"
    "~Ctrl Shift<Key>KP_3:     move-selected(+1, +1)\n"
    "~Ctrl Shift<Key>KP_4:     move-selected(-1,  0)\n"
    "~Ctrl Shift<Key>KP_6:     move-selected(+1,  0)\n"
    "~Ctrl Shift<Key>KP_7:     move-selected(-1, -1)\n"
    "~Ctrl Shift<Key>KP_8:     move-selected( 0, -1)\n"
    "~Ctrl Shift<Key>KP_9:     move-selected(+1, -1)\n"
    "~Ctrl Shift<Key>B:        move-selected(-1,  0)\n"
    "~Ctrl Shift<Key>F:        move-selected(+1,  0)\n"
    "~Ctrl Shift<Key>P:        move-selected( 0, -1)\n"
    "~Ctrl Shift<Key>N:        move-selected( 0, +1)\n"
    "~Meta ~Ctrl<Key>KP_4:     select-prev()\n"
    "~Meta ~Ctrl<Key>KP_6:     select-next()\n"
    "~Meta ~Ctrl<Key>KP_7:     select-first()\n"
    "~Meta ~Ctrl<Key>KP_0:     select-first()\n"
    "~Meta ~Ctrl<Key>B:        select-prev()\n"
    "~Meta ~Ctrl<Key>F:        select-next()\n"
    "~Meta ~Ctrl<Key>P:        select-prev()\n"
    "~Meta ~Ctrl<Key>N:        select-next()\n"
    "~Meta ~Ctrl<Key>A:        select-first()\n"
    "~Meta Ctrl<Key>A:         select-all()\n"
    ;

// These translations override the XmPrimitive base translations
static const char *extraTranslations =
    "~Meta ~Shift Ctrl<Key>osfLeft:       move-selected(-grid,     0)\n"
    "~Meta ~Shift Ctrl<Key>osfRight:      move-selected(+grid,     0)\n"
    "~Meta ~Shift Ctrl<Key>osfUp:         move-selected(    0, -grid)\n"
    "~Meta ~Shift Ctrl<Key>osfDown:       move-selected(    0, +grid)\n"
    "~Meta ~Shift Ctrl<Key>osfBeginLine:  move-selected(-grid, -grid)\n"
    "~Meta ~Ctrl Shift<Key>osfLeft:       move-selected(-1,  0)\n"
    "~Meta ~Ctrl Shift<Key>osfRight:      move-selected(+1,  0)\n"
    "~Meta ~Ctrl Shift<Key>osfUp:         move-selected( 0, -1)\n"
    "~Meta ~Ctrl Shift<Key>osfDown:       move-selected( 0, +1)\n"
    "~Meta ~Ctrl Shift<Key>osfBeginLine:  move-selected(-1, -1)\n"
    "~Meta ~Shift ~Ctrl<Key>osfLeft:      select-prev()\n"
    "~Meta ~Shift ~Ctrl<Key>osfRight:     select-next()\n"
    "~Meta ~Shift ~Ctrl<Key>osfUp:        select-prev()\n"
    "~Meta ~Shift ~Ctrl<Key>osfDown:      select-next()\n"
    "~Meta ~Shift ~Ctrl<Key>osfBeginLine: select-first()\n"
    "~Meta ~Shift Ctrl<Key>Left:          move-selected(-grid,     0)\n"
    "~Meta ~Shift Ctrl<Key>Right:         move-selected(+grid,     0)\n"
    "~Meta ~Shift Ctrl<Key>Up:            move-selected(    0, -grid)\n"
    "~Meta ~Shift Ctrl<Key>Down:          move-selected(    0, +grid)\n"
    "~Meta ~Ctrl Shift<Key>Left:          move-selected(-1,  0)\n"
    "~Meta ~Ctrl Shift<Key>Right:         move-selected(+1,  0)\n"
    "~Meta ~Ctrl Shift<Key>Up:            move-selected( 0, -1)\n"
    "~Meta ~Ctrl Shift<Key>Down:          move-selected( 0, +1)\n"
    "~Meta ~Shift ~Ctrl<Key>Left:         select-prev()\n"
    "~Meta ~Shift ~Ctrl<Key>Right:        select-next()\n"
    "~Meta ~Shift ~Ctrl<Key>Up:           select-prev()\n"
    "~Meta ~Shift ~Ctrl<Key>Down:         select-next()\n"
;
#endif // IF_MOTIF

// Method function declarations

static void ClassInitialize();

static void Initialize(Widget request, 
		       Widget w
#ifdef IF_MOTIF
		       , ArgList args,
		       Cardinal *num_args
#endif // IF_MOTIF
		       );

static void Redisplay(Widget w, XEvent *event, Region region);

#ifdef IF_MOTIF
static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes);
#endif // IF_MOTIF

static Boolean SetValues(Widget old, 
			 Widget request, 
			 Widget new_w
#ifdef IF_MOTIF
			 , ArgList args, 
			 Cardinal *num_args
#endif // IF_MOTIF
			 );

static void Destroy(Widget w);


#ifdef IF_MOTIF
// Class record initialization

GraphEditClassRec graphEditClassRec = {
  {	 /* core fields */
    /* superclass               */  (WidgetClass) &xmPrimitiveClassRec,
    /* class_name               */  CONST_CAST(char *,"GraphEdit"),
    /* widget_size              */  sizeof(GraphEditRec),
    /* class_initialize         */  ClassInitialize,
    /* class_part_initialize    */  XtWidgetClassProc(0),
    /* class_inited             */  False,
    /* initialize               */  Initialize,
    /* initialize_hook          */  XtArgsProc(0),
    /* realize                  */  Realize,
    /* actions                  */  actions,
    /* num_actions              */  XtNumber(actions),
    /* resources                */  resources,
    /* num_resources            */  XtNumber(resources),
    /* xrm_class                */  NULLQUARK,
    /* compress_motion          */  True,
    /* compress_exposure        */  True,
    /* compress_enterleave      */  True,
    /* visible_interest         */  False,
    /* destroy                  */  Destroy,
    /* resize                   */  XtInheritResize,
    /* expose                   */  Redisplay,
    /* set_values               */  SetValues,
    /* set_values_hook          */  XtArgsFunc(0),
    /* set_values_almost        */  XtInheritSetValuesAlmost,
    /* get_values_hook          */  XtArgsProc(0),
    /* accept_focus             */  XtAcceptFocusProc(0),
    /* version                  */  XtVersion,
    /* callback_private         */  XtPointer(0),
    /* tm_table                 */  CONST_CAST(char *,defaultTranslations),
    /* query_geometry           */  XtInheritQueryGeometry,
    /* display_accelerator      */  XtInheritDisplayAccelerator,
    /* extension                */  XtPointer(0)
  },
  {	 /* Primitive fields */
    /* border_highlight         */ XmInheritBorderHighlight,
    /* border_unhighlight       */ XmInheritBorderUnhighlight,
    /* translations             */ XtInheritTranslations,
    /* arm_and_activate         */ XtActionProc(0), // XmInheritArmAndActivate?
    /* syn_resources            */ (XmSyntheticResource *)0,
    /* num_syn_resources        */ 0,
    /* extension                */ XtPointer(0)
#if defined(__sgi) && !defined(LesstifVersion)
	// Paul Sydney <sydney@ulua.mhpcc.af.mil> reports that OSF/Motif
	// on an SGI Indy running IRIX 6.5 has an extra
	// `_SG_vendorExtension' field.  If this is not initialized
	// explicitly, then EGCS 1.1 gives a warning.
	, 0
#endif
  },
  {	/* GraphEdit fields */
    /* extension                */ XtPointer(0)
  },
};

WidgetClass graphEditWidgetClass = (WidgetClass)&graphEditClassRec;
#endif // IF_MOTIF

#ifndef IF_MOTIF
static const Glib::SignalProxyInfo GraphEdit_signal_compare_nodes_info =
{
  "compare-nodes",
  (GCallback) &Glib::SignalProxyNormal::slot0_void_callback,
  (GCallback) &Glib::SignalProxyNormal::slot0_void_callback
};
#endif // IF_MOTIF

// Method function definitions

// Set widget to minimal size
#ifdef IF_MOTIF
void graphEditSizeChanged(Widget w)
#else // NOT IF_MOTIF
void GtkGraphEdit::graphEditSizeChanged()
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    Arg args[10];
    int arg;

    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->res_.graphEdit.graph;
    const Dimension highlight_thickness = _w->res_.primitive.highlight_thickness;
    Boolean& sizeChanged                = _w->graphEditP.sizeChanged;
    const GraphGC& graphGC              = _w->graphEditP.graphGC;
    const Dimension extraWidth          = _w->res_.graphEdit.extraWidth;
    const Dimension extraHeight         = _w->res_.graphEdit.extraHeight;

    // Could it be this is invoked without any graph yet?
    if (graph == 0)
	return;

    sizeChanged = False;

    BoxRegion r = graph->region(graphGC);

    Dimension myWidth  = r.origin(X) + r.space(X) + highlight_thickness * 2;
    Dimension myHeight = r.origin(Y) + r.space(Y) + highlight_thickness * 2;

    Dimension parentWidth  = 0;
    Dimension parentHeight = 0;

    Widget parent = XtParent(w);
    if (!XmIsScrolledWindow(parent))
	parent = XtParent(parent);	// Skip clipping window
    if (XmIsScrolledWindow(parent))
    {
	// Get the size allowed by our parent
	Dimension parentSpacing;

	arg = 0;
	XtSetArg(args[arg], XtNwidth,   &parentWidth);   arg++;
	XtSetArg(args[arg], XtNheight,  &parentHeight);  arg++;
	XtSetArg(args[arg], XmNspacing, &parentSpacing); arg++;
	XtGetValues(parent, args, arg);

	if (parentWidth >= parentSpacing)
	    parentWidth -= parentSpacing;
	if (parentHeight >= parentSpacing)
	    parentHeight -= parentSpacing;
    }

    Dimension width  = max(parentWidth, myWidth);
    Dimension height = max(parentHeight, myHeight);

    width  += extraWidth;
    height += extraHeight;

    Dimension width_return;
    Dimension height_return;
    XtGeometryResult result = 
	XtMakeResizeRequest(w, width, height, &width_return, &height_return);
    if (result == XtGeometryAlmost)
	result = XtMakeResizeRequest(w, width_return, height_return,
				     &width_return, &height_return);

    if (result == XtGeometryYes)
    {
	// Normally, we should let our manager resize ourselves.
	// But LessTif 0.87 wants it this way.
	XtResizeWidget(w, width_return, height_return, 0);

	graphEditRedraw(w);
    }
#else // NOT IF_MOTIF
    std::cerr << "graphEditSizeChanged\n";
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
// Return the graph's Graphic Context
const GraphGC& graphEditGetGraphGC(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w = GraphEditWidget(w);
    const GraphGC& graphGC   = _w->graphEditP.graphGC;

    return graphGC;
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Return the graph
Graph *graphEditGetGraph(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    GraphEditWidget _w = GraphEditWidget(w);
    return _w->res_.graphEdit.graph;
}
#endif // IF_MOTIF


#ifdef IF_MOTIF
// Set grid pixmap
static void setGrid(Widget w, Boolean reset = False)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Pixel     gridColor  = _w->res_.graphEdit.gridColor;
    const Pixel     background = _w->res_.core.background_pixel;
    const Boolean   showGrid   = _w->res_.graphEdit.showGrid;
    Dimension& gridHeight      = _w->res_.graphEdit.gridHeight;
    Dimension& gridWidth       = _w->res_.graphEdit.gridWidth;
    Pixmap& gridPixmap         = _w->graphEditP.gridPixmap;

    gridWidth  = max(gridWidth,  2);
    gridHeight = max(gridHeight, 2);

    if (reset && gridPixmap != None)
    {
	// delete old pixmap
	XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), ParentRelative);
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);

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
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);

	delete[] gridData;
    }
}

#else // NOT IF_MOTIF

// Set grid pixmap
void GtkGraphEdit::setGrid(bool reset)
{
    const Gdk::Color     background = get_style()->get_bg(Gtk::STATE_NORMAL);

    gridWidth  = max(gridWidth,  2);
    gridHeight = max(gridHeight, 2);

    Glib::RefPtr<Gdk::Window> win = get_window();
    if (reset && gridPixmap)
    {
	// delete old pixmap
	win->set_back_pixmap(Pixmap(), true);
	win->clear();

	gridPixmap.clear();
    }

    if (!gridPixmap)
    {
	// Create grid pixmap
	int gridDataSize = ((gridWidth + 7) / 8) * gridHeight + 1;
	char *gridData = new char [gridDataSize];
	for (int i = 0; i < gridDataSize; i++)
	    gridData[i] = 0x00;
	if (showGrid)
	    gridData[0] = 0x01;

#ifdef IF_MOTIF
	int depth = PlanesOfScreen(XtScreen(w));
	gridPixmap = 
	    XCreatePixmapFromBitmapData(XtDisplay(w), 
					XtWindow(w),
					gridData, 
					gridWidth, gridHeight,
					gridColor, background,
					depth);

 	XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), gridPixmap);
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
#else // NOT IF_MOTIF
	int depth = win->get_depth();
	gridPixmap = Gdk::Pixmap::create_from_data(win,
						   gridData, 
						   gridWidth, gridHeight, depth,
						   gridColor, background);

	win->set_back_pixmap(gridPixmap, true);
	win->clear();
#endif // IF_MOTIF

	delete[] gridData;
    }
}
#endif // IF_MOTIF


#ifdef IF_MOTIF
// Redraw
static void RedrawCB(XtPointer client_data, XtIntervalId *id)
{
    const Widget w                      = Widget(client_data);
    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->res_.graphEdit.graph;
    const GraphGC& graphGC              = _w->graphEditP.graphGC;
    const Boolean sizeChanged           = _w->graphEditP.sizeChanged;
    const Boolean redisplayEnabled      = _w->graphEditP.redisplayEnabled;
    const Boolean highlight_drawn       = _w->res_.primitive.highlight_drawn;
    const Dimension highlight_thickness = _w->res_.primitive.highlight_thickness;
    XtIntervalId& redrawTimer           = _w->graphEditP.redrawTimer;

    (void) id;			// Use it
    assert(redrawTimer == *id);
    redrawTimer = 0;

    if (graph == 0)
	return;			// No graph to draw

    if (!redisplayEnabled)
	return;			// Display disabled

    setGrid(w);

    if (sizeChanged)
	graphEditSizeChanged(w);

    // Redraw XmPrimitive border
    if (highlight_drawn)
	graphEditClassRec.primitive_class.border_highlight(w);

    // Check for pending redrawings
    Boolean redraw_all = True;
    GraphNode *node;
    for (node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (!node->redraw())
	{
	    redraw_all = False;
	    break;
	}
    }

    setGrid(w);

    if (redraw_all)
    {
	XClearArea(XtDisplay(w), XtWindow(w),
		   highlight_thickness, highlight_thickness, 
		   _w->res_.core.width  - highlight_thickness * 2, 
		   _w->res_.core.height - highlight_thickness * 2,
		   False);

	graph->draw(w, EVERYWHERE, graphGC);
    }

    for (node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (!redraw_all && node->redraw())
	{
	    BoxRegion r = node->region(graphGC);
	    XClearArea(XtDisplay(w), XtWindow(w), r.origin(X), r.origin(Y),
		       r.space(X), r.space(Y), False);

	    graph->draw(w, r, graphGC);
	}

	node->redraw() = False;
    }
}

#else // NOT IF_MOTIF

// Redraw
void GtkGraphEdit::RedrawCB(void)
{
    redrawTimer.disconnect();

    if (graph == 0)
	return;			// No graph to draw

    if (!redisplayEnabled)
	return;			// Display disabled

    setGrid();

    if (sizeChanged)
	graphEditSizeChanged();

    // Check for pending redrawings
    Boolean redraw_all = True;
    GraphNode *node;
    for (node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (!node->redraw())
	{
	    redraw_all = False;
	    break;
	}
    }

    setGrid();

    if (redraw_all)
    {
	get_window()->clear_area(highlight_thickness, highlight_thickness, 
				 get_width()  - highlight_thickness * 2, 
				 get_height() - highlight_thickness * 2);

	graph->draw(this, EVERYWHERE, graphGC);
    }

    for (node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (!redraw_all && node->redraw())
	{
	    BoxRegion r = node->region(graphGC);
	    get_window()->clear_area(r.origin(X), r.origin(Y),
				     r.space(X), r.space(Y));

	    graph->draw(this, r, graphGC);
	}

	node->redraw() = False;
    }
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Launch redrawing procedure
static void StartRedraw(Widget w)
{
    const GraphEditWidget _w  = GraphEditWidget(w);
    XtIntervalId& redrawTimer = _w->graphEditP.redrawTimer;

    if (redrawTimer != 0)
	return;			// Redraw pending

    // Redraw after we are back in the event loop
    redrawTimer = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
				  0, RedrawCB, XtPointer(w));
}

#else // NOT IF_MOTIF

// Launch redrawing procedure
void GtkGraphEdit::StartRedraw(void)
{
    if (redrawTimer)
	return;			// Redraw pending

    // Redraw after we are back in the event loop
    redrawTimer = Glib::signal_idle().connect(sigc::bind_return(MEM_FUN(*this, &GtkGraphEdit::RedrawCB), false));
}

#endif // IF_MOTIF

#ifdef IF_MOTIF
// Redraw entire graph
void graphEditRedraw(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;

    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	graphEditRedrawNode(w, node);
    }
}

#else // NOT IF_MOTIF

// Redraw entire graph
void GtkGraphEdit::graphEditRedraw(void)
{
    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	graphEditRedrawNode(node);
    }
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Redraw a specific region
void graphEditRedrawNode(Widget w, GraphNode *node)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    if (!node->hidden())
    {
	node->redraw() = True;
	StartRedraw(w);
    }
}
#else // NOT IF_MOTIF
// Redraw a specific region
void GtkGraphEdit::graphEditRedrawNode(GraphNode *node)
{
    if (!node->hidden())
    {
	node->redraw() = true;
	StartRedraw();
    }
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Disable redrawing for a while; return old state
Boolean graphEditEnableRedisplay(Widget w, Boolean state)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w   = GraphEditWidget(w);
    Boolean& redisplayEnabled  = _w->graphEditP.redisplayEnabled;

    Boolean old_state = redisplayEnabled;
    redisplayEnabled = state;

    if (redisplayEnabled)
	StartRedraw(w);

    return old_state;
}
#else // NOT IF_MOTIF
// Disable redrawing for a while; return old state
bool GtkGraphEdit::enable_redisplay(bool state)
{
    Boolean old_state = redisplayEnabled;
    redisplayEnabled = state;

    if (redisplayEnabled)
	StartRedraw();

    return old_state;
}
#endif // IF_MOTIF

#ifdef IF_MOTIF

// Converters

#define done(type, value) \
    {							\
	if (toVal->addr != 0) {			\
	    if (toVal->size < sizeof(type)) {		\
		toVal->size = sizeof(type);		\
		return False;				\
	    }						\
	    *(type *)(toVal->addr) = (value);		\
	}						\
	else {						\
	    static type static_val;			\
	    static_val = (value);			\
	    toVal->addr = (XPointer)&static_val;	\
	}						\
							\
	toVal->size = sizeof(type);			\
	return True;					\
    }



// Convert String to EdgeAttachMode and vice versa

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
	    (String *)0, (Cardinal *)0);
    
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
	    (String *)0, (Cardinal *)0);

    EdgeAttachMode mode = *((EdgeAttachMode *)fromVal->addr);

    const _XtString s = "unknown";
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

    done(const _XtString, s);
}


// Convert String to LayoutMode and vice versa

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
	    (String *)0, (Cardinal *)0);
    
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
	    (String *)0, (Cardinal *)0);

    LayoutMode mode = *((LayoutMode *)fromVal->addr);

    const _XtString s = "unknown";
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

    done(const _XtString, s);
}


// Convert String to SelfEdgePosition and vice versa

static Boolean CvtStringToSelfEdgePosition (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtStringToSelfEdgePosition", "wrongParameters",
	    "XtToolkitError",
	    "String to SelfEdgePosition conversion needs no extra arguments",
	    (String *)0, (Cardinal *)0);
    
    string s = downcase((char *)fromVal->addr);

    SelfEdgePosition pos = NorthWest;
    if (s == "northwest")
	pos = NorthWest;
    else if (s == "northeast")
	pos = NorthEast;
    else if (s == "southwest")
	pos = SouthWest;
    else if (s == "southeast")
	pos = SouthEast;
    else
	XtDisplayStringConversionWarning(display, (String)fromVal->addr,
	    XtRSelfEdgePosition);

    done(SelfEdgePosition, pos);
}

static Boolean CvtSelfEdgePositionToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtSelfEdgePositionToString", "wrongParameters",
	    "XtToolkitError",
	    "SelfEdgePosition to String conversion needs no extra arguments",
	    (String *)0, (Cardinal *)0);

    SelfEdgePosition pos = *((SelfEdgePosition *)fromVal->addr);

    const _XtString s = "unknown";
    switch (pos)
    {
    case NorthWest:
	s = "northwest";
	break;
    case NorthEast:
	s = "northeast";
	break;
    case SouthWest:
	s = "southwest";
	break;
    case SouthEast:
	s = "southwest";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(const _XtString, s);
}


// Convert String to SelfEdgeDirection and vice versa

static Boolean CvtStringToSelfEdgeDirection (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtStringToSelfEdgeDirection", "wrongParameters",
	    "XtToolkitError",
	    "String to SelfEdgeDirection conversion needs no extra arguments",
	    (String *)0, (Cardinal *)0);
    
    string s = downcase((char *)fromVal->addr);

    SelfEdgeDirection dir = Counterclockwise;
    if (s == "counterclockwise")
	dir = Counterclockwise;
    else if (s == "clockwise")
	dir = Clockwise;
    else
	XtDisplayStringConversionWarning(display, (String)fromVal->addr,
	    XtRSelfEdgeDirection);

    done(SelfEdgeDirection, dir);
}

static Boolean CvtSelfEdgeDirectionToString (Display *display, XrmValue *,
    Cardinal *num_args, XrmValue *fromVal, XrmValue *toVal,
    XtPointer *)
{
    if (*num_args != 0)
	XtAppWarningMsg(XtDisplayToApplicationContext(display),
	    "CvtSelfEdgeDirectionToString", "wrongParameters",
	    "XtToolkitError",
	    "SelfEdgeDirection to String conversion needs no extra arguments",
	    (String *)0, (Cardinal *)0);

    SelfEdgeDirection pos = *((SelfEdgeDirection *)fromVal->addr);

    const _XtString s = "unknown";
    switch (pos)
    {
    case Clockwise:
	s = "clockwise";
	break;
    case Counterclockwise:
	s = "counterclockwise";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(const _XtString, s);
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
	    (String *)0, (Cardinal *)0);

    Boolean mode = *((Boolean *)fromVal->addr);

    const _XtString s = "unknown";
    switch (mode)
    {
    case True:
	s = "on";
	break;
    case False:
	s = "off";
	break;
    default:
	XtDisplayStringConversionWarning(display, s, XtRString);
	break;
    }

    done(const _XtString, s);
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
	    (String *)0, (Cardinal *)0);

    Dimension d = *((Dimension *)fromVal->addr);

    std::ostringstream os;
    os << d;
    string os_s(os);
    String s = (String)XtNewString(os_s.chars());

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
	    (String *)0, (Cardinal *)0);

    Cardinal d = *((Cardinal *)fromVal->addr);

    std::ostringstream os;
    os << d;
    string os_s(os);
    String s = (String)XtNewString(os_s.chars());

    done(String, s);
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning We do not do converters
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Initialize class
static void ClassInitialize()
{
    // Register own string -> type converters
    XtSetTypeConverter(XtRString, XtREdgeAttachMode, 
		       CvtStringToEdgeAttachMode,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRString, XtRLayoutMode, 
		       CvtStringToLayoutMode,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRString, XtRSelfEdgePosition, 
		       CvtStringToSelfEdgePosition,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRString, XtRSelfEdgeDirection, 
		       CvtStringToSelfEdgeDirection,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));

    // Register own type -> string converters
    XtSetTypeConverter(XtREdgeAttachMode, XtRString, 
		       CvtEdgeAttachModeToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRLayoutMode, XtRString, 
		       CvtLayoutModeToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRSelfEdgePosition, XtRString, 
		       CvtSelfEdgePositionToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));
    XtSetTypeConverter(XtRSelfEdgeDirection, XtRString, 
		       CvtSelfEdgeDirectionToString,
		       XtConvertArgList(0), 0, 
		       XtCacheAll, XtDestructor(0));

    // Register standard type -> string converters
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
#endif // IF_MOTIF

// Initialize widget

#ifdef IF_MOTIF
inline void createCursor(Widget w, Cursor& cursor, unsigned int shape)
{
    if (cursor == 0)
	cursor = XCreateFontCursor(XtDisplay(w), shape);
}
#ifdef NAG_ME
#warning No cursors yet
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void setGCs(Widget w)
{
    const GraphEditWidget _w        = GraphEditWidget(w);

    // read-only
    const Dimension edgeWidth       = _w->res_.graphEdit.edgeWidth;
    const Pixmap selectTile         = _w->res_.graphEdit.selectTile;
    const Pixel background          = _w->res_.core.background_pixel;
    const Pixel nodeColor           = _w->res_.graphEdit.nodeColor;
    const Pixel edgeColor           = _w->res_.graphEdit.edgeColor;
    const Pixel frameColor          = _w->res_.graphEdit.frameColor;
    const Pixel outlineColor        = _w->res_.graphEdit.outlineColor;
    const Pixel selectColor         = _w->res_.graphEdit.selectColor;
    const Boolean dashedLines       = _w->res_.graphEdit.dashedLines;

    // write-only
    GC& nodeGC                      = _w->graphEditP.nodeGC;
    GC& edgeGC                      = _w->graphEditP.edgeGC;
    GC& invertGC                    = _w->graphEditP.invertGC;
    GC& clearGC                     = _w->graphEditP.clearGC;
    GC& frameGC                     = _w->graphEditP.frameGC;
    GC& outlineGC                   = _w->graphEditP.outlineGC;

    int line_style = dashedLines ? LineOnOffDash : LineSolid;

    // set nodeGC
    XGCValues gcv;
    gcv.foreground = nodeColor;
    gcv.line_width = 1;
    gcv.line_style = line_style;
    nodeGC = XtGetGC(w, GCForeground | GCLineWidth | GCLineStyle, &gcv);

    // set edgeGC
    gcv.foreground = edgeColor;
    gcv.line_width = edgeWidth;
    gcv.line_style = line_style;
    edgeGC = XtGetGC(w, GCForeground | GCLineWidth | GCLineStyle, &gcv);

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
#else // NOT IF_MOTIF
void GtkGraphEdit::setGCs(void)
{
    Gdk::LineStyle line_style = dashedLines ? Gdk::LINE_ON_OFF_DASH : Gdk::LINE_SOLID;

    // set nodeGC
    nodeGC = Gdk::GC::create(get_window());
    nodeGC->set_foreground(nodeColor);
    nodeGC->set_line_attributes(1, line_style, Gdk::CAP_BUTT, Gdk::JOIN_MITER);

    // set edgeGC
    edgeGC = Gdk::GC::create(get_window());
    edgeGC->set_foreground(edgeColor);
    edgeGC->set_line_attributes(edgeWidth, line_style, Gdk::CAP_BUTT, Gdk::JOIN_MITER);

    // set invertGC
    invertGC = Gdk::GC::create(get_window());
    if (selectTile)
    {
	invertGC->set_foreground(selectColor);
	invertGC->set_fill(Gdk::STIPPLED);
	invertGC->set_stipple(selectTile);
	invertGC->set_function(Gdk::COPY);
    }
    else
    {
	invertGC->set_foreground(selectColor);
	invertGC->set_function(Gdk::INVERT);
#ifdef NAG_ME
#warning How to set plane mask?
#endif
	// gcv.plane_mask = selectColor ^ background;
    }

    const Gdk::Color     background = get_style()->get_bg(Gtk::STATE_NORMAL);

    // set clearGC
    clearGC = Gdk::GC::create(get_window());
    clearGC->set_foreground(background);
    clearGC->set_function(Gdk::COPY);

    // set frameGC
    frameGC = Gdk::GC::create(get_window());
    frameGC->set_foreground(frameColor);
    frameGC->set_function(Gdk::INVERT);
    frameGC->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_BUTT, Gdk::JOIN_MITER);
#ifdef NAG_ME
#warning How to set plane mask?
#endif
    // gcv.plane_mask = frameColor ^ background;

    // set outlineGC
    outlineGC = Gdk::GC::create(get_window());
    outlineGC->set_foreground(outlineColor);
    outlineGC->set_function(Gdk::INVERT);
    outlineGC->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_BUTT, Gdk::JOIN_MITER);
#ifdef NAG_ME
#warning How to set plane mask?
#endif
    // gcv.plane_mask = outlineColor ^ background;
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void setGraphGC(Widget w)
{
    const GraphEditWidget _w        = GraphEditWidget(w);

    // Read only
    const Dimension arrowAngle      = _w->res_.graphEdit.arrowAngle;
    const Dimension arrowLength     = _w->res_.graphEdit.arrowLength;
    const Dimension hintSize        = _w->res_.graphEdit.hintSize;
    const Boolean showHints         = _w->res_.graphEdit.showHints;
    const Boolean showAnnotations   = _w->res_.graphEdit.showAnnotations;
    const GC nodeGC                 = _w->graphEditP.nodeGC;
    const GC edgeGC                 = _w->graphEditP.edgeGC;
    const GC invertGC               = _w->graphEditP.invertGC;
    const GC clearGC                = _w->graphEditP.clearGC;

    const Dimension selfEdgeDiameter = 
	_w->res_.graphEdit.selfEdgeDiameter;
    const SelfEdgePosition selfEdgePosition = 
	_w->res_.graphEdit.selfEdgePosition;
    const SelfEdgeDirection selfEdgeDirection = 
	_w->res_.graphEdit.selfEdgeDirection;
    const EdgeAttachMode edgeAttachMode = 
	EdgeAttachMode(_w->res_.graphEdit.edgeAttachMode);

    // Write only
    GraphGC& graphGC                = _w->graphEditP.graphGC;

    // Set graphGC
    graphGC = GraphGC(nodeGC, edgeGC, invertGC, clearGC);
    graphGC.arrowAngle        = arrowAngle;
    graphGC.arrowLength       = arrowLength;
    graphGC.edgeAttachMode    = EdgeAttachMode(edgeAttachMode);
    graphGC.drawHints         = showHints;
    graphGC.drawAnnotations   = showAnnotations;
    graphGC.hintSize          = hintSize;
    graphGC.selfEdgeDiameter  = selfEdgeDiameter;
    graphGC.selfEdgePosition  = selfEdgePosition;
    graphGC.selfEdgeDirection = selfEdgeDirection;

    // Get print colors

    if (_w->res_.graphEdit.nodePrintColor != 0)
    {
	XColor exact_def;
	Status ok = 
	    XParseColor(XtDisplay(w), _w->res_.core.colormap, 
			_w->res_.graphEdit.nodePrintColor, &exact_def);

	if (ok)
	{
	    graphGC.node_red   = exact_def.red;
	    graphGC.node_green = exact_def.green;
	    graphGC.node_blue  = exact_def.blue;
	}
	else
	{
	    Cardinal one = 1;

	    XtAppWarningMsg(XtWidgetToApplicationContext(w),
			    "GraphEdit::Initialize", "badColor",
			    "XtToolkitError",
			    "Cannot parse " XtNnodePrintColor " \"%s\"",
			    (String *)&_w->res_.graphEdit.nodePrintColor, &one);
	}
    }

    if (_w->res_.graphEdit.edgePrintColor != 0)
    {
	XColor exact_def;
	Status ok = 
	    XParseColor(XtDisplay(w), _w->res_.core.colormap, 
			_w->res_.graphEdit.edgePrintColor, &exact_def);

	if (ok)
	{
	    graphGC.edge_red   = exact_def.red;
	    graphGC.edge_green = exact_def.green;
	    graphGC.edge_blue  = exact_def.blue;
	}
	else
	{
	    Cardinal one = 1;

	    XtAppWarningMsg(XtWidgetToApplicationContext(w),
			    "GraphEdit::Initialize", "badColor",
			    "XtToolkitError",
			    "Cannot parse " XtNedgePrintColor " spec \"%s\"",
			    (String *)&_w->res_.graphEdit.edgePrintColor, &one);
	}
    }
}
#else // NOT IF_MOTIF
void GtkGraphEdit::setGraphGC(void)
{
    // Set graphGC
    graphGC = GraphGC(nodeGC, edgeGC, invertGC, clearGC);
    graphGC.arrowAngle        = arrowAngle;
    graphGC.arrowLength       = arrowLength;
    graphGC.edgeAttachMode    = EdgeAttachMode(edgeAttachMode);
    graphGC.drawHints         = showHints;
    graphGC.drawAnnotations   = showAnnotations;
    graphGC.hintSize          = hintSize;
    graphGC.selfEdgeDiameter  = selfEdgeDiameter;
    graphGC.selfEdgePosition  = selfEdgePosition;
    graphGC.selfEdgeDirection = selfEdgeDirection;

    // Get print colors

    if (nodePrintColor != 0)
    {
	Gdk::Color exact_def = Gdk::Color(Glib::ustring(nodePrintColor));
	bool ok = get_colormap()->alloc_color(exact_def);

	if (ok)
	{
	    graphGC.node_red   = exact_def.get_red();
	    graphGC.node_green = exact_def.get_green();
	    graphGC.node_blue  = exact_def.get_blue();
	}
	else
	{
	    Cardinal one = 1;

	    std::cerr << "Bad color " << nodePrintColor << "\n";
	}
    }

    if (edgePrintColor != 0)
    {
	Gdk::Color exact_def = Gdk::Color(Glib::ustring(edgePrintColor));
	bool ok = get_colormap()->alloc_color(exact_def);

	if (ok)
	{
	    graphGC.edge_red   = exact_def.get_red();
	    graphGC.edge_green = exact_def.get_green();
	    graphGC.edge_blue  = exact_def.get_blue();
	}
	else
	{
	    Cardinal one = 1;

	    std::cerr << "Bad color " << edgePrintColor << "\n";
	}
    }
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void Initialize(Widget request, Widget w, ArgList, Cardinal *)
#else // NOT IF_MOTIF
GtkGraphEdit::GtkGraphEdit(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    // read-only
    const GraphEditWidget _w        = GraphEditWidget(w);

    // write-only
    GraphEditState& state           = _w->graphEditP.state;
    Cursor& moveCursor              = _w->res_.graphEdit.moveCursor;
    Cursor& selectCursor            = _w->res_.graphEdit.selectCursor;
    Cursor& selectBottomLeftCursor  = _w->res_.graphEdit.selectBottomLeftCursor;
    Cursor& selectBottomRightCursor = _w->res_.graphEdit.selectBottomRightCursor;
    Cursor& selectTopLeftCursor     = _w->res_.graphEdit.selectTopLeftCursor;
    Cursor& selectTopRightCursor    = _w->res_.graphEdit.selectTopRightCursor;
    Pixmap& gridPixmap              = _w->graphEditP.gridPixmap;
    Boolean& sizeChanged            = _w->graphEditP.sizeChanged;
    Boolean& redisplayEnabled       = _w->graphEditP.redisplayEnabled;
    Time& lastSelectTime            = _w->graphEditP.lastSelectTime;
    XtIntervalId& redrawTimer       = _w->graphEditP.redrawTimer;
    Dimension& requestedWidth       = _w->res_.graphEdit.requestedWidth;
    Dimension& requestedHeight      = _w->res_.graphEdit.requestedHeight;
#else // NOT IF_MOTIF
    // Set some sensible defaults
    layoutMode = RegularLayoutMode;
    dashedLines = false;
    rotation = 0;
    graph = NULL;
    moveDelta = 4;
    rubberEdges = true;
    rubberArrows = true;
    rubberAnnotations = false;
    opaqueMove = false;
    autoRaise = false;
    showHints = true;
    showAnnotations = true;
    autoLayout = false;
    showGrid = true;
    gridWidth = 16;
    gridHeight = 16;
    snapToGrid = true;
    hintSize = 0;
    edgeWidth = 1;
    arrowAngle = 45;
    nodePrintColor = strdup("red");
    edgePrintColor = strdup("red");

    highlight_thickness = 2;
#endif // IF_MOTIF

    // init state
    state = NopState;

    // init sizeChanged
    sizeChanged = False;

    // init redisplayEnabled
    redisplayEnabled = True;

    // init lastSelectTime
    lastSelectTime = 0;

    // init redrawTimer
    redrawTimer = NO_TIMER;

    // set GCs
#ifdef IF_MOTIF
    setGCs(w);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Due to the eccentricities of Gtk, setGCs cannot
#warning be called until after this widget is realized.
#endif
    signal_realize().connect(sigc::mem_fun(*this, &GtkGraphEdit::setGCs));
#endif // IF_MOTIF

    // set Graph GC
    setGraphGC(M_ARGS_1(w));

#ifdef IF_MOTIF
    // set grid pixmap
    gridPixmap = None;
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // create cursors if not already set
    createCursor(w, moveCursor,              XC_fleur);
    createCursor(w, selectCursor,            XC_plus);
    createCursor(w, selectBottomLeftCursor,  XC_ll_angle);
    createCursor(w, selectBottomRightCursor, XC_lr_angle);
    createCursor(w, selectTopLeftCursor,     XC_ul_angle);
    createCursor(w, selectTopRightCursor,    XC_ur_angle);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No cursors yet
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // save requested size
    requestedWidth  = request->core.width;
    requestedHeight = request->core.height;
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No request in constructor
#endif
    requestedWidth  = 0;
    requestedHeight = 0;
#endif // IF_MOTIF

    // set size
    graphEditSizeChanged(M_ARGS_1(w));

#ifdef IF_MOTIF
    // Override XmPrimitive translations
    static XtTranslations translations = 
	XtParseTranslationTable(extraTranslations);

    XtOverrideTranslations(w, translations);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No translations
#endif
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
inline void defineCursor(Widget w, Cursor cursor)
#else // NOT IF_MOTIF
void GtkGraphEdit::defineCursor(Cursor cursor)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    if (cursor != 0)
	XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
    else
	XUndefineCursor(XtDisplay(w), XtWindow(w));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Sorry, no cursors yet
#endif
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
// Realize widget
static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes)
{
    const GraphEditWidget _w = GraphEditWidget(w);
    Cursor defaultCursor     = _w->res_.graphEdit.defaultCursor;

    // Call superclass realize method
    graphEditClassRec.core_class.superclass->
	core_class.realize(w, value_mask, attributes);
    
    // Setup default cursor
    defineCursor(w, defaultCursor);
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Redisplay widget
static void Redisplay(Widget w, XEvent *event, Region)
#else // NOT IF_MOTIF
bool GtkGraphEdit::on_expose_event(GdkEventExpose* event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w       = GraphEditWidget(w);
    const Graph* graph             = _w->res_.graphEdit.graph;
    const GraphGC& graphGC         = _w->graphEditP.graphGC;
    const Boolean sizeChanged      = _w->graphEditP.sizeChanged;
    const Boolean redisplayEnabled = _w->graphEditP.redisplayEnabled;
    const Boolean highlight_drawn  = _w->res_.primitive.highlight_drawn;
#endif // IF_MOTIF

    if (!redisplayEnabled)
    {
	graphEditRedraw(M_ARGS_1(w));
#ifdef IF_MOTIF
	return;
#else // NOT IF_MOTIF
	return false; // propagate
#endif // IF_MOTIF
    }

    if (sizeChanged)
	graphEditSizeChanged(M_ARGS_1(w));

    setGrid(M_ARGS_1(w));

#ifdef IF_MOTIF
    // Redraw XmPrimitive border
    if (highlight_drawn)
	graphEditClassRec.primitive_class.border_highlight(w);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Primitive border?
#endif
#endif // IF_MOTIF

#ifndef IF_MOTIF
    Gtk::Widget *w = this;
#endif
    graph->draw(w, BoxRegion(point((XEvent *)event), size((XEvent *)event)), graphGC);
}


#ifdef IF_MOTIF
// Set widget values
static Boolean SetValues(Widget old, Widget, Widget new_w, 
			 ArgList, Cardinal *)
{
    GraphEditWidget before = GraphEditWidget(old);
    GraphEditWidget after  = GraphEditWidget(new_w);

    Boolean redisplay = False;

    // redisplay graph if changed
    if (before->res_.graphEdit.graph != after->res_.graphEdit.graph)
    {
	redisplay = True;

	// Re-layout if auto-layout is enabled
	if (after->res_.graphEdit.autoLayout)
	{
	    Cardinal zero = 0;
	    _Layout(new_w, 0, 0, &zero);
	}

	// Snap to grid if enabled
	if (after->res_.graphEdit.snapToGrid)
	{
	    Cardinal zero = 0;
	    _SnapToGrid(new_w, 0, 0, &zero);
	}
    }

    Boolean new_gcs = False;

    // reset GCs if changed
    if (before->res_.graphEdit.edgeWidth      != after->res_.graphEdit.edgeWidth ||
	before->res_.graphEdit.selectTile     != after->res_.graphEdit.selectTile ||
	before->res_.graphEdit.dashedLines    != after->res_.graphEdit.dashedLines ||
	before->res_.graphEdit.nodeColor      != after->res_.graphEdit.nodeColor ||
	before->res_.graphEdit.edgeColor      != after->res_.graphEdit.edgeColor ||
	before->res_.graphEdit.frameColor     != after->res_.graphEdit.frameColor ||
	before->res_.graphEdit.outlineColor   != after->res_.graphEdit.outlineColor ||
	before->res_.graphEdit.gridColor      != after->res_.graphEdit.gridColor ||
	before->res_.graphEdit.selectColor    != after->res_.graphEdit.selectColor)
    {    
	setGCs(new_w);
	new_gcs = True;
	redisplay = True;
    }

    // reset GraphGC if changed
    if (new_gcs ||
	before->res_.graphEdit.arrowAngle      != after->res_.graphEdit.arrowAngle     ||
	before->res_.graphEdit.arrowLength     != after->res_.graphEdit.arrowLength    ||
	before->res_.graphEdit.showHints       != after->res_.graphEdit.showHints      ||
	before->res_.graphEdit.hintSize        != after->res_.graphEdit.hintSize       ||
	before->res_.graphEdit.edgeAttachMode  != after->res_.graphEdit.edgeAttachMode ||
	before->res_.graphEdit.showAnnotations != after->res_.graphEdit.showAnnotations)
    {
	setGraphGC(new_w);
	redisplay = True;
    }

    if (before->res_.graphEdit.nodePrintColor != after->res_.graphEdit.nodePrintColor ||
	before->res_.graphEdit.edgePrintColor != after->res_.graphEdit.edgePrintColor)
    {
	setGraphGC(new_w);
    }

    // reset grid pixmap if changed
    if (before->res_.graphEdit.gridWidth  != after->res_.graphEdit.gridWidth  ||
	before->res_.graphEdit.gridHeight != after->res_.graphEdit.gridHeight ||
	before->res_.graphEdit.showGrid   != after->res_.graphEdit.showGrid)
    {
	setGrid(new_w, True);
	redisplay = True;
    }

    // Always recompute size
    after->graphEditP.sizeChanged = True;

    return redisplay;
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No SetValues
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Destroy widget
static void Destroy(Widget)
{
    // Delete graph?
}
#else // NOT IF_MOTIF
GtkGraphEdit::~GtkGraphEdit(void)
{
}
#endif // IF_MOTIF

// Action function definitions

// Helping stuff

// Find node at point
#ifdef IF_MOTIF
GraphNode *graphEditGetNodeAtPoint(Widget w, const BoxPoint& p)
#else // NOT IF_MOTIF
GraphNode *GtkGraphEdit::graphEditGetNodeAtPoint(const BoxPoint& p)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w  = GraphEditWidget(w);
    const Graph* graph        = _w->res_.graphEdit.graph;
    GraphGC& graphGC          = _w->graphEditP.graphGC;
#endif // IF_MOTIF
    GraphNode *found = 0;

    // Could it be this is invoked without any graph yet?
    if (graph == 0)
	return 0;

    // note that we return the last matching node in the list;
    // thus on overlapping nodes we select the top one

    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
	node = graph->nextVisibleNode(node))
	if (p <= node->sensitiveRegion(graphGC))
	    found = node;

    return found;
}

#ifdef IF_MOTIF    
GraphNode *graphEditGetNodeAtEvent(Widget w, XEvent *event)
#else // NOT IF_MOTIF
GraphNode *GtkGraphEdit::graphEditGetNodeAtEvent(XEvent *event)
#endif // IF_MOTIF
{
    return graphEditGetNodeAtPoint(M_ARGS_2(w, point(event)));
}


// Get frame region
#ifdef IF_MOTIF
static BoxRegion frameRegion(Widget w)
#else // NOT IF_MOTIF
BoxRegion GtkGraphEdit::frameRegion(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEditP.startAction;
    const BoxPoint& endAction   = _w->graphEditP.endAction;
#endif // IF_MOTIF

    BoxPoint origin(min(startAction[X], endAction[X]),
	min(startAction[Y], endAction[Y]));

    BoxSize space(abs(startAction[X] - endAction[X]),
	abs(startAction[Y] - endAction[Y]));

    return BoxRegion(origin, space);
}

// Get frame cursor
#ifdef IF_MOTIF
static void setRegionCursor(Widget w)
#else // NOT IF_MOTIF
void GtkGraphEdit::setRegionCursor(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEditP.startAction;
    const BoxPoint& endAction   = _w->graphEditP.endAction;

    Cursor selectCursor            = _w->res_.graphEdit.selectCursor;
    Cursor selectBottomLeftCursor  = _w->res_.graphEdit.selectBottomLeftCursor;
    Cursor selectBottomRightCursor = _w->res_.graphEdit.selectBottomRightCursor;
    Cursor selectTopLeftCursor     = _w->res_.graphEdit.selectTopLeftCursor;
    Cursor selectTopRightCursor    = _w->res_.graphEdit.selectTopRightCursor;

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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning NO CURSORS YET
#endif
#endif // IF_MOTIF
}

inline void myXDrawLine(
#ifdef IF_MOTIF
    Display *display, 
#endif // IF_MOTIF
    Drawable d, 
    GC gc,
    const BoxPoint& f, const BoxPoint& t)
{
    if (f != t) {
#ifdef IF_MOTIF
	XDrawLine(display, d, gc, f[X], f[Y], t[X], t[Y]);
#else // NOT IF_MOTIF
	d->draw_line(gc, f[X], f[Y], t[X], t[Y]);
#endif // IF_MOTIF
    }
}
    
#ifdef IF_MOTIF
static void redrawSelectFrame(Widget w, const BoxRegion& r)
#else // NOT IF_MOTIF
void GtkGraphEdit::redrawSelectFrame(const BoxRegion& r)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    const GC frameGC = _w->graphEditP.frameGC;

    Display *display = XtDisplay(w);
    Window window    = XtWindow(w);

    // North
    myXDrawLine(display, window, frameGC, 
		r.origin() + BoxPoint(1, 0),
		r.origin() + BoxPoint(r.space(X) - 1, 0));

    // South
    myXDrawLine(display, window, frameGC,
		r.origin() + BoxPoint(0, r.space(Y)),
		r.origin() + BoxPoint(r.space(X), r.space(Y)));

    // East
    myXDrawLine(display, window, frameGC,
		r.origin(),
		r.origin() + BoxPoint(0, r.space(Y) - 1));

    // West
    myXDrawLine(display, window, frameGC,
		r.origin() + BoxPoint(r.space(X), 0),
		r.origin() + BoxPoint(r.space(X), r.space(Y) - 1));
#else // NOT IF_MOTIF
    Window window    = get_window();

    // North
    myXDrawLine(window, frameGC, 
		r.origin() + BoxPoint(1, 0),
		r.origin() + BoxPoint(r.space(X) - 1, 0));

    // South
    myXDrawLine(window, frameGC,
		r.origin() + BoxPoint(0, r.space(Y)),
		r.origin() + BoxPoint(r.space(X), r.space(Y)));

    // East
    myXDrawLine(window, frameGC,
		r.origin(),
		r.origin() + BoxPoint(0, r.space(Y) - 1));

    // West
    myXDrawLine(window, frameGC,
		r.origin() + BoxPoint(r.space(X), 0),
		r.origin() + BoxPoint(r.space(X), r.space(Y) - 1));
#endif // IF_MOTIF
}


#ifdef IF_MOTIF
static void drawSelectFrames(Widget w, 
			     const BoxRegion& r0, 
			     const BoxRegion& r1)
#else // NOT IF_MOTIF
void GtkGraphEdit::drawSelectFrames(const BoxRegion& r0, 
				    const BoxRegion& r1)
#endif // IF_MOTIF
{
    // Clear old frame (by redrawing it)
    redrawSelectFrame(M_ARGS_2(w, r0));

    // Draw new frame
    redrawSelectFrame(M_ARGS_2(w, r1));

    // Set appropriate cursor
    setRegionCursor(M_ARGS_1(w));
}


// Draw the selection frame
#ifdef IF_MOTIF
inline void drawSelectFrame(Widget w)
#else // NOT IF_MOTIF
void GtkGraphEdit::drawSelectFrame(void)
#endif // IF_MOTIF
{
    drawSelectFrames(M_ARGS_3(w, frameRegion(M_ARGS_1(w)),
			      BoxRegion(BoxPoint(0, 0), BoxSize(0, 0))));
}


// Redraw selection frame
#ifdef IF_MOTIF
static void redrawSelectFrame(Widget w, const BoxPoint& p)
#else // NOT IF_MOTIF
void GtkGraphEdit::redrawSelectFrame(const BoxPoint& p)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    BoxPoint& endAction      = _w->graphEditP.endAction;
#endif // IF_MOTIF

    BoxRegion r0 = frameRegion(M_ARGS_1(w));
    endAction = p;
    BoxRegion r1 = frameRegion(M_ARGS_1(w));

    drawSelectFrames(M_ARGS_3(w, r0, r1));
}


// Find min possible offset
#ifdef IF_MOTIF
static void getMinimalOffset(Widget w)
#else // NOT IF_MOTIF
void GtkGraphEdit::getMinimalOffset(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->res_.graphEdit.graph;
    const Dimension highlight_thickness = _w->res_.primitive.highlight_thickness;
    const GraphGC& graphGC              = _w->graphEditP.graphGC;
    BoxPoint& minimalOffset             = _w->graphEditP.minimalOffset;
#endif // IF_MOTIF

    const Dimension min_origin = highlight_thickness + 2;

    Boolean found[NDimensions];
    found[X] = False;
    found[Y] = False;

    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
	node = graph->nextVisibleNode(node))
    {
	if (node->selected())
	{
	    BoxRegion r = node->region(graphGC);

	    for (BoxDimension d = X; d <= Y; d++)
	    {
		if (!found[d] || minimalOffset[d] < min_origin - r.origin(d))
		{
		    minimalOffset[d] = min_origin - r.origin(d);
		    found[d] = True;
		}
	    }
	}
    }
}

// Return current offset
#ifdef IF_MOTIF
static BoxPoint actionOffset(Widget w)
#else // NOT IF_MOTIF
BoxPoint GtkGraphEdit::actionOffset(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w      = GraphEditWidget(w);
    const Dimension gridWidth     = _w->res_.graphEdit.gridWidth;
    const Dimension gridHeight    = _w->res_.graphEdit.gridHeight;
    const Boolean snapToGrid      = _w->res_.graphEdit.snapToGrid;
    const BoxPoint& startAction   = _w->graphEditP.startAction;
    const BoxPoint& endAction     = _w->graphEditP.endAction;
    const BoxPoint& minimalOffset = _w->graphEditP.minimalOffset;
#endif // IF_MOTIF

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
#ifdef IF_MOTIF
static void drawOutlines(Widget w, const BoxPoint& offset)
#else // NOT IF_MOTIF
void GtkGraphEdit::drawOutlines(const BoxPoint& offset)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w            = GraphEditWidget(w);
    const Graph* graph                  = _w->res_.graphEdit.graph;
    const Boolean rubberArrows          = _w->res_.graphEdit.rubberArrows;
    const Boolean rubberAnnotations     = _w->res_.graphEdit.rubberAnnotations;
    const Boolean rubberEdges           = _w->res_.graphEdit.rubberEdges;
    const GraphGC& graphGC              = _w->graphEditP.graphGC;
    const GC& outlineGC                 = _w->graphEditP.outlineGC;
#else // NOT IF_MOTIF
    const GC& outlineGC                 = this->outlineGC;
    Gtk::Widget *w = this;
#endif // IF_MOTIF

    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
	node = graph->nextVisibleNode(node))
    {
	if (node->selected())
	{
	    // this should also handle opaqueMove (FIXME)...
	    BoxRegion r = node->region(graphGC);
#ifdef IF_MOTIF
	    XDrawRectangle(XtDisplay(w), XtWindow(w), outlineGC,
		r.origin(X) + offset[X], r.origin(Y) + offset[Y],
		r.space(X), r.space(Y));
#else // NOT IF_MOTIF
	    get_window()->draw_rectangle(outlineGC, false,
					 r.origin(X) + offset[X],
					 r.origin(Y) + offset[Y],
					 r.space(X), r.space(Y));
#endif // IF_MOTIF
	}
    }

    if (rubberEdges)
    {
	GraphGC gc(graphGC);
	gc.edgeGC           = outlineGC;
	gc.offsetIfSelected = offset;
	gc.drawArrowHeads   = rubberArrows;
	gc.drawAnnotations  = (gc.drawAnnotations && rubberAnnotations);

	for (GraphEdge *edge = graph->firstVisibleEdge(); edge != 0;
	    edge = graph->nextVisibleEdge(edge))
	{
	    // if (edge->from()->selected() || edge->to()->selected())
	    edge->draw(w, EVERYWHERE, gc);
	}
    }
}

// Move Node to specified position and call callbacks
#ifdef IF_MOTIF
static void moveTo(Widget w, 
		   GraphNode *node, 
		   const BoxPoint& newPos,
		   Boolean isLast)
#else // NOT IF_MOTIF
void GtkGraphEdit::moveTo(GraphNode *node, 
			  const BoxPoint& newPos,
			  Boolean isLast)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w  = GraphEditWidget(w);
    Graph* graph              = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    if (node->pos() != newPos)
    {
	GraphEditPositionChangedInfo info;

	info.graph        = graph;
	info.node         = node;
	info.old_position = node->pos();
	info.new_position = newPos;
	info.is_last      = isLast;

#ifdef IF_MOTIF
	XtCallCallbacks(w, XtNpositionChangedCallback, XtPointer(&info));
#else // NOT IF_MOTIF
	signal_position_changed().emit(&info);
#endif // IF_MOTIF

	node->moveTo(newPos);
    }
}

// Call ``selection changed'' callbacks
#ifdef IF_MOTIF
static void selectionChanged(Widget w, XEvent *event, Boolean double_click)
#else // NOT IF_MOTIF
void GtkGraphEdit::selectionChanged(XEvent *event, Boolean double_click)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w  = GraphEditWidget(w);
    Graph* graph              = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    GraphEditSelectionChangedInfo info;
    info.graph        = graph;
    info.event        = event;
    info.double_click = double_click;

#ifdef IF_MOTIF
    XtCallCallbacks(w, XtNselectionChangedCallback, XtPointer(&info));
#else // NOT IF_MOTIF
    signal_selection_changed().emit(&info);
#endif // IF_MOTIF
}


// Action functions

// Select all nodes
#ifdef IF_MOTIF
static Boolean _SelectAll(Widget w, XEvent *, String *, Cardinal *)
#else // NOT IF_MOTIF
Boolean GtkGraphEdit::_SelectAll(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    Boolean changed = False;
    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
	node = graph->nextVisibleNode(node))
    {
	if (!node->selected())
	{
	    changed = True;
	    node->selected() = True;
	    graphEditRedrawNode(M_ARGS_2(w, node));
	}
    }

    return changed;
}

#ifdef IF_MOTIF
static void SelectAll(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::SelectAll(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    if (_SelectAll(w, event, params, num_params))
	selectionChanged(w, event, False);
#else // NOT IF_MOTIF
    if (_SelectAll())
	selectionChanged(NULL, False);
#endif // IF_MOTIF
}


// Unselect all nodes
#ifdef IF_MOTIF
static Boolean _UnselectAll(Widget w, XEvent *, String *, Cardinal *)
#else // NOT IF_MOTIF
Boolean GtkGraphEdit::_UnselectAll(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    Boolean changed = False;
    for (GraphNode *node = graph->firstNode(); node != 0;
	node = graph->nextNode(node))
    {
	if (node->selected())
	{
	    changed = True;
	    node->selected() = False;
	    graphEditRedrawNode(M_ARGS_2(w, node));
	}
    }

    return changed;
}

#ifdef IF_MOTIF
static void UnselectAll(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::UnselectAll(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    if (_UnselectAll(w, event, params, num_params))
	selectionChanged(w, event, False);
#else // NOT IF_MOTIF
    if (_UnselectAll())
	selectionChanged(NULL, False);
#endif // IF_MOTIF
}

// Find nodes connected to ROOT
static void find_connected_nodes(GraphNode *root, GraphNodePointerArray& nodes)
{
    for (int i = 0; i < nodes.size(); i++)
	if (nodes[i] == root)
	    return;

    // if (!root->hidden()) 
	nodes += root;

    GraphEdge *edge;
    for (edge = root->firstFrom(); edge != 0; edge = root->nextFrom(edge))
	find_connected_nodes(edge->to(), nodes);

    for (edge = root->firstTo(); edge != 0; edge = root->nextTo(edge))
	find_connected_nodes(edge->from(), nodes);
}

// Select an entire subgraph
#ifdef IF_MOTIF
static Boolean select_graph(Widget w, GraphNode *root, Boolean set = True)
#else // NOT IF_MOTIF
Boolean GtkGraphEdit::select_graph(GraphNode *root, Boolean set)
#endif // IF_MOTIF
{
    // Find all connected nodes
    GraphNodePointerArray nodes;
    find_connected_nodes(root, nodes);

    // Select them
    Boolean changed = False;
    for (int i = 0; i < nodes.size(); i++)
    {
	GraphNode *node = nodes[i];
	if (node->selected() != set)
	{
	    node->selected() = set;
	    graphEditRedrawNode(M_ARGS_2(w, node));
	    changed = True;
	}
    }

    return changed;
}

#ifdef IF_MOTIF
inline Boolean unselect_graph(Widget w, GraphNode *root)
#else // NOT IF_MOTIF
Boolean GtkGraphEdit::unselect_graph(GraphNode *root)
#endif // IF_MOTIF
{
    return select_graph(M_ARGS_3(w, root, False));
}

// Raise node NODE such that it is placed on top of all others
#ifdef IF_MOTIF
void graphEditRaiseNode(Widget w, GraphNode *node)
#else // NOT IF_MOTIF
void GtkGraphEdit::graphEditRaiseNode(GraphNode *node)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w = GraphEditWidget(w);
    Graph* graph             = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    // The last node in the list is drawn last (i.e. on top)
    graph->makeNodeLast(node);
}

// Same, but only if the autoRaise resource is set
#ifdef IF_MOTIF
static void raise_node(Widget w, GraphNode *node)
#else // NOT IF_MOTIF
void GtkGraphEdit::raise_node(GraphNode *node)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    Boolean autoRaise        = _w->res_.graphEdit.autoRaise;
#endif // IF_MOTIF

    if (autoRaise)
	graphEditRaiseNode(M_ARGS_2(w, node));
}

// Begin selecting or moving
#ifdef IF_MOTIF
static void _SelectOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params, SelectionMode mode, Boolean follow)
#else // NOT IF_MOTIF
void GtkGraphEdit::_SelectOrMove(XEvent *event,
				 SelectionMode mode, Boolean follow)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);

    Graph* graph             = _w->res_.graphEdit.graph;
    Cursor moveCursor        = _w->res_.graphEdit.moveCursor;

    GraphEditState& state    = _w->graphEditP.state;
    BoxPoint& startAction    = _w->graphEditP.startAction;
    BoxPoint& endAction      = _w->graphEditP.endAction;
    Time& lastSelectTime     = _w->graphEditP.lastSelectTime;
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Get the input focus
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
#ifdef NAG_ME
#warning Get the input focus?
#endif
#endif // IF_MOTIF

    BoxPoint p = point(event);
    startAction = p;
    endAction   = p;

    Time t = time(event);
#ifdef IF_MOTIF
    Boolean double_click = 
	(Time(t - lastSelectTime) <= Time(XtGetMultiClickTime(XtDisplay(w))));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning multi-click time hardwired.
#endif
    Boolean double_click = 
	(Time(t - lastSelectTime) <= Time(200));
#endif // IF_MOTIF
    lastSelectTime = t;

    GraphNode *node = graphEditGetNodeAtPoint(M_ARGS_2(w, p));

    if (mode == SetSelection)
    {
	GraphEditPreSelectionInfo info;
	info.graph        = graph;
	info.node         = node;
	info.event        = event;
	info.doit         = True;
	info.double_click = double_click;

#ifdef IF_MOTIF
	XtCallCallbacks(w, XtNpreSelectionCallback, XtPointer(&info));
#else // NOT IF_MOTIF
	signal_pre_selection().emit(&info);
#endif // IF_MOTIF

	if (!info.doit)
	    return;
    }

    if (node == 0 || node->hidden())
    {
	// On the background
	switch (mode)
	{
	case SetSelection:
#ifdef IF_MOTIF
	    if (double_click)
		SelectAll(w, event, params, num_params);
	    else
		UnselectAll(w, event, params, num_params);
#else // NOT IF_MOTIF
	    if (double_click)
		SelectAll();
	    else
		UnselectAll();
#endif // IF_MOTIF
	    break;

	case ExtendSelection:
	case ToggleSelection:
	    // Nothing to do on background
	    break;
	}

	if (follow)
	{
	    // Not on a node: draw a frame
	    state = SelectState;

	    // start drawing a frame
	    drawSelectFrame(M_ARGS_1(w));
	}
    }
    else
    {
	// On a node
	Boolean changed = False;
	switch (mode)
	{
	case SetSelection:
	    if (!node->selected())
	    {
		// Create new selection
#ifdef IF_MOTIF
		changed = _UnselectAll(w, event, params, num_params);
#else // NOT IF_MOTIF
		changed = _UnselectAll();
#endif // IF_MOTIF
	    }
	    // FALL THROUGH

	case ExtendSelection:
	    if (double_click)
	    {
		// Select all connected nodes
		changed = select_graph(M_ARGS_2(w, node));
	    }
	    else
	    {
		// Select single node
		if (!node->selected())
		{		
		    node->selected() = True;
		    graphEditRedrawNode(M_ARGS_2(w, node));
		    raise_node(M_ARGS_2(w, node));
		    changed = True;
		}
	    }
	    break;

	case ToggleSelection:
	    if (double_click)
	    {
		// Toggle all connected modes
		if (node->selected())
		    changed = select_graph(M_ARGS_2(w, node));
		else
		    changed = unselect_graph(M_ARGS_2(w, node));
	    }
	    else
	    {
		// Toggle single node
		node->selected() = !node->selected();
		graphEditRedrawNode(M_ARGS_2(w, node));
		raise_node(M_ARGS_2(w, node));
		changed = True;
	    }
	    break;
	}

	if (changed)
	    selectionChanged(M_ARGS_3(w, event, double_click));

	if (follow)
	{
	    // Wait for movement
	    state = DeltaState;

	    // Set moving cursor
	    defineCursor(M_ARGS_2(w, moveCursor));
	}
    }
}

#ifdef IF_MOTIF
static void SelectOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::SelectOrMove(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, SetSelection, True);
#else // NOT IF_MOTIF
    _SelectOrMove(event, SetSelection, True);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void ExtendOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::ExtendOrMove(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, ExtendSelection, True);
#else // NOT IF_MOTIF
    _SelectOrMove(event, ExtendSelection, True);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void ToggleOrMove(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::ToggleOrMove(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, ToggleSelection, True);
#else // NOT IF_MOTIF
    _SelectOrMove(event, ToggleSelection, True);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void Select(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::Select(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, SetSelection, False);
#else // NOT IF_MOTIF
    _SelectOrMove(event, SetSelection, False);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void Extend(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::Extend(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, ExtendSelection, False);
#else // NOT IF_MOTIF
    _SelectOrMove(event, ExtendSelection, False);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void Toggle(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::Toggle(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _SelectOrMove(w, event, params, num_params, ToggleSelection, False);
#else // NOT IF_MOTIF
    _SelectOrMove(event, ToggleSelection, False);
#endif // IF_MOTIF
}

// Keep on acting...
#ifdef IF_MOTIF
static void Follow(Widget w, XEvent *event, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::Follow(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w    = GraphEditWidget(w);
    const BoxPoint& startAction = _w->graphEditP.startAction;
    BoxPoint& endAction         = _w->graphEditP.endAction;
    GraphEditState& state       = _w->graphEditP.state;
    BoxPoint& lastOffset        = _w->graphEditP.lastOffset;
    const Dimension moveDelta   = _w->res_.graphEdit.moveDelta;
#endif // IF_MOTIF

    BoxPoint p = point(event);

    switch(state)
    {
	case SelectState:
	    // Draw new select frame
	    redrawSelectFrame(M_ARGS_2(w, p));
	    break;

	case MoveState:
	{
	    // Draw new move frames
	    endAction = p;
	    BoxPoint newOffset = actionOffset(M_ARGS_1(w));
	    if (newOffset != lastOffset)
	    {
		drawOutlines(M_ARGS_2(w, lastOffset));
		drawOutlines(M_ARGS_2(w, lastOffset = newOffset));
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
		getMinimalOffset(M_ARGS_1(w));
		graphEditSizeChanged(M_ARGS_1(w));
		drawOutlines(M_ARGS_2(w, lastOffset = actionOffset(M_ARGS_1(w))));
		state = MoveState;
	    }
	    break;

	default:
	    // Do nothing
	    break;
    }
}

// Now, all is done.

#ifdef IF_MOTIF
static void move_selected_nodes(Widget w, const BoxPoint& offset)
#else // NOT IF_MOTIF
void GtkGraphEdit::move_selected_nodes(const BoxPoint& offset)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEditP.graphGC;
#endif // IF_MOTIF

    if (offset == BoxPoint(0, 0))
	return;

    // Clear graph area
    BoxRegion r = graph->region(graphGC);
#ifdef IF_MOTIF
    XClearArea(XtDisplay(w), XtWindow(w), r.origin(X), r.origin(Y),
	       r.space(X), r.space(Y), False);
#else // NOT IF_MOTIF
    get_window()->clear_area(r.origin(X), r.origin(Y),
			     r.space(X), r.space(Y));
#endif // IF_MOTIF

    // Move selected nodes
    GraphNode *lastNode = 0;
    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (node->selected())
	{
	    if (lastNode)
		moveTo(M_ARGS_4(w, lastNode, lastNode->pos() + offset, False));
	    lastNode = node;
	}
    }
    if (lastNode)
	moveTo(M_ARGS_4(w, lastNode, lastNode->pos() + offset, True));

    // resize widget to graph size and redraw graph
    graphEditSizeChanged(M_ARGS_1(w));
    graphEditRedraw(M_ARGS_1(w));
}

#ifdef IF_MOTIF
static void End(Widget w, XEvent *event, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::End(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEditP.graphGC;
    const BoxPoint& lastOffset = _w->graphEditP.lastOffset;
    Cursor defaultCursor       = _w->res_.graphEdit.defaultCursor;

    BoxPoint& endAction        = _w->graphEditP.endAction;
    GraphEditState& state      = _w->graphEditP.state;
#endif // IF_MOTIF

    Boolean changed = False;

    switch(state)
    {
	case SelectState:
	{
	    drawSelectFrame(M_ARGS_1(w));
	    endAction = point(event);

	    BoxRegion selected = frameRegion(M_ARGS_1(w));
	    Boolean have_unselected_nodes = False;

	    // Find all nodes in frame and select them
	    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
		node = graph->nextVisibleNode(node))
	    {
		if (!node->selected())
		{
		    // Intersection must be non-empty
		    BoxRegion intersection = 
			selected & node->region(graphGC);

		    if (!intersection.isEmpty())
		    {
			have_unselected_nodes = True;
			node->selected() = True;
			graphEditRedrawNode(M_ARGS_2(w, node));
			changed = True;
		    }
		}
	    }

	    if (!have_unselected_nodes)
	    {
		// All selected nodes are already selected - unselect them
		for (GraphNode *node = graph->firstVisibleNode(); node != 0;
		     node = graph->nextVisibleNode(node))
		{
		    if (node->selected())
		    {
			// Intersection must be non-empty
			BoxRegion intersection = 
			    selected & node->region(graphGC);

			if (!intersection.isEmpty())
			{
			    node->selected() = False;
			    graphEditRedrawNode(M_ARGS_2(w, node));
			    changed = True;
			}
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
	    drawOutlines(M_ARGS_2(w, lastOffset));

            // move nodes
	    endAction = point(event);
	    BoxPoint offset = actionOffset(M_ARGS_1(w));
	    move_selected_nodes(M_ARGS_2(w, offset));

	    state = NopState;
	    break;
	}

	default:
	    // Do nothing
	    break;
    }

    if (changed)
	selectionChanged(M_ARGS_3(w, event, False));

    defineCursor(M_ARGS_2(w, defaultCursor));
}

// Key movement action
#ifdef IF_MOTIF
static void MoveSelected(Widget w, XEvent *, String *params, 
			 Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::MoveSelected(XEvent *, string movx, string movy)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w      = GraphEditWidget(w);
    const Dimension gridWidth     = _w->res_.graphEdit.gridWidth;
    const Dimension gridHeight    = _w->res_.graphEdit.gridHeight;
    const BoxPoint& minimalOffset = _w->graphEditP.minimalOffset;
#endif // IF_MOTIF

    BoxPoint grid(gridWidth, gridHeight);

#ifdef IF_MOTIF
    if (num_params == 0 || *num_params != 2)
    {
	std::cerr << "move-selected: usage: move-selected(X, Y)\n";
	return;
    }
#endif // IF_MOTIF

    BoxPoint offset;

    string offset_s[2];
#ifdef IF_MOTIF
    offset_s[X] = params[0];
    offset_s[Y] = params[1];
#else // NOT IF_MOTIF
    offset_s[X] = movx;
    offset_s[Y] = movy;
#endif // IF_MOTIF

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
	    const char *str = s.chars();
	    c = strtol(str, &ptr, 0);
	    if (ptr == str)
	    {
		std::cerr << "move-selected: illegal argument " << str << "\n";
		return;
	    }
	}
    }

    getMinimalOffset(M_ARGS_1(w));
    
    for (d = X; d <= Y; d++)
    {
	// Offset must not move nodes out of area
	if (offset[d] < minimalOffset[d])
	    offset[d] = minimalOffset[d];
    }

    if (offset != BoxPoint(0, 0))
    {
	move_selected_nodes(M_ARGS_2(w, offset));
	graphEditSizeChanged(M_ARGS_1(w));
    }
}


// Select single node
#ifdef IF_MOTIF
static void select_single_node(Widget w, XEvent *event, GraphNode *selectNode)
#else // NOT IF_MOTIF
void GtkGraphEdit::select_single_node(XEvent *event, GraphNode *selectNode)
#endif // IF_MOTIF
{
    if (selectNode == 0)
	return;

#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    Boolean changed = False;

    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (node != selectNode && node->selected())
	{
	    node->selected() = False;
	    changed = True;
	    graphEditRedrawNode(M_ARGS_2(w, node));
	}
    }

    if (!selectNode->selected())
    {
	selectNode->selected() = True;
	changed = True;
	raise_node(M_ARGS_2(w, selectNode));
	graphEditRedrawNode(M_ARGS_2(w, selectNode));
    }

    if (changed)
	selectionChanged(M_ARGS_3(w, event, False));
}

// Select first node
#ifdef IF_MOTIF
static void SelectFirst(Widget w, XEvent *event, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::SelectFirst(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    select_single_node(M_ARGS_3(w, event, graph->firstVisibleNode()));
}

// Select next node
#ifdef IF_MOTIF
static void SelectNext(Widget w, XEvent *event, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::SelectNext(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    GraphNode *selectNode = 0;
    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (node->selected())
	{
	    selectNode = graph->nextVisibleNode(node);
	    break;
	}
    }

    if (selectNode == 0)
	selectNode = graph->firstVisibleNode();

    select_single_node(M_ARGS_3(w, event, selectNode));
}

// Select previous node
#ifdef IF_MOTIF
static void SelectPrev(Widget w, XEvent *event, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::SelectPrev(XEvent *event)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    GraphNode *lastNode = 0;
    GraphNode *selectNode = 0;
    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	if (node->selected())
	    selectNode = lastNode;
	lastNode = node;
    }

    if (selectNode == 0)
	selectNode = lastNode;

    select_single_node(M_ARGS_3(w, event, selectNode));
}

// Return nearest grid position near P
static BoxPoint NearestGridPosition(const BoxPoint& grid, const BoxPoint& p)
{
    BoxPoint pos(p);

    for (BoxDimension d = X; d <= Y; d++)
	if (grid[d] > 0)
	    pos[d] = ((pos[d] + grid[d] / 2) / grid[d]) * grid[d];

    return pos;
}


// Return final position (if snapToGrid is enabled, for example)
#ifdef IF_MOTIF
BoxPoint graphEditFinalPosition(Widget w, const BoxPoint& p)
#else // NOT IF_MOTIF
BoxPoint GtkGraphEdit::final_position(const BoxPoint& p)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    const GraphEditWidget _w   = GraphEditWidget(w);
    const Boolean snapToGrid   = _w->res_.graphEdit.snapToGrid;
    const Dimension gridWidth  = _w->res_.graphEdit.gridWidth;
    const Dimension gridHeight = _w->res_.graphEdit.gridHeight;
#endif // IF_MOTIF

    if (snapToGrid)
    {
	BoxPoint grid(gridWidth, gridHeight);
	return NearestGridPosition(grid, p);
    }
    else
	return p;
}

#ifdef IF_MOTIF
// Snap nodes to grid
static void _SnapToGrid(Widget w, XEvent *, String *params, 
			Cardinal *num_params)
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
    const Dimension gridWidth  = _w->res_.graphEdit.gridWidth;
    const Dimension gridHeight = _w->res_.graphEdit.gridHeight;

    BoxPoint grid(gridWidth, gridHeight);

    if (*num_params >= 1)
	grid[X] = atoi(params[0]);
    if (*num_params >= 2)
	grid[Y] = atoi(params[1]);

    for (GraphNode *node = graph->firstVisibleNode(); node != 0;
	node = graph->nextVisibleNode(node))
    {
	BoxPoint pos = NearestGridPosition(grid, node->pos());
	if (pos != node->pos())
	{
            // Set new node position
	    moveTo(w, node, pos, graph->nextVisibleNode(node) == 0);
	}
    }
}

static void SnapToGrid(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
{
    _SnapToGrid(w, event, params, num_params);
    graphEditRedraw(w);
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No snap-to-grid
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
static int get_new_rotation(Widget w, const _XtString *params, Cardinal *num_params,
			    const _XtString name, const _XtString default_param, 
			    const _XtString extra_args = "")
{
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Cardinal rotation    = _w->res_.graphEdit.rotation;

    string param = "";
    if (num_params && *num_params >= 1)
	param = params[0];
    if (param.empty())
	param = default_param;

    int new_rotation = atoi(param.chars());
    new_rotation = (new_rotation % 360 + 360) % 360;

    if (new_rotation % 90 != 0)
    {
	std::cerr << name << ": usage: " << name << "(" 
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
    const Graph* graph         = _w->res_.graphEdit.graph;
    Cardinal& rotation         = _w->res_.graphEdit.rotation;

    int new_rotation = 
	get_new_rotation(w, CONST_CAST(const _XtString*,params), num_params, "rotate", "+90");
    if (new_rotation < 0)
	return;

    int width  = _w->res_.core.width;
    int height = _w->res_.core.height;

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
    const Boolean autoLayout   = _w->res_.graphEdit.autoLayout;

    _Rotate(w, event, params, num_params);
    if (autoLayout)
    {
	Cardinal zero = 0;
	_Layout(w, event, 0, &zero);
    }
    graphEditRedraw(w);
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning No rotation
#endif
#endif // IF_MOTIF


// Layout nodes

static Graph *layout_graph = 0;
#ifdef IF_MOTIF
static Widget layout_widget = 0;
#else // NOT IF_MOTIF
static GtkGraphEdit *layout_widget = 0;
#endif // IF_MOTIF

static void LayoutNodeCB(const char *node_name, int x, int y)
{
    GraphNode *node = (GraphNode *)strtoul(node_name, 0, 16);
    assert(node != 0);

    node->moveTo(BoxPoint(x, y));
}

static void LayoutHintCB(const char *from_name, const char *to_name, int x, int y)
{
    GraphNode *from = (GraphNode *)strtoul(from_name, 0, 16);
    assert(from != 0);
    GraphNode *to   = (GraphNode *)strtoul(to_name, 0, 16);
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
	    edge->hidden() = True;

	    // ... fetch its annotation ...
	    LineGraphEdge *ge = ptr_cast(LineGraphEdge, edge);
	    EdgeAnnotation *anno = ge->annotation();
	    if (anno != 0)
		anno = anno->dup();

	    // ... and insert an edge hint at the end
	    // of the path between FROM and TO.
	    HintGraphNode *hint = new HintGraphNode(BoxPoint(x, y));
	 
	    *layout_graph += hint;
	    *layout_graph += new LineGraphEdge(edge->from(), hint, anno);
	    *layout_graph += new LineGraphEdge(hint, edge->to());

	    return;
	}
    }
}

static int LayoutCompareCB(const char *name1, const char *name2)
{
    GraphNode *node1 = (GraphNode *)strtoul(name1, 0, 16);
    assert(node1 != 0);
    GraphNode *node2 = (GraphNode *)strtoul(name2, 0, 16);
    assert(node2 != 0);

    GraphEditCompareNodesInfo info;

    info.graph = layout_graph;
    info.node1 = node1;
    info.node2 = node2;

#ifdef IF_MOTIF
    XtCallCallbacks(layout_widget, XtNcompareNodesCallback, XtPointer(&info));
#else // NOT IF_MOTIF
    layout_widget->signal_compare_nodes().emit(&info);
#endif // IF_MOTIF

    return info.result;
}

static string node_name(GraphNode *node)
{
    char buffer[BUFSIZ];
    sprintf(buffer, "0x%lx", (unsigned long) node);
    return string(buffer);
}

static void remove_all_hints(Graph *graph)
{
    // Find all hint nodes
    GraphNodePointerArray hints;

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
	edge->hidden() = False;
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

#ifdef IF_MOTIF
static void _Layout(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::_Layout(LayoutMode mode)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    Graph* graph               = _w->res_.graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEditP.graphGC;
    Cardinal& rotation         = _w->res_.graphEdit.rotation;
    LayoutMode mode            = _w->res_.graphEdit.layoutMode;
    Boolean& autoLayout        = _w->res_.graphEdit.autoLayout;
#endif // IF_MOTIF

    static const char *graph_name = "graph";

#ifdef IF_MOTIF
    if (num_params && *num_params > 0 && params[0][0] != '\0')
    {
	LayoutMode mode_param;
	XrmValue v1, v2;
	v1.addr = XPointer(params[0]);
	v1.size = sizeof(String);
	v2.addr = XPointer(&mode_param);
	v2.size = sizeof(LayoutMode);

	Boolean ok = 
	    XtConvertAndStore(w, XtRString, &v1, XtRLayoutMode, &v2);
	if (ok)
	    mode = mode_param;
    }
#endif // IF_MOTIF

#ifdef IF_MOTIF
    Cardinal new_num_params = 
	(num_params && *num_params > 0 ? *num_params - 1 : 0);
    int new_rotation = 
	get_new_rotation(w, CONST_CAST(const _XtString*,params) - 1, &new_num_params, 
			 "layout", "+0", "MODE, ");
    if (new_rotation < 0)
	return;
#else // NOT IF_MOTIF
    int new_rotation = 0;
#endif // IF_MOTIF

    // Don't get called again while setting values from hooks
    Boolean old_autoLayout = autoLayout;
    autoLayout = False;

    // Call hooks before layouting
    GraphEditLayoutInfo info;
    info.graph    = graph;
    info.mode     = mode;
    info.rotation = new_rotation;
#ifdef IF_MOTIF
    XtCallCallbacks(w, XtNpreLayoutCallback, XtPointer(&info));
#else // NOT IF_MOTIF
    signal_pre_layout().emit(&info);
#endif // IF_MOTIF

    // Remove all hint nodes
    remove_all_hints(graph);

    // Send graph to layouter
    Layout::add_graph(graph_name);

    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
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

	const string name = node_name(node);
	Layout::add_node(graph_name, name.chars());
	Layout::set_node_width(graph_name, name.chars(), width);
	Layout::set_node_height(graph_name, name.chars(), height);
	Layout::set_node_position(graph_name, name.chars(), -1, -1);
    }

    for (GraphEdge *edge = graph->firstVisibleEdge(); 
	 edge != 0;
	 edge = graph->nextVisibleEdge(edge))
    {
        const string s1 = node_name(edge->from());
        const string s2 = node_name(edge->to());
	Layout::add_edge(graph_name, 
			 s1.chars(), s2.chars());
    }

    // Layout the graph
#ifdef IF_MOTIF
    layout_widget = w;
#else // NOT IF_MOTIF
    layout_widget = this;
#endif // IF_MOTIF
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

#ifdef IF_MOTIF
    // ... and re-rotate it.
    std::ostringstream os;
    os << new_rotation;

    const string rotation_s( os );
    Cardinal rotate_num_params = 1;
    const _XtString rotate_params[1];
    rotate_params[0] = rotation_s.chars();

    rotation = 0;
    _Rotate(w, event, CONST_CAST(String*,rotate_params), &rotate_num_params);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning NO rotations yet
#endif
#endif // IF_MOTIF

    // Layout is done
#ifdef IF_MOTIF
    XtCallCallbacks(w, XtNpostLayoutCallback, XtPointer(&info));
#else // NOT IF_MOTIF
    signal_post_layout().emit(&info);
#endif // IF_MOTIF

    autoLayout = old_autoLayout;
}

// DoLayout() should be named Layout(), but this conflicts with the
// `Layout' class on some pre-ARM C++ compilers :-(
#ifdef IF_MOTIF
static void DoLayout(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::DoLayout(LayoutMode mode)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _Layout(w, event, params, num_params);
#else // NOT IF_MOTIF
    _Layout(mode);
#endif // IF_MOTIF
    graphEditRedraw(M_ARGS_1(w));
}


// Normalize graph
#ifdef IF_MOTIF
static void _Normalize(Widget w, XEvent *, String *, Cardinal *)
#else // NOT IF_MOTIF
void GtkGraphEdit::_Normalize()
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w   = GraphEditWidget(w);
    const Graph* graph         = _w->res_.graphEdit.graph;
    const GraphGC& graphGC     = _w->graphEditP.graphGC;
    const Dimension gridHeight = _w->res_.graphEdit.gridHeight;
    const Dimension gridWidth  = _w->res_.graphEdit.gridWidth;
#endif // IF_MOTIF

    BoxRegion r = graph->region(graphGC);

    for (GraphNode *node = graph->firstVisibleNode(); 
	 node != 0;
	 node = graph->nextVisibleNode(node))
    {
	BoxPoint pos = node->pos() - r.origin() 
	    + BoxPoint(gridHeight, gridWidth);

	if (pos != node->pos())
	{
            // set new node position
	    moveTo(M_ARGS_4(w, node, pos, graph->nextVisibleNode(node) == 0));
	}
    }
}

#ifdef IF_MOTIF
static void Normalize(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::Normalize(void)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    _Normalize(w, event, params, num_params);
#else // NOT IF_MOTIF
    _Normalize();
#endif // IF_MOTIF
    graphEditRedraw(M_ARGS_1(w));
}


// Show and hide edges

#ifdef IF_MOTIF
static void considerEdges(Widget w, XEvent *, String *params,
			  Cardinal *num_params, Boolean shallBeHidden)
#else // NOT IF_MOTIF
void GtkGraphEdit::considerEdges(ShowHideMode themode, Boolean shallBeHidden)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    const GraphEditWidget _w = GraphEditWidget(w);
    const Graph* graph       = _w->res_.graphEdit.graph;
#endif // IF_MOTIF

    // get the mode
#ifdef IF_MOTIF
    ShowHideMode themode = NopeShowHideMode;
#endif // IF_MOTIF
    Boolean changedSomething = False;

#ifdef IF_MOTIF
    string p = "any";
    if (*num_params >= 1)
	p = params[0];

    if (p == "from")
	themode = FromShowHideMode;
    else if (p == "to")
	themode = ToShowHideMode;
    else if (p == "any")
	themode = AnyShowHideMode;
    else if (p == "both")
	themode = BothShowHideMode;
    else
	std::cerr << "show-edges: bad mode " << '`' << p << "'" << "\n";
#endif // IF_MOTIF

    for (GraphEdge *edge = graph->firstEdge(); edge != 0;
	edge = graph->nextEdge(edge))
    {
	Boolean set = False;

	switch (themode)
	{
	    // There should be a better way of coding this, but I don't know...

	    case FromShowHideMode:
		set = edge->from()->selected();
		break;

	    case ToShowHideMode:
		set = edge->to()->selected();
		break;

	    case AnyShowHideMode:
		set = edge->to()->selected() || edge->from()->selected();
		break;

	    case BothShowHideMode:
		set = edge->to()->selected() && edge->from()->selected();
		break;

	    case NopeShowHideMode:
		set = False;
		break;
	}

	if (set)
	{
	    if (edge->hidden() != shallBeHidden)
	    {
		changedSomething = True;
		edge->hidden() = shallBeHidden;
	    }
	}
    }

    if (changedSomething)
	graphEditRedraw(M_ARGS_1(w));
}

#ifdef IF_MOTIF
static void ShowEdges(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::ShowEdges(ShowHideMode how)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    considerEdges(w, event, params, num_params, False);
#else // NOT IF_MOTIF
    considerEdges(how, False);
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
static void HideEdges(Widget w, XEvent *event, String *params,
    Cardinal *num_params)
#else // NOT IF_MOTIF
void GtkGraphEdit::HideEdges(ShowHideMode how)
#endif // IF_MOTIF
{
#ifdef IF_MOTIF
    considerEdges(w, event, params, num_params, True);
#else // NOT IF_MOTIF
    considerEdges(how, True);
#endif // IF_MOTIF
}

// **********************************************************************

#ifndef IF_MOTIF

// GtkGraphEdit accessors

Graph *
GtkGraphEdit::get_graph(void)
{
    return graph;
}

bool
GtkGraphEdit::get_show_grid(void)
{
    return showGrid;
}

void
GtkGraphEdit::set_show_grid(bool b)
{
    showGrid = b;
}

bool
GtkGraphEdit::get_snap_to_grid(void)
{
    return snapToGrid;
}

void
GtkGraphEdit::set_snap_to_grid(bool b)
{
    snapToGrid = b;
}

bool
GtkGraphEdit::get_show_hints(void)
{
    return showHints;
}

void
GtkGraphEdit::set_show_hints(bool b)
{
    showHints = b;
}

bool
GtkGraphEdit::get_show_annotations(void)
{
    return showAnnotations;
}

void
GtkGraphEdit::set_show_annotations(bool b)
{
    showAnnotations = b;
}

bool
GtkGraphEdit::get_auto_layout(void)
{
    return autoLayout;
}

void
GtkGraphEdit::set_auto_layout(bool b)
{
    autoLayout = b;
}

LayoutMode
GtkGraphEdit::get_layout_mode(void)
{
    return layoutMode;
}

void
GtkGraphEdit::set_layout_mode(LayoutMode lm)
{
    layoutMode = lm;
}

int
GtkGraphEdit::get_grid_width(void)
{
    return gridWidth;
}

void
GtkGraphEdit::set_grid_width(int i)
{
    gridWidth = i;
}

int
GtkGraphEdit::get_grid_height(void)
{
    return gridHeight;
}

void
GtkGraphEdit::set_grid_height(int i)
{
    gridHeight = i;
}

bool
GtkGraphEdit::get_dashed_lines(void)
{
    return dashedLines;
}

void
GtkGraphEdit::set_dashed_lines(bool b)
{
    dashedLines = b;
}

Cardinal
GtkGraphEdit::get_rotation(void)
{
    return rotation;
}

void
GtkGraphEdit::set_rotation(Cardinal c)
{
    rotation = c;
}

sigc::signal<void, GraphEditPositionChangedInfo *>
GtkGraphEdit::signal_position_changed()
{
    return signal_position_changed_;
}

sigc::signal<void, GraphEditSelectionChangedInfo *>
GtkGraphEdit::signal_selection_changed()
{
    return signal_selection_changed_;
}

sigc::signal<void, GraphEditPreSelectionInfo *>
GtkGraphEdit::signal_pre_selection()
{
    return signal_pre_selection_;
}

sigc::signal<void, GraphEditCompareNodesInfo *>
GtkGraphEdit::signal_compare_nodes()
{
    return signal_compare_nodes_;
}

sigc::signal<void, GraphEditLayoutInfo *>
GtkGraphEdit::signal_pre_layout()
{
    return signal_pre_layout_;
}

sigc::signal<void, GraphEditLayoutInfo *>
GtkGraphEdit::signal_post_layout()
{
    return signal_post_layout_;
}

const GraphGC&
GtkGraphEdit::get_graph_GC(void)
{
    return graphGC;
}

void
GtkGraphEdit::set_graph(Graph *gr)
{
    graph = gr;
}

#endif // IF_MOTIF
