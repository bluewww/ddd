// $Id$ -*- C++ -*-
// DDD window management

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

char windows_rcsid[] = 
    "$Id$";

#define LOG_GEOMETRY 0
#define LOG_EVENTS   1
#define LOG_MOVES    0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#include "Tool.h"
#include "XErrorB.h"

#ifdef IF_MOTIF
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
#else // NOT IF_MOTIF
#include "GtkMultiPaned.h"
#endif // IF_MOTIF

//-----------------------------------------------------------------------------
// Window management
//-----------------------------------------------------------------------------

// Shells (only used if separate windows are used)
#if defined(IF_XM)
Widget command_shell;
Widget data_disp_shell;
Widget source_view_shell;
#else
GUI::Window *command_shell = NULL;
GUI::Window *data_disp_shell = NULL;
GUI::Window *source_view_shell = NULL;
#endif

// Command tool
#if defined(IF_XM)
Widget tool_shell;
Widget tool_buttons_w;
#else
GUI::Window *tool_shell;
GUI::Box *tool_buttons_w;
#endif

// Shell state stuff
enum WindowState { PoppingUp, PoppedUp, PoppedDown, 
		   Iconic, Transient, UnknownShell };

#if LOG_EVENTS
static std::ostream& operator << (std::ostream& os, WindowState s)
{
    switch (s)
    {
    case PoppingUp:
	return os << "PoppingUp";
    case PoppedUp:
	return os << "PoppedUp";
    case PoppedDown:
	return os << "PoppedDown";
    case Iconic:
	return os << "Iconic";
    case Transient:
	return os << "Transient";
    case UnknownShell:
	return os << "UnknownShell";
    }

    return os;
}
#endif

static WindowState& state(Widget w)
{
    static WindowState command_shell_state     = PoppedDown;
    static WindowState data_disp_shell_state   = PoppedDown;
    static WindowState source_view_shell_state = PoppedDown;
    static WindowState tool_shell_state        = PoppedDown;

    if (w == 0)
	/* do nothing */;
    else if (w == command_shell)
	return command_shell_state;
    else if (w == data_disp_shell)
	return data_disp_shell_state;
    else if (w == source_view_shell)
	return source_view_shell_state;
    else if (w == tool_shell)
	return tool_shell_state;

    static WindowState dummy;
    dummy = UnknownShell;
    return dummy;
}

#if !defined(IF_XM)

static WindowState& state(GUI::Widget *w)
{
    static WindowState command_shell_state     = PoppedDown;
    static WindowState data_disp_shell_state   = PoppedDown;
    static WindowState source_view_shell_state = PoppedDown;
    static WindowState tool_shell_state        = PoppedDown;

    if (w == 0)
	/* do nothing */;
    else if (w == command_shell)
	return command_shell_state;
    else if (w == data_disp_shell)
	return data_disp_shell_state;
    else if (w == source_view_shell)
	return source_view_shell_state;
    else if (w == tool_shell)
	return tool_shell_state;

    static WindowState dummy;
    dummy = UnknownShell;
    return dummy;
}

#endif

static bool popped_down(Widget w)
{
    WindowState st = state(w);
    return st == PoppedDown || st == UnknownShell;
}

static void set_state(WindowState& var, WindowState state)
{
    if (state == PoppingUp && var == PoppedUp)
	return;
    var = state;
}

static void set_state(Widget w, WindowState s)
{
    WindowState& var = state(w);
    if (var != UnknownShell)
    {
	set_state(var, s);
#if LOG_EVENTS
	std::clog << XtName(w) << " is " << state(w) << "\n";
#endif
    }
}

#if !defined(IF_XM)

static void set_state(GUI::Widget *w, WindowState s)
{
    WindowState& var = state(w);
    if (var != UnknownShell)
    {
	set_state(var, s);
#if LOG_EVENTS
	std::clog << XtName(w) << " is " << state(w) << "\n";
#endif
    }
}

#endif

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

#ifdef IF_MOTIF
// Return current tool shell position relative to root window
static BoxPoint tool_shell_pos()
{
    int root_x = 0;
    int root_y = 0;

    if (tool_shell != 0 && XtIsRealized(tool_shell) && 
	state(tool_shell) == PoppedUp)
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
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning tool_shell_pos not supported.
#endif
#endif // IF_MOTIF

static XtIntervalId move_tool_shell_timer = NO_TIMER;

static BoxPoint last_tool_shell_position;
static BoxPoint tool_shell_move_offset(0, 0);

static void move_tool_shell(const BoxPoint& pos, bool verify = true);

// Verify shell position after movement
static void VerifyToolShellPositionCB(XtPointer = 0, XtIntervalId *id = 0)
{
    (void) id;			// Use it
    assert (*id == move_tool_shell_timer);
    move_tool_shell_timer = NO_TIMER;

#if LOG_MOVES
    std::clog << "Tool position found:    " << tool_shell_pos() << "\n";
    std::clog << "Tool position expected: " << last_tool_shell_position << "\n";
#endif

#ifdef IF_MOTIF
    BoxPoint diff = tool_shell_pos() - last_tool_shell_position;
    if (diff != BoxPoint(0, 0))
    {
	tool_shell_move_offset = -diff;
	move_tool_shell(last_tool_shell_position, false);
    }
#ifdef NAG_ME
#warning Do not move tool shell
#endif
#endif // IF_MOTIF
}

// Move tool shell to POS.  If VERIFY is set, verify and correct 
// any displacement induced by the window manager.
static void move_tool_shell(const BoxPoint& pos, bool verify)
{
#if 0
    // Make sure we don't move the tool shell off the screen
    pos[X] = max(pos[X], 0);
    pos[Y] = max(pos[Y], 0);
#endif

    if (tool_shell == 0)
	return;

#ifdef IF_MOTIF
    if (pos != tool_shell_pos())
    {
#if LOG_MOVES
	std::clog << "Moving tool to: " << pos[X] << ", " << pos[Y] << "\n";
#endif

	BoxPoint given_pos = pos + tool_shell_move_offset;

	std::ostringstream os;
	os << "+" << given_pos[X] << "+" << given_pos[Y];
	last_tool_shell_geometry = string(os);
	last_tool_shell_position = pos;

	// Move tool shell to POS
	XtVaSetValues(tool_shell,
		      XmNgeometry, last_tool_shell_geometry.chars(),
		      XmNx, given_pos[X],
		      XmNy, given_pos[Y],
		      XtPointer(0));

	// Verify tool shell position
	if (move_tool_shell_timer != 0)
	{
	    XtRemoveTimeOut(move_tool_shell_timer);
	    move_tool_shell_timer = 0;
	}

	if (verify)
	{
	    move_tool_shell_timer = 
		XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell),
				100, VerifyToolShellPositionCB, XtPointer(0));
	}
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Do not move tool shell
#endif
#endif // IF_MOTIF
}

