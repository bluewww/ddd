// $Id$ -*- C++ -*-
// DDD window management

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

char windows_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#define LOG_GEOMETRY 0
#define LOG_EVENTS   0

#include "windows.h"

#include "AppData.h"
#include "BoxRegion.h"
#include "DataDisp.h"
#include "SourceView.h"
#include "TimeOut.h"
#include "assert.h"
#include "cmdtty.h"
#include "ddd.h"
#include "editing.h"
#include "exectty.h"
#include "exit.h"
#include "findParent.h"
#include "frame.h"
#include "wm.h"
#include "MinMaxA.h"

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/MainW.h>
#include <Xm/ScrollBar.h>
#if XmVersion >= 1002
#include <Xm/VendorS.h>		// XmIsMotifWMRunning()
#else
#include <Xm/VendorE.h>		// XmIsMotifWMRunning()
#endif
#include <X11/Xutil.h>

#include "Sash.h"		// XmIsSash()
#include <Xm/Separator.h>	// XmIsSeparator()
#include <Xm/SeparatoG.h>	// XmIsSeparatorGadget()

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

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

// Flags: shell visibility
static int command_shell_visibility     = VisibilityFullyObscured;
static int data_disp_shell_visibility   = VisibilityFullyObscured;
static int source_view_shell_visibility = VisibilityFullyObscured;
static int tool_shell_visibility        = VisibilityFullyObscured;

// Place command tool in upper right edge of REF
static void recenter_tool_shell(Widget ref = 0);

// Place command tool in upper right edge of REF, with a distance of
// TOP_OFFSET and RIGHT_OFFSET
static void recenter_tool_shell(Widget ref, int top_offset, int right_offset);

// Have command tool follow REF
static void follow_tool_shell(Widget ref = 0);

// Get current offset of command tool in TOP_OFFSET and RIGHT_OFFSET;
// return true iff successful.
static bool get_tool_offset(Widget ref, int& top_offset, int& right_offset);

// Last offsets as actually used
static int last_top_offset;
static int last_right_offset;
static bool offsets_initialized = false;

// Last saved geometry of tool shell
static string last_tool_shell_geometry = "+0+0";

static void initialize_offsets()
{
    if (!offsets_initialized)
    {
	last_top_offset   = app_data.tool_top_offset;
	last_right_offset = app_data.tool_right_offset;
	offsets_initialized = true;
    }
}

// Return current tool shell position relative to root window
static BoxPoint tool_shell_pos()
{
    int root_x = 0;
    int root_y = 0;

    if (tool_shell != 0 && XtIsRealized(tool_shell))
    {
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(tool_shell), XtWindow(tool_shell), 
			     &attr);
	Window child;
	XTranslateCoordinates(XtDisplay(tool_shell), XtWindow(tool_shell), 
			      attr.root, 0, 0, &root_x, &root_y, &child);
    }

    return BoxPoint(root_x, root_y);
}

// Move tool shell to POS
static void move_tool_shell(BoxPoint pos)
{
#if 0
    // Make sure we don't move the tool shell off the screen
    pos[X] = max(pos[X], 0);
    pos[Y] = max(pos[Y], 0);
#endif

    if (tool_shell == 0)
	return;

    if (pos != tool_shell_pos())
    {
	ostrstream os;
	os << "+" << pos[X] << "+" << pos[Y];
	last_tool_shell_geometry = string(os);

	// Move tool shell to POS
	XtVaSetValues(tool_shell,
		      XmNgeometry, last_tool_shell_geometry.chars(),
		      XmNx, pos[X],
		      XmNy, pos[Y],
		      NULL);
    }
}

static void RecenterToolShellCB(XtPointer = 0, XtIntervalId *id = 0)
{
    if (tool_shell == 0)
	return;

    static XtIntervalId recenter_tool_shell_timer = 0;

    if (id != 0)
    {
	assert(*id = recenter_tool_shell_timer);
	recenter_tool_shell_timer = 0;
    }
    else if (recenter_tool_shell_timer != 0)
    {
	XtRemoveTimeOut(recenter_tool_shell_timer);
	recenter_tool_shell_timer = 0;
    }

    bool have_visible_tool_shell = false;

    if (XtIsRealized(tool_buttons_w))
    {
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(tool_buttons_w), 
			     XtWindow(tool_buttons_w), &attr);
	have_visible_tool_shell = (attr.map_state == IsViewable);
    }

    if (have_visible_tool_shell)
    {
	recenter_tool_shell();
    }
    else
    {
	// Try again in 200 ms
	recenter_tool_shell_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell), 
			    200, RecenterToolShellCB, XtPointer(0));
    }
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

    if (iconic || w == tool_shell)
	XtVaSetValues(w, 
		      XmNgeometry, "+0+0", 
		      XmNx, 0,
		      XmNy, 0,
		      NULL);

    if (w == tool_shell)
    {
	XtManageChild(tool_buttons_w);
	if (!XtIsRealized(tool_shell))
	    XtRealizeWidget(tool_shell);
	if (!iconic)
	    RecenterToolShellCB();
    }
    if (w != toplevel && XtIsRealized(w))
	XtPopup(w, XtGrabNone);
}

