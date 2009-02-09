// $Id$ -*- C++ -*-
// Panned Graph Editor

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001, 2004 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char PannedGraphEdit_rcsid[] = 
    "$Id$";

#include "config.h"
#include "misc.h"
#include "PannedGE.h"
#include "BoxPoint.h"
#include "verify.h"
#include "strclass.h"
#include "Graph.h"

#if HAVE_ATHENA && \
    HAVE_X11_XAW_FORM_H && \
    HAVE_X11_XAW_PANNER_H && \
    HAVE_X11_XAW_PORTHOLE_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Panner.h>
#include <X11/Xaw/Porthole.h>
#include "PannedGEP.h"

static void PortholeCB(Widget w, XtPointer client_data, XtPointer call_data);
static void PannerCB(Widget w, XtPointer client_data, XtPointer call_data);


// Method function declarations

extern "C" {
    static void Resize(Widget w);
}

// Resource list

static XtResource resources[] = {
#define offset(field) XtOffsetOf(PannedGraphEditRec, pannedGraphEdit.field)
    // {name, class, type, size, offset, default_type, default_addr}

    { XTRESSTR(XtNminimumPannerWidth), XTRESSTR(XtCMinimumPannerWidth), XtRDimension, 
      sizeof(Dimension), offset(minimumPannerWidth), 
      XtRImmediate, XtPointer(50) },

    { XTRESSTR(XtNminimumPannerHeight), XTRESSTR(XtCMinimumPannerHeight), XtRDimension, 
      sizeof(Dimension), offset(minimumPannerHeight), 
      XtRImmediate, XtPointer(50) },

    { XTRESSTR(XtNmaximumScale), XTRESSTR(XtCMaximumScale), XtRDimension, 
      sizeof(Dimension), offset(maximumScale), 
      XtRImmediate, XtPointer(33) }

#undef offset
};

// Class record initialization

// Use `struct _PannedGraphEditClassRec' here instead of 
// the more appropriate `PannedGraphEditClassRec' due to a bug in GCC 2.96.
// GCC 2.96 message is:
// PannedGE.C: In function `void __static_initialization_and_destruction_0
// (int, int)':
// PannedGE.C:88: non-lvalue in unary `&'

struct _PannedGraphEditClassRec pannedGraphEditClassRec = {
  {	 /* core fields */
    /* superclass               */  portholeWidgetClass,
    /* class_name               */  CONST_CAST(char *,"PannedGraphEdit"),
    /* widget_size              */  sizeof(PannedGraphEditRec),
    /* class_initialize         */  XtProc(XawInitializeWidgetSet),
    /* class_part_initialize    */  XtWidgetClassProc(0),
    /* class_inited             */  False,
    /* initialize               */  XtInitProc(0),
    /* initialize_hook          */  XtArgsProc(0),
    /* realize                  */  XtInheritRealize,
    /* actions                  */  XtActionList(0),
    /* num_actions              */  0,
    /* resources                */  resources,
    /* num_resources            */  XtNumber(resources),
    /* xrm_class                */  NULLQUARK,
    /* compress_motion          */  True,
    /* compress_exposure        */  True,
    /* compress_enterleave      */  True,
    /* visible_interest         */  False,
    /* destroy                  */  XtWidgetProc(0),
    /* resize                   */  Resize,
    /* expose                   */  XtInheritExpose,
    /* set_values               */  XtSetValuesFunc(0),
    /* set_values_hook          */  XtArgsFunc(0),
    /* set_values_almost        */  XtInheritSetValuesAlmost,
    /* get_values_hook          */  XtArgsProc(0),
    /* accept_focus             */  XtAcceptFocusProc(0),
    /* version                  */  XtVersion,
    /* callback_private         */  XtPointer(0),
    /* tm_table                 */  String(0),
    /* query_geometry           */  XtInheritQueryGeometry,
    /* display_accelerator      */  XtInheritDisplayAccelerator,
    /* extension                */  XtPointer(0)
  },
  {     /* Composite fields */
    /* geometry_manager         */  XtInheritGeometryManager,
    /* change_managed           */  XtInheritChangeManaged,
    /* insert_child             */  XtInheritInsertChild,
    /* delete_child             */  XtInheritDeleteChild,
    /* extension                */  XtPointer(0)
  },
  {     /* Porthole fields */
    /* dummy                    */  0
  },
  {	/* PannedGraphEdit fields */
    /* extension                */  XtPointer(0)
  },
};

WidgetClass pannedGraphEditWidgetClass = WidgetClass(&pannedGraphEditClassRec);


