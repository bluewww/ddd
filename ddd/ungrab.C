// $Id$ -*- C++ -*-
// Ungrab frozen mouse pointer

// Copyright (C) 1997-1998 Technische Universitaet Braunschweig, Germany.
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

char ungrab_rcsid[] = 
    "$Id$";

#include "ungrab.h"

#include <stdlib.h>

#if defined(IF_XM)
#include <X11/Xlib.h>
#include <Xm/MessageB.h>
#endif

#include "gtk_wrapper.h"

#include "AppData.h"
#include "CodeCache.h"
#include "GDBAgent.h"
#include "TimeOut.h"
#include "assert.h"
#include "buttons.h"
#include "ddd.h"
#include "disp-read.h"
#include "file.h"
#include "post.h"
#include "question.h"
#include "status.h"
#include "string-fun.h"
#include "windows.h"
#include "verify.h"
#include "Command.h"
#include "HelpCB.h"
#include "charsets.h"
#include "wm.h"


//-----------------------------------------------------------------------------
// Checking grab state
//-----------------------------------------------------------------------------

// If we interrupt the debugged program while it has grabbed the
// pointer, disaster will occur, as the pointer will remain grabbed
// and users won't be able to to use their display.

// We use a second connection for grabbing and ungrabbing, such that
// we do not disturb the grab state of DDD.
#if defined(IF_XM)
static Display *grab_display = 0;
#else
static GUI::RefPtr<GUI::Display> grab_display;
#endif

#if defined(IF_XM)

// Return true iff pointer is grabbed.
static bool mouse_pointer_grabbed()
{
    if (!app_data.check_grabs)
    {
	if (grab_display != 0 && grab_display != XtDisplay(command_shell))
	{
	    XCloseDisplay(grab_display);
	    grab_display = 0;
	}
	return false;
    }

    if (grab_display == 0) {
	grab_display = XOpenDisplay(XDisplayString(XtDisplay(command_shell)));
    }
    if (grab_display == NO_DISPLAY)
	grab_display = XtDisplay(command_shell);

    // We check whether the pointer is grabbed by attempting to grab
    // it for ourselves.  If it's already grabbed, this may be by our
    // debuggee.

    // Warning! Debugging this function may cause your pointer to freeze!
    int grab_result = XGrabPointer(grab_display,
				   DefaultRootWindow(grab_display),
				   True, 0, GrabModeSync, GrabModeSync,
				   None, None, CurrentTime);
    if (grab_result == AlreadyGrabbed || grab_result == GrabFrozen)
	return true;

    XUngrabPointer(grab_display, CurrentTime);
    XFlush(grab_display);


    return false;
}

#else

// Return true iff pointer is grabbed.
static bool mouse_pointer_grabbed()
{
    if (!app_data.check_grabs)
    {
	std::cerr << "Warning: comparing displays!\n";
	if (grab_display != 0 && grab_display != command_shell->get_display())
	{
	    grab_display->close();
	    grab_display.clear();
	}
	return false;
    }

    if (!grab_display) {
	grab_display = GUI::Display::open(command_shell->get_display()->get_name());
    }
    if (!grab_display)
	grab_display = command_shell->get_display();

    // We check whether the pointer is grabbed by attempting to grab
    // it for ourselves.  If it's already grabbed, this may be by our
    // debuggee.

    // Warning! Debugging this function may cause your pointer to freeze!
    GUI::RefPtr<GUI::Screen> screen = grab_display->get_default_screen();
    GUI::RefPtr<GUI::XWindow> root_window = screen->get_root_window();
    GUI::Cursor *temp_cursor;
    // FIXME: Define temp_cursor?
    GUI::RefPtr<GUI::XWindow> confine;
    GUI::GrabStatus grab_result =
	root_window->pointer_grab(true, GUI::EventMask(0),
				  confine,
				  *temp_cursor, GDK_CURRENT_TIME);

    if (grab_result == GUI::GRAB_ALREADY_GRABBED || grab_result == GUI::GRAB_FROZEN)
	return true;

    grab_display->pointer_ungrab(GDK_CURRENT_TIME);
    grab_display->flush();


    return false;
}

#endif

// Return true iff the debuggee is a running X program
static bool running_x_program()
{
    ProgramInfo info;
    if (!info.running)
	return false;		// Not running

    // Every X program has an int variable named `_Xdebug'.
    string display_value = gdbValue("_Xdebug");

    if (display_value.empty())
	return false;		// No X program

    // We may check for active connections here (is the display
    // actually open?), but I don't think this is worth it...

    return true;		// Okay
}


//-----------------------------------------------------------------------------
// Automatic continuation
//-----------------------------------------------------------------------------

struct GrabCheckLocker {
    static bool lock;

    GrabCheckLocker()
    {
	assert(!lock);
	lock = true;
    }
    ~GrabCheckLocker()
    {
	assert(lock);
	lock = false;
    }
};

bool GrabCheckLocker::lock = false;

#if defined(IF_XM)

