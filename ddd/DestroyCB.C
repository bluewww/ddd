// $Id$
// Destroy Callbacks

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char DestroyCB_rcsid[] = 
    "$Id$";

#include "DestroyCB.h"
#include "TimeOut.h"
#include <Xm/DialogS.h>

static void DestroyCB(XtPointer client_data, XtIntervalId *)
{
    Widget w = Widget(client_data);
    if (w != 0)
	XtDestroyWidget(w);
}

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle(Widget widget)
{
    XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 0, DestroyCB, 
		    XtPointer(widget));
}


// Callbacks

// Destroy the ancestor shell
void DestroyShellCB(Widget widget, XtPointer, XtPointer call_data)
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

    DestroyThisCB(widget, XtPointer(w), call_data);
}

// Destroy specific widget
void DestroyThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);
    DestroyWhenIdle(w);
}

// Unmanage the ancestor shell
void UnmanageShellCB(Widget widget, XtPointer, XtPointer call_data)
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

    UnmanageThisCB(widget, XtPointer(w), call_data);
}

// Unmanage specific widget
void UnmanageThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);

    Widget shell = w;
    if (!XtIsShell(shell))
	shell = XtParent(shell);

    if (shell != 0 && XtIsShell(shell) && !XmIsDialogShell(shell))
	XtPopdown(shell);

    XtUnmanageChild(w);
}
