// $Id$  -*- C++ -*-
// Scrolled Graph Editor

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

char ScrolledGraphEdit_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream.h>
#include "ScrolleGEP.h"
#include "GraphEdit.h"

#define EVERYWHERE (BoxRegion(BoxPoint(0,0), BoxSize(MAXINT, MAXINT)))


// Method function declarations

static void Resize(Widget w);


// Class record initialization

ScrolledGraphEditClassRec scrolledGraphEditClassRec = {
  {	 /* core fields */
    /* superclass               */  xmScrolledWindowWidgetClass,
    /* class_name               */  "ScrolledGraphEdit",
    /* widget_size              */  sizeof(ScrolledGraphEditRec),
    /* class_initialize         */  XtProc(0),
    /* class_part_initialize    */  XtWidgetClassProc(0),
    /* class_inited             */  False,
    /* initialize               */  XtInitProc(0),
    /* initialize_hook          */  XtArgsProc(0),
    /* realize                  */  XtInheritRealize,
    /* actions                  */  NULL,
    /* num_actions              */  0,
    /* resources                */  NULL,
    /* num_resources            */  0,
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
  {     /* Constraint fields */
    /* resources                */  NULL,
    /* num_resources            */  0,
    /* constraint_size          */  0,
    /* initialize               */  XtInitProc(0),
    /* destroy                  */  XtWidgetProc(0),
    /* set_values               */  XtSetValuesFunc(0),
    /* extension                */  NULL,
  },
  {     /* Manager fields */
    /* translations             */  XtInheritTranslations,
    /* syn_resources            */  NULL,
    /* num_syn_resources        */  0,
    /* syn_constraint_resources */  NULL,
    /* num_syn_constraint_res   */  0,
    /* parent_process           */  XmInheritParentProcess,
    /* extension                */  NULL,
  },
  {     /* Scrolled window fields */
    /* mumble                   */  0,
  },
  {	/* ScrolledGraphEdit fields */
    /* extension                */  0,
  },
};

WidgetClass scrolledGraphEditWidgetClass = 
    (WidgetClass)&scrolledGraphEditClassRec;


static void Resize(Widget w)
{
    // resize child if it's a graphEdit child
    Widget child = ScrolledGraphEditWidget(w)->swindow.WorkWindow;
    if (XtIsSubclass(child, graphEditWidgetClass))
	graphEditSizeChanged(child);

    // call superclass resize method
    scrolledGraphEditClassRec.core_class.superclass->core_class.resize(w);
}


Widget createScrolledGraphEdit(Widget parent, String name,
			       ArgList arglist, Cardinal argcount)
{
    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNscrollingPolicy, XmAUTOMATIC); arg++;
    string swindow_name = string(name) + "_swindow";

#if XmVersion >= 2000
    // In Motif 2.0 or later, the definition above crashes.
    // Use a standard scrolled window instead.
    Widget scrolledWindow = 
	XtCreateManagedWidget((char *)swindow_name, 
			      xmScrolledWindowWidgetClass,
			      parent, args, arg);
#else
    Widget scrolledWindow = 
	XtCreateManagedWidget((char *)swindow_name, 
			      scrolledGraphEditWidgetClass,
			      parent, args, arg);
#endif

    return XtCreateManagedWidget(name, graphEditWidgetClass,
				 scrolledWindow, arglist, argcount);
}
