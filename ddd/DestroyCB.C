// $Id$
// NORA Destroy Callbacks

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

char DestroyCB_rcsid[] = 
    "$Id$";

#include "findParent.h"
#include "DestroyCB.h"


// callbacks

// destroy the surrounding shell
void DestroyShellCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget shell = findShellParent(widget);
    XtDestroyWidget(shell);
}

// destroy specific widget
void DestroyThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget w = Widget(client_data);
    XtDestroyWidget(w);
}

// unmanage the surrounding shell
void UnmanageShellCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget shell = findShellParent(widget);
    XtUnmanageChild(shell);
}

// unmanage specific widget
void UnmanageThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget w = Widget(client_data);
    XtUnmanageChild(w);
}
