// $Id$ -*- C++ -*-
// DDD logo functions

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

char logo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "logo.h"
#include "config.h"
#include "Xpm.h"
#include "assert.h"

// X bitmaps
#include "icons/ddd.xbm"
#include "icons/dddmask.xbm"
#include "icons/dddsplash.xbm"

// X pixmaps
#ifdef XpmVersion
#include "icons/ddd.xpm"
#include "icons/dddsplash.xpm"
#endif

#include <iostream.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

#include "AppData.h"
#include "cook.h"


//-----------------------------------------------------------------------------
// DDD logo
//-----------------------------------------------------------------------------

#ifdef XpmVersion

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

static void add_color_key(XpmAttributes& attr, const string& color_key)
{
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
	    cerr << "XPM: invalid color key " << quote(color_key) << "\n";

	attr.valuemask &= ~XpmColorKey;
    }
#else // !defined(XpmColorKey)
    (void) attr;		// Use it
    (void) color_key;		// Use it
#endif
}



// Return pixmaps suitable for icons on the root window
Pixmap iconlogo(Widget w)
{
    Window root = RootWindowOfScreen(XtScreen(w));
    static Pixmap icon = 0;
    if (icon != 0)
	return icon;

#ifdef XpmVersion
    if (app_data.color_wm_icons)
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
				       (char *)ddd_bits, ddd_width, ddd_height,
				       gcv.foreground, gcv.background,
				       depth);
    return icon;
}

Pixmap iconmask(Widget w)
{
    // The bitmap mask is used for both the XPM and the XBM version.
    return XCreateBitmapFromData(XtDisplay(w),
				 RootWindowOfScreen(XtScreen(w)),
				 (char *)dddmask_bits,
				 dddmask_width, dddmask_height);
}

// Return a small DDD logo suitable for the widget W
Pixmap versionlogo(Widget w)
{
    static Pixmap logo = 0;
    if (logo != 0)
	return logo;

    Pixel foreground, background;

    assert(XtIsRealized(w));

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
				       (char *)ddd_bits, ddd_width, ddd_height,
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

// Return the DDD splash screen
Pixmap dddsplash(Widget w, const string& color_key)
{
    assert(XtIsRealized(w));

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
	add_color_key(attr, color_key);

	int ret = xpm("dddsplash.xpm",
		      XpmCreatePixmapFromData(XtDisplay(w), XtWindow(w),
					      dddsplash_xpm, &logo, 
					      (Pixmap *)0, &attr));
	XpmFreeAttributes(&attr);

	if (ret == XpmSuccess)
	    return logo;

	if (logo != 0)
	    XFreePixmap(XtDisplay(w), logo);
	logo = 0;
    }
#else
    (void) color_key;		// Use it
#endif // defined(XpmVersion)

    logo = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				       (char *)dddsplash_bits,
				       dddsplash_width, dddsplash_height,
				       color(w, "black", 
					     BlackPixelOfScreen(XtScreen(w))),
				       color(w, "white", 
					     WhitePixelOfScreen(XtScreen(w))),
				       depth);
    return logo;
}



//-----------------------------------------------------------------------
// Toolbar icons
//-----------------------------------------------------------------------

// X Bitmaps
#include "icons/toolbar/breakat.xbm"
#include "icons/toolbar/clearat.xbm"
#include "icons/toolbar/deref.xbm"
#include "icons/toolbar/display.xbm"
#include "icons/toolbar/findbwd.xbm"
#include "icons/toolbar/findfwd.xbm"
#include "icons/toolbar/hide.xbm"
#include "icons/toolbar/lookup.xbm"
#include "icons/toolbar/print.xbm"
#include "icons/toolbar/rotate.xbm"
#include "icons/toolbar/set.xbm"
#include "icons/toolbar/show.xbm"
#include "icons/toolbar/undisplay.xbm"
#include "icons/toolbar/unwatch.xbm"
#include "icons/toolbar/watch.xbm"

#include "icons/toolbar/breakat-xx.xbm"
#include "icons/toolbar/clearat-xx.xbm"
#include "icons/toolbar/deref-xx.xbm"
#include "icons/toolbar/display-xx.xbm"
#include "icons/toolbar/findbwd-xx.xbm"
#include "icons/toolbar/findfwd-xx.xbm"
#include "icons/toolbar/hide-xx.xbm"
#include "icons/toolbar/lookup-xx.xbm"
#include "icons/toolbar/print-xx.xbm"
#include "icons/toolbar/rotate-xx.xbm"
#include "icons/toolbar/set-xx.xbm"
#include "icons/toolbar/show-xx.xbm"
#include "icons/toolbar/undisplay-xx.xbm"
#include "icons/toolbar/unwatch-xx.xbm"
#include "icons/toolbar/watch-xx.xbm"

