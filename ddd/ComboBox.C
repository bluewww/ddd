// $Id$ -*- C++ -*-
// Create a combo box

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

char ComboBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

// Comment out to rely on our replacement routines
// #define USE_XM_COMBOBOX 0

#include "ComboBox.h"

#include "frame.h"
#include "charsets.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/ArrowB.h>
#include <Xm/TextF.h>
#include <Xm/List.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

// Whether to use XmComboBox
#ifndef USE_XM_COMBOBOX
#if XmVersion >= 2000
#define USE_XM_COMBOBOX 1
#else
#define USE_XM_COMBOBOX 0
#endif
#endif

#if USE_XM_COMBOBOX
#include <Xm/ComboBox.h>
#endif


//-----------------------------------------------------------------------
// ComboBox helpers
//-----------------------------------------------------------------------

// Make sure menu stays on top.  This prevents conflicts with
// auto-raise windows which would otherwise hide menu panels.
static void AutoRaiseEH(Widget w, XtPointer, XEvent *event, Boolean *)
{

    if (event->type != VisibilityNotify)
	return;

    switch (event->xvisibility.state)
    {
    case VisibilityFullyObscured:
    case VisibilityPartiallyObscured:
	XRaiseWindow(XtDisplay(w), frame(w));
	break;
    }
}

struct ComboBoxInfo
{
    Widget text;		// The text to be updated
    Widget button;		// The arrow button
    Widget list;		// The list to select from
    Widget shell;		// The shell that contains the list
    XtIntervalId timer;		// The timer that controls popup time
    bool popped_up;		// True iff the combo box is popped up

    ComboBoxInfo()
	: text(0), button(0), list(0), shell(0), timer(0), popped_up(false)
    {}
};

static void PopdownComboListCB(Widget, XtPointer client_data, XtPointer)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;

    XtPopdown(info->shell);
    info->popped_up = false;
}

static void RefreshComboTextCB(Widget w, XtPointer client_data,
			       XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;

    XmString item = cbs->item;
    String item_s;
    XmStringGetLtoR(item, CHARSET_TT, &item_s);
    XmTextFieldSetString(info->text, item_s);
    XtFree(item_s);

    if (info->shell != 0)
	PopdownComboListCB(w, client_data, call_data);
}

static void CloseWhenActivatedCB(XtPointer client_data, XtIntervalId *id)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;
    
    assert(*id == info->timer);
    (void) id;

    info->timer = 0;
}

static void PopupComboListCB(Widget w, XtPointer client_data, 
			     XtPointer call_data)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;

    if (info->popped_up)
    {
	PopdownComboListCB(w, client_data, call_data);
	return;
    }

    Position text_x, text_y;
    XtTranslateCoords(info->text, 0, 0, &text_x, &text_y);

    Dimension text_height = 0;
    Dimension text_width  = 0;
    XtVaGetValues(info->text, XmNheight, &text_height,
		  XmNwidth, &text_width, NULL);

    Position button_x, button_y;
    XtTranslateCoords(info->button, 0, 0, &button_x, &button_y);

    Dimension button_height = 0;
    Dimension button_width  = 0;
    XtVaGetValues(info->button, XmNheight, &button_height,
		  XmNwidth, &button_width, NULL);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(XtParent(info->list), NULL, &size);

    Position x       = text_x;
    Position y       = text_y + text_height;
    Dimension width  = button_x - text_x + button_width;
    Dimension height = size.height;

    XtVaSetValues(info->shell, XmNx, x, XmNy, y, 
		  XmNwidth, width, XmNheight, height, NULL);
    XtPopup(info->shell, XtGrabNonexclusive);
    raise_shell(info->shell);
    info->popped_up = true;

    static Cursor cursor = XCreateFontCursor(XtDisplay(info->shell), XC_arrow);
    XDefineCursor(XtDisplay(info->shell), XtWindow(info->shell), cursor);

    // If we release the button within the next 250ms, keep the menu open.
    // Otherwise, pop it down again.
    if (info->timer != 0)
	XtRemoveTimeOut(info->timer);
    info->timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(info->shell), 250,
			CloseWhenActivatedCB, XtPointer(info));
}

static void ActivatePopdownComboListCB(Widget w, XtPointer client_data, 
				       XtPointer call_data)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;
    if (info->timer == 0)
	PopdownComboListCB(w, client_data, call_data);
}


//-----------------------------------------------------------------------
// ComboBox access
//-----------------------------------------------------------------------

