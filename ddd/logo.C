// $Id$ -*- C++ -*-
// DDD logos and logo functions

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
#include "string-fun.h"
#include "AppData.h"
#include "cook.h"
#include "LessTifH.h"
#include "InitImage.h"

// X bitmaps
#include "icons/ddd.xbm"
#include "icons/dddmask.xbm"
#include "icons/dddsplash.xbm"

// X pixmaps
#ifdef XpmVersion
#include "icons/ddd.xpm"
#include "icons/dddsplash.xpm"

#else
static char **ddd_xpm = 0;
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

// Add a color key specification
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

// Add a `close colors' specification.  The default value 40000 is
// taken from the XPM documentation -- `seems to be right about many
// situations'.
static void add_closeness(XpmAttributes& attr, int closeness = 40000)
{
#ifdef XpmCloseness		// Not available in XPM 3.2 and earlier
    attr.valuemask |= XpmCloseness;
    attr.closeness = closeness;
#else // !defined(XpmCloseness)
    (void) attr;		// Use it
    (void) closeness;		// Use it
#endif
}

#endif // defined(XpmVersion)


// Return pixmaps suitable for icons on the root window
static Pixmap _iconlogo(Widget w)
{
    Pixmap icon = 0;

    Window root = RootWindowOfScreen(XtScreen(w));
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
	add_closeness(attr);

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

static Pixmap _iconmask(Widget w)
{
    // The bitmap mask is used for both the XPM and the XBM version.
    return XCreateBitmapFromData(XtDisplay(w),
				 RootWindowOfScreen(XtScreen(w)),
				 (char *)dddmask_bits,
				 dddmask_width, dddmask_height);
}

Pixmap iconlogo(Widget w)
{
    static Pixmap icon = _iconlogo(w);
    return icon;
}

Pixmap iconmask(Widget w)
{
    static Pixmap icon = _iconmask(w);
    return icon;
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
	add_closeness(attr);

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
#include "icons/toolbar/delete.xbm"
#include "icons/toolbar/deref.xbm"
#include "icons/toolbar/disable.xbm"
#include "icons/toolbar/display.xbm"
#include "icons/toolbar/enable.xbm"
#include "icons/toolbar/findbwd.xbm"
#include "icons/toolbar/findfwd.xbm"
#include "icons/toolbar/hide.xbm"
#include "icons/toolbar/lookup.xbm"
#include "icons/toolbar/maketemp.xbm"
#include "icons/toolbar/newbreak.xbm"
#include "icons/toolbar/newdisplay.xbm"
#include "icons/toolbar/newwatch.xbm"
#include "icons/toolbar/plot.xbm"
#include "icons/toolbar/print.xbm"
#include "icons/toolbar/properties.xbm"
#include "icons/toolbar/rotate.xbm"
#include "icons/toolbar/set.xbm"
#include "icons/toolbar/show.xbm"
#include "icons/toolbar/undisplay.xbm"
#include "icons/toolbar/unwatch.xbm"
#include "icons/toolbar/watch.xbm"

// Same, but insensitive
#include "icons/toolbar/breakat.xbmxx"
#include "icons/toolbar/clearat.xbmxx"
#include "icons/toolbar/delete.xbmxx"
#include "icons/toolbar/deref.xbmxx"
#include "icons/toolbar/disable.xbmxx"
#include "icons/toolbar/display.xbmxx"
#include "icons/toolbar/enable.xbmxx"
#include "icons/toolbar/findbwd.xbmxx"
#include "icons/toolbar/findfwd.xbmxx"
#include "icons/toolbar/hide.xbmxx"
#include "icons/toolbar/lookup.xbmxx"
#include "icons/toolbar/maketemp.xbmxx"
#include "icons/toolbar/newbreak.xbmxx"
#include "icons/toolbar/newdisplay.xbmxx"
#include "icons/toolbar/newwatch.xbmxx"
#include "icons/toolbar/plot.xbmxx"
#include "icons/toolbar/print.xbmxx"
#include "icons/toolbar/properties.xbmxx"
#include "icons/toolbar/rotate.xbmxx"
#include "icons/toolbar/set.xbmxx"
#include "icons/toolbar/show.xbmxx"
#include "icons/toolbar/undisplay.xbmxx"
#include "icons/toolbar/unwatch.xbmxx"
#include "icons/toolbar/watch.xbmxx"

#ifdef XpmVersion
// X Pixmaps
#include "icons/toolbar/breakat.xpm"
#include "icons/toolbar/clearat.xpm"
#include "icons/toolbar/delete.xpm"
#include "icons/toolbar/deref.xpm"
#include "icons/toolbar/disable.xpm"
#include "icons/toolbar/display.xpm"
#include "icons/toolbar/enable.xpm"
#include "icons/toolbar/findbwd.xpm"
#include "icons/toolbar/findfwd.xpm"
#include "icons/toolbar/hide.xpm"
#include "icons/toolbar/lookup.xpm"
#include "icons/toolbar/maketemp.xpm"
#include "icons/toolbar/newbreak.xpm"
#include "icons/toolbar/newdisplay.xpm"
#include "icons/toolbar/newwatch.xpm"
#include "icons/toolbar/plot.xpm"
#include "icons/toolbar/print.xpm"
#include "icons/toolbar/properties.xpm"
#include "icons/toolbar/rotate.xpm"
#include "icons/toolbar/set.xpm"
#include "icons/toolbar/show.xpm"
#include "icons/toolbar/undisplay.xpm"
#include "icons/toolbar/unwatch.xpm"
#include "icons/toolbar/watch.xpm"

// Same, but insensitive
#include "icons/toolbar/breakat.xpmxx"
#include "icons/toolbar/clearat.xpmxx"
#include "icons/toolbar/delete.xpmxx"
#include "icons/toolbar/deref.xpmxx"
#include "icons/toolbar/disable.xpmxx"
#include "icons/toolbar/display.xpmxx"
#include "icons/toolbar/enable.xpmxx"
#include "icons/toolbar/findbwd.xpmxx"
#include "icons/toolbar/findfwd.xpmxx"
#include "icons/toolbar/hide.xpmxx"
#include "icons/toolbar/lookup.xpmxx"
#include "icons/toolbar/maketemp.xpmxx"
#include "icons/toolbar/newbreak.xpmxx"
#include "icons/toolbar/newdisplay.xpmxx"
#include "icons/toolbar/newwatch.xpmxx"
#include "icons/toolbar/plot.xpmxx"
#include "icons/toolbar/print.xpmxx"
#include "icons/toolbar/properties.xpmxx"
#include "icons/toolbar/rotate.xpmxx"
#include "icons/toolbar/set.xpmxx"
#include "icons/toolbar/show.xpmxx"
#include "icons/toolbar/undisplay.xpmxx"
#include "icons/toolbar/unwatch.xpmxx"
#include "icons/toolbar/watch.xpmxx"

#else
static char **breakat_xpm        = 0;
static char **clearat_xpm        = 0;
static char **delete_xpm         = 0;
static char **deref_xpm          = 0;
static char **disable_xpm        = 0;
static char **display_xpm        = 0;
static char **enable_xpm         = 0;
static char **findbwd_xpm        = 0;
static char **findfwd_xpm        = 0;
static char **hide_xpm           = 0;
static char **lookup_xpm         = 0;
static char **maketemp_xpm       = 0;
static char **newbreak_xpm       = 0;
static char **newdisplay_xpm     = 0;
static char **newwatch_xpm       = 0;
static char **plot_xpm           = 0;
static char **print_xpm          = 0;
static char **properties_xpm     = 0;
static char **rotate_xpm         = 0;
static char **set_xpm            = 0;
static char **show_xpm           = 0;
static char **undisplay_xpm      = 0;
static char **unwatch_xpm        = 0;
static char **watch_xpm          = 0;

static char **breakat_xx_xpm     = 0;
static char **clearat_xx_xpm     = 0;
static char **delete_xx_xpm      = 0;
static char **deref_xx_xpm       = 0;
static char **disable_xx_xpm     = 0;
static char **display_xx_xpm     = 0;
static char **enable_xx_xpm      = 0;
static char **findbwd_xx_xpm     = 0;
static char **findfwd_xx_xpm     = 0;
static char **hide_xx_xpm        = 0;
static char **lookup_xx_xpm      = 0;
static char **maketemp_xx_xpm    = 0;
static char **newbreak_xx_xpm    = 0;
static char **newdisplay_xx_xpm  = 0;
static char **newwatch_xx_xpm    = 0;
static char **plot_xx_xpm        = 0;
static char **print_xx_xpm       = 0;
static char **properties_xx_xpm  = 0;
static char **rotate_xx_xpm      = 0;
static char **set_xx_xpm         = 0;
static char **show_xx_xpm        = 0;
static char **undisplay_xx_xpm   = 0;
static char **unwatch_xx_xpm     = 0;
static char **watch_xx_xpm       = 0;
#endif // !XpmVersion

static char get_sign(string& g)
{
    if (g.contains('+', 0) || g.contains('-', 0) || g.contains('x', 0))
    {
	char c = g[0];
	g = g.after(0);
	return c;
    }
    else
	return '\0';
}

static XImage *get_subimage(XImage *image, String geometry, 
			    String name = "image",
			    String resource = "geometry")
{
    if (geometry == 0)
    {
	cerr << "No " << resource << "\n";
	return 0;
    }
	
    string g = geometry;

    Dimension width  = read_positive_nr(g);
    char cross       = get_sign(g);
    Dimension height = read_positive_nr(g);
    char sign_x      = get_sign(g);
    int x            = read_positive_nr(g);
    char sign_y      = get_sign(g);
    int y            = read_positive_nr(g);

    if (cross != 'x' || sign_x == '\0' || sign_y == '\0')
    {
	cerr << "Cannot parse " << resource << " " << quote(geometry) << "\n";
	return 0;
    }

    if (sign_x == '-')
	x = image->width - width - x;
    if (sign_y == '-')
	y = image->height - height - y;

    if (x < 0 || y < 0 || 
	x + width > image->width || y + height > image->height)
    {
	cerr << "No such " << resource << " " << quote(geometry) << " in " 
	     << name << "\n";
	return 0;
    }

    if (image->f.sub_image == 0)
    {
	cerr << "Cannot get subimage of " << name 
	     << " - requires X11R6 or XPM";
	return 0;
    }

    XImage *subimage = XSubImage(image, x, y, width, height);
    if (subimage == 0)
    {
	cerr << "Invalid " << resource << " " << quote(geometry) << " in "
	     << name << "\n";
	return 0;
    }

    return subimage;
}

static XImage *get_button_subimage(XImage *image, String name)
{
    if (!app_data.button_images && !app_data.button_captions)
    {
#if 0				// This is a legitimate DDD setting
	cerr << "Must set either " << XtNbuttonImages << " or " 
	     << XtNbuttonCaptions << "\n";
#endif
	return 0;
    }

    if (!app_data.button_images)
	return get_subimage(image, app_data.button_caption_geometry, 
			    name, XtNbuttonCaptionGeometry);

    if  (!app_data.button_captions)
	return get_subimage(image, app_data.button_image_geometry, 
			    name, XtNbuttonImageGeometry);

    return 0;			// Leave unchanged
}



static void install_icon(Widget w, String name,
			 char **xpm_data, unsigned char *xbm_data,
			 int width, int height,
			 const string& color_key,
			 Pixel background,
			 const XWindowAttributes& win_attr,
			 bool is_button = false)
{
#ifdef XpmVersion
    int depth = PlanesOfScreen(XtScreen(w));

    if (depth > 1 && color_key != "m")
    {
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
	add_closeness(attr);

	XImage *image = 0;
	XImage *shape = 0;

	int ret = 
	    xpm(name, XpmCreateImageFromData(XtDisplay(w), xpm_data, 
					     &image, &shape, &attr));

	XpmFreeAttributes(&attr);
	if (shape != 0)
	    XDestroyImage(shape);

	if (ret == XpmSuccess && image != 0)
	{
	    if (is_button)
	    {
		XImage *subimage = get_button_subimage(image, name);
		if (subimage != 0)
		{
		    XDestroyImage(image);
		    image = subimage;
		}
	    }
	    Boolean ok = XmInstallImage(image, name);
	    if (ok)
		return;
	}

	cerr << "Could not install " << quote(name) << " pixmap\n";
	if (image != 0)
	    XDestroyImage(image);
    }
#else
    (void) w;
    (void) xpm_data;
    (void) color_key;
    (void) background;
    (void) win_attr;
#endif // !defined(XpmVersion)

    // Install the bitmap version
    XImage *image = CreateImageFromBitmapData(xbm_data, width, height);

    if (is_button)
    {
	XImage *subimage = get_button_subimage(image, name);
	if (subimage != 0)
	{
	    // Using XDestroyImage() instead of XFree() here would
	    // also free the static XBM data.
	    XFree(image);
	    image = subimage;
	}
    }

    Boolean ok = XmInstallImage(image, name);
    if (ok)
	return;

    cerr << "Could not install " << quote(name) << " bitmap\n";
    if (image != 0)
	XDestroyImage(image);
}

static void install_button_icon(Widget w, String name,
				char **xpm_data, char **xpm_xx_data,
				unsigned char *xbm_data,
				unsigned char *xbm_xx_data,
				int width, int height, 
				const string& color_key,
				const string& active_color_key,
				Pixel background,
				Pixel arm_background,
				const XWindowAttributes& win_attr)
{
    install_icon(w, name,
		 xpm_data,
		 xbm_data,
		 width, height,
		 color_key, background, win_attr, true);

    string insensitive_name = string(name) + "-xx";
    install_icon(w, insensitive_name,
		 xpm_xx_data,
		 xbm_xx_data, 
		 width, height,
		 color_key, background, win_attr, true);

    string armed_name = string(name) + "-arm";
    install_icon(w, armed_name,
		 xpm_data,
		 xbm_data,
		 width, height,
		 active_color_key, arm_background, win_attr, true);

    string highlight_name = string(name) + "-hi";
    install_icon(w, highlight_name,
		 xpm_data,
		 xbm_data,
		 width, height,
		 active_color_key, background, win_attr, true);
}

// Install toolbar icons in Motif cache.  COLOR_KEY indicates the XPM
// visual type for inactive buttons.  ACTIVE_COLOR_KEY is the XPM visual
// type for active buttons (entered or armed).
void install_icons(Widget shell, 
		   const string& color_key,
		   const string& active_color_key)
{
    static bool installed = false;
    if (installed)
	return;
    installed = true;

    // Determine attributes
    XWindowAttributes win_attr;
    XGetWindowAttributes(XtDisplay(shell), 
			 RootWindowOfScreen(XtScreen(shell)),
			 &win_attr);

    Pixel background;
    XtVaGetValues(shell, XmNbackground, &background, NULL);

    // Determine default arm background
    Pixel foreground, top_shadow, bottom_shadow, select;
    XmGetColors(XtScreen(shell), win_attr.colormap, background,
		&foreground, &top_shadow, &bottom_shadow, &select);

    // LessTif does not return a suitable select color
    Pixel arm_background;
    if (lesstif_version < 1000)
	arm_background = background;
    else
	arm_background = select;

    // DDD icon (always in color)
    install_icon(shell, DDD_ICON, 
		 ddd_xpm,
		 ddd_bits,
		 ddd_width, ddd_height,
		 "best", background, win_attr);

    // Toolbar icons
    install_button_icon(shell, BREAK_AT_ICON, 
      		        breakat_xpm, breakat_xx_xpm,
		        breakat_bits, breakat_xx_bits, 
		        breakat_width, breakat_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, CLEAR_AT_ICON, 
      		        clearat_xpm, clearat_xx_xpm,
		        clearat_bits, clearat_xx_bits, 
		        clearat_width, clearat_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, DELETE_ICON, 
      		        delete_xpm, delete_xx_xpm,
		        delete_bits, delete_xx_bits, 
		        delete_width, delete_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, DISPREF_ICON, 
      		        deref_xpm, deref_xx_xpm,
		        deref_bits, deref_xx_bits, 
		        deref_width, deref_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, DISABLE_ICON, 
      		        disable_xpm, disable_xx_xpm,
		        disable_bits, disable_xx_bits, 
		        disable_width, disable_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, DISPLAY_ICON, 
      		        display_xpm, display_xx_xpm,
		        display_bits, display_xx_bits, 
		        display_width, display_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, ENABLE_ICON, 
      		        enable_xpm, enable_xx_xpm,
		        enable_bits, enable_xx_bits, 
		        enable_width, enable_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, FIND_BACKWARD_ICON, 
      		        findbwd_xpm, findbwd_xx_xpm,
		        findbwd_bits, findbwd_xx_bits, 
		        findbwd_width, findbwd_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, FIND_FORWARD_ICON, 
      		        findfwd_xpm, findfwd_xx_xpm,
		        findfwd_bits, findfwd_xx_bits, 
		        findfwd_width, findfwd_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, HIDE_ICON, 
      		        hide_xpm, hide_xx_xpm,
		        hide_bits, hide_xx_bits, 
		        hide_width, hide_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, LOOKUP_ICON, 
      		        lookup_xpm, lookup_xx_xpm,
		        lookup_bits, lookup_xx_bits, 
		        lookup_width, lookup_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, MAKETEMP_ICON, 
      		        maketemp_xpm, maketemp_xx_xpm,
		        maketemp_bits, maketemp_xx_bits, 
		        maketemp_width, maketemp_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, NEW_BREAK_ICON, 
      		        newbreak_xpm, newbreak_xx_xpm,
		        newbreak_bits, newbreak_xx_bits, 
		        newbreak_width, newbreak_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, NEW_DISPLAY_ICON, 
      		        newdisplay_xpm, newdisplay_xx_xpm,
		        newdisplay_bits, newdisplay_xx_bits, 
		        newdisplay_width, newdisplay_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, NEW_WATCH_ICON, 
      		        newwatch_xpm, newwatch_xx_xpm,
		        newwatch_bits, newwatch_xx_bits, 
		        newwatch_width, newwatch_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, PLOT_ICON, 
      		        plot_xpm, plot_xx_xpm,
		        plot_bits, plot_xx_bits, 
		        plot_width, plot_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, PRINT_ICON, 
      		        print_xpm, print_xx_xpm,
		        print_bits, print_xx_bits, 
		        print_width, print_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, PROPERTIES_ICON, 
      		        properties_xpm, properties_xx_xpm,
		        properties_bits, properties_xx_bits, 
		        properties_width, properties_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, ROTATE_ICON, 
      		        rotate_xpm, rotate_xx_xpm,
		        rotate_bits, rotate_xx_bits, 
		        rotate_width, rotate_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, SET_ICON, 
      		        set_xpm, set_xx_xpm,
		        set_bits, set_xx_bits, 
		        set_width, set_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, SHOW_ICON, 
      		        show_xpm, show_xx_xpm,
		        show_bits, show_xx_bits, 
		        show_width, show_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, UNDISPLAY_ICON, 
      		        undisplay_xpm, undisplay_xx_xpm,
		        undisplay_bits, undisplay_xx_bits, 
		        undisplay_width, undisplay_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, UNWATCH_ICON, 
      		        unwatch_xpm, unwatch_xx_xpm,
		        unwatch_bits, unwatch_xx_bits, 
		        unwatch_width, unwatch_height,
			color_key, active_color_key,
			background, arm_background, win_attr);

    install_button_icon(shell, WATCH_ICON, 
      		        watch_xpm, watch_xx_xpm,
		        watch_bits, watch_xx_bits, 
		        watch_width, watch_height,
			color_key, active_color_key,
			background, arm_background, win_attr);
}



//-----------------------------------------------------------------------
// Set pixmap and label
//-----------------------------------------------------------------------

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
	    Dimension shadow_thickness    = 0;
	    Dimension highlight_thickness = 0;

	    XtVaGetValues(w,
			  XmNforeground, &foreground,
			  XmNbackground, &background,
			  XmNshadowThickness, &shadow_thickness,
			  XmNhighlightThickness, &highlight_thickness,
			  NULL);

	    string s1 = image;
	    string s2 = s1 + "-xx";
	    string s3 = s1 + "-arm";
	    string s4 = s1 + "-hi";

	    Pixmap p1 = XmGetPixmap(XtScreen(w), s1, foreground, background);
	    Pixmap p2 = XmGetPixmap(XtScreen(w), s2, foreground, background);
	    Pixmap p3 = XmGetPixmap(XtScreen(w), s3, foreground, background);
	    Pixmap p4 = XmGetPixmap(XtScreen(w), s4, foreground, background);

	    if (shadow_thickness != 0)
	    {
		// The button is active - pixmaps are swapped
		Pixmap swap = p4;
		p4 = p1;
		p1 = swap;
	    }

	    if (p1 != XmUNSPECIFIED_PIXMAP)
	    {
		XtSetArg(args[arg], XmNlabelPixmap, p1); arg++;
	    }
	    if (p2 != XmUNSPECIFIED_PIXMAP)
	    {
		XtSetArg(args[arg], XmNlabelInsensitivePixmap, p2); arg++;
	    }
	    if (p3 != XmUNSPECIFIED_PIXMAP)
	    {
		XtSetArg(args[arg], XmNarmPixmap, p3); arg++;
	    }
	    if (p4 != XmUNSPECIFIED_PIXMAP && highlight_thickness == 0)
	    {
		XtSetArg(args[arg], XmNhighlightPixmap, p4); arg++;
	    }
	}
	XtSetValues(w, args, arg);
    }
    XmStringFree(old_label);
}
