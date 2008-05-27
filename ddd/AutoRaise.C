// $Id$ -*- C++ -*-
// Auto-Raising Menus

// Copyright (C) 1999 Universitaet Passau, Germany.
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

char AutoRaise_rcsid[] = 
    "$Id$";

#include "AutoRaise.h"
#include <Xm/Xm.h>
#include <iostream>

#include "bool.h"
#include "frame.h"
#include "longName.h"

#ifndef LOG_AUTO_RAISE
#define LOG_AUTO_RAISE 0
#endif


//-----------------------------------------------------------------------
// Auto-raise stuff
//-----------------------------------------------------------------------

struct MMresource_values {
    Boolean  auto_raise_menu;
    Cardinal auto_raise_menu_delay;
};

static XtResource MMsubresources[] = {
    {
	XTRESSTR(XtNautoRaiseMenu),
	XTRESSTR(XtCAutoRaiseMenu),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(MMresource_values, auto_raise_menu),
	XmRImmediate,
	XtPointer(False)
    },
    {
	XTRESSTR(XtNautoRaiseMenuDelay),
	XTRESSTR(XtCAutoRaiseMenuDelay),
	XmRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(MMresource_values, auto_raise_menu_delay),
	XmRImmediate,
	XtPointer(100)
    }
};


// Ignore auto-raise for a certain delay
static void DontIgnoreAutoRaiseCB(XtPointer client_data, XtIntervalId *)
{
    bool& ignore = *((bool *)client_data);
    ignore = false;
}

// Make sure menu stays on top.  This prevents conflicts with
// auto-raise windows which would otherwise hide menu panels.
static void AutoRaiseEH(Widget shell, XtPointer, XEvent *event, Boolean *)
{
    static bool ignore = false;

    if (event->type != VisibilityNotify)
	return;

    if (ignore)
    {
#if LOG_AUTO_RAISE
	std::clog << "Ignoring visibility change of " << longName(shell) << "\n";
#endif
	return;
    }

    switch (event->xvisibility.state)
    {
    case VisibilityUnobscured:
	// This window has just been raised.  Be sure to ignore
	// further auto-raises.

#if LOG_AUTO_RAISE
	std::clog << "Raised " << longName(shell) << "\n";
#endif

	MMresource_values values;
	XtGetApplicationResources(shell, &values, 
				  MMsubresources, XtNumber(MMsubresources), 
				  ArgList(0), 0);

	if (values.auto_raise_menu_delay > 0)
	{
	    // Ignore all further auto-raise events during
	    // AUTO_RAISE_MENU_DELAY ms.  The reason is that raising this
	    // window may obscure another auto-raised window, and we do not
	    // want these two to be in conflict.

	    ignore = true;
	    XtAppAddTimeOut(XtWidgetToApplicationContext(shell),
			    values.auto_raise_menu_delay,
			    DontIgnoreAutoRaiseCB, (XtPointer)&ignore);
	}
	break;

    case VisibilityFullyObscured:
    case VisibilityPartiallyObscured:

#if LOG_AUTO_RAISE
	std::clog << "Raising " << longName(shell) << "\n";
#endif
	XRaiseWindow(XtDisplay(shell), frame(shell));

	break;
    }
}

// Auto-raise SHELL unconditionally
void _auto_raise(Widget shell)
{
#if LOG_AUTO_RAISE
    std::clog << "Auto-raise enabled for " << longName(shell) << "\n";
#endif

    XtAddEventHandler(shell, VisibilityChangeMask, False,
		      AutoRaiseEH, XtPointer(0));
}

// Auto-raise SHELL if resource is set
void auto_raise(Widget shell)
{
    // Get flag
    MMresource_values values;
    XtGetApplicationResources(shell, &values, 
			      MMsubresources, XtNumber(MMsubresources), 
			      ArgList(0), 0);

    if (values.auto_raise_menu)
	_auto_raise(shell);
}
