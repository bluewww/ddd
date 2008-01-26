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

#ifdef IF_MOTIF
#include <X11/Xlib.h>
#include <Xm/MessageB.h>
#endif // IF_MOTIF

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
static DISPLAY_P grab_display = NO_DISPLAY;

// Return true iff pointer is grabbed.
static bool mouse_pointer_grabbed()
{
    if (!app_data.check_grabs)
    {
	if (grab_display != 0 && grab_display != XtDisplay(command_shell))
	{
#ifdef IF_MOTIF
	    XCloseDisplay(grab_display);
#else // NOT IF_MOTIF
	    grab_display->close();
#endif // IF_MOTIF
	    grab_display = NO_DISPLAY;
	}
	return false;
    }

    if (grab_display == NO_DISPLAY) {
#ifdef IF_MOTIF
	grab_display = XOpenDisplay(XDisplayString(XtDisplay(command_shell)));
#else // NOT IF_MOTIF
	grab_display = Gdk::Display::open(command_shell->get_display()->get_name());
#endif // IF_MOTIF
    }
    if (grab_display == NO_DISPLAY)
	grab_display = XtDisplay(command_shell);

    // We check whether the pointer is grabbed by attempting to grab
    // it for ourselves.  If it's already grabbed, this may be by our
    // debuggee.

    // Warning! Debugging this function may cause your pointer to freeze!
#ifdef IF_MOTIF
    int grab_result = XGrabPointer(grab_display,
				   DefaultRootWindow(grab_display),
				   True, 0, GrabModeSync, GrabModeSync,
				   None, None, CurrentTime);
    if (grab_result == AlreadyGrabbed || grab_result == GrabFrozen)
	return true;

    XUngrabPointer(grab_display, CurrentTime);
    XFlush(grab_display);
#else // NOT IF_MOTIF
    Glib::RefPtr<Gdk::Screen> screen = grab_display->get_default_screen();
    Glib::RefPtr<Gdk::Window> root_window = screen->get_root_window();
    Gdk::Cursor temp_cursor;
    // FIXME: Define temp_cursor?
    Gdk::GrabStatus grab_result =
	root_window->pointer_grab(true, Gdk::EventMask(0), NO_WINDOW, temp_cursor, GDK_CURRENT_TIME);

    if (grab_result == Gdk::GRAB_ALREADY_GRABBED || grab_result == Gdk::GRAB_FROZEN)
	return true;

    grab_display->pointer_ungrab(GDK_CURRENT_TIME);
    grab_display->flush();
#endif // IF_MOTIF


    return false;
}

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

#ifdef IF_MOTIF
static void CheckUngrabCB(XtPointer client_data, XtIntervalId *)
#else // NOT IF_MOTIF
static bool CheckUngrabCB(int *client_data)
#endif // IF_MOTIF
{
    int& count = *((int *)client_data);

    // Don't delay further checks
    GrabCheckLocker lock;

    static DIALOG_P dialog = 0;
#ifndef IF_MOTIF
    static LABEL_P dialog_label = 0;
#endif // IF_MOTIF
    if (dialog == 0)
    {
#ifdef IF_MOTIF
	dialog = 
	    verify(XmCreateWorkingDialog(find_shell(gdb_w), 
					 XMST("grab_warning"),
					 ArgList(0), 0));
#else // NOT IF_MOTIF
	dialog = new Gtk::Dialog(XMST("grab_warning"), *find_shell(gdb_w));
	dialog_label = new Gtk::Label(XMST("Pointer grab"));
	dialog_label->show();
	dialog->get_vbox()->pack_start(*dialog_label, Gtk::PACK_SHRINK);
#endif // IF_MOTIF
	Delay::register_shell(dialog);
#ifdef IF_MOTIF
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
#else // NOT IF_MOTIF
	Gtk::Button *button;
	button = dialog->add_button(XMST("Cancel"), Gtk::RESPONSE_CANCEL);
#endif // IF_MOTIF
    }

    if (!mouse_pointer_grabbed() && count > 0)
    {
	// All done - thanks!
	XtUnmanageChild(dialog);
#ifdef IF_MOTIF
	return;
#else // NOT IF_MOTIF
	return false;
#endif // IF_MOTIF
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
#ifdef IF_MOTIF
    XtVaSetValues(dialog, XmNmessageString, msg.xmstring(), XtPointer(0));
#else // NOT IF_MOTIF
    dialog_label->set_text(msg.xmstring());
#endif // IF_MOTIF
    manage_and_raise(dialog);

#ifdef IF_MOTIF    
    if (count > 0)
    {
	// Try again in 1s.
	count--;

	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 1000,
			CheckUngrabCB, XtPointer(&count));
    }
#else // NOT IF_MOTIF
    return (count > 0);
#endif // IF_MOTIF
}

// Start grab check
#ifdef IF_MOTIF
static void CheckGrabCB(XtPointer client_data, XtIntervalId *)
#else // NOT IF_MOTIF
static bool CheckGrabCB(XtIntervalId *client_data)
#endif // IF_MOTIF
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
#ifdef IF_MOTIF
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
		    CheckUngrabCB, XtPointer(&count));
#else // NOT IF_MOTIF
    Glib::signal_idle().connect(sigc::bind(PTR_FUN(CheckUngrabCB), &count));
#endif // IF_MOTIF
}

// Check for pointer grabs in a few seconds
void check_grabs(bool start)
{
    static XtIntervalId check_grab_timer = NO_TIMER;

    if (GrabCheckLocker::lock)
	return;

    if (check_grab_timer != NO_TIMER)
    {
#ifdef IF_MOTIF
	XtRemoveTimeOut(check_grab_timer);
#else // NOT IF_MOTIF
	check_grab_timer.disconnect();
#endif // IF_MOTIF
	check_grab_timer = NO_TIMER;
    }

    if (!start)
        return;

#ifdef IF_MOTIF
    check_grab_timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			app_data.check_grab_delay,
			CheckGrabCB, XtPointer(&check_grab_timer));
#else // NOT IF_MOTIF
    check_grab_timer = 
	Glib::signal_timeout().connect(sigc::bind(PTR_FUN(CheckGrabCB),
						  &check_grab_timer),
				       app_data.check_grab_delay);
#endif // IF_MOTIF
}