#ifdef XpmVersion
// X Pixmaps
#include "icons/toolbar/breakat.xpm"
#include "icons/toolbar/clearat.xpm"
#include "icons/toolbar/deref.xpm"
#include "icons/toolbar/display.xpm"
#include "icons/toolbar/findbwd.xpm"
#include "icons/toolbar/findfwd.xpm"
#include "icons/toolbar/hide.xpm"
#include "icons/toolbar/lookup.xpm"
#include "icons/toolbar/print.xpm"
#include "icons/toolbar/rotate.xpm"
#include "icons/toolbar/set.xpm"
#include "icons/toolbar/show.xpm"
#include "icons/toolbar/undisplay.xpm"
#include "icons/toolbar/unwatch.xpm"
#include "icons/toolbar/watch.xpm"

#include "icons/toolbar/breakat-xx.xpm"
#include "icons/toolbar/clearat-xx.xpm"
#include "icons/toolbar/deref-xx.xpm"
#include "icons/toolbar/display-xx.xpm"
#include "icons/toolbar/findbwd-xx.xpm"
#include "icons/toolbar/findfwd-xx.xpm"
#include "icons/toolbar/hide-xx.xpm"
#include "icons/toolbar/lookup-xx.xpm"
#include "icons/toolbar/print-xx.xpm"
#include "icons/toolbar/rotate-xx.xpm"
#include "icons/toolbar/set-xx.xpm"
#include "icons/toolbar/show-xx.xpm"
#include "icons/toolbar/undisplay-xx.xpm"
#include "icons/toolbar/unwatch-xx.xpm"
#include "icons/toolbar/watch-xx.xpm"
#endif // XpmVersion


static void install_icon(Widget w, string name,
			 char **xpm_data, unsigned char *xbm_data,
			 int width, int height,
			 int strip_captions, const string& color_key)
{
#ifdef XpmVersion
    int depth = PlanesOfScreen(XtScreen(w));

    if (depth > 1 && color_key != "m")
    {
	XWindowAttributes win_attr;
	XGetWindowAttributes(XtDisplay(w), 
			     RootWindowOfScreen(XtScreen(w)),
			     &win_attr);

	Pixel background;
	XtVaGetValues(w, XmNbackground, &background, NULL);

	XpmColorSymbol cs;
	cs.name  = "Background";
	cs.value = 0;
	cs.pixel = background;

	XpmAttributes attr;
	attr.valuemask    = 
	    XpmVisual | XpmColormap | XpmDepth | XpmColorSymbols;
	attr.visual       = win_attr.visual;
	attr.colormap     = win_attr.colormap;
	attr.depth        = win_attr.depth;
	attr.colorsymbols = &cs;
	attr.numsymbols   = 1;
	add_color_key(attr, color_key);

	XImage *image = 0;
	XImage *shape = 0;

	int ret = 
	    xpm(name, XpmCreateImageFromData(XtDisplay(w), xpm_data, 
					     &image, &shape, &attr));

	XpmFreeAttributes(&attr);
	XFree(shape);

	if (ret == XpmSuccess && image != 0)
	{
	    image->height -= strip_captions;
	    Boolean ok = XmInstallImage(image, name);
	    if (ok)
		return;
	}

	cerr << "Could not install " << quote(name) << " pixmap\n";
	XFree(image);
    }
#endif // defined(XpmVersion)

    // Install the bitmap version
    XImage *image = (XImage *)XtCalloc(1, sizeof(XImage));
    image->width            = width;
    image->height           = height;
    image->xoffset          = 0;
    image->format           = XYBitmap;
    image->data             = (char *)xbm_data;
    image->byte_order       = MSBFirst;
    image->bitmap_unit      = 8;
    image->bitmap_bit_order = LSBFirst;
    image->bitmap_pad       = 8;
    image->depth            = 1;
    image->bytes_per_line   = 2;

    image->height -= strip_captions;
    Boolean ok = XmInstallImage(image, name);
    if (ok)
	return;

    cerr << "Could not install " << quote(name) << " bitmap\n";
    XtFree((char *)image);
};

static void install_icon(Widget w, string name,
			 char **xpm_data, char **xpm_xx_data,
			 unsigned char *xbm_data, unsigned char *xbm_xx_data,
			 int width, int height, 
			 int strip_captions, const string& color_key)
{
    install_icon(w, name,
		 xpm_data,
		 xbm_data,
		 width, height, strip_captions, color_key);
    install_icon(w, name + "-xx",
		 xpm_xx_data,
		 xbm_xx_data, 
		 width, height, strip_captions, color_key);
}

