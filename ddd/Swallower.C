// $Id$ -*- C++ -*-
// Swallower Widget - swallow a user application

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
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

char Swallower_rcsid[] = 
    "$Id$";

#include "Swallower.h"
#include "SwallowerP.h"

#include <X11/Xlib.h>
#include "assert.h"
#include "XErrorB.h"
#include "casts.h"

// Resources

static XtResource resources[] = {
#define offset(field) XtOffsetOf(SwallowerRec, swallower.field)
    // {name, class, type, size, offset, default_type, default_addr}
    { XTRESSTR(XtNwindow), XtCWindow, XtRPointer, sizeof(Window),
        offset(window), XtRImmediate, XtPointer(None) },
    { XTRESSTR(XtNwindowGoneCallback), XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(windowGoneProc), XtRCallback, XtPointer(0) },
    { XTRESSTR(XtNwindowCreatedCallback), XtCCallback, XtRCallback, sizeof(XtPointer),
	offset(windowCreatedProc), XtRCallback, XtPointer(0) },
#undef offset
};



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

static void CheckIfWindowHasGone(Widget w, XtPointer, XEvent *, Boolean *);


// Class record initialization

SwallowerClassRec swallowerClassRec = {
  {	 /* core fields */
    /* superclass               */  (WidgetClass) &coreClassRec,
    /* class_name               */  CONST_CAST(char *,"Swallower"),
    /* widget_size              */  sizeof(SwallowerRec),
    /* class_initialize         */  ClassInitialize,
    /* class_part_initialize    */  XtWidgetClassProc(0),
    /* class_inited             */  False,
    /* initialize               */  Initialize,
    /* initialize_hook          */  XtArgsProc(0),
    /* realize                  */  Realize,
    /* actions                  */  XtActionList(0),
    /* num_actions              */  0,
    /* resources                */  resources,
    /* num_resources            */  XtNumber(resources),
    /* xrm_class                */  NULLQUARK,
    /* compress_motion          */  True,
    /* compress_exposure        */  True,
    /* compress_enterleave      */  True,
    /* visible_interest         */  False,
    /* destroy                  */  Destroy,
    /* resize                   */  Resize,
    /* expose                   */  XtExposeProc(0),
    /* set_values               */  SetValues,
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
  {	/* Swallower fields */
    /* extension                */  XtPointer(0)
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

	    // We don't dispatch this event - root has no associated
	    // widget.  Hence, XtWindowToWidget may return 0, which
	    // some toolkits (especially LessTif) don't check for.
	}
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

    if (window != None && (w->core.width <= 1 || w->core.height <= 1))
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

static void WindowHasGone(Widget w, XEvent *event)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    // Our child has gone
    SwallowerInfo info;
    info.window = window;
    info.event  = event;

    // Mark as `gone'
    window = None;

    // No further need to check for events
    XtRemoveEventHandler(w, SubstructureNotifyMask, False, 
			 CheckIfWindowHasGone, XtPointer(w));

    // Call the callbacks
    XtCallCallbacks(w, XtNwindowGoneCallback, XtPointer(&info));
}

static void WindowHasGone(Widget w, const XErrorEvent *event)
{
    WindowHasGone(w, (XEvent *)event);
}

static void CheckIfWindowHasGone(Widget w, XtPointer, XEvent *event, Boolean *)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    if (event->type == ReparentNotify && event->xreparent.window == window ||
	event->type == DestroyNotify && event->xdestroywindow.window == window)
    {
	// Our child has gone
	WindowHasGone(w, event);
    }
}

static void Swallow(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    if (window == None || !XtIsRealized(w))
	return;

    XErrorBlocker blocker(XtDisplay(w));

#if 0
    XSync(XtDisplay(w), False);
    SwallowerCheckEvents();
#endif

    XUnmapWindow(XtDisplay(w), window);

    XResizeWindow(XtDisplay(w), window, w->core.width, w->core.height);
    XSetWindowBorderWidth(XtDisplay(w), window, 0);

    XReparentWindow(XtDisplay(w), window, XtWindow(w), 0, 0);

    XMapWindow(XtDisplay(w), window);

    if (blocker.error_occurred())
	WindowHasGone(w, &blocker.error_event());

    if (window != None)
    {
	// Check for events
	XtAddEventHandler(w, SubstructureNotifyMask, False, 
			  CheckIfWindowHasGone, XtPointer(w));
    }
}

static void Spitout(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    if (window == None || !XtIsRealized(w))
	return;

    XErrorBlocker blocker(XtDisplay(w));

#if 0
    XSync(XtDisplay(w), False);
    SwallowerCheckEvents();
#endif

    Window root = RootWindowOfScreen(XtScreen(w));

    XUnmapWindow(XtDisplay(w), window);

    XReparentWindow(XtDisplay(w), window, root, 0, 0);

    XMapWindow(XtDisplay(w), window);

    // No further need to check for events
    XtRemoveEventHandler(w, SubstructureNotifyMask, False, 
			 CheckIfWindowHasGone, XtPointer(w));

    // Mark window as `unused'
    window = None;
}

static void Realize(Widget w, 
		    XtValueMask *value_mask,
		    XSetWindowAttributes *attributes)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    XErrorBlocker blocker(XtDisplay(w));

    if (window != None && (w->core.width <= 1 || w->core.height <= 1))
    {
	// Query current width or height
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(w), window, &attr);

	XtMakeResizeRequest(w, attr.width, attr.height, 0, 0);
    }

    if (blocker.error_occurred())
	WindowHasGone(w, &blocker.error_event());

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

    if (before->swallower.window != None &&
	before->swallower.window != after->swallower.window)
    {
	Spitout(old);
    }

    if (after->swallower.window != None &&
	before->swallower.window != after->swallower.window)
    {
	Swallow(new_w);
    }

    return False;		// No redisplay (yet)
}

// Resize: resize child as well
static void Resize(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    Window& window = _w->swallower.window;

    if (window != None)
    {
	XErrorBlocker blocker(XtDisplay(w));

	XResizeWindow(XtDisplay(w), window, w->core.width, w->core.height);

	if (blocker.error_occurred())
	    WindowHasGone(w, &blocker.error_event());
    }
}

// Destroy widget
static void Destroy(Widget w)
{
    const SwallowerWidget _w = SwallowerWidget(w);
    const SwallowerWidget next = _w->swallower.next;

#if 0
    // If we still have a swallowed window, spit it out
    Spitout(w);
#endif

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