extern "C" {
    static void Resize(Widget w)
    {
	XtCheckSubclass(w, pannedGraphEditWidgetClass, "Bad widget class");
	const PannedGraphEditWidget pw = PannedGraphEditWidget(w);

	for (unsigned i = 0; i < pw->composite.num_children; i++)
	{
	    // Resize each graphEdit child
	    Widget child = pw->composite.children[i];
	    if (XtIsSubclass(child, graphEditWidgetClass))
		graphEditSizeChanged(child);
	}

	// Call superclass resize method
	pannedGraphEditClassRec.core_class.superclass->core_class.resize(w);
    }
}


// create a graph editor with panner; return the graph editor widget ID
Widget createPannedGraphEdit(Widget parent, const _XtString name, 
			     ArgList graph_edit_args,
			     Cardinal graph_edit_arg)
{
    Arg args[10];
    int arg = 0;

    string form_name = string(name) + "_form";
    arg = 0;
    XtSetArg(args[arg], ARGSTR(XtNborderWidth),     0); arg++;
    XtSetArg(args[arg], ARGSTR(XtNdefaultDistance), 0); arg++;
    Widget form =
	verify(XtCreateManagedWidget(form_name.chars(), 
				     formWidgetClass, parent, args, arg));

    string panner_name = string(name) + "_panner";
    arg = 0;
    XtSetArg(args[arg], ARGSTR(XtNresize),    False);          arg++;
    XtSetArg(args[arg], ARGSTR(XtNresizable), True);           arg++;
    XtSetArg(args[arg], ARGSTR(XtNbottom),    XawChainBottom); arg++;
    XtSetArg(args[arg], ARGSTR(XtNtop),       XawChainBottom); arg++;
    XtSetArg(args[arg], ARGSTR(XtNleft),      XawChainRight);  arg++;
    XtSetArg(args[arg], ARGSTR(XtNright),     XawChainRight);  arg++;
    Widget panner = 
	verify(XtCreateWidget(panner_name.chars(), 
			      pannerWidgetClass, form, args, arg));

    string porthole_name = string(name) + "_porthole";
    arg = 0;
    XtSetArg(args[arg], ARGSTR(XtNresizable), True);           arg++;
    XtSetArg(args[arg], ARGSTR(XtNbottom),    XawChainBottom); arg++;
    XtSetArg(args[arg], ARGSTR(XtNtop),       XawChainTop);    arg++;
    XtSetArg(args[arg], ARGSTR(XtNleft),      XawChainLeft);   arg++;
    XtSetArg(args[arg], ARGSTR(XtNright),     XawChainRight);  arg++;
    Widget porthole = 
	verify(XtCreateManagedWidget(porthole_name.chars(),
				     pannedGraphEditWidgetClass, 
				     form, args, arg));

    Widget graph_edit = 
	verify(XtCreateManagedWidget(name, graphEditWidgetClass, porthole, 
				     graph_edit_args, graph_edit_arg));
    XtVaSetValues(graph_edit,
		  XtNresizable, True,
		  XtPointer(0));

    // Allow the porthole and the panner to talk to each other
    XtAddCallback(porthole, XtNreportCallback, PortholeCB, XtPointer(panner));
    XtAddCallback(panner, XtNreportCallback, PannerCB, XtPointer(graph_edit));

    // Propagate requested width and height of graph editor to form
    Dimension width, height;
    XtVaGetValues(graph_edit,
		  XtNrequestedWidth, &width,
		  XtNrequestedHeight, &height,
		  XtPointer(0));

    if (width > 0)
	XtVaSetValues(form, XmNwidth, width, XtPointer(0));
    if (height > 0)
	XtVaSetValues(form, XmNheight, height, XtPointer(0));

    return graph_edit;
}

