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

#include "config.h"

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

#if defined(IF_MOTIF)
static void DestroyCB(XtPointer client_data, XtIntervalId *id)
#else
static bool DestroyCB(Widget w)
#endif
{
#if defined(IF_MOTIF)
    Widget w = Widget(client_data);
#endif

    if (w != 0)
    {
#ifdef IF_MOTIF
	XtRemoveCallback(w, XmNdestroyCallback, CancelTimer, XtPointer(*id));
	XtDestroyWidget(w);
#else // NOT IF_MOTIF
	delete w;
	return false;
#endif // IF_MOTIF
    }
}

#if defined(IF_MOTIF)

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle(Widget widget)
{
    XtIntervalId id = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(widget), 0, DestroyCB, 
			XtPointer(widget));

    // Should WIDGET be destroyed beforehand, cancel the timer
    XtAddCallback(widget, XmNdestroyCallback, CancelTimer, XtPointer(id));
}

#endif

#if defined(IF_XMMM)

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle1(GUI::Widget *widget)
{
    XtIntervalId id = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(widget->internal()), 0, DestroyCB, 
			XtPointer(widget->internal()));

    // Should WIDGET be destroyed beforehand, cancel the timer
    XtAddCallback(widget->internal(), XmNdestroyCallback, CancelTimer, XtPointer(id));
}

#elif !defined(IF_MOTIF)

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle(Gtk::Widget *widget)
{
    XtIntervalId id = 
	Glib::signal_idle().connect(sigc::bind(sigc::ptr_fun(DestroyCB), widget));

    // Should WIDGET be destroyed beforehand, cancel the timer
}

// Destroy WIDGET as soon as we are idle
void DestroyWhenIdle1(GUI::Widget *widget)
{
    XtIntervalId id = 
	Glib::signal_idle().connect(sigc::bind(sigc::ptr_fun(DestroyCB), widget->internal()));

    // Should WIDGET be destroyed beforehand, cancel the timer
}

#endif


// Callbacks

// Destroy the ancestor shell
void DestroyShellCB(CB_ARG_LIST_1(widget))
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

#ifdef IF_MOTIF
    DestroyThisCB(widget, XtPointer(w), XtPointer(0));
#else // NOT IF_MOTIF
    DestroyThisCB(w);
#endif // IF_MOTIF
}

// Destroy specific widget
void DestroyThisCB(
#ifdef IF_MOTIF
    Widget, XtPointer client_data, XtPointer
#else // NOT IF_MOTIF
    Widget w
#endif // IF_MOTIF
    )
{
#ifdef IF_MOTIF
    Widget w = Widget(client_data);
#endif // IF_MOTIF
    DestroyWhenIdle(w);
}

// Unmanage the ancestor shell
void UnmanageShellCB(CB_ARG_LIST_1(widget))
{
    Widget w = widget;

    while (w != 0 && !XtIsShell(XtParent(w)))
	w = XtParent(w);

#if defined(IF_XM)
    UnmanageThisCB1(widget, XtPointer(w), XtPointer(0));
#else
    UnmanageThisCB2(widget);
#endif
}

// Unmanage specific widget
#if defined(IF_MOTIF)

void UnmanageThisCB1(Widget, XtPointer client_data, XtPointer)
{
    Widget w = Widget(client_data);

    Widget shell = w;
    if (!XtIsShell(shell))
	shell = XtParent(shell);

    if (shell != 0 && XtIsShell(shell) && !XmIsDialogShell(shell))
	XtPopdown(XtCastShell(shell));

    XtUnmanageChild(w);
}

#endif

#if !defined(IF_XM)

void UnmanageThisCB(GUI::Widget *w)
{
    w->hide();
}

void UnmanageThisCB2(Widget w)
{
#if defined(IF_XMMM)
    UnmanageThisCB1(w, NULL, NULL);
#else
    w->hide();
#endif
}

#endif

