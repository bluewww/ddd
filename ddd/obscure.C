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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char obscure_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "obscure.h"

#include "config.h"
#include "Delay.h"
#include "HelpCB.h"
#include "DestroyCB.h"
#include "MString.h"
#include "commandQ.h"
#include "ddd.h"
#include "verify.h"
#include "windows.h"
#include "wm.h"
#include "misc.h"
#include "GDBAgent.h"
#include "post.h"
#include "status.h"
#include "logo.h"
#include "Delay.h"

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
#include <Xm/Xm.h>
#include <Xm/MessageB.h>

//-----------------------------------------------------------------------------
// Obscure Features
//-----------------------------------------------------------------------------

static void meltdown(Display *dpy, const WidgetArray& ws);

static bool dungeon_collapsed = false;

static void DungeonCollapseCB(XtPointer client_data, XtIntervalId *)
{
    static Widget dungeon_error = 0;
    if (dungeon_error)
	DestroyWhenIdle(dungeon_error);

    Widget w = (Widget)client_data;
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

    WidgetArray ws;
    for (int i = Delay::shells().size() - 1; i >= 0; i--)
	ws += Delay::shells()[i];
    ws += Widget(-1);

    meltdown(XtDisplay(w), ws);

    dungeon_collapsed = true;
}

static void LetDungeonCollapseCB(Widget w, XtPointer, XtPointer)
{
    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 5000, DungeonCollapseCB,
		    XtPointer(w));
}

#ifdef HAVE_RAND
inline int rnd(int x) { return rand() % x; }
#else /* HAVE_RANDOM */
inline int rnd(int x) { return random() % x; }
#endif

static void init_random_seed()
{
    static bool seed_initialized = false;
    if (seed_initialized)
	return;

    time_t tm;
    time(&tm);

#ifdef HAVE_SRAND
    srand((int)tm);
#elif defined(HAVE_SRANDOM)
    srandom((int)tm);
#endif

    seed_initialized = true;
}

#define CMP(x, y, z) (cmd[x] == cmd[y] + (z))

void handle_obscure_commands(string& cmd, Widget origin)
{
    if (cmd.length() == 5
	&& CMP(1, 0, 1) && CMP(3, 0, 2)	&& CMP(4, 3, -1) && CMP(2, 1, 1)
	&& cmd[0] + cmd[1] + cmd[2] + cmd[3] + cmd[4] + '<' == 666)
    {
	init_random_seed();
	string msg;
	string name;

	switch (dungeon_collapsed ? 0 : rnd(5))
	{
	default:
	    msg = "Nothing happens.";
	    break;
	case 1:
	    msg  = "Welcome to adventure!";
	    name = "adventure";
	    break;
	case 2:
	    msg  = "Welcome to `Hunt the Wumpus.'";
	    name = "wumpus";
	    break;
	}

	if (msg != "")
	{
	    _gdb_out(msg + '\n');
	    set_status(msg);
	    cmd = "";
	}

	if (name != "")
	{
	    static Widget dialog = 0;
	    if (dialog != 0)
		DestroyWhenIdle(dialog);

	    Widget w = origin ? origin : command_shell;
	    Widget shell = find_shell(w);

	    dialog = verify(XmCreateQuestionDialog(shell, name, NULL, 0));
	    Delay::register_shell(dialog);
	    XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
	    XtAddCallback(dialog, XmNokCallback, LetDungeonCollapseCB, NULL);
	    XtManageChild(dialog);
	}
    }
}


// The following terms hold for the remainder of this code:
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

// Melt down screen

const int MIN_SIZE = 5;		// 10 - original value for entire screen
const int MAX_SIZE = 10;	// 20

const int MIN_DIST = 5;		// 10

const int MIN_WIDTH = 15;	// 30
const int WIDTH_ADD = 10;	// 20

const int FINISHED = 50;	// 50

