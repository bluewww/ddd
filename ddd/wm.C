// $Id$ -*- C++ -*-
// DDD window manager functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char wm_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wm.h"

#include "Command.h"
#include "ddd.h"
#include "string-fun.h"
#include "findParent.h"

#include <Xm/Xm.h>
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
    XtVaSetValues(shell,
		  XmNiconPixmap, icon,
		  XmNiconMask, mask,
		  NULL);

#if 0				// This should be done by the shell.
    wm_set_icon(XtDisplay(shell), XtWindow(shell), icon, mask);
#endif
}

void wm_set_name(Display *display, Window shell_window,
		 string title, string icon)
{
    strip_final_blanks(title);
    strip_final_blanks(icon);

    if (title != "")
	XStoreName(display, shell_window, (String)title);
    if (icon != "")
	XSetIconName(display, shell_window, (String)icon);
}

void wm_set_name(Widget shell, string title, string icon)
{
    strip_final_blanks(title);
    strip_final_blanks(icon);

    XtVaSetValues(shell,
		  XmNiconName, (char *)icon,
		  XmNtitle,    (char *)title,
		  NULL);
    
#if 0				// This should be done by the shell.
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
#endif
}

// Wait until W is mapped
void wait_until_mapped(Widget w, Widget shell)
{
    XSync(XtDisplay(w), false);
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

    XSync(XtDisplay(w), false);
    XmUpdateDisplay(w);
}

void raise_shell(Widget w)
{
    if (w == 0)
	return;

    // Place current shell on top
    Widget shell = findShellParent(w);
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

void manage_and_raise(Widget w)
{
    if (w != 0)
    {
	// If shell is withdrawn or iconic, realize dialog as icon
	bool iconic = false;
	Widget shell = find_shell(w);
  	XWindowAttributes attr;
	iconic = (!XtIsRealized(shell)
		  || XGetWindowAttributes(XtDisplay(shell), 
					  XtWindow(shell), &attr)
		  && attr.map_state != IsViewable);

	if (iconic)
	    XtVaSetValues(w, XmNinitialState, IconicState, NULL);

	XtManageChild(w);
	raise_shell(w);
    }
}