#ifdef IF_MOTIF
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

    if (XtIsRealized(tool_buttons_w) && state(tool_shell) == PoppedUp)
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

static void follow_tool_shell(Widget ref)
{
    initialize_offsets();
    recenter_tool_shell(ref, last_top_offset, last_right_offset);
}

#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning RecenterToolShellCB not implemented
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void FollowToolShellCB(XtPointer = 0, XtIntervalId *id = 0)
{
    if (tool_shell == 0)
	return;

    static XtIntervalId follow_tool_shell_timer = 0;

    if (id != 0)
    {
	assert(*id = follow_tool_shell_timer);
	follow_tool_shell_timer = 0;
    }
    else if (follow_tool_shell_timer != 0)
    {
	XtRemoveTimeOut(follow_tool_shell_timer);
	follow_tool_shell_timer = 0;
    }

    bool have_visible_tool_shell = false;

    if (XtIsRealized(tool_buttons_w) && state(tool_shell) == PoppedUp)
    {
	XWindowAttributes attr;
	XGetWindowAttributes(XtDisplay(tool_buttons_w), 
			     XtWindow(tool_buttons_w), &attr);
	have_visible_tool_shell = (attr.map_state == IsViewable);
    }

    if (have_visible_tool_shell)
    {
	follow_tool_shell();
    }
    else
    {
	// Try again in 200 ms
	follow_tool_shell_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell), 
			    200, FollowToolShellCB, XtPointer(0));
    }
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning FollowToolShellCB not implemented
#endif
#endif // IF_MOTIF

#if defined(IF_XM)

bool started_iconified(Widget w)
{
    Widget toplevel = w;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);

    assert(XtIsTopLevelShell(toplevel));

    // Well isn't it iconic - don't you think?
    Boolean iconic;
    XtVaGetValues(toplevel, XmNiconic, &iconic, XtPointer(0));
    return iconic;
}

#else

bool started_iconified(GUI::Widget *w)
{
#ifdef NAG_ME
#warning started_iconified not implemented
#endif
    return false;
}

#endif

#if defined(IF_XM)

// Popup initial shell
void initial_popup_shell(Widget w)
{
    if (w == 0)
	return;

    Boolean iconic = started_iconified(w);
    XtVaSetValues(w, 
		  XmNiconic, iconic,
		  XmNinitialState, iconic ? IconicState : NormalState,
		  XtPointer(0));
    WindowState state = iconic ? Iconic : PoppingUp;

    set_state(w, state);

    if (iconic || w == tool_shell)
	XtVaSetValues(w, 
		      XmNgeometry, "+0+0", 
		      XmNx, 0,
		      XmNy, 0,
		      XtPointer(0));

    if (w == tool_shell)
    {
	XtManageChild(tool_buttons_w);
	if (!XtIsRealized(tool_shell))
	    XtRealizeWidget(tool_shell);
	if (!iconic)
	    RecenterToolShellCB();
    }

    Widget toplevel = w;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);
    assert(XtIsTopLevelShell(toplevel));

    if (w != toplevel && XtIsRealized(w)) {
	XtPopup(w, XtGrabNone);
    }
}

#else

// Popup initial shell
void initial_popup_shell(GUI::Widget *w)
{
    if (w == 0)
	return;

    // FIXME: Is it possible to start a Gnome application iconified
    // (like the -iconic option in X)?  If not, we need not worry
    // about the possibility of an Iconic state.
    set_state(w, PoppingUp);

    if (w == tool_shell)
    {
	tool_buttons_w->show();
	if (!tool_shell->is_realized()) {
	    std::cerr << "TOOL SHELL NOT REALIZED\n";
	    // tool_shell->realize();
	}
	// if (!iconic)
	//    RecenterToolShellCB();
    }

    w->show();
}

#endif

#if defined(IF_XM)

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

    if (XtIsRealized(w)) {
	XtPopup(w, XtGrabNone);
    }

    set_state(w, PoppingUp);

    // Uniconify window
    if (XtIsRealized(w))
	XMapWindow(XtDisplay(w), XtWindow(w));
    raise_shell(w);
}

