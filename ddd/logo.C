// $Id$ -*- C++ -*-
// DDD logo functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char logo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "logo.h"

#include "config.h"

#if defined(HAVE_XPM) && defined(HAVE_X11_XPM_H)
#include <X11/xpm.h>
#endif

// X bitmaps
#include "ddd.xbm"
#include "dddlogo.xbm"
#include "dddmask.xbm"

// X pixmaps
#ifdef XpmVersion
#include "ddd.xpm"
#include "dddlogo.xpm"
#endif

#include <iostream.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "AppData.h"
#include "cook.h"

//-----------------------------------------------------------------------------
// DDD logo
//-----------------------------------------------------------------------------

#ifdef XpmVersion

#ifndef XpmSuccess
#define XpmSuccess       0
#endif
#ifndef XpmColorError
#define XpmColorError    1
#endif
#ifndef XpmOpenFailed
#define XpmOpenFailed   -1
#endif
#ifndef XpmFileInvalid
#define XpmFileInvalid  -2
#endif
#ifndef XpmNoMemory
#define XpmNoMemory     -3
#endif
#ifndef XpmColorFailed
#define XpmColorFailed  -4
#endif

static int xpm(String name, int ret)
{
    if (ret != XpmSuccess)
    {
	cerr << "XPM: " << name << ": ";
	switch (ret)
	{
	case XpmColorError:
	    cerr << "warning: failed to allocate some color\n";
	    ret = XpmSuccess;	// ignore
	    break;

	case XpmOpenFailed:
	    cerr << "could not open file\n";
	    break;

	case XpmFileInvalid:
	    cerr << "could not parse file\n";
	    break;
	    
	case XpmNoMemory:
	    cerr << "insufficient working storage\n";
	    break;

	case XpmColorFailed:
	    cerr << "no color found\n";
	    break;

	default:
	    cerr << "error " << ret << "\n";
	    break;
	}
    }

    return ret;
}
#endif // defined(XpmVersion)

// Return pixmaps suitable for icons on the root window
Pixmap iconlogo(Widget w)
{
    Window root = RootWindowOfScreen(XtScreen(w));
    static Pixmap icon = 0;
    if (icon != 0)
	return icon;

#ifdef XpmVersion
    if (app_data.color_icons)
    {
	// Try XPM file
	XWindowAttributes root_attr;
	XGetWindowAttributes(XtDisplay(w), root, &root_attr);

	XpmAttributes attr;
	attr.valuemask = XpmVisual | XpmColormap | XpmDepth;
	attr.visual   = root_attr.visual;
	attr.colormap = root_attr.colormap;
	attr.depth    = root_attr.depth;

	int ret = xpm("ddd.xpm", XpmCreatePixmapFromData(XtDisplay(w), root,
							 ddd_xpm, &icon, 
							 (Pixmap *)0, &attr));
	XpmFreeAttributes(&attr);

	if (ret == XpmSuccess)
	    return icon;

	if (icon != 0)
	    XFreePixmap(XtDisplay(w), icon);
	icon = 0;
    }
#endif // defined(XpmVersion)

    // Try bitmap instead
    GC gc = DefaultGC(XtDisplay(w), XScreenNumberOfScreen(XtScreen(w)));
    XGCValues gcv;
    XGetGCValues(XtDisplay(w), gc, GCForeground | GCBackground, &gcv);
		      
    int depth = PlanesOfScreen(XtScreen(w));

    icon = XCreatePixmapFromBitmapData(XtDisplay(w), root,
				       ddd_bits, ddd_width, ddd_height,
				       gcv.foreground, gcv.background,
				       depth);
    return icon;
}

Pixmap iconmask(Widget w)
{
    // The bitmap mask is used for both the XPM and the XBM version.
    return XCreateBitmapFromData(XtDisplay(w),
				 RootWindowOfScreen(XtScreen(w)),
				 dddmask_bits,
				 dddmask_width, dddmask_height);
}

