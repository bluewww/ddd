// $Id$ -*- C++ -*-
// Ungrab frozen mouse pointer

// Copyright (C) 1997-1998 Technische Universitaet Braunschweig, Germany.
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

char ungrab_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ungrab.h"

#include <X11/Xlib.h>
#include <stdlib.h>
#include <Xm/MessageB.h>

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
static Display *grab_display = 0;

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

    if (grab_display == 0)
	grab_display = XOpenDisplay(XDisplayString(XtDisplay(command_shell)));
    if (grab_display == 0)
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

// Return true iff the debuggee is a running X program
static bool running_x_program()
{
    ProgramInfo info;
    if (!info.running)
	return false;		// Not running

    // Every X program has an int variable named `_Xdebug'.
    string display_value = gdbValue("_Xdebug");

    if (display_value == "")
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

static void CheckUngrabCB(XtPointer client_data, XtIntervalId *)
{
    int& count = *((int *)client_data);

    // Don't delay further checks
    GrabCheckLocker lock;

    static Widget dialog = 0;
    if (dialog == 0)
    {
	dialog = 
	    verify(XmCreateWorkingDialog(find_shell(gdb_w), "grab_warning",
					 NULL, 0));
	Delay::register_shell(dialog);
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
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
    else if (is_running_cmd(cmd, gdb))
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

	while (cmd != "")
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

// Start grab check
static void CheckGrabCB(XtPointer client_data, XtIntervalId *)
{
    XtIntervalId *id = (XtIntervalId *)client_data;
    *id = 0;

    // Don't delay further checks
    GrabCheckLocker lock;

    Delay delay;

    if (!mouse_pointer_grabbed())
	return;

    if (!running_x_program())
	return;			// The pointer is not grabbed by our debuggee

    static int count;
    count = app_data.grab_action_delay / 1000;
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
		    CheckUngrabCB, XtPointer(&count));
}

// Check for pointer grabs in a few seconds
void check_grabs(bool start)
{
    static XtIntervalId check_grab_timer = 0;

    if (GrabCheckLocker::lock)
	return;

    if (check_grab_timer != 0)
    {
	XtRemoveTimeOut(check_grab_timer);
	check_grab_timer = 0;
    }

    if (!start)
        return;

    check_grab_timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
			app_data.check_grab_delay,
			CheckGrabCB, XtPointer(&check_grab_timer));
}
