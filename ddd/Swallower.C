// $Id$ -*- C++ -*-
// Swallower Widget - swallow a user application

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char Swallower_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Swallower.h"
#include "SwallowerP.h"

#include <X11/Xlib.h>
#include "assert.h"


// Resources

static XtResource resources[] = {
#define offset(field) XtOffsetOf(SwallowerRec, swallower.field)
    // {name, class, type, size, offset, default_type, default_addr}
    { XtNwindow, XtCWindow, XtRPointer, sizeof(Window),
	offset(window), XtRImmediate, XtPointer(None) },
    { XtNwindowGoneCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(windowGoneProc), XtRCallback, XtPointer(0) },
    { XtNwindowCreatedCallback, XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(windowCreatedProc), XtRCallback, XtPointer(0) },
#undef offset
};



// Action function declarations

static XtActionsRec actions[] = {};

static char defaultTranslations[] = "";


// Method function declarations

static void ClassInitialize();

static void Initialize(Widget request, 
		       Widget w, 
		       ArgList args,
		       Cardinal *num_args);

static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes);

static Boolean SetValues(Widget old, 
			 Widget request, 
			 Widget new_w,
			 ArgList args, 
			 Cardinal *num_args);

static void Resize(Widget w);

static void Destroy(Widget w);


// Class record initialization

SwallowerClassRec swallowerClassRec = {
  {	 /* core fields */
    /* superclass               */  (WidgetClass) &coreClassRec,
    /* class_name               */  "Swallower",
    /* widget_size              */  sizeof(SwallowerRec),
    /* class_initialize         */  ClassInitialize,
    /* class_part_initialize    */  NULL,
    /* class_inited             */  False,
    /* initialize               */  Initialize,
    /* initialize_hook          */  NULL,
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
    /* resize                   */  Resize,
    /* expose                   */  NULL,
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
  {	/* Swallower fields */
    /* extension                */ NULL,
  },
};

WidgetClass swallowerWidgetClass = (WidgetClass)&swallowerClassRec;


static void ClassInitialize() {}

static SwallowerWidget all_swallowers = 0;

void SwallowerCheckEvents()
{
    if (all_swallowers == 0)
	return;

    Widget w = Widget(all_swallowers);

    Window root = RootWindowOfScreen(XtScreen(w));

    XEvent event;
    while (XCheckWindowEvent(XtDisplay(w), root, 
			     SubstructureNotifyMask, &event))
    {
	if (event.type == CreateNotify)
	{
	    SwallowerInfo info;
	    info.window = event.xcreatewindow.window;
	    info.event  = &event;

	    SwallowerWidget loop = all_swallowers;
	    while (loop != 0)
	    {
		XtCallCallbacks(Widget(loop), XtNwindowCreatedCallback, 
				XtPointer(&info));
		loop = loop->swallower.next;
	    }
	}

	XtDispatchEvent(&event);
    }
}

static void Initialize(Widget /* request */,
		       Widget w, 
		       ArgList /* args */,
		       Cardinal * /* num_args */)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const Window window = _w->swallower.window;
    SwallowerWidget& next = _w->swallower.next;

    if (window != None && (w->core.width == 0 || w->core.height == 0))
    {
	// Query current width or height
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(w), window, &attr);

	if (w->core.width == 0)
	    w->core.width = attr.width;
	if (w->core.height == 0)
	    w->core.height = attr.height;
    }

    // Get notified if any windows are created
    Window root = RootWindowOfScreen(XtScreen(w));
    XSelectInput(XtDisplay(w), root, SubstructureNotifyMask);

    // Register
    next = all_swallowers;
    all_swallowers = _w;
}

static void CheckIfWindowHasGone(Widget w, XtPointer, XEvent *event, Boolean *)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    if (event->type == ReparentNotify && event->xreparent.window == window ||
	event->type == DestroyNotify && event->xdestroywindow.window == window)
    {
	// Our child has gone
	SwallowerInfo info;
	info.window = window;
	info.event  = event;
	XtCallCallbacks(w, XtNwindowGoneCallback, XtPointer(&info));

	// Mark as `gone'
	window = 0;

	// No further need to check for events
	XtRemoveEventHandler(w, SubstructureNotifyMask, False, 
			     CheckIfWindowHasGone, XtPointer(w));
    }
}

static void Swallow(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const Window window = _w->swallower.window;

    if (window == 0 || !XtIsRealized(w))
	return;

    XUnmapWindow(XtDisplay(w), window);

    XResizeWindow(XtDisplay(w), window, w->core.width, w->core.height);

    XReparentWindow(XtDisplay(w), window, XtWindow(w), 0, 0);

    XMapWindow(XtDisplay(w), window);

    // Check for events
    XtAddEventHandler(w, SubstructureNotifyMask, False, 
		      CheckIfWindowHasGone, XtPointer(w));
}

static void Spitout(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const Window window = _w->swallower.window;

    if (window == 0 || !XtIsRealized(w))
	return;

    Window root = RootWindowOfScreen(XtScreen(w));

    XUnmapWindow(XtDisplay(w), window);

    XReparentWindow(XtDisplay(w), window, root, 0, 0);

    XMapWindow(XtDisplay(w), window);

    // No further need to check for events
    XtRemoveEventHandler(w, SubstructureNotifyMask, False, 
			 CheckIfWindowHasGone, XtPointer(w));
}

static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const Window window = _w->swallower.window;

    if (window != None && (w->core.width == 0 || w->core.height == 0))
    {
	// Query current width or height
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(w), window, &attr);

	XtMakeResizeRequest(w, attr.width, attr.height, 0, 0);
    }

    if (window == None)
    {
	String arg = XtName(w);
	Cardinal nargs = 1;
	XtAppErrorMsg(XtWidgetToApplicationContext(w),
	    "missingWindow", "Swallower Realize",
	    "XtToolkitError",
	    "Widget %s has no window to swallow", &arg, &nargs);
    }

    // Call superclass realize method
    swallowerClassRec.core_class.superclass->
	core_class.realize(w, value_mask, attributes);

    Swallow(w);
}

static Boolean SetValues(Widget old, 
			 Widget /* request */, 
			 Widget new_w,
			 ArgList /* args */, 
			 Cardinal * /* num_args */)
{
    SwallowerWidget before = SwallowerWidget(old);
    SwallowerWidget after  = SwallowerWidget(new_w);

    if (before->swallower.window != after->swallower.window)
    {
	Spitout(old);
	Swallow(new_w);
    }

    return False;		// No redisplay (yet)
}

// Resize: resize child as well
static void Resize(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const Window window = _w->swallower.window;

    XResizeWindow(XtDisplay(w), window, w->core.width, w->core.height);
}

// Destroy widget
static void Destroy(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const SwallowerWidget next = _w->swallower.next;

    // If we still have a swalloed window, spit it out
    Spitout(w);

    // Unregister
    SwallowerWidget loop = all_swallowers;
    SwallowerWidget prev = 0;
    while (loop != 0 && loop != _w)
    {
	prev = loop;
	loop = loop->swallower.next;
    }

    if (loop == 0)
    {
	String arg = XtName(w);
	Cardinal nargs = 1;
	XtAppWarningMsg(XtWidgetToApplicationContext(w),
	    "missingWidget", "Swallower Destroy",
	    "XtToolkitError",
	    "Widget %s is not registered", &arg, &nargs);
    }

    if (loop == _w)
    {
	if (prev == 0)
	{
	    assert(all_swallowers == _w);
	    all_swallowers = next;
	}
	else
	{
	    assert(prev->swallower.next == _w);
	    prev->swallower.next = next;
	}
    }
}
