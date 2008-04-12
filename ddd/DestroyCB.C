// $Id$
// Destroy Callbacks

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
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

char DestroyCB_rcsid[] = 
    "$Id$";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include "DestroyCB.h"
#include "TimeOut.h"

#if defined(IF_MOTIF)
#include <Xm/DialogS.h>
#include <Xm/Xm.h>
#endif

#if defined(IF_MOTIF)
static void CancelTimer(Widget, XtPointer client_data, XtPointer)
{
    XtIntervalId id = XtIntervalId(client_data);
    XtRemoveTimeOut(id);
}
#endif

#if defined(IF_XM)

static void DestroyCB(XtPointer client_data, XtIntervalId *id)
{
    Widget w = Widget(client_data);

    if (w != 0)
    {
	XtRemoveCallback(w, XmNdestroyCallback, CancelTimer, XtPointer(*id));
	XtDestroyWidget(w);
    }
}

#else

static bool DestroyCB(GUI::Widget *w)
{

    if (w != 0)
    {
	delete w;
	return false;
    }
}

#endif

#if defined(IF_XM)

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle(Widget widget)
{
    XtIntervalId id = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 0, DestroyCB, 
			XtPointer(widget));

    // Should WIDGET be destroyed beforehand, cancel the timer
    XtAddCallback(widget, XmNdestroyCallback, CancelTimer, XtPointer(id));
}

#else

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle(GUI::Widget *widget)
{
    GUI::signal_idle().connect(sigc::bind(sigc::ptr_fun(DestroyCB), widget));

    // Should WIDGET be destroyed beforehand, cancel the timer
}

#endif


// Callbacks

#if defined(IF_XM)

// Destroy the ancestor shell
void DestroyShellCB(Widget widget, XtPointer, XtPointer)
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

    DestroyThisCB(widget, XtPointer(w), XtPointer(0));
}

#else

// Destroy the ancestor shell
void DestroyShellCB(GUI::Widget *widget)
{
    std::cerr << "DestroyShellCB?\n";
}

#endif

// Destroy specific widget

#if defined(IF_MOTIF)

void DestroyThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);
    DestroyWhenIdle(w);
}

#else

void DestroyThisCB(GUI::Widget *w)
{
    DestroyWhenIdle(w);
}

#endif

#if defined(IF_XM)

// Unmanage the ancestor shell
void UnmanageShellCB(Widget widget, XtPointer, XtPointer call_data)
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

    UnmanageThisCB(widget, XtPointer(w), call_data);
}

#else

// Unmanage the ancestor shell
void UnmanageShellCB(GUI::Widget *widget)
{
    std::cerr << "UnmanageShellCB?\n";
}

#endif

// Unmanage specific widget
#if defined(IF_XM)

void UnmanageThisCB(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);

    Widget shell = w;
    if (!XtIsShell(shell))
	shell = XtParent(shell);

    if (shell != 0 && XtIsShell(shell) && !XmIsDialogShell(shell))
	XtPopdown(XtCastShell(shell));

    XtUnmanageChild(w);
}

#else

void UnmanageThisCB(GUI::Widget *w)
{
    w->hide();
}

#endif

