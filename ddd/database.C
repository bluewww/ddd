// $Id$
// An adaptation of the NCD `meltdown' program.

char database_rcsid[] = 
    "$Id$";

/*
 * Copyright 1990 David Lemke and Network Computing Devices
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Network Computing Devices not be 
 * used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  Network Computing 
 * Devices makes no representations about the suitability of this software 
 * for any purpose.  It is provided "as is" without express or implied 
 * warranty.
 *
 * NETWORK COMPUTING DEVICES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL NETWORK COMPUTING DEVICES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  	
 *		Dave Lemke
 *		lemke@ncd.com
 *
 *		Network Computing Devices, Inc
 *		350 North Bernardo Ave
 *		Mountain View, CA 94043
 *
 *		@(#)meltdown.c	1.2	90/02/22
 *
 */

#include        "config.h"

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

#include	<X11/Xlib.h>
#include	<X11/cursorfont.h>

#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<time.h>

#define	MIN_SIZE	10
#define	MAX_SIZE	100

#define	MIN_DIST	10

#define	MIN_WIDTH	30
#define	WIDTH_ADD	20

#define	FINISHED	50

#ifdef HAVE_RAND
#define	rnd(x)	(rand() % (x))
#elif HAVE_RANDOM
#define	rnd(x)	(random() % (x))
#endif

#define	min(a, b)	((a) < (b) ? (a) : (b))
#define	max(a, b)	((a) > (b) ? (a) : (b))

void meltdown(Display *dpy)
{
#ifndef rnd
    return;
#endif

    int screen = DefaultScreen(dpy);

#ifdef HAVE_SRAND
    srand(time(0));
#elif HAVE_SRANDOM
    srandom(time(0));
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
