// $Id$
// find specific parent

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char findParent_rcsid[] = 
    "$Id$";

#include <Xm/Xm.h>
#include <iostream.h>

#include "findParent.h"
#include "longName.h"

// Set this to 1 to allow debugging
int findParent_debug = 0;

// find a realized Shell
Widget findShellParent(Widget w)
{
    if (findParent_debug)
	clog << "findShellParent(" << longName(w) << ") = ";

    while (w != 0 && !(XtIsShell(w) && XtIsRealized(w)))
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

    while (w != 0 && !(XtIsTopLevelShell(w) && XtIsRealized(w)))
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
	if (XtIsTopLevelShell(w) && XtIsRealized(w))
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