#else

void popup_shell(GUI::Widget *w)
{
    if (w == 0)
	return;

    if (w == tool_shell)
    {
	if (!tool_shell->is_realized())
	{
	    initial_popup_shell(tool_shell);
	    // RecenterToolShellCB();
	}

	tool_buttons_w->show();
    }

    if (w->is_realized()) {
	w->show();
    }

    set_state(w, PoppingUp);

    // Uniconify window
#ifdef NAG_ME
#warning Iconified windows not handled.
#endif
    // raise_shell(w);
}

#endif

#if defined(IF_XM)

void popdown_shell(Widget w)
{
    if (w == 0)
	return;

    set_state(w, PoppedDown);

    if (w == tool_shell)
	XtUnmanageChild(tool_buttons_w);

    XtPopdown(w);
}

#else

void popdown_shell(GUI::Widget *w)
{
    if (w == 0)
	return;

    set_state(w, PoppedDown);

    if (w == tool_shell)
	tool_buttons_w->hide();

    w->hide();
}

#endif

void iconify_shell(Widget w)
{
#ifdef IF_MOTIF
    if (w == 0 || !XtIsRealized(w))
	return;

    set_state(w, Iconic);

    XIconifyWindow(XtDisplay(w), XtWindow(w),
		   XScreenNumberOfScreen(XtScreen(w)));
#else // NOT IF_MOTIF
    std::cerr << "iconify_shell not supported.\n";
#endif // IF_MOTIF
}

#if defined(IF_XM)

void uniconify_shell(Widget w)
{
    if (w == 0)
	return;

    if (state(w) == Iconic)
    {
	popup_shell(w);
    }
}

#else

void uniconify_shell(GUI::Widget *w)
{
    std::cerr << "uniconify_shell not supported.\n";
}

#endif

static void popup_tty(Widget shell)
{
    if (exec_tty_window())
    {
#ifdef IF_MOTIF
	XErrorBlocker blocker(XtDisplay(shell));

	// Uniconify window
	XMapWindow(XtDisplay(shell), exec_tty_window());

	// Place window on top
	XRaiseWindow(XtDisplay(shell), exec_tty_window());
#else // NOT IF_MOTIF
	std::cerr << "popup_tty called\n";
#endif // IF_MOTIF
    }
}

#ifdef IF_MOTIF
static void iconify_tty(Widget shell)
{
    if (exec_tty_window())
    {
	XIconifyWindow(XtDisplay(shell), exec_tty_window(),
		       XScreenNumberOfScreen(XtScreen(shell)));
    }
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
// Shell visibility stuff
static int& visibility(Widget w)
{
    static int command_shell_visibility     = VisibilityFullyObscured;
    static int data_disp_shell_visibility   = VisibilityFullyObscured;
    static int source_view_shell_visibility = VisibilityFullyObscured;
    static int tool_shell_visibility        = VisibilityFullyObscured;

    if (w == 0)
	/* do nothing */;
    else if (w == command_shell)
	return command_shell_visibility;
    else if (w == data_disp_shell)
	return data_disp_shell_visibility;
    else if (w == source_view_shell)
	return source_view_shell_visibility;
    else if (w == tool_shell)
	return tool_shell_visibility;

    static int dummy;
    dummy = VisibilityFullyObscured;
    return dummy;
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

    XErrorBlocker blocker(display);

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
    if (tool_shell && XtIsRealized(tool_shell) && 
	state(tool_shell) == PoppedUp)
	raise_above(XtDisplay(tool_shell), XtWindow(tool_shell), sibling);
}

inline void raise_tool_above(Widget w)
{
    if (w != 0 && XtIsRealized(w))
	raise_tool_above(XtWindow(w));
}

static XtIntervalId recentering_tool_shell_timer = 0;

void StructureNotifyEH(Widget w, XtPointer, XEvent *event, Boolean *)
{
    bool synthetic = (state(w) == Transient);

#if LOG_EVENTS
    if (synthetic)
	std::clog << "Synthetic event: ";
#endif

    switch (event->type)
    {
    case MapNotify:
	if (state(w) == UnknownShell)
	    return;

	// Reflect state
	set_state(w, PoppedUp);

#if 0
	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell))
	    && !app_data.command_toolbar && app_data.source_window)
	{
	    // Popup command tool again
	    popup_shell(tool_shell);
	    set_state(tool_shell, Transient);
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
	    if (state(command_shell) == Iconic)
	    {
		popup_shell(command_shell);
		set_state(command_shell, Transient);
	    }
	    if (state(data_disp_shell) == Iconic)
	    {
		popup_shell(data_disp_shell);
		set_state(data_disp_shell, Transient);
	    }
	    if (state(source_view_shell) == Iconic)
	    {
		popup_shell(source_view_shell);
		set_state(source_view_shell, Transient);
	    }
	    popup_tty(command_shell);
	}
	break;

    case UnmapNotify:
	// Reflect state
	if (state(w) == UnknownShell)
	    return;

	if (state(w) != Iconic && state(w) != PoppedDown)
	    set_state(w, Iconic);

	if (!synthetic
	    && (w == source_view_shell
		|| (source_view_shell == 0 && w == command_shell)))
	{
	    // Iconify command tool, too
	    iconify_shell(tool_shell);
	    set_state(tool_shell, Transient);
	}

	if (!synthetic && state(w) != PoppedDown && app_data.group_iconify)
	{
	    // Iconify all other windows as well
	    if (state(command_shell) == PoppedUp)
	    {
		iconify_shell(command_shell);
		set_state(command_shell, Transient);
	    }
	    if (state(data_disp_shell) == PoppedUp)
	    {
		iconify_shell(data_disp_shell);
 		set_state(data_disp_shell, Transient);
	    }
	    if (state(source_view_shell) == PoppedUp)
	    {
		iconify_shell(source_view_shell);
		set_state(source_view_shell, Transient);
	    }
	    iconify_tty(command_shell);
	}
	break;

    case VisibilityNotify:
    {
	visibility(w) = event->xvisibility.state;

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
	    && (state(tool_shell) == PoppedUp 
		|| state(tool_shell) == Transient))
	{
	    // Let `sticky' command tool follow the source window
	    initialize_offsets();

	    if (w == tool_shell)
	    {
		// Command tool has been moved
#if LOG_EVENTS || LOG_MOVES
		std::clog << "Tool has been moved to " << point(event) << "\n";
#endif

		if (recentering_tool_shell_timer == 0)
		{
		    // Record offset
		    get_tool_offset(0, last_top_offset, last_right_offset);
		}
	    }

	    if (w == source_view_shell || 
		w == command_shell && source_view_shell == 0)
	    {
		// Source shell has been moved -- let command tool follow
#if LOG_EVENTS || LOG_MOVES
		std::clog << "Shell has been moved to " << point(event) << "\n";
#endif

		FollowToolShellCB();
	    }
	}
	break;
    }

    default:
	// Any other event...
	break;
    }
}
#endif // IF_MOTIF


