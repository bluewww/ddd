// $Id$ -*- C++ -*-
// DDD window management

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

char windows_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "windows.h"
#include "wm.h"

#include "assert.h"
#include "exectty.h"
#include "cmdtty.h"
#include "AppData.h"
#include "exit.h"
#include "ddd.h"
#include "SourceView.h"
#include "findParent.h"

#include <Xm/Xm.h>
#include <X11/Xutil.h>

//-----------------------------------------------------------------------------
// Window management
//-----------------------------------------------------------------------------

// Shells (only used if separate windows are used)
Widget command_shell;
Widget data_disp_shell;
Widget source_view_shell;

// Command tool
Widget tool_shell;
Widget tool_buttons_w;

// Flags: shell state
enum WindowState { PoppedUp, PoppedDown, Iconic, Transient };
static WindowState command_shell_state     = PoppedDown;
static WindowState data_disp_shell_state   = PoppedDown;
static WindowState source_view_shell_state = PoppedDown;
static WindowState tool_shell_state        = PoppedDown;

// Place command tool in upper right edge of REF
static void recenter_tool_shell(Widget ref);

static void RecenterToolShellCB(XtPointer, XtIntervalId *)
{
    XWindowAttributes attr;
    XGetWindowAttributes(XtDisplay(tool_buttons_w), 
			 XtWindow(tool_buttons_w), &attr);
    if (attr.map_state != IsViewable)
    {
	// Try again in 200 ms
	XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell), 200,
			RecenterToolShellCB, XtPointer(0));
	return;
    }

    recenter_tool_shell(source_view->source());
}

// Popup initial shell
void initial_popup_shell(Widget w)
{
    if (w == 0)
	return;

    // assert(XtIsTopLevelShell(w));

    Widget toplevel = w;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);

    assert(XtIsTopLevelShell(toplevel));

    // Well isn't it iconic - don't you think?
    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    XtVaSetValues(w, 
		  XmNiconic, iconic, 
		  XmNinitialState, iconic ? IconicState : NormalState,
		  NULL);
    WindowState state = iconic ? Iconic : PoppedUp;

    if (w == command_shell)
	command_shell_state     = state;
    else if (w == data_disp_shell)
	data_disp_shell_state   = state;
    else if (w == source_view_shell)
	source_view_shell_state = state;
    else if (w == tool_shell)
	tool_shell_state        = state;

    if (iconic && w != tool_shell)
	XtVaSetValues(w, 
		      XmNgeometry, "+0+0", 
		      XmNx, 0,
		      XmNy, 0,
		      NULL);

    if (w == tool_shell)
    {
	if (iconic)
	{
	    popdown_shell(tool_shell);
	}
	else
	{
	    XtManageChild(tool_buttons_w);
	    XtManageChild(tool_shell);
	    XtPopup(tool_shell, XtGrabNone);
	    XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell), 0,
			    RecenterToolShellCB, XtPointer(0));
	}
    }
    else if (w != toplevel)
    {
	XtPopup(w, XtGrabNone);
    }
}

void popup_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == tool_shell)
	XtManageChild(tool_buttons_w);

    XtPopup(w, XtGrabNone);

    if (w == command_shell)
	command_shell_state        = PoppedUp;
    else if (w == data_disp_shell)
	data_disp_shell_state      = PoppedUp;
    else if (w == source_view_shell)
	source_view_shell_state    = PoppedUp;
    else if (w == tool_shell)
	tool_shell_state           = PoppedUp;

    // Deiconify window
    XMapWindow(XtDisplay(w), XtWindow(w));
    raise_shell(w);
}

void popdown_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == command_shell)
	command_shell_state     = PoppedDown;
    else if (w == data_disp_shell)
	data_disp_shell_state   = PoppedDown;
    else if (w == source_view_shell)
	source_view_shell_state = PoppedDown;
    else if (w == tool_shell)
	tool_shell_state        = PoppedDown;

    XtPopdown(w);
}