static void CheckUngrabCB(XtPointer client_data, XtIntervalId *)
{
    int& count = *((int *)client_data);

    // Don't delay further checks
    GrabCheckLocker lock;

    static Widget dialog = 0;
    if (dialog == 0)
    {
	dialog = 
	    verify(XmCreateWorkingDialog(find_shell(gdb_w), 
					 XMST("grab_warning"),
					 ArgList(0), 0));
	Delay::register_shell(dialog);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
    }

    if (!mouse_pointer_grabbed() && count > 0)
    {
	// All done - thanks!
	XtUnmanageChild(dialog);
	return;
    }

    MString msg = rm("The mouse pointer appears to be frozen.") + cr();

    string cmd = app_data.grab_action;
    if (is_quit_cmd(cmd))
	msg += rm("Quitting " + gdb->title());
    else if (cmd == "kill")
	msg += rm("Killing debuggee");
    else if (is_running_cmd(cmd))
	msg += rm("Continuing execution");
    else
	msg += rm("Taking action");

    msg += rm(" automatically");

    if (count == 0)
    {
	msg += rm(".");

	// Delay has passed -- continue debuggee.
	if (!cmd.contains("\n"))
	    cmd += "\n";

	while (!cmd.empty())
	{
	    string line = cmd.before('\n');
	    gdb_command(line);
	    cmd = cmd.after('\n');
	}
    }
    else
    {
	msg += rm(" in ") + tt(itostring(count)) + rm(" seconds.");
    }
    XtVaSetValues(dialog, XmNmessageString, msg.xmstring(), XtPointer(0));
    manage_and_raise(dialog);

    if (count > 0)
    {
	// Try again in 1s.
	count--;

	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 1000,
			CheckUngrabCB, XtPointer(&count));
    }
}

#else

static bool CheckUngrabCB(int *client_data)
{
    int& count = *((int *)client_data);

    // Don't delay further checks
    GrabCheckLocker lock;

    static GUI::Dialog *dialog = 0;
    static GUI::Label *dialog_label = 0;
    if (dialog == 0)
    {
	dialog = new GUI::Dialog(*find_shell1(gdb_w), "grab_warning");
	dialog_label = new GUI::Label(*dialog, "dialog_label", "Pointer grab");
	dialog_label->show();
	Delay::register_shell(dialog);
	GUI::Button *button;
	button = dialog->add_button("Cancel");
	std::cerr << "Connect to cancel...\n";
    }

    if (!mouse_pointer_grabbed() && count > 0)
    {
	// All done - thanks!
	dialog->hide();
	return false;
    }

    GUI::String msg = GUI::String("The mouse pointer appears to be frozen.\n");

    string cmd = app_data.grab_action;
    if (is_quit_cmd(cmd))
	msg += GUI::String("Quitting ") + GUI::String(gdb->title().chars());
    else if (cmd == "kill")
	msg += GUI::String("Killing debuggee");
    else if (is_running_cmd(cmd))
	msg += GUI::String("Continuing execution");
    else
	msg += GUI::String("Taking action");

    msg += GUI::String(" automatically");

    if (count == 0)
    {
	msg += GUI::String(".");

	// Delay has passed -- continue debuggee.
	if (!cmd.contains("\n"))
	    cmd += "\n";

	while (!cmd.empty())
	{
	    string line = cmd.before('\n');
	    gdb_command(line);
	    cmd = cmd.after('\n');
	}
    }
    else
    {
	msg += GUI::String(" in ") + GUI::String(itostring(count).chars()) + GUI::String(" seconds.");
    }
    dialog_label->set_text(msg.c_str());
    manage_and_raise(dialog);

    return (count > 0);
}

#endif

// Start grab check
#if defined(IF_XM)
static void CheckGrabCB(XtPointer client_data, XtIntervalId *)
#else
static bool CheckGrabCB(XtIntervalId *client_data)
#endif
{
    XtIntervalId *id = (XtIntervalId *)client_data;
    *id = NO_TIMER;

    // Don't delay further checks
    GrabCheckLocker lock;

    Delay delay;

    if (!mouse_pointer_grabbed())
	return MAYBE_FALSE;

    if (!running_x_program())
	return MAYBE_FALSE;		// The pointer is not grabbed by our debuggee

    static int count;
    count = app_data.grab_action_delay / 1000;
#if defined(IF_XM)
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
		    CheckUngrabCB, XtPointer(&count));
#else
    Glib::signal_idle().connect(sigc::bind(PTR_FUN(CheckUngrabCB), &count));
#endif
}

// Check for pointer grabs in a few seconds
void check_grabs(bool start)
{
    static XtIntervalId check_grab_timer = NO_TIMER;

    if (GrabCheckLocker::lock)
	return;

    if (check_grab_timer != NO_TIMER)
    {
#if defined(IF_XM)
	XtRemoveTimeOut(check_grab_timer);
#else
	check_grab_timer.disconnect();
#endif
	check_grab_timer = NO_TIMER;
    }

    if (!start)
        return;

#if defined(IF_XM)
    check_grab_timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			app_data.check_grab_delay,
			CheckGrabCB, XtPointer(&check_grab_timer));
#else
    check_grab_timer = 
	Glib::signal_timeout().connect(sigc::bind(PTR_FUN(CheckGrabCB),
						  &check_grab_timer),
				       app_data.check_grab_delay);
#endif
}
