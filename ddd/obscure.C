// $Id$ -*- C++ -*-
// Obscure features

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

// For the `meltdown' function, the following terms hold:
//
// Copyright 1990 David Lemke and Network Computing Devices
//
// Permission to use, copy, modify, distribute, and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation, and that the name of Network
// Computing Devices not be used in advertising or publicity
// pertaining to distribution of the software without specific,
// written prior permission.  Network Computing Devices makes no
// representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied
// warranty.
//
// NETWORK COMPUTING DEVICES DISCLAIMS ALL WARRANTIES WITH REGARD TO
// THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES BE LIABLE
// FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
// AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
// OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
// SOFTWARE.
//
// Author:  	
//		Dave Lemke
//		lemke@ncd.com
//
//		Network Computing Devices, Inc
//		350 North Bernardo Ave
//		Mountain View, CA 94043
//
//		@(#)meltdown.c	1.2	90/02/22


const char obscure_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "obscure.h"

#include "config.h"
#include "Delay.h"
#include "HelpCB.h"
#include "MString.h"
#include "commandQ.h"
#include "ddd.h"
#include "verify.h"
#include "windows.h"
#include "wm.h"
#include "misc.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

extern "C" {
#if defined(HAVE_RANDOM) && !defined(HAVE_RANDOM_DECL) && !defined(random)
long int random();
#endif
#if defined(HAVE_SRANDOM) && !defined(HAVE_SRANDOM_DECL) && !defined(srandom)
void srandom(unsigned int seed);
#endif
#if defined(HAVE_RAND) && !defined(HAVE_RAND_DECL) && !defined(rand)
int rand();
#endif
#if defined(HAVE_SRAND) && !defined(HAVE_SRAND_DECL) && !defined(srand)
void srand(unsigned int seed);
#endif
};

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

//-----------------------------------------------------------------------------
// Obscure Features
//-----------------------------------------------------------------------------

static void meltdown(Display *dpy);

static bool dungeon_collapsed = false;

