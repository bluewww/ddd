// $Id$ -*- C++ -*-
// Swallower demo - swallow a given X client

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

char swallow_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "findWindow.h"
#include "Swallower.h"

#include <stdlib.h>
#include <iostream.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

static void GoneCB(Widget, XtPointer, XtPointer)
{
    // Swallowed window has gone
    exit(EXIT_SUCCESS);
}

static void CreatedCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    // New window has been created
    SwallowerInfo *info = (SwallowerInfo *)call_data;
    String name = String(client_data);

    // We must search for a child since the WM may have added decorations
    Window window = findWindow(XtDisplay(w), info->window, name);
    if (window != None)
    {
	cout << "Using window " << (void *)window << "\n";

	// We got our window!
	XtVaSetValues(w, XtNwindow, window, NULL);
	XtRealizeWidget(XtParent(w));
	XtRemoveCallback(w, XtNwindowCreatedCallback, CreatedCB, client_data);
    }
}

int main(int argc, char *argv[])
{
    Arg args[10];
    Cardinal arg;
    XtAppContext app_context;

    if (argc < 2)
    {
	cerr << argv[0] << ": usage: " << argv[0] << " window-name\n";
	exit(EXIT_FAILURE);
    }

    // Initialize it all
    arg = 0;
    Widget toplevel = XtAppInitialize(&app_context, "Swallow",
        XtPointer(0), 0, &argc, argv, 0, args, arg);

    String name  = argv[1];
    Window child = findWindow(toplevel, name);
    arg = 0;
    XtSetArg(args[arg], XtNwindow, child); arg++;
    Widget swallower = 
	XtCreateManagedWidget("swallower", swallowerWidgetClass, 
			      toplevel, args, arg);
    XtAddCallback(swallower, XtNwindowGoneCallback, GoneCB, XtPointer(0));

    if (child == None)
    {
	cout << "Waiting for " << argv[1] << " to be created\n";
	XtAddCallback(swallower, XtNwindowCreatedCallback, CreatedCB,
		      XtPointer(argv[1]));
    }
    else
    {
	cout << "Using window " << (void *)child << '\n';
	XtRealizeWidget(toplevel);
    }

    for (;;)
    {
	XEvent event;
	XtAppNextEvent(app_context, &event);
	SwallowerCheckEvents();
	XtDispatchEvent(&event);
    }

    return EXIT_SUCCESS;
}