void popup_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == tool_shell)
    {
	if (!XtIsRealized(tool_shell))
	{
	    initial_popup_shell(tool_shell);
	    RecenterToolShellCB();
	}

	XtManageChild(tool_buttons_w);
    }

    if (XtIsRealized(w))
	XtPopup(w, XtGrabNone);

    if (w == command_shell)
	command_shell_state        = PoppedUp;
    else if (w == data_disp_shell)
	data_disp_shell_state      = PoppedUp;
    else if (w == source_view_shell)
	source_view_shell_state    = PoppedUp;
    else if (w == tool_shell)
	tool_shell_state           = PoppedUp;

    // Uniconify window
    if (XtIsRealized(w))
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

    if (w == tool_shell)
	XtUnmanageChild(tool_buttons_w);

    XtPopdown(w);
}

void iconify_shell(Widget w)
{
    if (w == 0 || !XtIsRealized(w))
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

void uniconify_shell(Widget w)
{
    if (w == 0)
	return;

    if (w == command_shell && command_shell_state == Iconic
	|| w == data_disp_shell && data_disp_shell_state == Iconic
	|| w == source_view_shell && source_view_shell_state == Iconic
	|| w == tool_shell && tool_shell_state == Iconic)
    {
	popup_shell(w);
    }
}

void popup_tty(Widget shell)
{
    if (exec_tty_window())
    {
	// Uniconify window
	XMapWindow(XtDisplay(shell), exec_tty_window());

	// Place window on top
	XRaiseWindow(XtDisplay(shell), exec_tty_window());
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

static int visibility(Widget w)
{
    if (w == command_shell)
	return command_shell_visibility;
    else if (w == data_disp_shell)
	return data_disp_shell_visibility;
    else if (w == source_view_shell)
	return source_view_shell_visibility;
    else if (w == tool_shell)
	return tool_shell_visibility;
    else
	return VisibilityFullyObscured;
}

static BoxRegion region(Display *display, Window win)
{
    XWindowAttributes attr;

    Status ok;
    ok = XGetWindowAttributes(display, win, &attr);
    if (!ok)
	return BoxRegion();

    return BoxRegion(BoxPoint(attr.x, attr.y), 
		     BoxSize(attr.width, attr.height));
}

static bool obscures(Display *display, Window top, Window bottom)
{
    if (top == 0 || bottom == 0)
	return false;

    return region(display, bottom) <= region(display, top);
}

static bool obscures(Widget top, Widget bottom)
{
    if (top == 0 || bottom == 0 || !XtIsRealized(top) || !XtIsRealized(bottom))
	return false;

    if (visibility(bottom) == VisibilityUnobscured)
	return false;

    return obscures(XtDisplay(top), XtWindow(top), XtWindow(bottom));
}

// Raise WIN above SIBLING
static void raise_above(Display *display, Window win, Window sibling)
{
    if (win == 0 || sibling == 0)
	return;

    Window win_frame     = frame(display, win);
    Window sibling_frame = frame(display, sibling);

    if (win_frame != 0 && sibling_frame != 0 && win_frame != sibling_frame)
    {
	// Raise WIN just above SIBLING
	XWindowChanges changes;
	changes.stack_mode = Above;
	changes.sibling    = sibling_frame;

	XConfigureWindow(display, win_frame,
			 CWSibling | CWStackMode, &changes);
    }
    else
    {
	// Raise WIN on top
	XRaiseWindow(display, win);
    }
}

inline void raise_tool_above(Window sibling)
{
    if (tool_shell && XtIsRealized(tool_shell))
	raise_above(XtDisplay(tool_shell), XtWindow(tool_shell), sibling);
}

inline void raise_tool_above(Widget w)
{
    if (w != 0 && XtIsRealized(w))
	raise_tool_above(XtWindow(w));
}

static void follow_tool_shell(Widget ref)
{
    initialize_offsets();

    recenter_tool_shell(ref, last_top_offset, last_right_offset);
    get_tool_offset(ref, last_top_offset, last_right_offset);
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

#if LOG_EVENTS
    if (synthetic)
	clog << "synthetic event: ";
#endif

    switch (event->type)
    {
    case MapNotify:
#if LOG_EVENTS
	clog << XtName(w) << " is mapped\n";
#endif

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

#if 0
	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell))
	    && !app_data.command_toolbar && app_data.source_window)
	{
	    // Popup command tool again
	    popup_shell(tool_shell);
	    tool_shell_state = Transient;
	}
#endif

	if (tool_shell != 0)
	{
	    // Check position of command tool
	    BoxPoint max_pos(WidthOfScreen(XtScreen(tool_shell)) - 1,
			     HeightOfScreen(XtScreen(tool_shell)) - 1);
	    if (tool_shell_pos() >= max_pos)
		RecenterToolShellCB();
	}

	if (!synthetic && app_data.group_iconify)
	{
	    // Some shell was mapped - map all other shells as well
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
#if LOG_EVENTS
	clog << XtName(w) << " is unmapped\n";
#endif

	// Reflect state
	if (w == command_shell
	    && command_shell_state != Iconic
	    && command_shell_state != PoppedDown)
	{
	    command_shell_state = Iconic;
	}
	else if (w == data_disp_shell
		 && data_disp_shell_state != Iconic
		 && data_disp_shell_state != PoppedDown)
	{
	    data_disp_shell_state = Iconic;
	}
	else if (w == source_view_shell
		 && source_view_shell_state != Iconic
		 && source_view_shell_state != PoppedDown)
	{
	    source_view_shell_state = Iconic;
	}
	else if (w == tool_shell
		 && tool_shell_state != Iconic
		 && tool_shell_state != PoppedDown)
	{
	    tool_shell_state = Iconic;
	}
	else
	    return;

	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell)))
	{
	    // Iconify command tool, too
	    iconify_shell(tool_shell);
	    tool_shell_state = Transient;
	}

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

    case VisibilityNotify:
    {
	if (w == command_shell)
	    command_shell_visibility = event->xvisibility.state;
	else if (w == data_disp_shell)
	    data_disp_shell_visibility = event->xvisibility.state;
	else if (w == source_view_shell)
	    source_view_shell_visibility = event->xvisibility.state;
	else if (w == tool_shell)
	    tool_shell_visibility = event->xvisibility.state;

	// Check whether command tool is obscured by some DDD shell
	if (app_data.auto_raise_tool
	    && (obscures(command_shell, tool_shell)
		|| obscures(data_disp_shell, tool_shell)
		|| obscures(source_view_shell, tool_shell)))
	{
	    // Command tool is obscured
	    if (XmIsMotifWMRunning(tool_shell) && XmIsDialogShell(tool_shell))
	    {
		// We have MWM and the command tool is a Dialog Shell.
		// Hence, let MWM keep the command tool on top.
	    }
	    else
	    {
		// Raise command tool
		Widget shell = 
		    source_view_shell ? source_view_shell : command_shell;
		raise_tool_above(shell);
	    }
	}

#if 0				// Doesn't work yet - AZ
	// Check whether command tool is obscured by the exec window
	if (app_data.auto_raise_tool
	    && obscures(XtDisplay(tool_shell), exec_tty_window(), 
			XtWindow(tool_shell)))
	    raise_tool_above(exec_tty_window());
#endif

	break;
    }

    case ConfigureNotify:
    {
	if (app_data.sticky_tool
	    && have_tool_window()
	    && (tool_shell_state == PoppedUp 
		|| tool_shell_state == Transient))
	{
	    // Let `sticky' command tool follow the source window
	    initialize_offsets();

	    if (w == tool_shell)
	    {
		// Command tool has been moved
#if LOG_EVENTS
		clog << "Tool has been moved to " << point(event) << "\n";
#endif

		// Record offset
		get_tool_offset(0, last_top_offset, last_right_offset);
	    }

	    if (w == source_view_shell || 
		w == command_shell && source_view_shell == 0)
	    {
		// Source shell has been moved -- let command tool follow
#if LOG_EVENTS
		clog << "Shell has been moved to " << point(event) << "\n";
#endif

		follow_tool_shell();
	    }
	}
	break;
    }

    default:
	// Any other event...
	break;
    }
}


