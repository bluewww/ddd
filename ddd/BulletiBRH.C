// $Id$ -*- C++ -*-
// BulletinBoard Resize Handler

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

char BulletinBoardResizeHandler_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "BulletiBRH.h"

#include <Xm/DialogS.h>
#include <Xm/BulletinB.h>
#include <iostream.h>

#ifndef XmIsBulletinBoard
#define XmIsBulletinBoard(w) XtIsSubclass(w, xmBulletinBoardWidgetClass)
#endif

#include "assert.h"
#include "BoxSize.h"


// Whenever SHELL is resized, give the size to CHILD.
static void BulletinBoardResizeHandler(Widget bboard,
				       XtPointer client_data,
				       XEvent *event, 
				       Boolean * /* continue_to_dispatch */)
{
    if (event->type != ConfigureNotify)
	return;

    Widget child = Widget(client_data);
    assert(bboard == XtParent(child));

    clog << XtName(bboard) << ": resize request\n";

    XtPointer user_data;
    Dimension width, height;
    XtVaGetValues(bboard, 
		  XmNuserData, &user_data,
		  XmNwidth, &width,
		  XmNheight, &height,
		  NULL);

    if (user_data == 0)
    {
	BoxSize *size = new BoxSize(width, height);
	clog << XtName(bboard) << ": initial size " << *size << "\n";
	XtVaSetValues(bboard,
		      XmNuserData, XtPointer(size),
		      NULL);
    }
    else
    {
	BoxSize& old_size = *((BoxSize *)user_data);
	BoxSize  new_size(width, height);
	BoxSize offset = new_size - old_size;

	clog << XtName(bboard) 
	     << ": old size " << old_size 
	     << ", new size " << new_size
	     << ", offset " << offset << "\n";

	// Increase size of CHILD by OFFSET
	Dimension child_width, child_height, border_width;
	XtVaGetValues(child,
		      XmNwidth, &child_width,
		      XmNheight, &child_height,
		      XmNborderWidth, &border_width,
		      NULL);
	XtVaSetValues(child,
		      XmNwidth, child_width + offset[X],
		      XmNheight, child_height + offset[Y],
		      NULL);

	old_size = new_size;
    }
}

// Add the handler to BBOARD.
void add_bulletin_board_resize_handler(Widget child)
{
    Widget bboard = XtParent(child);
    Widget shell  = XtParent(bboard);

    assert(XmIsBulletinBoard(bboard));
    assert(XmIsDialogShell(shell));
    (void) shell;		// Use it

#if 0				// Doesn't work yet -AZ
    EventMask event_mask = StructureNotifyMask;

    XtAddEventHandler(bboard, event_mask, False,
		      BulletinBoardResizeHandler, XtPointer(child));
#else
    (void) BulletinBoardResizeHandler; // Use it
#endif
}
