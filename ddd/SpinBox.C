// $Id$ -*- C++ -*-
// Create a spin box

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

char SpinBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

// #define as 0 to rely exclusively on our replacement routines
// #define USE_XM_SPINBOX 0

#include "SpinBox.h"

#include "assert.h"
#include "strclass.h"
#include "string-fun.h"
#include "verify.h"

#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/ArrowB.h>
#include <Xm/TextF.h>

// Whether to use XmSpinBox
#ifndef USE_XM_SPINBOX
#if XmVersion >= 2000
#define USE_XM_SPINBOX 1
#else
#define USE_XM_SPINBOX 0
#endif
#endif

#if USE_XM_SPINBOX
#include <Xm/SpinB.h>
#endif


//-----------------------------------------------------------------------
// SpinBox helpers
//-----------------------------------------------------------------------

#if !USE_XM_SPINBOX
static void add_to_value(Widget text, int offset)
{
    String value = XmTextFieldGetString(text);
    int v = atoi(value) + offset;

    if (v >= 0)
    {
	string s = itostring(v);
	XmTextFieldSetString(text, (String)s);
    }
    XtFree(value);
}

static Widget spin_text;
static XtIntervalId spin_timer;
static int spin_offset;

static void RepeatSpinCB(XtPointer, XtIntervalId *id)
{
    assert(*id == spin_timer);
    (void) id;

    add_to_value(spin_text, spin_offset);

    spin_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(spin_text),
				 200, RepeatSpinCB, 0);
}

static void StartSpinCB(Widget w, XtPointer client_data, XtPointer)
{
    spin_text = Widget(client_data);

    unsigned char direction = (unsigned char)-1;
    XtVaGetValues(w, XmNarrowDirection, &direction, NULL);

    switch (direction)
    {
    case XmARROW_LEFT:
    case XmARROW_UP:
	spin_offset = -1;
	break;

    case XmARROW_RIGHT:
    case XmARROW_DOWN:
	spin_offset = +1;
	break;

    default:
	spin_offset = 0;
	break;
    }

    add_to_value(spin_text, spin_offset);

    if (spin_timer != 0)
    {
	XtRemoveTimeOut(spin_timer);
	spin_timer = 0;
    }

    spin_timer = XtAppAddTimeOut(XtWidgetToApplicationContext(spin_text),
				 250, RepeatSpinCB, 0);
}

static void StopSpinCB(Widget, XtPointer, XtPointer)
{
    if (spin_timer != 0)
    {
	XtRemoveTimeOut(spin_timer);
	spin_timer = 0;
    }
}

static Widget create_spin_arrow(Widget parent, unsigned char direction,
				Widget text)
{
    Pixel foreground;
    XtVaGetValues(parent, XmNbackground, &foreground, 0);

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNarrowDirection,  direction);  arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0);          arg++;
    XtSetArg(args[arg], XmNforeground,      foreground); arg++;
    Widget arrow = XmCreateArrowButton(parent, "arrow", args, arg);
    XtManageChild(arrow);

    XtAddCallback(arrow, XmNarmCallback, StartSpinCB, XtPointer(text));
    XtAddCallback(arrow, XmNdisarmCallback, StopSpinCB, XtPointer(text));

    return arrow;
}
#endif // !USE_XM_SPINBOX


//-----------------------------------------------------------------------
// SpinBox creation
//-----------------------------------------------------------------------

// Create a spin box
Widget CreateSpinBox(Widget parent, String name, ArgList _args, Cardinal _arg)
{
    ArgList args = new Arg[_arg + 10];
    Cardinal arg = 0;

    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];

    Widget spin = parent;

#if USE_XM_SPINBOX
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    spin = XmCreateSpinBox(parent, "spin", args, arg);
    XtManageChild(spin);
#endif

    Widget text = verify(XmCreateTextField(spin, name, args, arg));
    XtManageChild(text);
    
#if !USE_XM_SPINBOX
    create_spin_arrow(parent, XmARROW_LEFT,  text);
    create_spin_arrow(parent, XmARROW_RIGHT, text);
#endif

    delete[] args;
    return text;
}
