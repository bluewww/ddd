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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "wm.h"

#include "Command.h"
#include "ddd.h"
#include "string-fun.h"
#include "findParent.h"

#if defined(IF_XM)
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <X11/Xutil.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#endif

//-----------------------------------------------------------------------------
// Window Manager Functions
//-----------------------------------------------------------------------------

#if defined(IF_XM)
void wm_set_icon(Display *display, Window shell, Pixmap icon, Pixmap mask)
{
    XWMHints *wm_hints = XAllocWMHints();

    wm_hints->flags       = IconPixmapHint | IconMaskHint;
    wm_hints->icon_pixmap = icon;
    wm_hints->icon_mask   = mask;

    XSetWMHints(display, shell, wm_hints);

    XFree((void *)wm_hints);
}
#else
void wm_set_icon(GUI::RefPtr<GUI::Display> display, GUI::RefPtr<GUI::XWindow> shell,
		 GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask)
{
#ifdef NAG_ME
#warning wm_set_icon not implemented
#endif
}
#endif

#if defined(IF_XM)
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
#else
void wm_set_icon(GUI::Widget *shell, GUI::RefPtr<GUI::Pixmap> icon, GUI::RefPtr<GUI::Pixmap> mask)
{
#ifdef NAG_ME
#warning wm_set_icon not implemented
#endif
}
#endif

#if defined(IF_XM)
void wm_set_name(Display *display, Window shell_window,
		 string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    if (!title.empty()) {
	XStoreName(display, shell_window, title.chars());
    }
    if (!icon.empty())
	XSetIconName(display, shell_window, icon.chars());
}
#else
void wm_set_name(GUI::RefPtr<GUI::Display> display, GUI::RefPtr<GUI::XWindow> shell_window,
		 string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    if (!title.empty()) {
	shell_window->set_title(title.chars());
    }
#ifdef NAG_ME
#warning Set icon?
#endif 
}
#endif

#if defined(IF_XM)
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
#else
void wm_set_name(GUI::Widget *shell, string title, string icon)
{
    strip_space(title);
    strip_space(icon);

    std::cerr << "SET_TITLE\n";
    
#if 0				// This should be done by the shell.
    wm_set_name(XtDisplay(shell), XtWindow(shell), title, icon);
#endif
}
#endif

#if defined(IF_XM)
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
#else
// Wait until W is mapped
void wait_until_mapped(GUI::Widget *w, GUI::Widget *shell)
{
#ifdef NAG_ME
#warning wait_until_mapped not implemented.
#endif
}
#endif

#if defined(IF_XM)
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
#else
void raise_shell(GUI::Widget *w)
{
    if (w == 0 || !w->is_realized())
	return;

    // Place current shell on top
    GUI::Widget *shell = findShellParent(w);
    if (shell != 0 && shell->is_realized())
    {
	shell->show();

#ifdef NAG_ME
#warning XmProcessTraversal not implemented
#endif
    }
}
#endif

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
void manage_and_raise(GUI::Widget *w)
{
    if (w != 0)
	w->show();
}
#endif

#if !defined(IF_XM)
bool
text_copy_from(GUI::Widget *w)
{
    std::cerr << "text_copy_from: not implemented\n";
}

bool
text_cut_from(GUI::Widget *w)
{
    std::cerr << "text_cut_from: not implemented\n";
}
#endif
