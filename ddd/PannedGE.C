// $Id$ -*- C++ -*-
// Panned Graph Editor

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

char PannedGraphEdit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "misc.h"
#include "PannedGE.h"
#include "BoxPoint.h"
#include "verify.h"

#if defined(HAVE_ATHENA_WIDGETS) \
    && defined(HAVE_X11_XAW_FORM_H) \
    && defined(HAVE_X11_XAW_PANNER_H) \
    && defined(HAVE_X11_XAW_PORTHOLE_H)

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

static void Resize(Widget w);

// Resource list

static XtResource resources[] = {
#define offset(field) XtOffsetOf(PannedGraphEditRec, pannedGraphEdit.field)
    // {name, class, type, size, offset, default_type, default_addr}

    { XtNminimumPannerWidth, XtCMinimumPannerWidth, XtRDimension, 
      sizeof(Dimension), offset(minimumPannerWidth), 
      XtRImmediate, XtPointer(50) },

    { XtNminimumPannerHeight, XtCMinimumPannerHeight, XtRDimension, 
      sizeof(Dimension), offset(minimumPannerHeight), 
      XtRImmediate, XtPointer(50) },

    { XtNmaximumScale, XtCMaximumScale, XtRDimension, 
      sizeof(Dimension), offset(maximumScale), 
      XtRImmediate, XtPointer(33) }

#undef offset
};

// Class record initialization

PannedGraphEditClassRec pannedGraphEditClassRec = {
  {	 /* core fields */
    /* superclass               */  portholeWidgetClass,
    /* class_name               */  "PannedGraphEdit",
    /* widget_size              */  sizeof(PannedGraphEditRec),
    /* class_initialize         */  XawInitializeWidgetSet,
    /* class_part_initialize    */  XtWidgetClassProc(0),
    /* class_inited             */  False,
    /* initialize               */  XtInitProc(0),
    /* initialize_hook          */  XtArgsProc(0),
    /* realize                  */  XtInheritRealize,
    /* actions                  */  NULL,
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
    /* callback_private         */  NULL,
    /* tm_table                 */  NULL,
    /* query_geometry           */  XtInheritQueryGeometry,
    /* display_accelerator      */  XtInheritDisplayAccelerator,
    /* extension                */  NULL,
  },
  {     /* Composite fields */
    /* geometry_manager         */  XtInheritGeometryManager,
    /* change_managed           */  XtInheritChangeManaged,
    /* insert_child             */  XtInheritInsertChild,
    /* delete_child             */  XtInheritDeleteChild,
    /* extension                */  NULL,
  },
  {     /* Porthole fields */
    /* dummy                    */  0,
  },
  {	/* PannedGraphEdit fields */
    /* extension                */  0,
  },
};

WidgetClass pannedGraphEditWidgetClass = WidgetClass(&pannedGraphEditClassRec);


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


