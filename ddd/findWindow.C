// $Id$ -*- C++ -*-
// Find a window with given title, name, or class

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char findWindow_rcsid[] = 
    "$Id$";

#include "findWindow.h"

#include "XErrorB.h"
#include "bool.h"
#include <iostream>

#ifndef LOG_FIND_WINDOWS
#define LOG_FIND_WINDOWS 0
#endif

inline bool eq(const _XtString s1, const _XtString s2)
{
#if LOG_FIND_WINDOWS
    if (s1 != 0)
	std::cout << "Found " << s1 << "\n";
#endif

    return s1 != 0 && s2 != 0 && strcmp(s1, s2) == 0;
}

bool windowMatches(Display *display, Window window,
		   const _XtString title, const _XtString res_name, const _XtString res_class)
{
    XErrorBlocker blocker(display);

    bool matches = false;

    char *window_title = 0;
    Status st = XFetchName(display, window, &window_title);
    if (st != 0)
    {
	if (eq(window_title, title))
	    matches = true;	// Title matches
	XFree(window_title);
    }

    if (matches)
	return matches;

    XClassHint class_hints;
    st = XGetClassHint(display, window, &class_hints);
    if (st != 0)
    {
	if (eq(class_hints.res_name, res_name))
	    matches = true;	// Name matches

	if (eq(class_hints.res_class, res_class))
	    matches = true;	// Class matches

	XFree(class_hints.res_name);
	XFree(class_hints.res_class);
    }

    return matches;
}

Window findWindow(Display *display, Window window,
		  const _XtString title, const _XtString res_name, const _XtString res_class)
{
    XErrorBlocker blocker(display);

    if (windowMatches(display, window, title, res_name, res_class))
	return window;

    Window root;
    Window parent;
    Window *children;
    unsigned int nchildren;
    Status st = XQueryTree(display, window, 
			   &root, &parent, &children, &nchildren);
    if (st != 0)
    {
	for (unsigned int i = 0; i < nchildren; i++)
	{
	    Window win = findWindow(display, children[i],
				     title, res_name, res_class);
	    if (win != None)
		return win;
	}

	XFree(children);
    }

    return None;
}
