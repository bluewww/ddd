// $Id$ -*- C++ -*-
// DDD window management

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char windows_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "windows.h"

#include "assert.h"
#include "exectty.h"
#include "AppData.h"
#include "exit.h"

#include <Xm/Xm.h>

//-----------------------------------------------------------------------------
// Window management
//-----------------------------------------------------------------------------

// Shells (only used if separate windows are used)
Widget command_shell;
Widget data_disp_shell;
Widget source_view_shell;

// Flags: shell state
enum WindowState { PoppedUp, PoppedDown, Iconic, Transient };
static WindowState command_shell_state     = PoppedDown;
static WindowState data_disp_shell_state   = PoppedDown;
static WindowState source_view_shell_state = PoppedDown;

void initial_popup_shell(Widget w)
{
    if (w == 0)
	return;

    assert(XtIsTopLevelShell(w));

    Widget toplevel = w;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);

    assert(XtIsTopLevelShell(toplevel));

    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, NULL);
    XtVaSetValues(w, XmNiconic, iconic, NULL);
    WindowState state = iconic ? Iconic : PoppedUp;

    if (w == command_shell)
	command_shell_state        = state;
    else if (w == data_disp_shell)
	data_disp_shell_state      = state;
    else if (w == source_view_shell)
	source_view_shell_state    = state;

    XtPopup(w, XtGrabNone);
}

void popup_shell(Widget w)
{
    if (w == 0)
	return;

    XtPopup(w, XtGrabNone);

    if (w == command_shell)
	command_shell_state        = PoppedUp;
    else if (w == data_disp_shell)
	data_disp_shell_state      = PoppedUp;
    else if (w == source_view_shell)
	source_view_shell_state    = PoppedUp;

    // Deiconify window
    XMapWindow(XtDisplay(w), XtWindow(w));

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

    // if (synthetic)
    //    clog << "synthetic event: ";

    switch (event->type)
    {
    case MapNotify:
	// clog << XtName(w) << " is mapped\n";

	// Reflect state
	if (w == command_shell && command_shell_state != PoppedUp)
	    command_shell_state = PoppedUp;
	else if (w == data_disp_shell && data_disp_shell_state != PoppedUp)
	    data_disp_shell_state = PoppedUp;
	else if (w == source_view_shell && source_view_shell_state != PoppedUp)
	    source_view_shell_state = PoppedUp;
	else
	    return;

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
	else
	    return;

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


void gdbCloseCommandWindowCB(Widget w, 
			    XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(command_shell);
}

void gdbCloseSourceWindowCB(Widget w, 
			   XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(source_view_shell);
}

void gdbCloseDataWindowCB(Widget w,
			 XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    popdown_shell(data_disp_shell);
}

void gdbCloseExecWindowCB(Widget w,
			 XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, client_data, call_data);
	return;
    }
    kill_exec_tty();
}


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


//-----------------------------------------------------------------------------
// Shell counter
//-----------------------------------------------------------------------------

int running_shells()
{
    return int(command_shell_state != PoppedDown)
	+ int(source_view_shell_state != PoppedDown)
	+ int(data_disp_shell_state != PoppedDown);
}
