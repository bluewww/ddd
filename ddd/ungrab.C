// $Id$ -*- C++ -*-
// Ungrab frozen mouse pointer

// Copyright (C) 1997-1998 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char ungrab_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ungrab.h"

#include <X11/Xlib.h>
#include <stdlib.h>

#include "AppData.h"
#include "ddd.h"
#include "disp-read.h"
#include "string-fun.h"
#include "windows.h"
#include "status.h"
#include "post.h"
#include "question.h"
#include "GDBAgent.h"
#include "CodeCache.h"


//-----------------------------------------------------------------------------
// Pointer grabbing and ungrabbing
//-----------------------------------------------------------------------------

// If we interrupt the debugged program while it has grabbed the
// pointer, disaster will occur, as the pointer will remain grabbed
// and users won't be able to to use their display.

// We use a second connection for grabbing and ungrabbing, such that
// we do not disturb the grab state of DDD.
static Display *grab_display = 0;

// Return True if pointer is grabbed.
static bool mouse_pointer_grabbed()
{
    if (!app_data.ungrab_mouse_pointer)
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
				   True, 0, GrabModeAsync, GrabModeAsync,
				   None, None, CurrentTime);

    if (grab_result == AlreadyGrabbed || grab_result == GrabFrozen)
	return true;

    XUngrabPointer(grab_display, CurrentTime);
    XFlush(grab_display);

    return false;
}

// Return the GDB value of EXPR.
static string gdb_value(const string& expr)
{
    string val = gdb_question(gdb->print_command(expr));
    if (is_invalid(val))
	return "";

    val = get_disp_value_str(val, gdb);
    if (val.contains('('), 0)
	val = val.after(')');

    read_leading_blanks(val);
    strip_final_blanks(val);
    return val;
}

// Attempt to make debugged program ungrab pointer.
void ungrab_mouse_pointer()
{
    if (!gdb->has_system_calls())
	return;			// No way to ungrab pointer

    if (!mouse_pointer_grabbed())
	return;			// No grab

    Delay d;

    // Someone has grabbed the pointer.  Check if it's the debuggee.

    // Check if the debuggee has an X display.
    string display_symbol = "_XHeadOfDisplayList";
    string display_value = gdb_value(display_symbol);
    if (display_value == "")
	return;			// No X display
    if (strtol(display_value, NULL, 0) == 0)
	return;			// X display not open yet

    if (!mouse_pointer_grabbed())
	return;			// No more grab

    // Check if the debuggee can re-grab the pointer.
    string grab_result_s = 
	gdb_value("XGrabPointer("
		  + display_symbol
		  + ", XDefaultRootWindow(" + display_symbol + ")"
		  + ", " + itostring(True)
		  + ", " + itostring(0)
		  + ", " + itostring(GrabModeAsync)
		  + ", " + itostring(GrabModeAsync)
		  + ", " + itostring(None)
		  + ", " + itostring(None)
		  + ", " + itostring(CurrentTime)
		  + ")");
    if (grab_result_s == "")
	return;			// Something went wrong
    int grab_result = strtol(grab_result_s, NULL, 0);
    if (grab_result == AlreadyGrabbed || grab_result == GrabFrozen)
	return;			// Someone else grabbed it

    if (!mouse_pointer_grabbed())
	return;			// No more grab

    // Okay, looks like the debuggee has grabbed the mouse pointer.
    // Make it ungrab the pointer again.

    // Vadim Rogozin  <dima@mercury.co.il> suggests to call from GDB:
    //
    //     call XUngrabPointer(_XHeadOfDisplayList, 0)
    //     call XFlush(_XHeadOfDisplayList)
    //
    // He checked it for Solaris and it does work.

    // We use `print' instead of `call' and `XSync' instead of `XFlush'.
    // Normally, these two calls should not harm the debuggee.

    StatusDelay delay("Ungrabbing mouse pointer");

    string ungrab_result_s = 
	gdb_value("XUngrabPointer(" + display_symbol + ", " 
		  + itostring(CurrentTime) + ")");
    string sync_result_s =
	gdb_value("XSync(" + display_symbol + ", " + itostring(True) + ")");

    if (is_invalid(ungrab_result_s) || is_invalid(sync_result_s))
	delay.outcome = "failed";
}