//-----------------------------------------------------------------------------
// Closing shells
//-----------------------------------------------------------------------------

// Return number of running input shells
int running_shells()
{
    int shells = 0;

    if (command_shell_state != PoppedDown)
	shells++;
    if (source_view_shell_state != PoppedDown)
	shells++;
    if (data_disp_shell_state != PoppedDown)
	shells++;

    return shells;
}


// Generic close callback
void DDDCloseCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    Widget shell = findTopLevelShellParent(w);

    if (shell == command_shell)
	gdbCloseCommandWindowCB(w, client_data, call_data);
    else if (shell == data_disp_shell)
	gdbCloseDataWindowCB(w, client_data, call_data);
    else if (shell == source_view_shell)
	gdbCloseSourceWindowCB(w, client_data, call_data);
    else if (shell == tool_shell)
	gdbCloseToolWindowCB(w, client_data, call_data);
    else
	popdown_shell(shell);
}



// Specific close and open callbacks

// Debugger console
void gdbCloseCommandWindowCB(Widget w, XtPointer, XtPointer)
{
    if (!have_data_window() && !have_source_window() && !have_code_window())
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    if ((app_data.separate_source_window || !have_source_window())
	&& (app_data.separate_data_window || !have_data_window()))
    {
	popdown_shell(command_shell);
    }

    unmanage_paned_child(XtParent(gdb_w));

    app_data.debugger_console = false;

    update_options();
}

