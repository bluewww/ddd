// $Id$ -*- C++ -*-
// DDD window manager functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char wm_rcsid[] = 
    "$Id$";

#include "wm.h"

#include "Command.h"
#include "ddd.h"
#include "string-fun.h"
#include "findParent.h"

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <X11/Xutil.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

//-----------------------------------------------------------------------------
// Window Manager Functions
//-----------------------------------------------------------------------------
    
void wm_set_icon(Display *display, Window shell, Pixmap icon, Pixmap mask)
{
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags       = IconPixmapHint | IconMaskHint;
    wm_hints->icon_pixmap = icon;
    wm_hints->icon_mask   = mask;

    XSetWMHints(display, shell, wm_hints);

    XFree((void *)wm_hints);
}

void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask)
{
    if (XtIsWMShell(shell))
    {
	XtVaSetValues(shell,
		      XmNiconPixmap, icon,
		      XmNiconMask, mask,
		      XtPointer(0));
    }

#if 0				// This should be done by the shell.
    wm_set_icon(XtDisplay(shell), XtWindow(shell), icon, mask);
#endif
}

void wm_set_name(Display *display, Window shell_window,
		 string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    if (!title.empty())
	XStoreName(display, shell_window, title.chars());
    if (!icon.empty())
	XSetIconName(display, shell_window, icon.chars());
}

void wm_set_name(Widget shell, string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    XtVaSetValues(shell,
		  XmNiconName, icon.chars(),
		  XmNtitle,    title.chars(),
		  XtPointer(0));
    
#if 0				// This should be done by the shell.
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
#endif
}

// Wait until W is mapped
void wait_until_mapped(Widget w, Widget shell)
{
    XSync(XtDisplay(w), False);
    XmUpdateDisplay(w);

    if (shell == 0)
	shell = find_shell(w);

    if (XtIsRealized(w) && XtIsRealized(shell))
    {
  	XWindowAttributes attr;
	while (XGetWindowAttributes(XtDisplay(w), XtWindow(w), &attr)
	       && attr.map_state != IsViewable)
	{
	    if (XGetWindowAttributes(XtDisplay(shell), XtWindow(shell), &attr)
		&& attr.map_state != IsViewable)
		break;		// Shell is withdrawn or iconic

	    // Wait for exposure event
	    XEvent event;
	    XtAppNextEvent(XtWidgetToApplicationContext(w), &event);
	    XtDispatchEvent(&event);
	}
    }

    XSync(XtDisplay(w), False);
    XmUpdateDisplay(w);
}

void raise_shell(Widget w)
{
    if (w == 0 || !XtIsRealized(w))
	return;

    // Place current shell on top
    Widget shell = findShellParent(w);
    if (shell != 0 && XtIsRealized(shell))
    {
	XRaiseWindow(XtDisplay(w), XtWindow(shell));

#if 0
	wait_until_mapped(w);

	// Get focus
	XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent, 
		       XtLastTimestampProcessed(XtDisplay(w)));
#endif

	// Try this one
	XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    }
}

void manage_and_raise(Widget w)
{
    if (w != 0)
    {
	// If top-level shell is withdrawn or iconic, realize dialog as icon
	bool iconic = false;
	Widget shell = find_shell(w);
	if (shell != 0)
	{
	    XWindowAttributes attr;
	    iconic = (!XtIsRealized(shell)
		      || XGetWindowAttributes(XtDisplay(shell), 
					      XtWindow(shell), &attr)
		      && attr.map_state != IsViewable);

	    if (iconic)
		XtVaSetValues(w, XmNinitialState, IconicState, XtPointer(0));
	}

	XtManageChild(w);

	shell = w;
	while (shell != 0 && !XtIsShell(shell))
	    shell = XtParent(shell);

	if (shell != 0 && !XmIsDialogShell(shell))
	{
	    if (!XtIsRealized(shell))
		XtRealizeWidget(shell);
	    XtPopup(shell, XtGrabNone);
	}

	raise_shell(w);
    }
}
