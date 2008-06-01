// $Id$  -*- C++ -*-
// Scrolled Graph Editor

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char ScrolledGraphEdit_rcsid[] = 
    "$Id$";

#include <iostream>
#include "ScrolleGEP.h"
#include "GraphEdit.h"
#include "verify.h"
#include "strclass.h"

// We have no special class for scrolling a graph editor, but use the
// Motif ScrolledWindow class instead.

WidgetClass scrolledGraphEditWidgetClass = xmScrolledWindowWidgetClass;

static void ResizeEH(Widget, XtPointer client_data, XEvent *, Boolean *)
{
    Widget graphEdit = Widget(client_data);
    graphEditSizeChanged(graphEdit);
}

Widget createScrolledGraphEdit(Widget parent, const _XtString name,
			       ArgList arglist, Cardinal argcount)
{
    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNscrollingPolicy, XmAUTOMATIC); arg++;
    string swindow_name = string(name) + "_swindow";

    XtSetArg(args[arg], XmNborderWidth,     0); arg++;
    XtSetArg(args[arg], XmNspacing,         0); arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;

    Widget scrolledWindow = 
	verify(XtCreateManagedWidget(swindow_name.chars(), 
				     scrolledGraphEditWidgetClass,
				     parent, args, arg));

    Widget graphEdit = 
	verify(XtCreateManagedWidget(name, graphEditWidgetClass,
				     scrolledWindow, arglist, argcount));

    XtAddEventHandler(scrolledWindow, StructureNotifyMask, False,
		      ResizeEH, XtPointer(graphEdit));

    // Propagate requested width and height of graph editor to scrolled window
    Dimension width, height;
    XtVaGetValues(graphEdit, 
		  XtNrequestedWidth, &width,
		  XtNrequestedHeight, &height,
		  XtPointer(0));

    if (width > 0)
	XtVaSetValues(scrolledWindow, XmNwidth, width, XtPointer(0));
    if (height > 0)
	XtVaSetValues(scrolledWindow, XmNheight, height, XtPointer(0));

    return graphEdit;
}

// For a given graph editor W, return its scroller
Widget scrollerOfGraphEdit(Widget w)
{
    XtCheckSubclass(w, GraphEditWidgetClass, "Bad widget class");

    Widget parent = w;
    while (parent != 0)
    {
	if (XtIsSubclass(parent, scrolledGraphEditWidgetClass))
	    break;
	parent = XtParent(parent);
    }
    return parent;
}