void gdbOpenCommandWindowCB(Widget, XtPointer, XtPointer)
{
    manage_paned_child(XtParent(gdb_w));

    if (app_data.separate_source_window)
	popup_shell(command_shell);

    app_data.debugger_console = true;

    update_options();
}

bool have_command_window()
{
    return XtIsManaged(XtParent(gdb_w));
}

bool have_visible_command_window()
{
    return have_command_window() 
	&& command_shell_state == PoppedUp
	&& (!app_data.separate_data_window
	    || !app_data.separate_source_window
	    || command_shell_visibility == VisibilityUnobscured);
}


// Source window
void gdbCloseSourceWindowCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    if (!have_command_window() && !have_data_window() && !have_code_window())
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    // Popdown shell
    popdown_shell(source_view_shell);

    // Unmanage source
    unmanage_paned_child(source_view->source_form());

    if (source_view_shell != 0)
	unmanage_paned_child(source_view->code_form());

    if (!XtIsManaged(source_view->code_form()))
	gdbCloseToolWindowCB(w, client_data, call_data);

    app_data.source_window = false;

    update_options();
}

void gdbCloseCodeWindowCB(Widget w, XtPointer client_data, 
			    XtPointer call_data)
{
    if (!have_command_window() && !have_data_window() && !have_source_window())
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    // Unmanage code
    unmanage_paned_child(source_view->code_form());

    if (!XtIsManaged(source_view->source_form()))
	gdbCloseToolWindowCB(w, client_data, call_data);

    app_data.disassemble = false;

    update_options();
}

void gdbOpenSourceWindowCB(Widget w, XtPointer client_data,
			   XtPointer call_data)
{
    manage_paned_child(source_view->source_form());
    if (source_view_shell != 0 && app_data.disassemble)
	manage_paned_child(source_view->code_form());
    Widget arg_cmd_w = XtParent(source_arg->top());
    manage_paned_child(arg_cmd_w);

    popup_shell(source_view_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB(w, client_data, call_data);

    app_data.source_window = true;

    update_options();
}

void gdbOpenCodeWindowCB(Widget w, XtPointer client_data,
			 XtPointer call_data)
{
    manage_paned_child(source_view->code_form());
    Widget arg_cmd_w = XtParent(source_arg->top());
    manage_paned_child(arg_cmd_w);

    popup_shell(source_view_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB(w, client_data, call_data);

    app_data.disassemble = true;

    update_options();
}

bool have_source_window()
{
    return XtIsManaged(source_view->source_form());
}

bool have_code_window()
{
    return XtIsManaged(source_view->code_form());
}

bool have_visible_source_window()
{
    return have_source_window()
	&& (source_view_shell == 0 
	    || (source_view_shell_state == PoppedUp
		&& source_view_shell_visibility == VisibilityUnobscured));
}


// Data window
void gdbCloseDataWindowCB(Widget w, XtPointer, XtPointer)
{
    if (!have_source_window() && !have_command_window() && !have_code_window())
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), 0);
	return;
    }

    popdown_shell(data_disp_shell);

    Widget arg_cmd_w = XtParent(source_arg->top());
    if (data_disp->graph_cmd_w == arg_cmd_w)
    {
	// Don't close the common toolbar
    }
    else
    {
	unmanage_paned_child(data_disp->graph_cmd_w);
    }

    unmanage_paned_child(data_disp->graph_form());

    app_data.data_window = false;

    update_options();
}

void gdbOpenDataWindowCB(Widget, XtPointer, XtPointer)
{
    manage_paned_child(data_disp->graph_cmd_w);
    manage_paned_child(data_disp->graph_form());

    popup_shell(data_disp_shell);

    app_data.data_window = true;

    update_options();
}

bool have_data_window()
{
    return XtIsManaged(data_disp->graph_form());
}

