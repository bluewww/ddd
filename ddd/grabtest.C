// $Id$
// Simple grab tester -- debug this with DDD and `checkGrab' enabled.

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.


#include <X11/Xlib.h>
#include <unistd.h>		// sleep()
#include <stdlib.h>		// atoi()
#include <sys/types.h>
#include <iostream>
#include <signal.h>

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay((char *)0);
    if (display == 0)
    {
	std::cerr << "Cannot open display\n";
	exit(1);
    }
    Window window = DefaultRootWindow(display);

    int seconds = 5;
    if (argc > 1 && atoi(argv[1]) > 0)
	seconds = atoi(argv[1]);
    
    std::cout << "Grabbing...";
    std::cout.flush();

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
	std::cout << "GrabSuccess\n";
	std::cout << "Sleeping for " << seconds << " seconds...";
	std::cout.flush();

	sleep(seconds);
	XUngrabPointer(display, CurrentTime);
	XFlush(display);

	std::cout << "done.\n";
	break;

    case GrabNotViewable:
	std::cout << "GrabNotViewable\n";
	break;

    case AlreadyGrabbed:
	std::cout << "AlreadyGrabbed\n";
	break;

    case GrabFrozen:
	std::cout << "GrabFrozen\n";
	break;

    case GrabInvalidTime:
	std::cout << "GrabInvalidTime\n";
	break;

    default:
	std::cout << grab_result << "\n";
	break;
    }

    exit(0);
}