//-----------------------------------------------------------------------------
// Closing shells
//-----------------------------------------------------------------------------

// Return number of running input shells
int running_shells()
{
    int shells = 0;

    if (!popped_down(command_shell))
	shells++;
    if (!popped_down(source_view_shell))
	shells++;
    if (!popped_down(data_disp_shell))
	shells++;
    if (app_data.tty_mode)
	shells++;

    return shells;
}


#if defined(IF_XM)

// Generic close callback
void DDDCloseCB(Widget w, XtPointer, XtPointer)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), XtPointer(0));
	return;
    }

    Widget shell = findTopLevelShellParent(w);

    if (shell == command_shell)
    {
	if (data_disp_shell == 0)
	{
	    gdbCloseDataWindowCB(w, XtPointer(0), XtPointer(0));
	}

	if (source_view_shell == 0)
	{
	    gdbCloseCodeWindowCB(w, XtPointer(0), XtPointer(0));
	    gdbCloseSourceWindowCB(w, XtPointer(0), XtPointer(0));
	}

	gdbCloseCommandWindowCB(w, XtPointer(0), XtPointer(0));
    }
    else if (shell == data_disp_shell)
	gdbCloseDataWindowCB(w, XtPointer(0), XtPointer(0));
    else if (shell == source_view_shell)
	gdbCloseSourceWindowCB(w, XtPointer(0), XtPointer(0));
    else if (shell == tool_shell)
	gdbCloseToolWindowCB(Widget(0), XtPointer(0), XtPointer(0));
    else
	popdown_shell(shell);
}

#else

// Generic close callback
void DDDCloseCB(GUI::Widget *w)
{
    if (running_shells() == 1)
    {
	DDDExitCB(w, EXIT_SUCCESS);
	return;
    }

    GUI::Shell *shell = findTopLevelShellParent1(w);

    if (shell == command_shell)
    {
	if (data_disp_shell == 0)
	{
	    gdbCloseDataWindowCB(w);
	}

	if (source_view_shell == 0)
	{
	    gdbCloseCodeWindowCB(w);
	    gdbCloseSourceWindowCB(w);
	}

	gdbCloseCommandWindowCB(w);
    }
    else if (shell == data_disp_shell)
	gdbCloseDataWindowCB(w);
    else if (shell == source_view_shell)
	gdbCloseSourceWindowCB(w);
    else if (shell == tool_shell)
	gdbCloseToolWindowCB();
    else
	popdown_shell(shell);
}

#endif


// Specific close and open callbacks

// Debugger console
#if defined(IF_XM)
void gdbCloseCommandWindowCB(Widget w, XtPointer, XtPointer)
#else
void gdbCloseCommandWindowCB(GUI::Widget *w)
#endif
{
    if (!app_data.tty_mode && 
	!have_data_window() && !have_source_window() && !have_code_window())
    {
#if defined(IF_XM)
	DDDExitCB(w, EXIT_SUCCESS, XtPointer(0));
#else
	DDDExitCB(w, EXIT_SUCCESS);
#endif
	return;
    }

    if ((app_data.separate_source_window || !have_source_window())
	&& (app_data.separate_data_window || !have_data_window()))
    {
	popdown_shell(command_shell);
    }

#ifdef IF_MOTIF
    // Unmanaged the ScrolledWindow parent:
    unmanage_paned_child(XtParent(gdb_w));
#else // NOT IF_MOTIF
    unmanage_paned_child(gdb_w);
#endif // IF_MOTIF

    app_data.debugger_console = false;

    update_options();
}

#if defined(IF_XM)
void gdbOpenCommandWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbOpenCommandWindowCB(void)
#endif
{
#if defined(IF_XM)
    // Manage the ScrolledWindow parent:
    manage_paned_child(XtParent(gdb_w));
#else
    manage_paned_child(gdb_w);
#endif

    if (app_data.separate_source_window)
	popup_shell(command_shell);

    app_data.debugger_console = true;

    update_options();
}