bool have_visible_data_window()
{
    return have_data_window()
	&& (data_disp_shell == 0 
	    || (data_disp_shell_state == PoppedUp
		&& data_disp_shell_visibility == VisibilityUnobscured));
}


// Execution window
void gdbCloseExecWindowCB(Widget, XtPointer, XtPointer)
{
    app_data.separate_exec_window = False;

    kill_exec_tty();
    update_options();
}

void gdbOpenExecWindowCB(Widget, XtPointer, XtPointer)
{
    app_data.separate_exec_window = True;

    if (exec_tty_pid() == 0)
	startup_exec_tty();
    popup_tty(command_shell);
    update_options();
}

bool have_exec_window()
{
    exec_tty_running();
    return exec_tty_pid() > 0;
}

bool have_visible_exec_window()
{
    // We cannot determine easily whether the exec window is visible
    return have_exec_window();
}


// Tool window
void gdbCloseToolWindowCB(Widget, XtPointer, XtPointer)
{
    popdown_shell(tool_shell);
    update_options();
}

void gdbOpenToolWindowCB(Widget, XtPointer, XtPointer)
{
    if (tool_shell == 0)
	return;

    XtVaSetValues(tool_shell,
		  XmNgeometry, last_tool_shell_geometry.chars(),
		  NULL);

    popup_shell(tool_shell);

    wait_until_mapped(tool_shell);
    RecenterToolShellCB();

    update_options();
}

bool have_tool_window()
{
    return tool_shell != 0;
}

bool have_visible_tool_window()
{
    return have_tool_window() 
	&& tool_shell_state == PoppedUp 
	&& tool_shell_visibility == VisibilityUnobscured;
}


//-----------------------------------------------------------------------------
// Toggling shells
//-----------------------------------------------------------------------------

void gdbToggleCommandWindowCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenCommandWindowCB(w, client_data, call_data);
    else
	gdbCloseCommandWindowCB(w, client_data, call_data);
}

void gdbToggleSourceWindowCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenSourceWindowCB(w, client_data, call_data);
    else
	gdbCloseSourceWindowCB(w, client_data, call_data);
}

void gdbToggleCodeWindowCB(Widget w, XtPointer client_data,
			   XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenCodeWindowCB(w, client_data, call_data);
    else
	gdbCloseCodeWindowCB(w, client_data, call_data);

    update_options();
}

void gdbToggleDataWindowCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenDataWindowCB(w, client_data, call_data);
    else
	gdbCloseDataWindowCB(w, client_data, call_data);
}

void gdbToggleExecWindowCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenExecWindowCB(w, client_data, call_data);
    else
	gdbCloseExecWindowCB(w, client_data, call_data);
}

void gdbToggleToolWindowCB(Widget w, XtPointer client_data,
			      XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenToolWindowCB(w, client_data, call_data);
    else
	gdbCloseToolWindowCB(w, client_data, call_data);
}


//-----------------------------------------------------------------------------
// Command tool placement
//-----------------------------------------------------------------------------

// Place command tool in upper right edge of REF
static void recenter_tool_shell(Widget ref)
{
    recenter_tool_shell(ref, 
			app_data.tool_top_offset, app_data.tool_right_offset);
}

// Place command tool in upper right edge of REF, with a distance of
// TOP_OFFSET and RIGHT_OFFSET
static void recenter_tool_shell(Widget ref, int top_offset, int right_offset)
{
    if (ref == 0)
	ref = source_view->source();
    if (ref == 0 || !XtIsManaged(ref))
	ref = source_view->code();

    if (ref == 0 || tool_shell == 0 || 
	!XtIsRealized(ref) || !XtIsRealized(tool_shell) ||
	tool_shell_state != PoppedUp)
	return;

    Window ref_window  = XtWindow(ref);
    Window tool_window = XtWindow(tool_shell);
    Window tool_frame  = frame(tool_shell);

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
    int x = ref_attributes.width - tool_attributes.width - right_offset;
    int y = top_offset;

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

    move_tool_shell(BoxPoint(root_x, root_y));
}