void iconify_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == command_shell)
	command_shell_state     = Iconic;
    else if (w == data_disp_shell)
	data_disp_shell_state   = Iconic;
    else if (w == source_view_shell)
	source_view_shell_state = Iconic;
    else if (w == tool_shell)
	tool_shell_state        = Iconic;

    XIconifyWindow(XtDisplay(w), XtWindow(w),
		   XScreenNumberOfScreen(XtScreen(w)));
}

void popup_tty(Widget shell)
{
    if (exec_tty_window())
    {
	// Deiconify window
	XMapWindow(XtDisplay(shell), exec_tty_window());

	// Place window on top
	XWindowChanges changes;
	changes.stack_mode = Above;
	XReconfigureWMWindow(XtDisplay(shell), exec_tty_window(), 
			     XScreenNumberOfScreen(XtScreen(shell)),
			     CWStackMode, &changes);
    }
}

void iconify_tty(Widget shell)
{
    if (exec_tty_window())
    {
	XIconifyWindow(XtDisplay(shell), exec_tty_window(),
		       XScreenNumberOfScreen(XtScreen(shell)));
    }
}


void StructureNotifyEH(Widget w, XtPointer, XEvent *event, Boolean *)
{
    bool synthetic = false;

    if (w == command_shell)
	synthetic = (command_shell_state == Transient);
    else if (w == data_disp_shell)
	synthetic = (data_disp_shell_state == Transient);
    else if (w == source_view_shell)
	synthetic = (source_view_shell_state == Transient);
    else if (w == tool_shell)
	synthetic = (tool_shell_state == Transient);

    // if (synthetic)
    //    clog << "synthetic event: ";

    switch (event->type)
    {
    case MapNotify:
	// clog << XtName(w) << " is mapped\n";

	// Reflect state
	if (w == command_shell)
	    command_shell_state = PoppedUp;
	else if (w == data_disp_shell)
	    data_disp_shell_state = PoppedUp;
	else if (w == source_view_shell)
	    source_view_shell_state = PoppedUp;
	else if (w == tool_shell)
	    tool_shell_state = PoppedUp;
	else
	    return;

	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell))
	    && !(app_data.tool_bar))
	    popup_shell(tool_shell);

	if (!synthetic && app_data.group_iconify)
	{
	    // Map all other windows as well
	    if (command_shell_state == Iconic)
	    {
		popup_shell(command_shell);
		command_shell_state = Transient;
	    }
	    if (data_disp_shell_state == Iconic)
	    {
		popup_shell(data_disp_shell);
		data_disp_shell_state = Transient;
	    }
	    if (source_view_shell_state == Iconic)
	    {
		popup_shell(source_view_shell);
		source_view_shell_state = Transient;
	    }
	    popup_tty(command_shell);
	}
	break;

    case UnmapNotify:
	// clog << XtName(w) << " is unmapped\n";

	// Reflect state
	if (w == command_shell
	    && command_shell_state != Iconic
	    && command_shell_state != PoppedDown)
	    command_shell_state = Iconic;
	else if (w == data_disp_shell
		 && data_disp_shell_state != Iconic
		 && data_disp_shell_state != PoppedDown)
	    data_disp_shell_state = Iconic;
	else if (w == source_view_shell
		 && source_view_shell_state != Iconic
		 && source_view_shell_state != PoppedDown)
	    source_view_shell_state = Iconic;
	else if (w == tool_shell
		 && tool_shell_state != Iconic
		 && tool_shell_state != PoppedDown)
	    tool_shell_state = Iconic;
	else
	    return;

	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell)))
	    popdown_shell(tool_shell);

	if (!synthetic && app_data.group_iconify)
	{
	    // Iconify all other windows as well
	    if (command_shell_state == PoppedUp)
	    {
		iconify_shell(command_shell);
		command_shell_state = Transient;
	    }
	    if (data_disp_shell_state == PoppedUp)
	    {
		iconify_shell(data_disp_shell);
		data_disp_shell_state = Transient;
	    }
	    if (source_view_shell_state == PoppedUp)
	    {
		iconify_shell(source_view_shell);
		source_view_shell_state = Transient;
	    }
	    iconify_tty(command_shell);
	}
	break;

    default:
	// Any other event...
	break;
    }
}


//-----------------------------------------------------------------------------
// Closing shells
//-----------------------------------------------------------------------------

