// $Id$ -*- C++ -*-
// Find the WM frame surrounding WINDOW

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char frame_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "bool.h"
#include "frame.h"

// Find the WM frame surrounding WINDOW.
Window frame(Display *display, Window window)
{
    // The idea is that the WM frame surrounding WINDOW is a child of
    // root.  So we simply return the child of root that has WINDOW as
    // a child.

    Window root = 0;
    XWindowAttributes root_attr;
    bool have_root_attr = false;

    Window w = window;
    for (;;)
    {
	Window parent;
	Window *children = 0;
	unsigned int nchildren;
	Status ok = 
	    XQueryTree(display, w, &root, &parent, &children, &nchildren);
	XFree(children);

	if (!ok)
	    break;		// Not found

	if (parent == root)
	    return w;		// Got it

	// TVTWM (and other window managers?) cover the root window
	// entirely with a single (virtual) window.  Check for this.
	if (!have_root_attr)
	{
	    XGetWindowAttributes(display, root, &root_attr);
	    have_root_attr = true;
	}
	XWindowAttributes parent_attr;
	XGetWindowAttributes(display, parent, &parent_attr);

	if (parent_attr.width >= root_attr.width && 
	    parent_attr.height >= root_attr.height)
	{
	    // PARENT covers ROOT entirely -- is PARENT the child of ROOT?
	    Window grandparent;
	    Window *uncles = 0;
	    unsigned int nuncles;
	    Status ok = 
		XQueryTree(display, parent, 
			   &root, &grandparent, &uncles, &nuncles);
	    XFree(uncles);

	    if (ok && grandparent == root)
		return w;	// Got it
	}

	w = parent;
    }

    return window;		// Not found
}
