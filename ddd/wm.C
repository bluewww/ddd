// $Id$ -*- C++ -*-
// DDD window manager functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

const char wm_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "wm.h"

#include "commandQ.h"
#include "ddd.h"
#include "string-fun.h"

#include <Xm/Xm.h>

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

    XFree(wm_hints);
}

void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask)
{
    XtVaSetValues(shell,
		  XmNiconPixmap, icon,
		  XmNiconMask, mask,
		  NULL);
    wm_set_icon(XtDisplay(shell), XtWindow(shell), icon, mask);
}

void wm_set_group_leader(Display * /* display */,
			 Window /* shell_window */, Window /* leader_window */)
{
    // Disabled, since (at least in fvwm) it has no effect but to
    // disable generation of individual icons.
#if 0
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags        = WindowGroupHint;
    wm_hints->window_group = leader_window;

    XSetWMHints(display, shell_window, wm_hints);

    XFree(wm_hints);
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
    
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
}

// Wait until W is mapped
void wait_until_mapped(Widget w)
{
    XSync(XtDisplay(w), false);
    XmUpdateDisplay(w);

    Widget shell = find_shell(w);

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

    // Place window on top
    XWindowChanges changes;
    changes.stack_mode = Above;
    XReconfigureWMWindow(XtDisplay(w), XtWindow(w), 
			 XScreenNumberOfScreen(XtScreen(w)),
			 CWStackMode, &changes);

#if 0
    wait_until_mapped(w);

    // Get focus
    XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent, 
		   XtLastTimestampProcessed(XtDisplay(w)));
#endif

    // Try this one
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}