// Get current offset of command tool in TOP_OFFSET and RIGHT_OFFSET;
// return true iff successful.
static bool get_tool_offset(Widget ref, int& top_offset, int& right_offset)
{
    if (ref == 0)
	ref = source_view->source();
    if (ref == 0 || !XtIsManaged(ref))
	ref = source_view->code();

    if (ref == 0 || tool_shell == 0 || 
	!XtIsRealized(ref) || !XtIsRealized(tool_shell) || 
	!XtIsManaged(tool_buttons_w) ||
	tool_shell_state != PoppedUp)
	return false;

    Window ref_window  = XtWindow(ref);
    Window tool_window = XtWindow(tool_shell);
    Window tool_frame  = frame(tool_shell);

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

    // If the tool frame is off the screen, don't store the offset
    if (frame_attributes.x < 0
	|| frame_attributes.y < 0
	|| frame_attributes.x + frame_attributes.width
	   > WidthOfScreen(XtScreen(tool_shell))
	|| frame_attributes.y + frame_attributes.height
	   > HeightOfScreen(XtScreen(tool_shell)))
    {
	return false;
    }

    // Fetch root coordinates of upper right edge of command tool
    int tool_x, tool_y;
    Window tool_child;
    XTranslateCoordinates(XtDisplay(tool_shell), tool_window,
			  tool_attributes.root,
			  tool_attributes.width, 0,
			  &tool_x, &tool_y, &tool_child);

    // Fetch root coordinates of upper right edge of ref
    int ref_x, ref_y;
    Window ref_child;
    XTranslateCoordinates(XtDisplay(ref), ref_window,
			  ref_attributes.root,
			  ref_attributes.width, 0,
			  &ref_x, &ref_y, &ref_child);

    // Determine offsets
    int x = ref_x - tool_x;
    int y = tool_y - ref_y;

    // Correct them relative to frame thickness
    int frame_x, frame_y;
    Window frame_child;
    XTranslateCoordinates(XtDisplay(ref), tool_window,
			  tool_frame,
			  tool_attributes.width, 0, &frame_x, &frame_y,
			  &frame_child);

    x -= frame_attributes.width - frame_x + frame_attributes.border_width;
    y -= frame_y + frame_attributes.border_width;

    top_offset   = y;
    right_offset = x;
    return true;
}

// Store current offset of command tool in APP_DATA
void get_tool_offset()
{
    initialize_offsets();

    if (get_tool_offset(0, last_top_offset, last_right_offset))
    {
	app_data.tool_top_offset   = last_top_offset;
	app_data.tool_right_offset = last_right_offset;
    }
}



// Manage paned child with minimum size

inline bool is_internal_paned_child(Widget w)
{
    return XmIsSash(w) || XmIsSeparator(w) || XmIsSeparatorGadget(w) 
	|| XtIsShell(w);
}

static MinMaxAssoc preferred_sizes;

void save_preferred_paned_sizes(Widget paned)
{
    // Fetch children
    WidgetList children;
    Cardinal numChildren = 0;
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, NULL);

    // Fetch preferred sizes
    Cardinal i;
    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child))
	    continue;

	// Fetch preferred (= initial) height
	Dimension height = 0;
	XtVaGetValues(child, XmNheight, &height, NULL);
	{
	    XtWidgetGeometry size;
	    size.request_mode = CWHeight;
	    XtQueryGeometry(child, NULL, &size);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	clog << XtName(paned) << ": child " << XtName(child) 
	     << " has preferred height " << height << '\n';
#endif

	MinMax& preferred_size = preferred_sizes[child];
	preferred_size.min = preferred_size.max = height;
    }
}

const Dimension MIN_PANED_SIZE = 90;

static void paned_changed(Widget /* paned */)
{
    if (gdb_w != 0)
    {
	// Recenter the tool shell
	recenter_tool_shell(source_view->source());

	// Make sure the current command line is visible
	end_of_lineAct(gdb_w, 0, 0, 0);

	// Redisplay source and code glyphs
	source_view->update_glyphs();
    }
}

void manage_paned_child(Widget w)
{
    Widget paned = XtParent(w);

    if (paned == 0 || !XmIsPanedWindow(paned) || XtIsManaged(w))
    {
	XtManageChild(w);
	return;
    }

#if LOG_GEOMETRY
    clog << XtName(paned) << ": managing child " << XtName(w) << "\n";
    if (preferred_sizes.has(w))
    {
	const MinMax& preferred_size = preferred_sizes[w];
	clog << XtName(paned) << ": child " << XtName(w) 
	     << " has preferred height " << preferred_size.max << '\n';
    }
#endif

    // Fetch children
    WidgetList children;
    Cardinal numChildren = 0;
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, NULL);


    // Fetch current constraints
    MinMaxAssoc sizes;
    Cardinal i;
    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child))
	    continue;

	MinMax& size = sizes[child];
	XtVaGetValues(children[i], 
		      XmNpaneMinimum, &size.min, 
		      XmNpaneMaximum, &size.max,
		      NULL);

#if LOG_GEOMETRY
	clog << XtName(paned) << ": child " << XtName(child) 
	     << " is managed (min = " << size.min 
	     << ", max = " << size.max << ")\n";