// Return number of running input shells
static int running_shells()
{
    int shells = 0;

    if (command_shell_state != PoppedDown)
	shells++;
    if (source_view_shell_state != PoppedDown)
	shells++;
    if (data_disp_shell_state != PoppedDown)
	shells++;
    if (tty_running())
	shells++;

    return shells;
}

// Generic close callback
void DDDCloseCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    Widget shell = findTopLevelShellParent(w);
    popdown_shell(shell);
}

// Specific close callbacks
void gdbCloseCommandWindowCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }
    popdown_shell(command_shell);
}

void gdbCloseSourceWindowCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }
    popdown_shell(source_view_shell);
}

void gdbCloseDataWindowCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }
    popdown_shell(data_disp_shell);
}

void gdbCloseExecWindowCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }
    kill_exec_tty();
}

void gdbCloseToolWindowCB(Widget, XtPointer, XtPointer)
{
    popdown_shell(tool_shell);
}


//-----------------------------------------------------------------------------
// Opening shells
//-----------------------------------------------------------------------------

void gdbOpenCommandWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(command_shell);
}

void gdbOpenSourceWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(source_view_shell);
}

void gdbOpenDataWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(data_disp_shell);
}

void gdbOpenExecWindowCB(Widget, XtPointer, XtPointer)
{
    if (exec_tty_pid() == 0)
	startup_exec_tty();
    popup_tty(command_shell);
}

void gdbOpenToolWindowCB(Widget, XtPointer, XtPointer)
{
    popup_shell(tool_shell);
    wait_until_mapped(tool_shell);
    recenter_tool_shell(source_view->source());
}


//-----------------------------------------------------------------------------
// Command tool placement
//-----------------------------------------------------------------------------

// Find the WM frame surrounding WINDOW
static Window frame_window(Window window)
{
    // Find WM frame surrounding tool shell
    Window w = window;
    for (;;)
    {
	Window root;
	Window parent;
	Window *children;
	unsigned int nchildren;
	Status ok = XQueryTree(XtDisplay(tool_shell), w, 
			       &root, &parent, &children, &nchildren);
	XFree(children);

	if (!ok)
	    break;

	if (parent == root)
	    return w;		// Got it

	w = parent;
    }

    return window;		// Not found
}

// Place command tool in upper right edge of REF
static void recenter_tool_shell(Widget ref)
{
    const int offset = 8;	// Distance from REF edge

    if (ref == 0 || tool_shell == 0)
	return;

    Window ref_window  = XtWindow(ref);
    Window tool_window = XtWindow(tool_shell);
    Window tool_frame  = frame_window(tool_window);

    // Get location of upper right edge of REF
    XWindowAttributes ref_attributes;
    XGetWindowAttributes(XtDisplay(ref), ref_window, &ref_attributes);

    // Get tool shell attributes
    XWindowAttributes tool_attributes;
    XGetWindowAttributes(XtDisplay(tool_shell), tool_window, &tool_attributes);

    // Get tool frame attributes
    XWindowAttributes frame_attributes;
    XGetWindowAttributes(XtDisplay(tool_shell), tool_frame, 
			 &frame_attributes);

    // Determine new position relative to REF
    int x = ref_attributes.width - tool_attributes.width - offset;
    int y = offset;

    // Correct them relative to frame thickness
    int frame_x, frame_y;
    Window frame_child;
    XTranslateCoordinates(XtDisplay(ref), tool_window,
			  tool_frame,
			  tool_attributes.width, 0, &frame_x, &frame_y,
			  &frame_child);

    x -= frame_attributes.width - frame_x + frame_attributes.border_width;
    y += frame_y + frame_attributes.border_width;

    // Get root coordinates
    int root_x, root_y;
    Window ref_child;
    XTranslateCoordinates(XtDisplay(ref), ref_window, 
			  ref_attributes.root,
			  x, y, &root_x, &root_y,
			  &ref_child);

    // Move tool shell to ROOT_X, ROOT_Y
    XtVaSetValues(tool_shell,
		  XmNx, root_x,
		  XmNy, root_y,
		  NULL);
}
