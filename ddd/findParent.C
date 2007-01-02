// $Id$
// find specific parent

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char findParent_rcsid[] = 
    "$Id$";

#include "config.h"

#ifdef IF_MOTIF
#include <Xm/Xm.h>
#endif // IF_MOTIF
#include <iostream>

#include "findParent.h"
#include "longName.h"
#include "bool.h"

#ifdef IF_MOTIF
// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#endif // IF_MOTIF

// Set this to true to allow debugging
static const bool findParent_debug = false;

// Find a realized Shell
Widget findShellParent(Widget w)
{
    if (findParent_debug)
	std::clog << "findShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsShell(w)
		      || !XtIsRealized(w)
		      || XtDisplay(w)
		      || XtScreen(w)
		      || XtWindow(w)))
	w = XtParent(w);

    if (findParent_debug)
    {
	if (w != 0)
	    std::clog << longName(w);
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

    return w;
}


// find a (realized) toplevel Shell
WINDOW_P findTopLevelShellParent(Widget w)
{
    if (findParent_debug)
	std::clog << "findTopLevelShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsTopLevelShell(w)
		      || !XtIsRealized(w)
		      || !XtDisplay(w)
		      || !XtScreen(w)
		      || !XtWindow(w)))
	w = XtParent(w);

    if (findParent_debug)
    {
	if (w != 0)
	    std::clog << longName(w);
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

#ifdef IF_MOTIF
    return w;
#else // NOT IF_MOTIF
    return dynamic_cast<Gtk::Window *>(w);
#endif // IF_MOTIF
}


// find highest (realized) toplevel Shell
Widget findTheTopLevelShell(Widget w)
{
    if (findParent_debug)
	std::clog << "findTopLevelShellParent(" << longName(w) << ") = ";

    Widget found = 0;

    while (w != 0)
    {
	if (XtIsWidget(w)
	    && XtIsTopLevelShell(w) 
	    && XtIsRealized(w)
	    && XtDisplay(w) != 0
	    && XtScreen(w) != 0
	    && XtWindow(w) != 0)
	    found = w;
	w = XtParent(w);
    }

    if (findParent_debug)
    {
	if (found != 0)
	    std::clog << longName(found);
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

    return found;
}
