// $Id$ -*- C++ -*-
// Find a window with given title, name, or class

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

#ifndef _DDD_findWindow_h
#define _DDD_findWindow_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"

// Return true if one of the following is true:
// - TITLE is non-NULL and WINDOW has the title TITLE
// - APP_NAME is non-NULL and WINDOW has the application name APP_NAME
// - APP_CLASS is non-NULL and WINDOW has an application class APP_CLASS.
bool windowMatches(Display *display, Window window,
		   String title, String app_name, String app_class);

// Custom call
inline bool windowMatches(Display *display, Window window, String name)
{
    return windowMatches(display, window, name, name, name);
}


// Find a child of WINDOW that matches TITLE/APP_NAME/APP_CLASS.
// If none is found, return None.
Window findWindow(Display *display, Window window,
		  String title, String app_name, String app_class);

// Custom call
inline Window findWindow(Display *display, Window window, String name)
{
    return findWindow(display, window, name, name, name);
}


// Find a window on W's screen that matches TITLE/APP_NAME/APP_CLASS.
// If none is found, return None.
inline Window findWindow(Widget w, String title, 
			 String app_name, String app_class)
{
    return findWindow(XtDisplay(w), RootWindowOfScreen(XtScreen(w)), 
		      title, app_name, app_class);
}

// Custom call
inline Window findWindow(Widget w, String name)
{
    return findWindow(w, name, name, name);
}

#endif // _DDD_findWindow_h
// DON'T ADD ANYTHING BEHIND THIS #endif