static void PortholeCB(Widget w, 
		       XtPointer client_data, 
		       XtPointer call_data)
{
    // Setup panner after change in porthole

    XtCheckSubclass(w, pannedGraphEditWidgetClass, "Bad widget class");
    const PannedGraphEditWidget pw = PannedGraphEditWidget(w);

    Arg args[10];
    int arg = 0;
    XawPannerReport *report = (XawPannerReport *) call_data;
    Widget panner = Widget(client_data);

    Widget form = XtParent(w);
    Dimension form_width, form_height, form_border_width;
    XtVaGetValues(form,
		  XtNwidth,       &form_width,
		  XtNheight,      &form_height,
		  XtNborderWidth, &form_border_width,
		  XtPointer(0));

    WidgetList children;
    XtVaGetValues(w,
                  XtNchildren, &children,
                  XtPointer(0));
    Widget graph_edit = children[0];

    arg = 0;
    XtSetArg (args[arg], ARGSTR(XtNsliderX), report->slider_x); arg++;
    XtSetArg (args[arg], ARGSTR(XtNsliderY), report->slider_y); arg++;
    if (report->changed != (XawPRSliderX | XawPRSliderY))
    {
	XtSetArg (args[arg], ARGSTR(XtNsliderWidth),  report->slider_width);  arg++;
	XtSetArg (args[arg], ARGSTR(XtNsliderHeight), report->slider_height); arg++;
	XtSetArg (args[arg], ARGSTR(XtNcanvasWidth),  report->canvas_width);  arg++;
	XtSetArg (args[arg], ARGSTR(XtNcanvasHeight), report->canvas_height); arg++;
    }
    XtSetValues (panner, args, arg);

    // std::clog << "Form   size " << BoxPoint(form_width, form_height) << "\n";
    // std::clog << "Canvas size " << BoxPoint(report->canvas_width, 
    //                                         report->canvas_height) << "\n";

    Dimension scale, panner_border_width;
    XtVaGetValues(panner, 
		  XtNdefaultScale, &scale,
		  XtNborderWidth,  &panner_border_width,
		  XtPointer(0));

    // Determine new panner size
    Dimension panner_width  = 
	min(max((report->canvas_width * scale) / 100, 
		pw->pannedGraphEdit.minimumPannerWidth),
	    (form_width * pw->pannedGraphEdit.maximumScale) / 100);

    Dimension panner_height = 
	min(max((report->canvas_height * scale) / 100, 
		pw->pannedGraphEdit.minimumPannerHeight), 
	    (form_height * pw->pannedGraphEdit.maximumScale) / 100);

    // std::clog << "Panner size " << BoxPoint(panner_width, panner_height) << "\n";

    Dimension extra_width;
    Dimension extra_height;
    Graph *graph;
    XtVaGetValues(graph_edit,
		  XtNextraWidth,  &extra_width,
		  XtNextraHeight, &extra_height,
		  XtNgraph,       &graph,
		  XtPointer(0));

    bool need_panner = 
	graph != 0
	&& graph->firstVisibleNode() != 0
	&& (report->canvas_width  - extra_width  >= form_width ||
	    report->canvas_height - extra_height >= form_height);

    if (need_panner && panner_width > 0 && panner_height > 0)
    {
	// Map panner in lower right edge of the form

	Dimension panner_window_width = 
	    panner_width  + panner_border_width * 2;
	Dimension panner_window_height = 
	    panner_height + panner_border_width * 2;

	if (!XtIsManaged(panner))
	    XtManageChild(panner);

	// Add the panner size as extra space to graph editor such
	// that the panner cannot obscure parts of the graph.

	extra_width =  panner_window_width;
	extra_height = panner_window_height;
	XtVaSetValues(graph_edit,
		      XtNextraWidth,  extra_width,
		      XtNextraHeight, extra_height,
		      XtPointer(0));

	int panner_x = form_width  - form_border_width - panner_window_width;
	int panner_y = form_height - form_border_width - panner_window_height;

	XtConfigureWidget(panner,
			  panner_x, panner_y, 
			  panner_width, panner_height,
			  panner_border_width);
    }
    else
    {
	// Unmap panner
	XtUnmanageChild(panner);
	XtVaSetValues(graph_edit,
		      XtNextraWidth,  0,
		      XtNextraHeight, 0,
		      XtPointer(0));
    }
}

static void PannerCB(Widget /* panner */,
		     XtPointer client_data, 
		     XtPointer call_data)
{
    // Setup porthole after change in panner

    Arg args[2];
    int arg = 0;
    XawPannerReport *report = (XawPannerReport *)call_data;
    Widget graph_edit = Widget(client_data);

    XtSetArg (args[arg], XtNx, -report->slider_x); arg++;
    XtSetArg (args[arg], XtNy, -report->slider_y); arg++;
    XtSetValues(graph_edit, args, arg);
}

// For a given graph editor W, return its panner
Widget pannerOfGraphEdit(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    Widget parent = w;

    while (parent != 0)
    {
	if (XtIsSubclass(parent, formWidgetClass))
	    break;
	parent = XtParent(parent);
    }
    return parent;
}

#else // No Athena 

#include "ScrolledGE.h"

Widget createPannedGraphEdit(Widget parent, const _XtString name, 
			     ArgList graph_edit_args,
			     Cardinal graph_edit_arg)
{
    static bool warned = false;
    if (!warned)
    {
	std::cerr << "Warning: panned graph editors are not supported "
	  "in this configuration.\n";
	std::cerr << "Using scrolled graph editors instead.\n";
	warned = true;
    }

    return createScrolledGraphEdit(parent, name,
				   graph_edit_args, graph_edit_arg);
}

// For a given graph editor W, return its panner
Widget pannerOfGraphEdit(Widget w)
{
    return scrollerOfGraphEdit(w);
}

#endif