bool have_command_window()
{
#ifdef IF_MOTIF
    // Refers to ScrolledWindow parent:
    return XtIsManaged(XtParent(gdb_w));
#else // NOT IF_MOTIF
    return XtIsManaged(gdb_w);
#endif // IF_MOTIF
}


// Source window
#if defined(IF_XM)
void gdbCloseSourceWindowCB(Widget w, XtPointer, XtPointer)
#else
void gdbCloseSourceWindowCB(GUI::Widget *w)
#endif
{
    if (!app_data.tty_mode && 
	!have_command_window() && !have_data_window() && !have_code_window())
    {
#if defined(IF_XM)
	DDDExitCB(w, EXIT_SUCCESS, XtPointer(0));
#else
	DDDExitCB(w, EXIT_SUCCESS);
#endif
	return;
    }

    // Popdown shell
    if (source_view_shell)
    {
	popdown_shell(source_view_shell);
    }
    else if (!have_command_window() && 
	     !have_data_window() && 
	     !have_code_window())
    {
	popdown_shell(command_shell);
    }

    // Unmanage source
    unmanage_paned_child(source_view->source_form());

    if (source_view_shell != 0)
	unmanage_paned_child(source_view->code_form());

    if (!XtIsManaged(source_view->code_form()))
	gdbCloseToolWindowCB(CB_ARGS_NULL);

    app_data.source_window = false;

    update_options();
}

#if defined(IF_XM)
void gdbCloseCodeWindowCB(Widget w, XtPointer, XtPointer)
#else
void gdbCloseCodeWindowCB(GUI::Widget *w)
#endif
{
    if (!app_data.tty_mode && 
	!have_command_window() && !have_data_window() && !have_source_window())
    {
#if defined(IF_XM)
	DDDExitCB(w, EXIT_SUCCESS, XtPointer(0));
#else
	DDDExitCB(w, EXIT_SUCCESS);
#endif
	return;
    }

    // Unmanage code
    unmanage_paned_child(source_view->code_form());

    if (!XtIsManaged(source_view->source_form()))
	gdbCloseToolWindowCB(CB_ARGS_NULL);

    app_data.disassemble = false;

    update_options();
}

#if defined(IF_XM)