// Return a small DDD logo suitable for the widget W
Pixmap versionlogo(Widget w)
{
    static Pixmap logo = 0;
    if (logo != 0)
	return logo;

    Pixel foreground, background;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);
#ifdef XpmVersion
    XWindowAttributes win_attr;
    XGetWindowAttributes(XtDisplay(w), XtWindow(w), &win_attr);

    XpmColorSymbol cs;
    cs.name  = "Background";
    cs.value = 0;
    cs.pixel = background;

    XpmAttributes attr;
    attr.valuemask    = XpmVisual | XpmColormap | XpmDepth | XpmColorSymbols;
    attr.visual       = win_attr.visual;
    attr.colormap     = win_attr.colormap;
    attr.depth        = win_attr.depth;
    attr.colorsymbols = &cs;
    attr.numsymbols   = 1;
        
    int ret = xpm("ddd.xpm", XpmCreatePixmapFromData(XtDisplay(w), XtWindow(w),
						     ddd_xpm, &logo, 
						     (Pixmap *)0, &attr));
    XpmFreeAttributes(&attr);

    if (ret == XpmSuccess)
	return logo;

    if (logo != 0)
	XFreePixmap(XtDisplay(w), logo);
    logo = 0;
#endif // defined(XpmVersion)

    int depth = PlanesOfScreen(XtScreen(w));
    logo = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				       ddd_bits, ddd_width, ddd_height,
				       foreground, background,
				       depth);
    return logo;
}

// Convert NAME into a color, using PIX as default
static Pixel color(Widget w, String name, Pixel pixel)
{
    XrmValue from, to;
    from.size = strlen(name);
    from.addr = name;
    to.size   = sizeof(pixel);
    to.addr   = (String)&pixel;

    XtConvertAndStore(w, XtRString, &from, XtRPixel, &to);
    return pixel;
}

// Return a DDD logo suitable for the widget W
Pixmap dddlogo(Widget w, const string& color_key)
{
    Pixmap logo = 0;
    int depth = PlanesOfScreen(XtScreen(w));

#ifdef XpmVersion
    if (depth > 1 && color_key != "m")
    {
	XWindowAttributes win_attr;
	XGetWindowAttributes(XtDisplay(w), XtWindow(w), &win_attr);

	XpmAttributes attr;
	attr.valuemask    = XpmVisual | XpmColormap | XpmDepth;
	attr.visual       = win_attr.visual;
	attr.colormap     = win_attr.colormap;
	attr.depth        = win_attr.depth;

#ifdef XpmColorKey		// Not available in XPM 3.2 and earlier
	attr.valuemask |= XpmColorKey;
	if (color_key == "c")
	    attr.color_key = XPM_COLOR;
	else if (color_key == "g4")
	    attr.color_key = XPM_GRAY4;
	else if (color_key == "g")
	    attr.color_key = XPM_GRAY;
	else if (color_key == "m")
	    attr.color_key = XPM_MONO;
	else
	{
	    if (color_key != "best")
	    {
		cerr << "XPM: dddlogo.xpm: invalid color key " 
		     << quote(color_key) << "\n";
	    }
	    attr.valuemask &= ~XpmColorKey;
	}
#endif // defined(XpmColorKey)

	int ret = xpm("dddlogo.xpm",
		      XpmCreatePixmapFromData(XtDisplay(w), XtWindow(w),
					      dddlogo_xpm, &logo, 
					      (Pixmap *)0, &attr));
	XpmFreeAttributes(&attr);

	if (ret == XpmSuccess)
	    return logo;

	if (logo != 0)
	    XFreePixmap(XtDisplay(w), logo);
	logo = 0;
    }
#endif // defined(XpmVersion)

    logo = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				       dddlogo_bits,
				       dddlogo_width, dddlogo_height,
				       color(w, "black", 
					     BlackPixelOfScreen(XtScreen(w))),
				       color(w, "white", 
					     WhitePixelOfScreen(XtScreen(w))),
				       depth);
    return logo;
}

void get_dddlogo_size(Dimension& width, Dimension& height)
{
    width  = dddlogo_width;
    height = dddlogo_height;
}
