// $Id$
// find specific parent

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char findParent_rcsid[] = 
    "$Id$";

#include <Xm/Xm.h>
#include <iostream.h>

#include "findParent.h"
#include "longName.h"
#include "bool.h"

// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif

// Set this to true to allow debugging
bool findParent_debug = false;

// Find a realized Shell
Widget findShellParent(Widget w)
{
    if (findParent_debug)
	clog << "findShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsShell(w)
		      || XtDisplay(w) == 0
		      || XtScreen(w) == 0
		      || XtWindow(w) == 0
		      || !XtIsRealized(w)))
	w = XtParent(w);

    if (findParent_debug)
    {
	if (w != 0)
	    clog << longName(w);
	else
	    clog << "(none)";
	clog << "\n";
    }

    return w;
}


// find a (realized) toplevel Shell
Widget findTopLevelShellParent(Widget w)
{
    if (findParent_debug)
	clog << "findTopLevelShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsTopLevelShell(w)
		      || XtDisplay(w) == 0
		      || XtScreen(w) == 0
		      || XtWindow(w) == 0
		      || !XtIsRealized(w)))
	w = XtParent(w);

    if (findParent_debug)
    {
	if (w != 0)
	    clog << longName(w);
	else
	    clog << "(none)";
	clog << "\n";
    }

    return w;
}


// find highest (realized) toplevel Shell
Widget findTheTopLevelShell(Widget w)
{
    if (findParent_debug)
	clog << "findTopLevelShellParent(" << longName(w) << ") = ";

    Widget found = 0;

    while (w != 0)
    {
	if (XtIsWidget(w)
	    && XtIsTopLevelShell(w) 
	    && XtDisplay(w) != 0
	    && XtScreen(w) != 0
	    && XtWindow(w) != 0
	    && XtIsRealized(w))
	    found = w;
	w = XtParent(w);
    }

    if (findParent_debug)
    {
	if (found != 0)
	    clog << longName(found);
	else
	    clog << "(none)";
	clog << "\n";
    }

    return found;
}