void gdbOpenSourceWindowCB(Widget, XtPointer, XtPointer)
{
    manage_paned_child(source_view->source_form());
    if (source_view_shell != 0 && app_data.disassemble)
	manage_paned_child(source_view->code_form());
    Widget arg_cmd_w = XtParent(source_arg->top());
    manage_paned_child(arg_cmd_w);

    if (source_view_shell)
	popup_shell(source_view_shell);
    else
	popup_shell(command_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB(Widget(0), XtPointer(0), XtPointer(0));

    app_data.source_window = true;

    update_options();
}

#else

void gdbOpenSourceWindowCB(void)
{
    manage_paned_child(source_view->source_form());
    if (source_view_shell != 0 && app_data.disassemble)
	manage_paned_child(source_view->code_form());
    GUI::Widget *arg_cmd_w = source_arg->top()->get_parent();
    manage_paned_child(arg_cmd_w);

    if (source_view_shell)
	popup_shell(source_view_shell);
    else
	popup_shell(command_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB();

    app_data.source_window = true;

    update_options();
}

#endif

#if defined(IF_XM)

void gdbOpenCodeWindowCB(Widget, XtPointer, XtPointer)
{
    manage_paned_child(source_view->code_form());
    Widget arg_cmd_w = XtParent(source_arg->top());
    manage_paned_child(arg_cmd_w);

    if (source_view_shell)
	popup_shell(source_view_shell);
    else
	popup_shell(command_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB(Widget(0), XtPointer(0), XtPointer(0));

    app_data.disassemble = true;

    update_options();
}

#else

void gdbOpenCodeWindowCB(void)
{
    manage_paned_child(source_view->code_form());
    GUI::Widget *arg_cmd_w = source_arg->top()->get_parent();
    manage_paned_child(arg_cmd_w);

    if (source_view_shell)
	popup_shell(source_view_shell);
    else
	popup_shell(command_shell);

    if (!app_data.command_toolbar)
	gdbOpenToolWindowCB();

    app_data.disassemble = true;

    update_options();
}

#endif

bool have_source_window()
{
    return XtIsManaged(source_view->source_form());
}

bool have_code_window()
{
    return XtIsManaged(source_view->code_form());
}

#if defined(IF_XM)

// Data window
void gdbCloseDataWindowCB(Widget w, XtPointer, XtPointer)
{
    if (!app_data.tty_mode && 
	!have_source_window() && !have_command_window() && !have_code_window())
    {
	DDDExitCB(w, XtPointer(EXIT_SUCCESS), XtPointer(0));
	return;
    }

    if (data_disp_shell)
    {
	popdown_shell(data_disp_shell);
    }
    else if (!have_source_window() && 
	     !have_command_window() && 
	     !have_code_window())
    {
	popdown_shell(command_shell);
    }

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

#else

// Data window
void gdbCloseDataWindowCB(GUI::Widget *w)
{
    if (!app_data.tty_mode && 
	!have_source_window() && !have_command_window() && !have_code_window())
    {
	DDDExitCB(w, EXIT_SUCCESS);
	return;
    }

    if (data_disp_shell)
    {
	popdown_shell(data_disp_shell);
    }
    else if (!have_source_window() && 
	     !have_command_window() && 
	     !have_code_window())
    {
	popdown_shell(command_shell);
    }

    GUI::Widget *arg_cmd_w = source_arg->top()->get_parent();
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

#endif

#if defined(IF_XM)
void gdbOpenDataWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbOpenDataWindowCB(void)
#endif
{
    manage_paned_child(data_disp->graph_cmd_w);
    manage_paned_child(data_disp->graph_form());

    if (data_disp_shell)
	popup_shell(data_disp_shell);
    else
	popup_shell(command_shell);

    app_data.data_window = true;

    update_options();
}

bool have_data_window()
{
    return XtIsManaged(data_disp->graph_form());
}


// Execution window
#if defined(IF_XM)
void gdbCloseExecWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbCloseExecWindowCB(void)
#endif
{
    app_data.separate_exec_window = False;

    kill_exec_tty();
    update_options();
}

#if defined(IF_XM)
void gdbOpenExecWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbOpenExecWindowCB(void)
#endif
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


// Tool window
#if defined(IF_XM)
void gdbCloseToolWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbCloseToolWindowCB(void)
#endif
{
    if (tool_shell == 0 || !XtIsRealized(tool_shell))
	return;

    popdown_shell(tool_shell);
    update_options();
}

#if defined(IF_XM)
void gdbOpenToolWindowCB(Widget, XtPointer, XtPointer)
#else
void gdbOpenToolWindowCB(void)
#endif
{
    if (tool_shell == 0)
	create_command_tool();

    if (tool_shell == 0 || !XtIsRealized(tool_shell))
	return;

#ifdef IF_MOTIF
    XtVaSetValues(tool_shell,
		  XmNgeometry, last_tool_shell_geometry.chars(),
		  XtPointer(0));
#endif // IF_MOTIF

    popup_shell(tool_shell);

    wait_until_mapped(tool_shell);
#ifdef IF_MOTIF
    RecenterToolShellCB();
#endif // IF_MOTIF

    update_options();
}

bool have_tool_window()
{
    return tool_shell != 0;
}


//-----------------------------------------------------------------------------
// Toggling shells
//-----------------------------------------------------------------------------

#if defined(IF_XM)

void gdbToggleCommandWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    bool set = info->set;
    if (set)
	gdbOpenCommandWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseCommandWindowCB(w, XtPointer(0), XtPointer(0));
}

#else

void gdbToggleCommandWindowCB(GUI::Widget *w)
{
    bool set = get_active(w->internal());
    if (set)
	gdbOpenCommandWindowCB();
    else
	gdbCloseCommandWindowCB(w);
}

#endif

#if defined(IF_XM)

void gdbToggleSourceWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenSourceWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseSourceWindowCB(w, XtPointer(0), XtPointer(0));
}

#else

void gdbToggleSourceWindowCB(GUI::Widget *w)
{
    if (get_active(w->internal()))
	gdbOpenSourceWindowCB();
    else
	gdbCloseSourceWindowCB(w);
}

#endif

#if defined(IF_XM)

void gdbToggleCodeWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenCodeWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseCodeWindowCB(w, XtPointer(0), XtPointer(0));

    update_options(NO_UPDATE);
}

#else

void gdbToggleCodeWindowCB(GUI::Widget *w)
{
    if (get_active(w->internal()))
	gdbOpenCodeWindowCB();
    else
	gdbCloseCodeWindowCB(w);

    update_options(NO_UPDATE);
}

#endif

#if defined(IF_XM)

void gdbToggleDataWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenDataWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseDataWindowCB(w, XtPointer(0), XtPointer(0));
}

#else

void gdbToggleDataWindowCB(GUI::Widget *w)
{
    if (get_active(w->internal()))
	gdbOpenDataWindowCB();
    else
	gdbCloseDataWindowCB(w);
}

#endif

#if defined(IF_XM)

void gdbToggleExecWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenExecWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseExecWindowCB(w, XtPointer(0), XtPointer(0));
}

#else

void gdbToggleExecWindowCB(GUI::Widget *w)
{
    if (get_active(w->internal()))
	gdbOpenExecWindowCB();
    else
	gdbCloseExecWindowCB();
}

#endif

#if defined(IF_XM)

void gdbToggleToolWindowCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	gdbOpenToolWindowCB(w, XtPointer(0), XtPointer(0));
    else
	gdbCloseToolWindowCB(w, XtPointer(0), XtPointer(0));
}

#else

void gdbToggleToolWindowCB(GUI::Widget *w)
{
    if (get_active(w->internal()))
	gdbOpenToolWindowCB();
    else
	gdbCloseToolWindowCB();
}

#endif

//-----------------------------------------------------------------------------
// Command tool placement
//-----------------------------------------------------------------------------

#ifdef IF_MOTIF
static void RecenteredToolShellCB(XtPointer, XtIntervalId *id)
{
    (void) id;			// Use it
    assert (*id == recentering_tool_shell_timer);
    recentering_tool_shell_timer = 0;
}

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
	state(tool_shell) != PoppedUp)
	return;

#if LOG_MOVES
    std::clog << "Recentering tool to offset: " << right_offset 
	      << ", " << top_offset << "\n";
#endif

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

    last_top_offset     = top_offset;
    last_right_offset   = right_offset;
    offsets_initialized = true;

    if (recentering_tool_shell_timer)
	XtRemoveTimeOut(recentering_tool_shell_timer);

    recentering_tool_shell_timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(tool_shell), 
			500, RecenteredToolShellCB, XtPointer(0));
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
	state(tool_shell) != PoppedUp)
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

