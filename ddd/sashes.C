// $Id$ -*- C++ -*-
// Kill unmanaged sashes

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char sashes_rcsid[] = 
    "$Id$";

#include "sashes.h"

#include <Xm/Xm.h>
#include <Xm/PanedW.h>
#include <X11/StringDefs.h>

#include "Sash.h"


//-----------------------------------------------------------------------------
// Sashes
//-----------------------------------------------------------------------------

// Unmanage all sashes of PANED
void unmanage_sashes(Widget paned)
{
    if (paned == 0 || !XtIsSubclass(paned, xmPanedWindowWidgetClass))
	return;

    WidgetList children   = 0;
    Cardinal num_children = 0;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  XtPointer(0));

    for (Cardinal i = 0; i < num_children; i++)
	if (XmIsSash(children[i]))
	{
	    XtUnmanageChild(children[i]);
	    XtUnmapWidget(children[i]);
	}
}

// Disable traversal for all sashes of PANED
void untraverse_sashes(Widget paned)
{
    if (paned == 0 || !XtIsSubclass(paned, xmPanedWindowWidgetClass))
	return;

    WidgetList children   = 0;
    Cardinal num_children = 0;

    XtVaGetValues(paned,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  XtPointer(0));

    for (Cardinal i = 0; i < num_children; i++)
	if (XmIsSash(children[i]))
	    XtVaSetValues(children[i], XmNtraversalOn, False, XtPointer(0));
}
