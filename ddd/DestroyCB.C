// $Id$
// NORA Destroy Callbacks

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char DestroyCB_rcsid[] = 
    "$Id$";

#include "findParent.h"
#include "DestroyCB.h"


// callbacks

// destroy the surrounding shell
void DestroyShellCB(Widget widget, XtPointer, XtPointer)
{
    Widget shell = findShellParent(widget);
    XtDestroyWidget(shell);
}

// destroy specific widget
void DestroyThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);
    XtDestroyWidget(w);
}

// unmanage the surrounding shell
void UnmanageShellCB(Widget widget, XtPointer, XtPointer)
{
    Widget shell = findShellParent(widget);
    XtUnmanageChild(shell);
}

// unmanage specific widget
void UnmanageThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);
    XtUnmanageChild(w);
}