#if LOG_MOVES
    std::clog << "Current offset: " << x << ", " << y << "\n";
#endif

    top_offset   = y;
    right_offset = x;
    return true;
}
#endif // IF_MOTIF

// Store current offset of command tool in APP_DATA
void get_tool_offset()
{
#ifdef IF_MOTIF
    initialize_offsets();

    if (get_tool_offset(0, last_top_offset, last_right_offset))
    {
	app_data.tool_top_offset   = last_top_offset;
	app_data.tool_right_offset = last_right_offset;
    }
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning get_tool_offset not supported.
#endif
#endif // IF_MOTIF
}


// Manage paned child with minimum size

#ifdef IF_MOTIF
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
		  XmNnumChildren, &numChildren, XtPointer(0));

    // Fetch preferred sizes
    Cardinal i;
    for (i = 0; i < numChildren; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child))
	    continue;

	// Fetch preferred (= initial) height
	Dimension height = 0;
	XtVaGetValues(child, XmNheight, &height, XtPointer(0));
	{
	    XtWidgetGeometry size;
	    size.request_mode = CWHeight;
	    XtQueryGeometry(child, (XtWidgetGeometry *)0, &size);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	std::clog << XtName(paned) << ": child " << XtName(child) 
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
	RecenterToolShellCB();

	// Make sure the current command line is visible
	end_of_lineAct(gdb_w, 0, 0, 0);

	// Redisplay source and code glyphs
	source_view->update_glyphs();
    }
}
#endif // IF_MOTIF

#if defined(IF_XM)

void manage_paned_child(Widget w)
{
    Widget paned = XtParent(w);

    if (paned == 0 || !XmIsPanedWindow(paned) || XtIsManaged(w))
    {
	XtManageChild(w);
	return;
    }

#if LOG_GEOMETRY
    std::clog << XtName(paned) << ": managing child " << XtName(w) << "\n";
    if (preferred_sizes.has(w))
    {
	const MinMax& preferred_size = preferred_sizes[w];
	std::clog << XtName(paned) << ": child " << XtName(w) 
		  << " has preferred height " << preferred_size.max << '\n';
    }
#endif

    // Fetch children
    WidgetList children;
    Cardinal numChildren = 0;
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, XtPointer(0));


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
		      XtPointer(0));

#if LOG_GEOMETRY
	std::clog << XtName(paned) << ": child " << XtName(child) 
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
	    std::clog << XtName(paned) << ": child " << XtName(child) 
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
		  XmNnumChildren, &numChildren, XtPointer(0));

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
			  XtPointer(0));
	}
    }

    paned_changed(w);
}

#else

void manage_paned_child(GUI::Widget *w)
{
    static int errcnt = 0;
    if (complain && !errcnt++ == 0) std::cerr << "manage_paned_child() not implemented\n";
    GUI::MultiPaned::show_child(w);
}

#endif

#ifdef IF_MOTIF
// Return the number of resizable children of PANED
static int resizable_children(Widget paned)
{
    // Fetch children
    WidgetList children;
    Cardinal numChildren = 0;
    XtVaGetValues(paned, XmNchildren, &children, 
		  XmNnumChildren, &numChildren, XtPointer(0));

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
		      XtPointer(0));

	if (size.min < size.max)
	    n++;
    }

    return n;
}
#endif // IF_MOTIF

#if defined(IF_XM)

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
    std::clog << XtName(paned) << ": unmanaging child " << XtName(w) << "\n";
#endif

    Widget command = XtParent(gdb_w);

    if (resizable_children(paned) <= 2)
    {
	// Only two resizable children left - the remaining one must
	// be resized to a maximum.  Disable skipAdjust for a moment.
	Boolean skip_adjust = True;
	XtVaGetValues(command, XmNskipAdjust, &skip_adjust, XtPointer(0));
	XtVaSetValues(command, XmNskipAdjust, False, XtPointer(0));

	XtUnmanageChild(w);

	XtVaSetValues(command, XmNskipAdjust, skip_adjust, XtPointer(0));
    }
    else
    {
	// Resize the other child, but keep the command window intact
	Dimension height = 0;
	Dimension max = 1000;
	XtVaGetValues(command, 
		      XmNheight, &height,
		      XmNpaneMaximum, &max,
		      XtPointer(0));
	XtVaSetValues(command, XmNpaneMaximum, height, XtPointer(0));

	XtUnmanageChild(w);

	XtVaSetValues(command, XmNpaneMaximum, max, XtPointer(0));
    }

    paned_changed(w);
}

#else

// Unmanage W, but be sure the command window doesn't grow.
void unmanage_paned_child(GUI::Widget *w)
{
    static int errcnt = 0;
    if (complain && !errcnt++ == 0) std::cerr << "unmanage_paned_child() not implemented\n";
    GUI::MultiPaned::hide_child(w);
}

#endif

// Set the width of PANED to the maximum width of its children

#if defined(IF_XM)

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
		  XtPointer(0));

    for (Cardinal i = 0; i < num_children; i++)
    {
	Widget child = children[i];
	if (is_internal_paned_child(child) || !XtIsManaged(child))
	    continue;

	// Fetch preferred width
	XtWidgetGeometry size;
	size.request_mode = CWWidth;
	XtQueryGeometry(child, (XtWidgetGeometry *)0, &size);

#if LOG_GEOMETRY
	std::clog << XtName(paned) << ": child " << XtName(child)
		  << " wants width " << size.width << "\n";
#endif

	max_width = max(size.width, max_width);
    }
}

#else