// create a graph editor with panner; return the graph editor widget ID
Widget createPannedGraphEdit(Widget parent, String name, 
			     ArgList graph_edit_args,
			     Cardinal graph_edit_arg)
{
    Arg args[10];
    int arg = 0;

    string form_name = string(name) + "_form";
    arg = 0;
    XtSetArg(args[arg], XtNborderWidth,     0); arg++;
    XtSetArg(args[arg], XtNdefaultDistance, 0); arg++;
    Widget form =
	verify(XtCreateManagedWidget(form_name, 
				     formWidgetClass, parent, args, arg));

    string panner_name = string(name) + "_panner";
    arg = 0;
    XtSetArg(args[arg], XtNresize,    False);          arg++;
    XtSetArg(args[arg], XtNresizable, True);           arg++;
    XtSetArg(args[arg], XtNbottom,    XawChainBottom); arg++;
    XtSetArg(args[arg], XtNtop,       XawChainBottom); arg++;
    XtSetArg(args[arg], XtNleft,      XawChainRight);  arg++;
    XtSetArg(args[arg], XtNright,     XawChainRight);  arg++;
    Widget panner = 
	verify(XtCreateManagedWidget(panner_name, 
				     pannerWidgetClass, form, args, arg));

    string porthole_name = string(name) + "_porthole";
    arg = 0;
    Widget porthole = 
	verify(XtCreateManagedWidget(porthole_name,
				     pannedGraphEditWidgetClass, 
				     form, args, arg));

    Widget graph_edit = 
	verify(XtCreateManagedWidget(name, graphEditWidgetClass, porthole, 
				     graph_edit_args, graph_edit_arg));
    XtVaSetValues(graph_edit,
		  XtNresizable, True,
		  NULL);

    // Allow the porthole and the panner to talk to each other
    XtAddCallback(porthole, XtNreportCallback, PortholeCB, XtPointer(panner));
    XtAddCallback(panner, XtNreportCallback, PannerCB, XtPointer(graph_edit));

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
		  NULL);

    WidgetList children;
    XtVaGetValues(w,
                  XtNchildren, &children,
                  NULL);
    Widget graph_edit = children[0];

    arg = 0;
    XtSetArg (args[arg], XtNsliderX, report->slider_x); arg++;
    XtSetArg (args[arg], XtNsliderY, report->slider_y); arg++;
    if (report->changed != (XawPRSliderX | XawPRSliderY))
    {
	XtSetArg (args[arg], XtNsliderWidth,  report->slider_width);  arg++;
	XtSetArg (args[arg], XtNsliderHeight, report->slider_height); arg++;
	XtSetArg (args[arg], XtNcanvasWidth,  report->canvas_width);  arg++;
	XtSetArg (args[arg], XtNcanvasHeight, report->canvas_height); arg++;
    }
    XtSetValues (panner, args, arg);

    // clog << "Form   size " << BoxPoint(form_width, form_height) << "\n";
    // clog << "Canvas size " << BoxPoint(report->canvas_width, 
    // 			       report->canvas_height) << "\n";

    Dimension scale, panner_border_width;
    XtVaGetValues(panner, 
		  XtNdefaultScale, &scale,
		  XtNborderWidth,  &panner_border_width,
		  NULL);

    // Determine new panner size
    Dimension panner_width  = 
	min(max((report->canvas_width * scale) / 100, 
		pw->pannedGraphEdit.minimumPannerWidth),
	    (form_width * pw->pannedGraphEdit.maximumScale) / 100);

    Dimension panner_height = 
	min(max((report->canvas_height * scale) / 100, 
		pw->pannedGraphEdit.minimumPannerHeight), 
	    (form_height * pw->pannedGraphEdit.maximumScale) / 100);

    // clog << "Panner size " << BoxPoint(panner_width, panner_height) << "\n";

    Dimension extra_width;
    Dimension extra_height;
    XtVaGetValues(graph_edit,
		  XtNextraWidth,  &extra_width,
		  XtNextraHeight, &extra_height,
		  NULL);

    bool need_panner = 
	(report->canvas_width  - extra_width  >= form_width || 
	 report->canvas_height - extra_height >= form_height);

    if (need_panner && panner_width > 0 && panner_height > 0)
    {
	// Map panner in lower right edge of the form

	Dimension panner_window_width = 
	    panner_width  + panner_border_width  * 2;
	Dimension panner_window_height = 
	    panner_height + panner_border_width  * 2;

	if (!XtIsManaged(panner))
	    XtManageChild(panner);

	// Add the panner size as extra space to graph editor such
	// that the panner cannot obscure parts of the graph.

	extra_width =  panner_window_width;
	extra_height = panner_window_height;
	XtVaSetValues(graph_edit,
		      XtNextraWidth,  extra_width,
		      XtNextraHeight, extra_height,
		      NULL);

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
		      NULL);
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



#else // No Athena 

#include "ScrolledGE.h"

Widget createPannedGraphEdit(Widget parent, String name, 
			     ArgList graph_edit_args,
			     Cardinal graph_edit_arg)
{
    static bool warned = false;
    if (!warned)
    {
	cerr << "Warning: panned graph editors are not supported "
	    "in this configuration.\n";
	cerr << "Using scrolled graph editors instead.\n";
	warned = true;
    }

    return createScrolledGraphEdit(parent, name,
				   graph_edit_args, graph_edit_arg);
}

#endif
