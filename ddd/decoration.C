// $Id$ -*- C++ -*-
// Check for WM decoration

// Copyright (C) 1999 Technische Universitaet Braunschweig, Germany.
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

char decoration_rcsid[] = 
    "$Id$";

#include "decoration.h"

#include <unistd.h>		// sleep()
#include <Xm/Xm.h>
#include <Xm/DialogS.h>

#include <Xm/Label.h>

#include "DestroyCB.h"
#include "MString.h"
#include "frame.h"
#include "strclass.h"
#include "verify.h"
#include "version.h"
#include "wm.h"


//-----------------------------------------------------------------------------
// Decoration
//-----------------------------------------------------------------------------

static Widget init_label, init_shell;

// Return a transient position on SCREEN (for command tool etc.) in POS_X/POS_Y
void get_transient_pos(Screen *screen, Position& pos_x, Position& pos_y)
{
#if 0
    // Use lower right corner.
    pos_x = WidthOfScreen(screen) - 1;
    pos_y = HeightOfScreen(screen) - 1;
#else
    (void) screen;           // Use it     
    // This loses on some window managers; upper left corner is safer.
    pos_x = 0;
    pos_y = 0;
#endif
}


// Check if window manager decorates transients
void start_have_decorated_transients(Widget parent)
{
    Position pos_x, pos_y;
    get_transient_pos(XtScreen(parent), pos_x, pos_y);

    std::ostringstream os;
    os << "+" << pos_x << "+" << pos_y;
    string geometry(os);

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
    XtSetArg(args[arg], XmNx, pos_x);                   arg++;
    XtSetArg(args[arg], XmNy, pos_y);                   arg++;
    init_shell = verify(XmCreateDialogShell(parent, 
					    XMST("init_shell"), args, arg));

    arg = 0;
    MString label(DDD_NAME " " DDD_VERSION);
    XtSetArg(args[arg], XmNlabelString, label.xmstring()); arg++;
    init_label = verify(XmCreateLabel(init_shell, 
				      XMST(ddd_NAME), args, arg));
    XtManageChild(init_label);
}

bool have_decorated_transients(Widget parent)
{
    if (init_label == 0 || init_shell == 0)
	start_have_decorated_transients(parent);

    wait_until_mapped(init_label, init_shell);
    XmUpdateDisplay(init_label);

    XWindowAttributes shell_attributes;
    XGetWindowAttributes(XtDisplay(init_shell), XtWindow(init_shell), 
			 &shell_attributes);

#if 0
    std::clog << "shell window: " << XtWindow(init_shell)
	      << ", size: " << BoxPoint(shell_attributes.width, 
					shell_attributes.height) << "\n";
#endif

    // Wait up to 5 seconds until WM has decorated the init shell.
    // Problem: If we have no WM or a non-decorating WM, this delays
    // DDD for 5 seconds; this can be avoided by using an explicit
    // `decorateTool' resource value.
    Window frame_window = 0;
    for (int trial = 1; trial < 5; trial++)
    {
	frame_window = frame(XtDisplay(init_shell), XtWindow(init_shell));
	if (frame_window != XtWindow(init_shell))
	    break;
	XSync(XtDisplay(init_label), False);
	sleep(1);
    }

    XWindowAttributes frame_attributes;
    XGetWindowAttributes(XtDisplay(init_shell), frame_window,
			 &frame_attributes);

#if 0
    std::clog << "frame window: " << frame_window 
	      << ", size: " << BoxPoint(frame_attributes.width, 
					frame_attributes.height) << "\n";
#endif

    XtUnmapWidget(init_shell);
    DestroyWhenIdle(init_shell);

    // If the border supplied by the window manager border is more
    // than 5 pixels higher than wider, assume we have some kind of
    // title bar - the shell is decorated.
    int border_height = frame_attributes.height - shell_attributes.height;
    int border_width  = frame_attributes.width  - shell_attributes.width;

    return border_height - border_width > 5;
}
