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

// #define as 0 to rely exclusively on our replacement routines
// #define USE_XM_COMBOBOX 0

#include "ComboBox.h"

#include "bool.h"
#include "frame.h"
#include "charsets.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/ArrowB.h>
#include <Xm/TextF.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
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

struct ComboBoxInfo
{
    Widget top;			// The top-level window
    Widget text;		// The text to be updated
    Widget button;		// The arrow button
    Widget list;		// The list to select from
    Widget shell;		// The shell that contains the list
    XtIntervalId timer;		// The timer that controls popup time
    bool popped_up;		// True iff the combo box is popped up

    ComboBoxInfo()
	: top(0), text(0), button(0), list(0), shell(0), 
	  timer(0), popped_up(false)
    {}
};


#if !USE_XM_COMBOBOX
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

static void CloseWhenActivatedCB(XtPointer client_data, XtIntervalId *id)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;
    
    assert(*id == info->timer);
    (void) id;

    info->timer = 0;
}

static void PopdownComboListCB(Widget, XtPointer client_data, XtPointer)
{
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;

    XtVaSetValues(info->button, XmNarrowDirection, XmARROW_DOWN, NULL);
    XtPopdown(info->shell);
    info->popped_up = false;
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

    // Get size and position
    Position top_x, top_y;
    XtTranslateCoords(info->top, 0, 0, &top_x, &top_y);

    Dimension top_height = 0;
    Dimension top_width  = 0;
    XtVaGetValues(info->top, XmNheight, &top_height,
		  XmNwidth, &top_width, NULL);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(XtParent(info->list), NULL, &size);

    Position x       = top_x;
    Position y       = top_y + top_height;
    Dimension width  = top_width;
    Dimension height = size.height;

    XtVaSetValues(info->shell, XmNx, x, XmNy, y, 
		  XmNwidth, width, XmNheight, height, NULL);


    // Popup shell
    XtPopup(info->shell, XtGrabNone);
    raise_shell(info->shell);
    info->popped_up = true;

    // Unmanage horizontal scrollbar
    Widget horizontal_scrollbar = 0;
    XtVaGetValues(XtParent(info->list), XmNhorizontalScrollBar, 
		  &horizontal_scrollbar, NULL);
    if (horizontal_scrollbar != 0)
	XtUnmanageChild(horizontal_scrollbar);

    XtVaSetValues(info->button, XmNarrowDirection, XmARROW_UP, NULL);

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

#endif // !USE_XM_COMBOBOX


static void RefreshComboTextCB(Widget w, XtPointer client_data,
			       XtPointer call_data)
{
    (void) w;			// Use it

    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    ComboBoxInfo *info = (ComboBoxInfo *)client_data;

    XmString item = cbs->item;
    String item_s;
    XmStringGetLtoR(item, CHARSET_TT, &item_s);
    XmTextFieldSetString(info->text, item_s);
    XtFree(item_s);

#if !USE_XM_COMBOBOX
    PopdownComboListCB(w, client_data, call_data);
#endif
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

Boolean ComboBoxIsSimple(Widget text)
{
    return ComboBoxButton(text) != 0;
}

Widget ComboBoxTop(Widget text)
{
    XtPointer userData;
    XtVaGetValues(text, XmNuserData, &userData, NULL);
    ComboBoxInfo *info = (ComboBoxInfo *)userData;
    return info->top;
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

    ComboBoxInfo *info = new ComboBoxInfo;

    arg = 0;
    XtSetArg(args[arg], XmNshadowType, XmSHADOW_IN); arg++;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    info->top = verify(XmCreateFrame(parent, "frame", args, arg));
    XtManageChild(info->top);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget form = verify(XmCreateForm(info->top, "form", args, arg));
    XtManageChild(form);

#if USE_XM_COMBOBOX
    // ComboBoxes in OSF/Motif 2.0 sometimes resize themselves without
    // apparent reason.  Prevent this by constraining them in a form.
    arg = 0;
    XtSetArg(args[arg], XmNleftAttachment,     XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,   XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNresizable,          False);         arg++;
    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];
    Widget combo = verify(XmCreateDropDownComboBox(form, name, args, arg));
    XtManageChild(combo);

    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth, 0); arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetValues(combo, args, arg);

    info->text = XtNameToWidget(combo, "*Text");
    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    XtSetValues(info->text, args, arg);

    info->list = XtNameToWidget(combo, "*List");
    arg = 0;
    XtSetArg(args[arg], XmNshadowThickness,    2); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    XtSetValues(info->list, args, arg);

    info->shell = info->list;
    while (!XtIsShell(info->shell))
	info->shell = XtParent(info->shell);

    // Set form size explicitly.
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(combo, NULL, &size);
    XtVaSetValues(form, XmNheight, size.height, XmNwidth, size.width, NULL);

    // Set frame size explicitly, too
    Dimension shadow_thickness;
    XtVaGetValues(info->top, XmNshadowThickness, &shadow_thickness, NULL);
    XtVaSetValues(info->top, XmNheight, size.height + shadow_thickness * 2, 
		  XmNwidth, size.width + shadow_thickness * 2, NULL);
#else
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,        0);     arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0);     arg++;
    XtSetArg(args[arg], XmNshadowThickness,    0);     arg++;
    XtSetArg(args[arg], XmNresizable,          False); arg++;
    for (Cardinal i = 0; i < _arg; i++)
	args[arg++] = _args[i];
    info->text = verify(XmCreateTextField(form, name, args, arg));
    XtManageChild(info->text);

    Pixel foreground;
    XtVaGetValues(parent, XmNbackground, &foreground, 0);

    arg = 0;
    XtSetArg(args[arg], XmNarrowDirection,     XmARROW_DOWN);  arg++;
    XtSetArg(args[arg], XmNborderWidth,        0);             arg++;
    XtSetArg(args[arg], XmNforeground,         foreground);    arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0);             arg++;
    XtSetArg(args[arg], XmNshadowThickness,    0);             arg++;
    XtSetArg(args[arg], XmNresizable,          False);         arg++;
    XtSetArg(args[arg], XmNrightAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,   XmATTACH_FORM); arg++;
    info->button = XmCreateArrowButton(form, "comboBoxArrow", args, arg);
    XtManageChild(info->button);

    XtVaSetValues(info->text,
		  XmNleftAttachment,   XmATTACH_FORM,
		  XmNrightAttachment,  XmATTACH_WIDGET,
		  XmNrightWidget,      info->button,
		  XmNtopAttachment,    XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);

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
				     parent, args, arg);

    XtAddEventHandler(info->shell, VisibilityChangeMask, False,
		      AutoRaiseEH, XtPointer(0));

    arg = 0;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    info->list = XmCreateScrolledList(info->shell, "list", args, arg);
    XtManageChild(info->list);

    // Set form size explicitly.
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(info->text, NULL, &size);
    XtVaSetValues(form, XmNheight, size.height, XmNwidth, size.width, NULL);

    // Set frame size explicitly, too
    Dimension shadow_thickness;
    XtVaGetValues(info->top, XmNshadowThickness, &shadow_thickness, NULL);
    XtVaSetValues(info->top, XmNheight, size.height + shadow_thickness * 2, 
		  XmNwidth, size.width + shadow_thickness * 2, NULL);
#endif

    // Give shell a little more border
    XtVaSetValues(info->shell, XmNborderWidth, 1, NULL);

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
