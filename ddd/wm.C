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

char wm_rcsid[] = 
    "$Id$";

#include "config.h"

#include "wm.h"

#include "Command.h"
#include "ddd.h"
#include "string-fun.h"
#include "findParent.h"

#ifdef IF_MOTIF
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <X11/Xutil.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#endif // IF_MOTIF

//-----------------------------------------------------------------------------
// Window Manager Functions
//-----------------------------------------------------------------------------
    
void wm_set_icon(DISPLAY_P display, Window shell, Pixmap icon, Pixmap mask)
{
#ifdef IF_MOTIF
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags       = IconPixmapHint | IconMaskHint;
    wm_hints->icon_pixmap = icon;
    wm_hints->icon_mask   = mask;

    XSetWMHints(display, shell, wm_hints);

    XFree((void *)wm_hints);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning wm_set_icon not implemented
#endif
#endif // IF_MOTIF
}

void wm_set_icon(Widget shell, Pixmap icon, Pixmap mask)
{
#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning wm_set_icon not implemented
#endif
#endif // IF_MOTIF
}

void wm_set_name(DISPLAY_P display, Window shell_window,
		 string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    if (!title.empty()) {
#ifdef IF_MOTIF
	XStoreName(display, shell_window, title.chars());
#else // NOT IF_MOTIF
	shell_window->set_title(XMST(title.chars()));
#endif // IF_MOTIF
    }
#ifdef IF_MOTIF
    if (!icon.empty())
	XSetIconName(display, shell_window, icon.chars());
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set icon?
#endif 
#endif // IF_MOTIF
}

void wm_set_name(Widget shell, string title, string icon)
{
    strip_space(title);
    strip_space(icon);

#ifdef IF_MOTIF
    XtVaSetValues(shell,
		  XmNiconName, icon.chars(),
		  XmNtitle,    title.chars(),
		  XtPointer(0));
#else // NOT IF_MOTIF
    std::cerr << "SET_TITLE\n";
#endif // IF_MOTIF
    
#if 0				// This should be done by the shell.
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
#endif
}

// Wait until W is mapped
void wait_until_mapped(Widget w, Widget shell)
{
#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning wait_until_mapped not implemented.
#endif
#endif // IF_MOTIF
}

void raise_shell(Widget w)
{
    if (w == 0 || !XtIsRealized(w))
	return;

    // Place current shell on top
    Widget shell = findShellParent(w);
    if (shell != 0 && XtIsRealized(shell))
    {
#ifdef IF_MOTIF
	XRaiseWindow(XtDisplay(w), XtWindow(shell));
#else // NOT IF_MOTIF
	shell->show();
#endif // IF_MOTIF

#if 0
	wait_until_mapped(w);

	// Get focus
	XSetInputFocus(XtDisplay(w), XtWindow(w), RevertToParent, 
		       XtLastTimestampProcessed(XtDisplay(w)));
#endif

	// Try this one
#ifdef IF_MOTIF
	XmProcessTraversal(w, XmTRAVERSE_CURRENT);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning XmProcessTraversal not implemented
#endif
#endif // IF_MOTIF
    }
}

#if defined(IF_XM)

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

#else

void manage_and_raise1(GUI::Widget *w)
{
    if (w != 0)
	w->show();
}

void manage_and_raise(Widget w)
{
    if (w != 0) {
#if defined(IF_XMMM)
	XtManageChild(w);
#else
	w->show();
#endif
    }
}

#endif

#ifndef IF_MOTIF

bool
text_copy_from(Widget w)
{
    std::cerr << "text_copy_from: not implemented\n";
}

bool
text_cut_from(Widget w)
{
    std::cerr << "text_cut_from: not implemented\n";
}

#endif // IF_MOTIF
