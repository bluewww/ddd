// $Id$ -*- C++ -*-
// DDD logo functions

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

char logo_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "logo.h"
#include "dddlogo.xbm"
#include "dddmask.xbm"

#include <X11/Xlib.h>
#include <Xm/Xm.h>

//-----------------------------------------------------------------------------
// DDD logo
//-----------------------------------------------------------------------------

// Return pixmaps suitable for icons on the root window
Pixmap iconlogo(Widget w)
{
    GC gc = DefaultGC(XtDisplay(w), XScreenNumberOfScreen(XtScreen(w)));
    XGCValues gcv;
    XGetGCValues(XtDisplay(w), gc, GCForeground | GCBackground, &gcv);
		      
    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap icon = 
	XCreatePixmapFromBitmapData(XtDisplay(w),
				    RootWindowOfScreen(XtScreen(w)),
				    dddlogo_bits,
				    dddlogo_width, dddlogo_height,
				    gcv.foreground, gcv.background,
				    depth);

    return icon;
}

Pixmap iconmask(Widget w)
{
    return XCreateBitmapFromData(XtDisplay(w),
				 RootWindowOfScreen(XtScreen(w)),
				 dddmask_bits,
				 dddmask_width, dddmask_height);
}

// Return pixmaps suitable for the widget W
Pixmap versionlogo(Widget w)
{
    Pixel foreground, background;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap logo = 
	XCreatePixmapFromBitmapData(XtDisplay(w),
				    XtWindow(w),
				    dddlogo_bits,
				    dddlogo_width, dddlogo_height,
				    foreground, background,
				    depth);

    return logo;
}