// Meltdown the windows of the widgets in WS.  A widget value of 0 is
// ignored; a widget value of (Widget)-1 means to melt down the root
// window.
static void meltdown(Display *dpy, const WidgetArray& ws)
{
    Delay delay;
    Cursor cursor = logocursor(command_shell);

    int wi;
    for (wi = 0; wi < ws.size(); wi++)
    {
	// Setup logo cursors.  These will be restored as the delay goes.
	Widget w = ws[wi];
	if (w != 0 && w != Widget(-1) && XtIsRealized(w))
	    XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
    }

    init_random_seed();

    Window *windows = new Window[ws.size()];
    for (wi = 0; wi < ws.size(); wi++)
	windows[wi] = 0;

    for (wi = 0; wi < ws.size(); wi++)
    {
	int screen   = DefaultScreen(dpy);
	Window src   = RootWindow(dpy, screen);
	Widget w     = ws[wi];

	if (w == 0)
	    continue;
	if (w != Widget(-1))
	{
	    if (!XtIsRealized(w))
		continue;

	    screen = XScreenNumberOfScreen(XtScreen(w));
	    src    = XtWindow(w);
	}

	XWindowAttributes attr;
	if (XGetWindowAttributes(dpy, src, &attr) == 0
	    || attr.map_state != IsViewable)
	    continue;

	if (w != Widget(-1))
	{
	    Position x, y;
	    XtVaGetValues(w, XmNx, &x, XmNy, &y, NULL);
	    attr.x = x;
	    attr.y = y;
	}

	XSetWindowAttributes xswat;
	xswat.override_redirect = True;
	xswat.do_not_propagate_mask = KeyPressMask | KeyReleaseMask |
	    ButtonPressMask | ButtonReleaseMask;

	unsigned long vmask;
	vmask = CWOverrideRedirect | CWDontPropagate;

	Window& win = windows[wi];
	win = XCreateWindow(dpy, RootWindow(dpy, screen), attr.x, attr.y,
			    attr.width + attr.border_width,
			    attr.height + attr.border_width,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    vmask, &xswat);

	XGCValues gcvals;
	gcvals.graphics_exposures = False;

	// Copyplane gc wants to leave the data alone
	gcvals.foreground = 1;
	gcvals.background = 0;
	GC copygc = 
	    XCreateGC(dpy, win, 
		      GCForeground | GCBackground | GCGraphicsExposures,
		      &gcvals);

	gcvals.foreground = BlackPixel(dpy, screen);
	GC fillgc = XCreateGC(dpy, win, GCForeground, &gcvals);

	XDefineCursor(dpy, win, cursor);
	XMapWindow(dpy, win);

	// XSync(dpy, 0);

	short *heights = new short[attr.width];
	int i;
	for (i = 0; i < attr.width; i++)
	    heights[i] = 0;

	int finished = 0;
	for (;;) {
	    int width = rnd(MIN_WIDTH) + WIDTH_ADD;

	    // Give values near edges a better chance
	    int xloc = rnd(attr.width + MIN_WIDTH) - MIN_WIDTH;

	    if ((xloc + width) > attr.width)
		xloc = attr.width - width;
	    else if (xloc < 0)
		xloc = 0;

	    int yloc = attr.height;
	    for (i = xloc; i < (xloc + width); i++)	{
		yloc = min(yloc, heights[i]);
	    }
	    if (yloc == attr.height)
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
		if ((heights[i] < (attr.height - MIN_SIZE))
		    && (yloc >= (attr.height - MIN_SIZE)))
		    finished++;

		heights[i] = max(heights[i], yloc);
	    }
	    if (finished >= (attr.width - FINISHED))	{
		XSync(dpy, 0);
		break;
	    }
	}

	delete[] heights;
	XFreeGC(dpy, copygc);
	XFreeGC(dpy, fillgc);

	// sleep(1);
    }

    sleep(5);

    // Cleanup
    for (wi = ws.size() - 1; wi >= 0; wi--)
    {
	Window win = windows[wi];
	if (win != 0)
	{
	    XUnmapWindow(dpy, win);
	    XDestroyWindow(dpy, win);
	}
    }
    XFreeCursor(dpy, cursor);

    delete[] windows;

    return;
}