// Fetch the maximum width.  Do this for each paned window.
void get_paned_window_width(GUI::MultiPaned *paned, int& max_width)
{
    std::cerr << "get_paned_window_width: stub\n";
    max_width = 0;
}

#endif

#if defined(IF_XM)

// Set the found value.
void set_paned_window_size(Widget paned, Dimension max_width)
{
    if (paned == 0 || !XtIsSubclass(paned, xmPanedWindowWidgetClass))
	return;

    XtVaSetValues(paned, XmNwidth, max_width, XtPointer(0));

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
		  XtPointer(0));

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
	XtVaGetValues(child, 
		      XmNwidth, &width, 
		      XmNheight, &height, 
		      XtPointer(0));

	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth | CWHeight;
	    XtQueryGeometry(child, (XtWidgetGeometry *)0, &size);

	    width  = max(width, size.width);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	std::clog << XtName(paned) << ": child " << XtName(child) 
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
		  XtPointer(0));
}

#else

// Set the found value.
void set_paned_window_size(GUI::Container *paned, int max_width)
{
    std::cerr << "set_paned_window_size: stub\n";
}

#endif

//-----------------------------------------------------------------------------
// Main Window stuff
//-----------------------------------------------------------------------------

#if defined(IF_XM)

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
		  XtPointer(0));

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
	XtVaGetValues(child, 
		      XmNwidth, &width, 
		      XmNheight, &height, 
		      XtPointer(0));

	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth | CWHeight;
	    XtQueryGeometry(child, (XtWidgetGeometry *)0, &size);

	    width  = max(width, size.width);
	    height = max(height, size.height);
	}

#if LOG_GEOMETRY
	std::clog << XtName(main) << ": child " << XtName(child) << " wants " 
		  << BoxSize(width, height) << "\n";
#endif

	max_width = max(width, max_width);
	total_height += height;
    }

    XtVaSetValues(main, 
		  XmNwidth, max_width, 
		  XmNheight, total_height, 
		  XtPointer(0));
}

#else

// Set main window size
void set_main_window_size(GUI::Container *main)
{
    std::cerr << "set_main_window_size::stub\n";
}

#endif

//-----------------------------------------------------------------------------
// Scrolled Window stuff
//-----------------------------------------------------------------------------

#if defined(IF_XM)

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
		  XtPointer(0));

    if (vertical_scroll_bar != 0)
    {
	XtWidgetGeometry size;
	size.request_mode = CWWidth;
	XtQueryGeometry(vertical_scroll_bar, (XtWidgetGeometry *)0, &size);
	scrollbar_width = size.width;
    }

    // Give the ScrolledWindow the size specified for its child
    XtWidgetGeometry size;
    size.request_mode = CWWidth | CWHeight;
    XtQueryGeometry(child, (XtWidgetGeometry *)0, &size);

    if (target == 0)
	target = scroll;

#if LOG_GEOMETRY
    std::clog << XtName(target) << ": child " << XtName(child) << " wants " 
	      << BoxSize(size.width, size.height) << "\n";
#endif

    Dimension border_width     = 1;
    Dimension shadow_thickness = 2;
    XtVaGetValues(child,
		  XmNborderWidth, &border_width,
		  XmNshadowThickness, &shadow_thickness,
		  XtPointer(0));

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

    XtVaSetValues(target, XmNwidth,  width, XmNheight, height, XtPointer(0));
}

#else

// Promote child size to scrolled window
void set_scrolled_window_size(GUI::ScrolledText *child, GUI::Widget *target)
{
#ifdef NAG_ME
#warning set_scrolled_window_size not implemented.
#endif
}

#endif

#ifndef IF_MOTIF

// ****************************************************************************

// GTK Replacements for Xt Widget stuff

int complain = 0;

// It is hardly ever necessary to realize a widget explicitly in Gtk.
Boolean XtIsRealized(Widget w)
{
    return w->is_realized();
}

void XtRealizeWidget(Widget w)
{
    gtk_widget_realize(w->gobj());
}

bool XtIsWidget(Widget w)
{
    return (dynamic_cast<Gtk::Widget *>(w) != NULL);
}

// FIXME: Distinguish these types.

Glib::RefPtr<Gdk::Window> XtWindow(Widget w)
{
    return w->get_window();
}

Glib::RefPtr<Gdk::Display> XtDisplay(Widget w)
{
    return w->get_display();
}

Glib::RefPtr<Gdk::Screen> XtScreen(Widget w)
{
    return w->get_screen();
}

bool XtIsShell(Widget w)
{
    return (dynamic_cast<Gtk::Window *>(w) != NULL);
}

bool XtIsTopLevelShell(Widget w)
{
    return (dynamic_cast<Gtk::Window *>(w) != NULL);
}

bool XtIsWMShell(Widget w)
{
    return (dynamic_cast<Gtk::Window *>(w) != NULL);
}

bool XmIsDialogShell(Widget w)
{
    return (dynamic_cast<Gtk::Dialog *>(w) != NULL);
}

bool
get_active(Widget w)
{
    // This convenience function is needed because some callbacks are associated
    // both with ToggleButton and with a CheckMenuItem.
    Gtk::ToggleButton *tb = dynamic_cast<Gtk::ToggleButton *>(w);
    if (tb)
	return tb->get_active();
    Gtk::CheckMenuItem *mi = dynamic_cast<Gtk::CheckMenuItem *>(w);
    if (mi)
	return mi->get_active();
    return false;
}

#endif // IF_MOTIF