// Access ComboBox members
Widget ComboBoxList(Widget text)
{
    XtPointer userData;
    XtVaGetValues(text, XmNuserData, &userData, NULL);
    ComboBoxInfo *info = (ComboBoxInfo *)userData;
    return info->list;
}

Widget ComboBoxText(Widget text)
{
    return text;
}

Widget ComboBoxButton(Widget text)
{
    XtPointer userData;
    XtVaGetValues(text, XmNuserData, &userData, NULL);
    ComboBoxInfo *info = (ComboBoxInfo *)userData;
    return info->button;
}

// Set items
void ComboBoxSetList(Widget text, const StringArray& items)
{
    Widget list = ComboBoxList(text);

    XmStringTable xmlist = 
	XmStringTable(XtMalloc(items.size() * sizeof(XmString)));

    int i;
    for (i = 0; i < items.size(); i++)
	xmlist[i] = XmStringCreateLtoR(items[i], CHARSET_TT);

    XtVaSetValues(list,
		  XmNitems,     xmlist,
		  XmNitemCount, items.size(),
		  NULL);

    for (i = 0; i < items.size(); i++)
	XmStringFree(xmlist[i]);

    // It seems XMLIST is already owned by LIST
    // XtFree((char *)xmlist);
}


//-----------------------------------------------------------------------
// ComboBox creation
//-----------------------------------------------------------------------

// Create a combo box
Widget CreateComboBox(Widget parent, String name, ArgList _args, Cardinal _arg)
{
    ArgList args = new Arg[_arg + 10];
    Cardinal arg = 0;

    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];

    ComboBoxInfo *info = 0;

#if USE_XM_COMBOBOX
    info = new ComboBoxInfo;

    Widget combobox = 
	verify(XmCreateDropDownComboBox(parent, name, args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    XtSetValues(combobox, args, arg);
    XtManageChild(combobox);

    info->text = XtNameToWidget(combobox, "*Text");
    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness, 2); arg++;
    XtSetValues(info->text, args, arg);

    info->list = XtNameToWidget(combobox, "*List");
    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness, 2); arg++;
    XtSetValues(info->list, args, arg);
#endif

    // Add combo box arrow if still needed
    if (info == 0)
    {
	info = new ComboBoxInfo;

	info->text = verify(XmCreateTextField(parent, name, args, arg));
	XtManageChild(info->text);

	Pixel foreground;
	XtVaGetValues(parent, XmNbackground, &foreground, 0);

	arg = 0;
	XtSetArg(args[arg], XmNarrowDirection,  XmARROW_DOWN); arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0);            arg++;
	XtSetArg(args[arg], XmNforeground,      foreground);   arg++;
	info->button = XmCreateArrowButton(parent, "comboBoxArrow", args, arg);
	XtManageChild(info->button);
	XtAddCallback(info->button, XmNarmCallback,
		      PopupComboListCB, XtPointer(info));
	XtAddCallback(info->button, XmNactivateCallback,
		      ActivatePopdownComboListCB, XtPointer(info));

	XtAddCallback(info->text, XmNvalueChangedCallback,
		      PopdownComboListCB, XtPointer(info));
	XtAddCallback(info->text, XmNactivateCallback,
		      PopdownComboListCB, XtPointer(info));

	Widget shell = parent;
	while (!XtIsShell(shell))
	    shell = XtParent(shell);

	XtAddCallback(shell, XmNpopdownCallback,
		      PopdownComboListCB, XtPointer(info));

	arg = 0;
	XtSetArg(args[arg], XmNborderWidth, 0); arg++;
	info->shell = XtCreatePopupShell("comboBoxShell", 
					 overrideShellWidgetClass,
					 shell, args, arg);

	XtAddEventHandler(info->shell, VisibilityChangeMask, False,
			  AutoRaiseEH, XtPointer(0));

	arg = 0;
	XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
	info->list = XmCreateScrolledList(info->shell, "list", 
					  args, arg);
	XtManageChild(info->list);
    }

    // Store ComboBox info in text field
    XtVaSetValues(info->text, XmNuserData, XtPointer(info), NULL);

    // Synchronize text and list
    XtAddCallback(info->list, XmNbrowseSelectionCallback,
		  RefreshComboTextCB, XtPointer(info));
    XtAddCallback(info->list, XmNsingleSelectionCallback,
		  RefreshComboTextCB, XtPointer(info));
    XtAddCallback(info->list, XmNmultipleSelectionCallback,
		  RefreshComboTextCB, XtPointer(info));
    XtAddCallback(info->list, XmNextendedSelectionCallback,
		  RefreshComboTextCB, XtPointer(info));

    delete[] args;
    return info->text;
}