void install_icons(Widget shell, int strip_captions, const string& color_key)
{
    // DDD icon
    install_icon(shell, DDD_ICON, 
		 ddd_xpm,
		 ddd_bits,
		 ddd_width, ddd_height, 0, color_key);

    // Toolbar icons
    install_icon(shell, BREAK_AT_ICON, 
		 breakat_xpm, breakat_xx_xpm,
		 breakat_bits, breakat_xx_bits, 
		 breakat_width, breakat_height, strip_captions, color_key);

    install_icon(shell, CLEAR_AT_ICON, 
		 clearat_xpm, clearat_xx_xpm,
		 clearat_bits, clearat_xx_bits, 
		 clearat_width, clearat_height, strip_captions, color_key);

    install_icon(shell, DISPREF_ICON, 
		 deref_xpm, deref_xx_xpm,
		 deref_bits, deref_xx_bits, 
		 deref_width, deref_height, strip_captions, color_key);

    install_icon(shell, DISPLAY_ICON, 
		 display_xpm, display_xx_xpm,
		 display_bits, display_xx_bits, 
		 display_width, display_height, strip_captions, color_key);

    install_icon(shell, FIND_BACKWARD_ICON, 
		 findbwd_xpm, findbwd_xx_xpm,
		 findbwd_bits, findbwd_xx_bits, 
		 findbwd_width, findbwd_height, strip_captions, color_key);

    install_icon(shell, FIND_FORWARD_ICON, 
		 findfwd_xpm, findfwd_xx_xpm,
		 findfwd_bits, findfwd_xx_bits, 
		 findfwd_width, findfwd_height, strip_captions, color_key);

    install_icon(shell, HIDE_ICON, 
		 hide_xpm, hide_xx_xpm,
		 hide_bits, hide_xx_bits, 
		 hide_width, hide_height, strip_captions, color_key);

    install_icon(shell, LOOKUP_ICON, 
		 lookup_xpm, lookup_xx_xpm,
		 lookup_bits, lookup_xx_bits, 
		 lookup_width, lookup_height, strip_captions, color_key);

    install_icon(shell, PRINT_ICON, 
		 print_xpm, print_xx_xpm,
		 print_bits, print_xx_bits, 
		 print_width, print_height, strip_captions, color_key);

    install_icon(shell, ROTATE_ICON, 
		 rotate_xpm, rotate_xx_xpm,
		 rotate_bits, rotate_xx_bits, 
		 rotate_width, rotate_height, strip_captions, color_key);

    install_icon(shell, SET_ICON, 
		 set_xpm, set_xx_xpm,
		 set_bits, set_xx_bits, 
		 set_width, set_height, strip_captions, color_key);

    install_icon(shell, SHOW_ICON, 
		 show_xpm, show_xx_xpm,
		 show_bits, show_xx_bits, 
		 show_width, show_height, strip_captions, color_key);

    install_icon(shell, UNDISPLAY_ICON, 
		 undisplay_xpm, undisplay_xx_xpm,
		 undisplay_bits, undisplay_xx_bits, 
		 undisplay_width, undisplay_height, strip_captions, color_key);

    install_icon(shell, UNWATCH_ICON, 
		 unwatch_xpm, unwatch_xx_xpm,
		 unwatch_bits, unwatch_xx_bits, 
		 unwatch_width, unwatch_height, strip_captions, color_key);

    install_icon(shell, WATCH_ICON, 
		 watch_xpm, watch_xx_xpm,
		 watch_bits, watch_xx_bits, 
		 watch_width, watch_height, strip_captions, color_key);
}



//-----------------------------------------------------------------------
// Set pixmap
//-----------------------------------------------------------------------

void set_pixmap(Widget w, string image_name)
{
    if (w == 0)
	return;

    assert(XtIsSubclass(w, xmLabelWidgetClass));

    Pixel foreground = 0;
    Pixel background = 0;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);
    
    Pixmap p1 = XmGetPixmap(XtScreen(w), image_name, 
				foreground, background);
    Pixmap p2 = XmGetPixmap(XtScreen(w), image_name + "-xx", 
				foreground, background);

    Arg args[10];
    Cardinal arg = 0;

    if (p1 != XmUNSPECIFIED_PIXMAP)
    {
	XtSetArg(args[arg], XmNlabelPixmap, p1); arg++;
    }
    if (p2 != XmUNSPECIFIED_PIXMAP)
    {
	XtSetArg(args[arg], XmNlabelInsensitivePixmap, p2); arg++;
    }
    if (arg > 0)
	XtSetValues(w, args, arg);
}

void set_label(Widget w, const MString& new_label, char *image)
{
    if (w == 0)
	return;

    assert(XtIsSubclass(w, xmLabelWidgetClass));

    XmString old_label;
    XtVaGetValues(w, XmNlabelString, &old_label, NULL);
    if (XmStringCompare(new_label.xmstring(), old_label) == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNlabelString, new_label.xmstring()); arg++;

	if (image != 0)
	{
	    Pixel foreground = 0;
	    Pixel background = 0;

	    XtVaGetValues(w,
			  XmNforeground, &foreground,
			  XmNbackground, &background,
			  NULL);

	    string s1 = image;
	    string s2 = s1 + "-xx";

	    Pixmap p1 = XmGetPixmap(XtScreen(w), s1, foreground, background);
	    Pixmap p2 = XmGetPixmap(XtScreen(w), s2, foreground, background);

	    if (p1 != XmUNSPECIFIED_PIXMAP)
	    {
		XtSetArg(args[arg], XmNlabelPixmap, p1); arg++;
	    }
	    if (p2 != XmUNSPECIFIED_PIXMAP)
	    {
		XtSetArg(args[arg], XmNlabelInsensitivePixmap, p2); arg++;
	    }
	}
	XtSetValues(w, args, arg);
    }
    XmStringFree(old_label);
}