static void DungeonCollapseCB(XtPointer client_data, XtIntervalId *)
{
    Widget w = Widget(client_data);

    static Widget dungeon_error = 0;
    if (dungeon_error)
	XtDestroyWidget(dungeon_error);

    Widget shell = find_shell(w);
    dungeon_error = 
	verify(XmCreateErrorDialog(shell, "dungeon_collapse_error", NULL, 0));
    Delay::register_shell(dungeon_error);
    XtUnmanageChild(XmMessageBoxGetChild 
		    (dungeon_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(dungeon_error, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext("Suddenly, the dungeon collapses.", "rm");
    XtVaSetValues (dungeon_error,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild(dungeon_error);
    wait_until_mapped(dungeon_error);

    sleep(2);

    meltdown(XtDisplay(w));

    dungeon_collapsed = true;
}

#define CMP(x, y, z) (cmd[x] == cmd[y] + (z))

void handle_obscure_commands(string& cmd, Widget origin)
{
    // The DDD Display Dungeon is a mythical entity where all deleted
    // displays of all DDD instantiations go.  Entering the display
    // dungeon reveals the internals of all programs ever having been
    // debugged with DDD.  Be aware, however, that this is not for the
    // faint-hearted; it requires special magical energy and a
    // supremous power of will to enter the dungeon.  Attempts to
    // force its entry without being sufficiently prepared may cause
    // the dungeon to collapse, taking the adventurer and all of its
    // possessions into debris.                             -- AZ

    if (cmd.length() == 5
	&& CMP(1, 0, 1) && CMP(3, 0, 2)	&& CMP(4, 3, -1) && CMP(2, 1, 1)
	&& cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + '<' == 666)
    {
	if (dungeon_collapsed)
	{
	    cmd = "echo Nothing happens.\\n";
	}
	else
	{
	    cmd = "echo The dungeon shakes violently! \\n";

	    static int dungeon_collapsing = 0;
	    if (dungeon_collapsing++ == 0)
	    {
		Widget w = origin ? origin : command_shell;

		XtAppAddTimeOut(XtWidgetToApplicationContext(w), 10000, 
				DungeonCollapseCB, 
				XtPointer(w));
	    }
	}
    }
}



// Melt down screen

const int MIN_SIZE = 10;
const int MAX_SIZE = 100;

const int MIN_DIST = 10;

const int MIN_WIDTH = 30;
const int WIDTH_ADD = 20;

const int FINISHED = 50;

#ifdef HAVE_RAND
inline int rnd(int x) { return rand() % x; }
#else /* HAVE_RANDOM */
inline int rnd(int x) { return random() % x; }
#endif

static void meltdown(Display *dpy)
{
    int screen = DefaultScreen(dpy);

    time_t tm;
    time(&tm);
#ifdef HAVE_SRAND
    srand((int)tm);
#elif HAVE_SRANDOM
    srandom((int)tm);
#endif

    XSetWindowAttributes xswat;
    xswat.override_redirect = True;
    xswat.do_not_propagate_mask = KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask;

    unsigned long vmask;
    vmask = CWOverrideRedirect | CWDontPropagate;
    Window win = 
	XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0, 
		      DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
		      0, CopyFromParent, CopyFromParent, CopyFromParent,
		      vmask, &xswat);
    Cursor cursor = XCreateFontCursor(dpy, XC_top_left_arrow);
    XDefineCursor(dpy, win, cursor);
    XMapWindow(dpy, win);

    XGCValues	gcvals;
    gcvals.graphics_exposures = False;
    // copyplane gc wants to leave the data alone
    gcvals.foreground = 1;
    gcvals.background = 0;
    GC copygc = XCreateGC(dpy, win, 
			  GCForeground | GCBackground | GCGraphicsExposures,
			  &gcvals);

    gcvals.foreground = BlackPixel(dpy, screen);
    GC fillgc = XCreateGC(dpy, win, GCForeground, &gcvals);


    XSync(dpy, 0);
    sleep(1);

    short *heights = new short[DisplayWidth(dpy, screen)];
    int i;
    for (i = 0; i < DisplayWidth(dpy, screen); i++)
	heights[i] = 0;

    int	finished = 0;
    for (;;) {
	int width = rnd(MIN_WIDTH) + WIDTH_ADD;

	// give values near edges a better chance
	int xloc = rnd(DisplayWidth(dpy, screen) + MIN_WIDTH) - MIN_WIDTH;

	if ((xloc + width) > DisplayWidth(dpy, screen))
	    xloc = DisplayWidth(dpy, screen) - width;
	else if (xloc < 0)
	    xloc = 0;

	int yloc = DisplayHeight(dpy, screen);
	for (i = xloc; i < (xloc + width); i++)	{
	    yloc = min(yloc, heights[i]);
	}
	if (yloc == DisplayHeight(dpy, screen))
	    continue;

	int dist = rnd(yloc/10 + MIN_DIST);
	int size = rnd(max(yloc + MIN_SIZE, MAX_SIZE));

	XCopyArea(dpy, win, win, copygc, 
		  xloc, yloc, 
		  width, size,
		  xloc, yloc + dist);
	XFillRectangle(dpy, win, fillgc, 
		       xloc, yloc, 
		       width, dist);
	yloc += dist;
	for (i = xloc; i < (xloc + width); i++)	{
	    if ((heights[i] < (DisplayHeight(dpy, screen) - MIN_SIZE))
		&& (yloc >= (DisplayHeight(dpy, screen) - MIN_SIZE)))
		finished++;

	    heights[i] = max(heights[i], yloc);
	}
	if (finished >= (DisplayWidth(dpy, screen) - FINISHED))	{
	    XSync(dpy, 0);
	    break;
	}
    }

    sleep(5);

    // Cleanup
    delete[] heights;
    XFreeGC(dpy, copygc);
    XFreeGC(dpy, fillgc);
    XFreeCursor(dpy, cursor);
    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);

    return;
}
