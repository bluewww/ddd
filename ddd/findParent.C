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

// $Log$
// Revision 1.1  1995/05/01 15:47:58  zeller
// Initial revision
//
// Revision 1.8  1993/09/01  16:28:05  zeller
// Neu: debugging
// Fix: Schleifenbedingungen korrigiert
//
// Revision 1.7  1993/07/27  22:35:21  zeller
// Fix: Nur solche Shells finden, die auch realisiert sind
//
// Revision 1.6  1993/05/22  20:10:13  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.5  1993/04/22  11:04:04  zeller
// Lizenz verbessert
// 
// Revision 1.4  1993/04/16  11:41:59  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/02/20  14:57:17  zeller
// Neu findTheTopLevelShell()
// 
// Revision 1.2  1992/02/20  14:31:48  zeller
// findShellParent() korrigiert
// 
// Revision 1.1  1992/02/20  14:20:47  zeller
// Initial revision
// 

char findParent_rcsid[] = "$Id$";

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