#endif
    }

    assert(sizes.has(w));
    const MinMax& wsize = sizes[w];
    static const MinMax no_constraints;

    // Ensure that each child keeps at least the minimum size;
    // don't make it larger than its preferred maximum size.
    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (!XtIsManaged(child) && child != w)
	    continue;
	if (!sizes.has(child))
	    continue;

	Arg args[10];
	Cardinal arg = 0;

	MinMax& size = sizes[child];
	Dimension current_max = size.max;

	if (child != w
	    && wsize.min <= no_constraints.min 
	    && wsize.max >= no_constraints.max
	    && preferred_sizes.has(child))
	{
	    // W is resizable: give all other children at most their
	    // preferred (= initial) height
	    MinMax& preferred_size = preferred_sizes[child];

	    if (preferred_size.max < size.max)
	    {
		XtSetArg(args[arg], XmNpaneMaximum, preferred_size.max); arg++;
		current_max = preferred_size.max;
	    }

#if LOG_GEOMETRY
	    clog << XtName(paned) << ": child " << XtName(child) 
	         << " has preferred height " << preferred_size.max << '\n';
#endif
	}

	// Make each child (including W) at least MIN_PANED_SIZE high
	if (MIN_PANED_SIZE > size.min)
	{
	    XtSetArg(args[arg], XmNpaneMinimum,
		     min(MIN_PANED_SIZE, current_max)); arg++;
	}

	if (arg > 0)
	{
	    XtSetValues(child, args, arg);
	    size.changed = true;
	}
    }

    // Manage W
    XtManageChild(w);

    // Restore old constraints
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, NULL);

    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (!sizes.has(child) || !XtIsManaged(child))
	    continue;

	MinMax& size = sizes[child];

	if (size.changed)
	{
	    XtVaSetValues(child,
			  XmNpaneMinimum, size.min, 
			  XmNpaneMaximum, size.max,
			  NULL);
	}
    }

    paned_changed(w);
}

// Return the number of resizable children of PANED
static int resizable_children(Widget paned)
{
    // Fetch children
    WidgetList children;
    Cardinal numChildren = 0;
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, NULL);

    // Fetch current constraints
    MinMaxAssoc sizes;
    Cardinal i;
    int n = 0;
    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child) || !XtIsManaged(child))
	    continue;

	MinMax& size = sizes[child];
	XtVaGetValues(children[i], 
		      XmNpaneMinimum, &size.min, 
		      XmNpaneMaximum, &size.max,
		      NULL);

	if (size.min < size.max)
	    n++;
    }

    return n;
}


// Unmanage W, but be sure the command window doesn't grow.
void unmanage_paned_child(Widget w)
{
    Widget paned = XtParent(w);
    if (paned == 0 || !XmIsPanedWindow(paned) || !XtIsManaged(w))
    {
	XtUnmanageChild(w);
	return;
    }

#if LOG_GEOMETRY
    clog << XtName(paned) << ": unmanaging child " << XtName(w) << "\n";
#endif

    Widget command = XtParent(gdb_w);

    if (resizable_children(paned) <= 2)
    {
	// Only two resizable children left - the remaining one must
	// be resized to a maximum.  Disable skipAdjust for a moment.
	Boolean skip_adjust = True;
	XtVaGetValues(command, XmNskipAdjust, &skip_adjust, NULL);
	XtVaSetValues(command, XmNskipAdjust, False, NULL);

	XtUnmanageChild(w);

	XtVaSetValues(command, XmNskipAdjust, skip_adjust, NULL);
    }
    else
    {
	// Resize the other child, but keep the command window intact
	Dimension height = 0;
	Dimension max = 1000;
	XtVaGetValues(command, 
		      XmNheight, &height,
		      XmNpaneMaximum, &max,
		      NULL);
	XtVaSetValues(command, XmNpaneMaximum, height, NULL);

	XtUnmanageChild(w);

	XtVaSetValues(command, XmNpaneMaximum, max, NULL);
    }

    paned_changed(w);
}


// Set the width of PANED to the maximum width of its children

// Fetch the maximum width.  Do this for each paned window.
void get_paned_window_width(Widget paned, Dimension& max_width)
{
    if (paned == 0 || !XtIsSubclass(paned, xmPanedWindowWidgetClass))
	return;

    WidgetList children   = 0;
    Cardinal num_children = 0;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  NULL);

    for (Cardinal i = 0; i < num_children; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child) || !XtIsManaged(child))
	    continue;

	// Fetch preferred width
	XtWidgetGeometry size;
	size.request_mode = CWWidth;
	XtQueryGeometry(child, NULL, &size);

#if LOG_GEOMETRY
	clog << XtName(paned) << ": child " << XtName(child)
	     << " wants width " << size.width << "\n";
