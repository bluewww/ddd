// $Id$
// Simple grab tester -- debug this with DDD and `checkGrab' enabled.

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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


#include <X11/Xlib.h>
#include <unistd.h>		// sleep()
#include <stdlib.h>		// atoi()
#include <sys/types.h>
#include <iostream.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay((char *)0);
    if (display == 0)
    {
	cerr << "Cannot open display\n";
	exit(1);
    }
    Window window = DefaultRootWindow(display);

    int seconds = 5;
    if (argc > 1 && atoi(argv[1]) > 0)
	seconds = atoi(argv[1]);
    
    cout << "Grabbing...";
    cout.flush();

    // We pass GrabModeAsync for the keyboard such that it will keep
    // on working.
    int grab_result = XGrabPointer(display, window,
				   True, 0, GrabModeSync, GrabModeAsync,
				   None, None, CurrentTime);

    // Placing a breakpoint
    //
    // <====== here
    //
    // will interrupt this function while the pointer is grabbed.
    // Rely on DDD `checkGrab' to ungrab it.

    switch (grab_result)
    {
    case GrabSuccess:
	cout << "GrabSuccess\n";
	cout << "Sleeping for " << seconds << " seconds...";
	cout.flush();

	sleep(seconds);
	XUngrabPointer(display, CurrentTime);
	XFlush(display);

	cout << "done.\n";
	break;

    case GrabNotViewable:
	cout << "GrabNotViewable\n";
	break;

    case AlreadyGrabbed:
	cout << "AlreadyGrabbed\n";
	break;

    case GrabFrozen:
	cout << "GrabFrozen\n";
	break;

    case GrabInvalidTime:
	cout << "GrabInvalidTime\n";
	break;

    default:
	cout << grab_result << "\n";
	break;
    }

    exit(0);
}
