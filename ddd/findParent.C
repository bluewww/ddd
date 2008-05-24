// $Id$
// find specific parent

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

char findParent_rcsid[] = 
    "$Id$";

#include "config.h"

#if defined(IF_XM)
#include <Xm/Xm.h>
#endif
#include <iostream>

#include "findParent.h"
#include "longName.h"
#include "bool.h"

#if defined(IF_XM)
// ANSI C++ doesn't like the XtIsRealized() macro
#ifdef XtIsRealized
#undef XtIsRealized
#endif
#endif

// Set this to true to allow debugging
static const bool findParent_debug = false;

#if defined(IF_XM)
// Find a realized Shell
Widget findShellParent(Widget w)
{
    if (findParent_debug)
	std::clog << "findShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsShell(w)
		      || !XtIsRealized(w)
		      || XtDisplay(w) == None
		      || XtScreen(w) == None
		      || XtWindow(w) == None))
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
Widget findTopLevelShellParent(Widget w)
{
    if (findParent_debug)
	std::clog << "findTopLevelShellParent(" << longName(w) << ") = ";

    while (w != 0 && (!XtIsWidget(w)
		      || !XtIsTopLevelShell(w)
		      || !XtIsRealized(w)
		      || XtDisplay(w) == 0
		      || XtScreen(w) == 0
		      || XtWindow(w) == 0))
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
#else
// Find a realized Shell
GUI::Shell *findShellParent(GUI::Widget *w)
{
    if (findParent_debug)
	std::clog << "findShellParent(" << w->get_name() << ") = ";

    while (w != 0 && (!dynamic_cast<GUI::Shell *>(w)
		      || !w->is_realized()))
	w = w->get_parent();

    if (findParent_debug)
    {
	if (w != 0)
	    std::clog << w->get_name();
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

    return w?dynamic_cast<GUI::Shell *>(w):NULL;
}


// find a (realized) toplevel Shell
GUI::Shell *findTopLevelShellParent(GUI::Widget *w)
{
    if (findParent_debug)
	std::clog << "findTopLevelShellParent(" << w->get_name() << ") = ";

    while (w != 0 && (!dynamic_cast<GUI::Shell *>(w)
		      || !w->is_realized()))
	w = w->get_parent();

    if (findParent_debug)
    {
	if (w != 0)
	    std::clog << w->get_name();
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

    return w?dynamic_cast<GUI::Shell *>(w):NULL;
}


// find highest (realized) toplevel Shell
GUI::Shell *findTheTopLevelShell(GUI::Widget *w)
{
    if (findParent_debug)
	std::clog << "findTopLevelShellParent(" << w->get_name() << ") = ";

    GUI::Widget *found = NULL;

    while (w != 0)
    {
	if (dynamic_cast<GUI::Shell *>(w)
	    && w->is_realized())
	    found = w;
	w = w->get_parent();
    }

    if (findParent_debug)
    {
	if (found != 0)
	    std::clog << found->get_name();
	else
	    std::clog << "(none)";
	std::clog << "\n";
    }

    return w?dynamic_cast<GUI::Shell *>(found):NULL;
}
#endif