#endif

	max_width = max(size.width, max_width);
    }
}

// Set the found value.
void set_paned_window_size(Widget paned, Dimension max_width)
{
    if (paned == 0 || !XtIsSubclass(paned, xmPanedWindowWidgetClass))
	return;

    XtVaSetValues(paned, XmNwidth, max_width, NULL);

    WidgetList children     = 0;
    Cardinal num_children   = 0;
    Dimension spacing       = 8;
    Dimension margin_width  = 3;
    Dimension margin_height = 3;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  XmNspacing, &spacing,
		  XmNmarginWidth, &margin_width,
		  XmNmarginHeight, &margin_height,
		  NULL);

    Dimension total_height = 0;
    int managed_children = 0;
    for (Cardinal i = 0; i < num_children; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child))
	    continue;

	// Fetch preferred width
	Dimension width  = 0;
	Dimension height = 0;
	XtVaGetValues(child, XmNwidth, &width, XmNheight, &height, NULL);

	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth | CWHeight;
	    XtQueryGeometry(child, NULL, &size);

	    width  = max(width, size.width);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	clog << XtName(paned) << ": child " << XtName(child) 
	     << " wants height " << height << "\n";
#endif

	if (managed_children > 0)
	    total_height += spacing;

	total_height += height;
	managed_children++;
    }

    XtVaSetValues(paned,
		  XmNwidth, max_width + 2 * margin_width,
		  XmNheight, total_height + 2 * margin_height,
		  NULL);
}



//-----------------------------------------------------------------------------
// Main Window stuff
//-----------------------------------------------------------------------------

// Set main window size
void set_main_window_size(Widget main)
{
    if (main == 0 || !XtIsSubclass(main, xmMainWindowWidgetClass))
	return;

    WidgetList children   = 0;
    Cardinal num_children = 0;

    XtVaGetValues(main,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  NULL);

    Dimension max_width    = 0;
    Dimension total_height = 0;
    for (Cardinal i = 0; i < num_children; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child))
	    continue;
	if (XmIsScrollBar(child))
	    continue;

	// Fetch sizes
	Dimension width  = 0;
	Dimension height = 0;
	XtVaGetValues(child, XmNwidth, &width, XmNheight, &height, NULL);

	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth | CWHeight;
	    XtQueryGeometry(child, NULL, &size);

	    width  = max(width, size.width);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	clog << XtName(main) << ": child " << XtName(child) << " wants " 
	     << BoxSize(width, height) << "\n";
#endif

	max_width = max(width, max_width);
	total_height += height;
    }

    XtVaSetValues(main, XmNwidth, max_width, XmNheight, total_height, NULL);
}


//-----------------------------------------------------------------------------
// Scrolled Window stuff
//-----------------------------------------------------------------------------

// Promote child size to scrolled window
void set_scrolled_window_size(Widget child, Widget target)
{
    Widget scroll = XtParent(child);

    assert(XmIsScrolledWindow(scroll));

    Dimension scrollbar_width = 15;   // Additional space for scrollbar

    Widget vertical_scroll_bar = 0;
    Dimension spacing          = 4;
    Dimension margin_width     = 2;
    Dimension margin_height    = 2;
    XtVaGetValues(scroll, 
		  XmNverticalScrollBar, &vertical_scroll_bar,
		  XmNspacing, &spacing,
		  XmNmarginWidth, &margin_width,
		  XmNmarginHeight, &margin_height,
		  NULL);

    if (vertical_scroll_bar != 0)
    {
	XtWidgetGeometry size;
	size.request_mode = CWWidth;
	XtQueryGeometry(vertical_scroll_bar, NULL, &size);
	scrollbar_width = size.width;
    }

    // Give the ScrolledWindow the size specified for its child
    XtWidgetGeometry size;
    size.request_mode = CWWidth | CWHeight;
    XtQueryGeometry(child, NULL, &size);

    if (target == 0)
	target = scroll;

#if LOG_GEOMETRY
    clog << XtName(target) << ": child " << XtName(child) << " wants " 
	 << BoxSize(size.width, size.height) << "\n";
#endif

    Dimension border_width     = 1;
    Dimension shadow_thickness = 2;
    XtVaGetValues(child,
		  XmNborderWidth, &border_width,
		  XmNshadowThickness, &shadow_thickness,
		  NULL);

    Dimension width = size.width +
	border_width * 2 +
	// shadow_thickness * 2 +
	spacing +
	scrollbar_width +
	margin_width * 2;

    Dimension height = size.height +
	border_width * 2 +
	// shadow_thickness * 2 +
	margin_height * 2;

    XtVaSetValues(target, XmNwidth,  width, XmNheight, height, NULL);
}
