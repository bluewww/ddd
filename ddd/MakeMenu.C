// $Id$
// Create custom Motif Menus

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
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

char MakeMenu_rcsid[] = 
    "$Id$";

#include "MakeMenu.h"

#include "assert.h"
#include "strclass.h"
#include "MString.h"
#include "TimeOut.h"

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/MenuShell.h>
#include <X11/Xutil.h>

#if XmVersion >= 2000
#include <Xm/SpinB.h>
#endif

#include "LessTifH.h"
#include "bool.h"
#include "verify.h"
#include "findParent.h"
#include "frame.h"



// Pushmenu callbacks
static void ArmPushMenuCB(Widget, XtPointer, XtPointer);
static void RedrawPushMenuCB(Widget, XtPointer, XtPointer);
static void PopupPushMenuAct(Widget w, XEvent* e, String *, Cardinal *);
static void DecoratePushMenuAct(Widget w, XEvent* e, String *, Cardinal *);

static Widget MMcreatePushMenu(Widget parent, String name, MMDesc items[]);

static XtActionsRec actions [] = {
    {"popup-push-menu",            PopupPushMenuAct },
    {"decorate-push-menu",         DecoratePushMenuAct },
};

static char pushMenuTranslations[] = 
    "<Expose>:          decorate-push-menu()\n"
;

static char lesstif_pushMenuTranslations[] = 
    "None<Btn3Down>:	popup-push-menu()\n"
;

struct PushMenuInfo {
    Widget widget;		// The PushButton
    Widget subMenu;		// Submenu of this PushButton
    bool flat;			// Whether the PushButton is flattened

    PushMenuInfo(Widget w, Widget s, bool f)
	: widget(w), subMenu(s), flat(f)
    {}
};

//-----------------------------------------------------------------------
// Auto-raise stuff
//-----------------------------------------------------------------------

// Whether menus should be auto-raised
#define XtNautoRaiseMenu  "autoRaiseMenu"
#define XtCAutoRaiseMenu  "AutoRaiseMenu"

struct MMresource_values {
    Boolean auto_raise_menu;
};

static XtResource MMsubresources[] = {
    {
	XtNautoRaiseMenu,
	XtCAutoRaiseMenu,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(MMresource_values, auto_raise_menu),
	XmRImmediate,
	XtPointer(False)
    }
};


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

static void auto_raise(Widget shell)
{
    assert(XmIsMenuShell(shell));

    // Get text
    MMresource_values values;
    XtGetApplicationResources(shell, &values, 
			      MMsubresources, XtNumber(MMsubresources), 
			      NULL, 0);

    if (values.auto_raise_menu)
    {
	XtAddEventHandler(shell, VisibilityChangeMask, False,
			  AutoRaiseEH, XtPointer(0));
    }
}


//-----------------------------------------------------------------------
// Flat buttons
//-----------------------------------------------------------------------

// The currently unflattened button
static Widget active_button = 0;

static void flatten_button(Widget w)
{
    XtVaSetValues(w,
		  XmNhighlightThickness, 2,
		  XmNshadowThickness, 0,
		  NULL);
}

static void unflatten_button(Widget w)
{
    XtVaSetValues(w,
		  XmNhighlightThickness, 0,
		  XmNshadowThickness, 2,
		  NULL);
}

static void FlattenEH(Widget w,
		      XtPointer /* client_data */,
		      XEvent *event, 
		      Boolean * /* continue_to_dispatch */)
{
    if (event->xcrossing.state & (Button1Mask | Button2Mask | Button3Mask | 
				  Button4Mask | Button5Mask))
	return;			// Button is still pressed

    switch (event->type)
    {
    case EnterNotify:
    {
	unflatten_button(w);
	active_button = w;
	break;
    }

    case LeaveNotify:
    {
	flatten_button(w);
	active_button = 0;
	break;
    }
    }
}

static void FlattenCB(Widget w, XtPointer client_data, XtPointer)
{
    Boolean set = Boolean(client_data);
    if (set)
    {
	flatten_button(w);
	active_button = 0;
    }
    else
    {
	unflatten_button(w);
	active_button = w;
    }
}

static void ReflattenButtonCB(Widget /* shell */, XtPointer client_data, 
			      XtPointer = 0)
{
    Widget w = (Widget)client_data;
    EventMask event_mask = EnterWindowMask | LeaveWindowMask;
    XtAddEventHandler(w, event_mask, False, FlattenEH, 
		      XtPointer(0));
    XtAddCallback(w, XmNarmCallback,    FlattenCB, XtPointer(False));
    XtAddCallback(w, XmNdisarmCallback, FlattenCB, XtPointer(True));
    flatten_button(w);
}


//-----------------------------------------------------------------------
// Add items
//-----------------------------------------------------------------------

// Add items to shell.  If IGNORE_SEPS is set, all separators are ignored.
void MMaddItems(Widget shell, MMDesc items[], bool ignore_seps = false)
{
    static bool actions_added = false;
		
    if (!actions_added)
    {
	XtAppAddActions(XtWidgetToApplicationContext(shell), 
			actions, XtNumber(actions));
	actions_added = true;
    }

    Arg args[10];
    int arg;

    // Create lots of buttons...
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	String name             = item->name;
	MMType type             = item->type;
	Widget& widget          = item->widget;
	Widget *widgetptr       = item->widgetptr;
	MMDesc *subitems        = item->items;
	Widget& label           = item->label;

	if (type & MMIgnore)
	    continue;		// Don't create

	string subMenuName = string(name) + "Menu";
	string panelName   = string(name) + "Panel";
	static string textName  = "text";
	static string labelName = "label";
	static string spinName  = "spin";
	Widget subMenu = 0;
	Widget panel   = 0;
	Widget spin    = 0;
	bool flat = false;
	PushMenuInfo *info = 0;
	label = 0;

	switch(type & MMTypeMask) 
	{
	case MMFlatPush:
	    flat = true;
	    // FALL THROUGH

	case MMPush:
	    // Create a PushButton
	    arg = 0;
	    if (flat)
	    {
		XtSetArg(args[arg], XmNshadowThickness,    0); arg++;
		XtSetArg(args[arg], XmNhighlightThickness, 2); arg++;
	    }

	    if (lesstif_version < 1000)
	    {
		// LessTif wants the PushButton as parent of the menu
		widget = verify(XmCreatePushButton(shell, name, args, arg));

		if (subitems != 0)
		{
		    subMenu = MMcreatePushMenu(widget, subMenuName, subitems);
		    info = new PushMenuInfo(widget, subMenu, flat);
		    XtVaSetValues(widget, XmNuserData, XtPointer(info), NULL);
		}
	    }
	    else
	    {
		// Motif wants the shell as parent of the menu
		if (subitems != 0)
		{
		    subMenu = MMcreatePushMenu(shell, subMenuName, subitems);
		    info = new PushMenuInfo(0, subMenu, flat);
		    XtSetArg(args[arg], XmNuserData, XtPointer(info)); arg++;
		}

		widget = verify(XmCreatePushButton(shell, name, args, arg));

		if (info != 0)
		    info->widget = widget;
	    }
	    label = widget;
	    break;

	case MMToggle:
	    // Create a ToggleButton
	    assert(subitems == 0);

	    arg = 0;
	    label = widget = 
		verify(XmCreateToggleButton(shell, name, args, arg));
	    break;

	case MMLabel:
	    // Create a Label
	    assert(subitems == 0);

	    arg = 0;
	    label = widget = 
		verify(XmCreateLabel(shell, name, args, arg));
	    break;

	case MMArrow:
	    // Create an arrow
	    assert(subitems == 0);

	    arg = 0;
	    widget = verify(XmCreateArrowButton(shell, name, args, arg));
	    break;

	case MMMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    label = widget = 
		verify(XmCreateCascadeButton(shell, name, args, arg));

            if (lesstif_version <= 79)
	    {
		// LessTif 0.79 and earlier has a very tight packing
		// of menu items; place a few spaces around the labels
		// to increase item distance.
		XmString old_label;
		XtVaGetValues(widget, XmNlabelString, &old_label, NULL);
		MString new_label(old_label, true);
		XmStringFree(old_label);

		if (!new_label.isNull())
		{
		    new_label = MString("  ") + new_label + MString("  ");
		    XtVaSetValues(widget, 
				  XmNlabelString, new_label.xmstring(), 
				  NULL);
		}

		// Same applies to accelerator texts.
		XmString old_acc;
		XtVaGetValues(widget, XmNacceleratorText, &old_acc, NULL);
		MString new_acc(old_acc, true);
		XmStringFree(old_acc);

		if (!new_acc.isNull())
		{
		    new_acc = MString("  ") + new_acc;
		    XtVaSetValues(widget, 
				  XmNacceleratorText, new_acc.xmstring(), 
				  NULL);
		}
	    }
	    break;

	case MMRadioMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    subMenu = MMcreateRadioPulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    label = widget = 
		verify(XmCreateCascadeButton(shell, name, args, arg));
	    break;

	case MMOptionMenu:
	    // Create an option menu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    label = widget = 
		verify(XmCreateOptionMenu(shell, name, args, arg));
	    break;

	case MMPanel:
	case MMRadioPanel:
	case MMButtonPanel:
	    // Create a label with an associated panel
	    assert(subitems != 0);

	    arg = 0;
	    XtSetArg(args[arg], XmNorientation,  XmHORIZONTAL); arg++;
	    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
	    XtSetArg(args[arg], XmNentryBorder,  0); arg++;
	    XtSetArg(args[arg], XmNspacing,      0); arg++;
	    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
	    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
	    widget = verify(XmCreateRowColumn(shell, panelName, args, arg));

	    if (name[0] != '\0')
	    {
		arg = 0;
		label = verify(XmCreateLabel(widget, name, args, arg));
		XtManageChild(label);
	    }

	    switch (type)
	    {
	    case MMPanel:
		subMenu = MMcreatePanel(widget, subMenuName, subitems);
		break;

	    case MMRadioPanel:
		subMenu = MMcreateRadioPanel(widget, subMenuName, subitems);
		break;

	    case MMButtonPanel:
		subMenu = MMcreateButtonPanel(widget, subMenuName, subitems);
		break;
	    }
		
	    XtVaSetValues(subMenu,
			  XmNorientation, XmHORIZONTAL,
			  NULL);
	    XtManageChild(subMenu);
	    break;

	case MMScale:
	    // Create a scale
	    assert(subitems == 0);

	    arg = 0;
	    widget = verify(XmCreateScale(shell, name, args, arg));
	    break;

	case MMTextField:
	case MMEnterField:
	case MMSpinField:
	    // Create a label with an associated text field
	    assert(subitems == 0);

	    arg = 0;
	    XtSetArg(args[arg], XmNorientation,  XmHORIZONTAL); arg++;
	    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
	    XtSetArg(args[arg], XmNentryBorder,  0); arg++;
	    XtSetArg(args[arg], XmNspacing,      0); arg++;
	    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
	    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
	    panel = verify(XmCreateRowColumn(shell, name, args, arg));

	    arg = 0;
	    label = verify(XmCreateLabel(panel, labelName, args, arg));
	    XtManageChild(label);

	    spin = panel;
	    if ((type & MMTypeMask) == MMSpinField)
	    {
#if XmVersion >= 2000
		arg = 0;
		spin = XmCreateSpinBox(panel, spinName, args, arg);
		XtManageChild(spin);
#endif
	    }

	    arg = 0;
	    widget = verify(XmCreateTextField(spin, textName, args, arg));
	    XtManageChild(widget);
	    break;

	case MMSeparator:
	    // Create a separator
	    assert(subitems == 0);

	    if (ignore_seps)
		continue;
	    arg = 0;
	    widget = verify(XmCreateSeparator(shell, name, args, arg));
	    break;

	default:
	    // invalid type
	    assert(0);
	    abort();
	}

	if (type & MMHelp)
	{
	    arg = 0;
	    XtSetArg(args[arg], XmNmenuHelpWidget, item->widget); arg++;
	    XtSetValues(shell, args, arg);
	}

	if (panel == 0)
	    panel = widget;

	if (type & MMInsensitive)
	    XtSetSensitive(panel, False);

	if (!(type & MMUnmanaged))
	    XtManageChild(panel);

	if (widgetptr)
	    *widgetptr = widget;
    }
}


//-----------------------------------------------------------------------
// Custom menu creation
//-----------------------------------------------------------------------

// Create pulldown menu from items
Widget MMcreatePulldownMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget menu = verify(XmCreatePulldownMenu(parent, name, args, arg));
    MMaddItems(menu, items);
    auto_raise(XtParent(menu));

    return menu;
}

// Create radio pulldown menu from items
Widget MMcreateRadioPulldownMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    XtSetArg(args[arg], XmNisHomogeneous, True); arg++;
    XtSetArg(args[arg], XmNentryClass, xmToggleButtonWidgetClass); arg++;
    XtSetArg(args[arg], XmNradioBehavior, True); arg++;

    Widget menu = verify(XmCreatePulldownMenu(parent, name, args, arg));
    MMaddItems(menu, items);
    auto_raise(XtParent(menu));

    return menu;
}

// Create popup menu from items
Widget MMcreatePopupMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget menu = verify(XmCreatePopupMenu(parent, name, args, arg));
    MMaddItems(menu, items);

    // Don't auto-raise popup menus.
    // 1. There are conflicts with nested popups.
    // 2. During a popup, the pointer is grabbed such that we won't
    //    have an auto-raised window anyway.
    // auto_raise(XtParent(menu));

    return menu;
}


// Create menu bar from items
Widget MMcreateMenuBar(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget bar = verify(XmCreateMenuBar(parent, name, args, arg));
    MMaddItems(bar, items);
    XtManageChild(bar);

    return bar;
}

// Create work area from items
Widget MMcreateWorkArea(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget bar = verify(XmCreateWorkArea(parent, name, args, arg));
    MMaddItems(bar, items, true);
    XtManageChild(bar);

    return bar;
}

// Create panel from items
Widget MMcreatePanel(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget panel = verify(XmCreateWorkArea(parent, name, args, arg));
    MMaddItems(panel, items);
    XtManageChild(panel);

    return panel;
}

// Create radio panel from items
Widget MMcreateRadioPanel(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    XtSetArg(args[arg], XmNisHomogeneous, True); arg++;
    XtSetArg(args[arg], XmNentryClass, xmToggleButtonWidgetClass); arg++;
    XtSetArg(args[arg], XmNradioBehavior, True); arg++;
    Widget panel = verify(XmCreateRowColumn(parent, name, args, arg));
    MMaddItems(panel, items);
    XtManageChild(panel);

    return panel;
}

// Create button panel from items
Widget MMcreateButtonPanel(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget panel = verify(XmCreateRowColumn(parent, name, args, arg));
    MMaddItems(panel, items);
    XtManageChild(panel);

    return panel;
}

// Perform proc on items
void MMonItems(MMDesc items[], MMItemProc proc, XtPointer closure)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	if (item->type & MMIgnore)
	    continue;

	proc(item, closure);

	if (item->items)
	    MMonItems(item->items, proc, closure);
    }
}


//-----------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------

// Add callbacks to items
static void addCallback(MMDesc *item, XtPointer default_closure)
{
    Arg args[10];
    int arg;

    MMType type             = item->type;
    Widget widget           = item->widget;
    XtCallbackRec callback  = item->callback;
    Widget subMenu          = 0;
    void *userData          = 0;
    PushMenuInfo *info      = 0;
    
    if (callback.closure == 0)
	callback.closure = default_closure;

    bool flat = false;

    switch(type & MMTypeMask) 
    {
    case MMFlatPush:
	flat = true;
	// FALL THROUGH

    case MMPush:
	arg = 0;
	XtSetArg(args[arg], XmNuserData, &userData); arg++;
	XtGetValues(widget, args, arg);

	if (userData != 0)
	{
	    info = (PushMenuInfo *)userData;

	    // A 'push menu' is a menu associated with a push button.
	    // It pops up after pressing the button a certain time.
	    XtAddCallback(widget, XmNarmCallback,    ArmPushMenuCB, info);
	    XtAddCallback(widget, XmNarmCallback,    RedrawPushMenuCB, 0);
	    XtAddCallback(widget, XmNdisarmCallback, RedrawPushMenuCB, 0);

	    static XtTranslations translations =
		    XtParseTranslationTable(pushMenuTranslations);
	    XtAugmentTranslations(widget, translations);

	    if (lesstif_version <= 81)
	    {
		// In LessTif 0.81 and earlier, one must use button 3
		// to pop up push menus
		static XtTranslations lesstif_translations =
		    XtParseTranslationTable(lesstif_pushMenuTranslations);
		XtAugmentTranslations(widget, lesstif_translations);
	    }
	}

	if (flat)
	{
	    ReflattenButtonCB(widget, XtPointer(widget));
	}

	// FALL THROUGH

    case MMArrow:
	if (callback.callback != 0)
	    XtAddCallback(widget, 
			  XmNactivateCallback,
			  callback.callback, 
			  callback.closure);
	else
	    XtSetSensitive(widget, False);
	break;

    case MMToggle:
    case MMScale:
	if (callback.callback != 0)
	    XtAddCallback(widget,
			  XmNvalueChangedCallback,
			  callback.callback, 
			  callback.closure);
	else
	    XtSetSensitive(widget, False);
	break;

    case MMTextField:
    case MMSpinField:
	if (callback.callback != 0)
	    XtAddCallback(widget,
			  XmNvalueChangedCallback,
			  callback.callback, 
			  callback.closure);

	if ((type & MMTypeMask) == MMTextField)
	    break;

    case MMEnterField:
	if (callback.callback != 0)
	    XtAddCallback(widget,
			  XmNactivateCallback,
			  callback.callback, 
			  callback.closure);
	break;

    case MMMenu:
    case MMRadioMenu:
    case MMOptionMenu:
	arg = 0;
	XtSetArg(args[arg], XmNsubMenuId, &subMenu); arg++;
	XtGetValues(widget, args, arg);

	if (callback.callback != 0)
	{
	    XtAddCallback(subMenu, 
			  XmNmapCallback,
			  callback.callback, 
			  callback.closure);
	}
	break;

    case MMLabel:
    case MMSeparator:
    case MMPanel:
    case MMRadioPanel:
    case MMButtonPanel:
	assert(callback.callback == 0);
	break;

    default:
	// invalid type
	assert(0);
	abort();
    }
}

void MMaddCallbacks(MMDesc items[], XtPointer default_closure)
{
    MMonItems(items, addCallback, default_closure);
}


// Add help callback
static void addHelpCallback(MMDesc *item, XtPointer closure)
{
    Widget widget       = item->widget;
    XtCallbackProc proc = XtCallbackProc(closure);

    XtAddCallback(widget, XmNhelpCallback, proc, XtPointer(0));
}

void MMaddHelpCallback(MMDesc items[], XtCallbackProc proc)
{
    MMonItems(items, addHelpCallback, XtPointer(proc));
}



//-----------------------------------------------------------------------
// PushMenus
//-----------------------------------------------------------------------

// Create pushmenu from items
static Widget MMcreatePushMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    // By default, PushButton menus are activated using Button 1.
    arg = 0;

    if (XmVersion < 1002 || lesstif_version < 1000)
    {
	// Setting the menuPost resource is required by Motif 1.1 and
	// LessTif.  However, OSF/Motif 2.0 (and OSF/Motif 1.2,
	// according to Roy Dragseth <royd@math.uit.no>) choke on this
	// line - buttons become entirely insensitive.
	XtSetArg(args[arg], XmNmenuPost, "<Btn1Down>"); arg++;
    }

#if XmVersion >= 1002
    // Tear-off push menus don't work well - in LessTif, they cause
    // frequent X errors, and in Motif, they disable the old menus
    // once torn off.  So, we explicitly disable them.
    XtSetArg(args[arg], XmNtearOffModel, XmTEAR_OFF_DISABLED); arg++;
#endif
    
    Widget menu = verify(XmCreatePopupMenu(parent, name, args, arg));
    MMaddItems(menu, items);

    // Don't auto-raise popup menus.
    // 1. There are conflicts with nested popups.
    // 2. During a popup, the pointer is grabbed such that we won't
    //    have an auto-raised window anyway.
    // auto_raise(XtParent(menu));

    // LessTif places a passive grab on the parent, such that the
    // pointer is grabbed as soon as the menuPost event occurs.  This
    // grab breaks PushMenus, so we cancel it.  Motif places a passive
    // grab on button 3, such that the pointer is grabbed as soon as
    // button 3 is pressed.  In Motif 1.1, it even remains grabbed!
    // This breaks any X session, so we cancel it.
    XtUngrabButton(parent, AnyButton, AnyModifier);

    return menu;
}




static XEvent last_push_menu_event; // Just save it

// Remove time out again
static void RemoveTimeOutCB(Widget w, XtPointer client_data, XtPointer)
{
    XtIntervalId id = (XtIntervalId)client_data;
    XtRemoveTimeOut(id);
    XtRemoveCallback(w, XmNdisarmCallback, RemoveTimeOutCB, XtPointer(id));
}

// Popup menu right now
static void PopupPushMenuCB(XtPointer client_data, XtIntervalId *id)
{
    PushMenuInfo *info = (PushMenuInfo *)client_data;
    Widget w = info->widget;

    XtRemoveCallback(w, XmNdisarmCallback, RemoveTimeOutCB, XtPointer(*id));

    // Popup the menu
    XtCallActionProc(w, "popup-push-menu", &last_push_menu_event, 0, 0);

    // Unactivate pushbutton
    XtCallActionProc(w, "Disarm",          &last_push_menu_event, 0, 0);

#if XmVersion < 1002
    if (XtIsRealized(w))
    {
	// In Motif 1.1, the PushButton does not redisplay after being disarmed
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
    }
#endif
}

static void ReflattenButtonEH(Widget shell, XtPointer client_data, 
			      XEvent *event, Boolean *)
{
    switch (event->type)
    {
    case UnmapNotify:
	ReflattenButtonCB(shell, client_data);
	break;

    default:
	break;
    }
}


static void PopupPushMenuAct(Widget w, XEvent *event, String *, Cardinal *)
{
    if (!XmIsPushButton(w))
	return;

    void *userData = 0;
    XtVaGetValues(w, XmNuserData, &userData, XtPointer(0));
    if (userData == 0)
	return;

    PushMenuInfo *info = (PushMenuInfo *)userData;

    Widget shell = XtParent(info->subMenu);

    // Attempt to place menu below button
    Position button_x, button_y;
    XtTranslateCoords(w, 0, 0, &button_x, &button_y);

    Dimension height = 0;
    unsigned char unit_type;
    XtVaGetValues(w, XmNheight, &height, XmNunitType, &unit_type, NULL);
    Dimension button_height = 
	XmConvertUnits(w, XmVERTICAL, XmPIXELS, height, unit_type);

    Position x = button_x;
    Position y = button_y + button_height;

    event->xbutton.x_root = x;
    event->xbutton.y_root = y;
    XmMenuPosition(info->subMenu, &event->xbutton);

    if (info->flat)
    {
	// Don't have the PushMenu interfere with flattening.  Disable
	// flattening until the menu is popped down again.
	EventMask event_mask = EnterWindowMask | LeaveWindowMask;
	XtRemoveEventHandler(w, event_mask, False, FlattenEH, 
			     XtPointer(0));
	XtRemoveCallback(w, XmNarmCallback,    FlattenCB, XtPointer(False));
	XtRemoveCallback(w, XmNdisarmCallback, FlattenCB, XtPointer(True));

#if 0
	XtAddCallback(shell, XtNpopdownCallback, ReflattenButtonCB, 
		      XtPointer(w));
#else
	// XtNpopDownCallback isn't called by LessTif and unknown in
	// Motif 1.1.  *Sigh*.
	XtAddEventHandler(shell, StructureNotifyMask, False, 
			  ReflattenButtonEH, XtPointer(w));
#endif
    }

    XtManageChild(info->subMenu);
    XtPopup(shell, XtGrabNone);
}

static void DecoratePushMenuAct(Widget w, XEvent */* event */, 
				String *, Cardinal *)
{
    if (!XmIsPushButton(w) || !XtIsRealized(w))
	return;

    // clog << "Redraw " << XtName(w) << "\n";

    // Draw a little triangle in upper right corner
    XPoint points[4] = {
	{-6, 1}, {5, 0}, {-3, 3}, {-2, -3}
    };

    Dimension width;
    Dimension highlightThickness;
    Dimension shadowThickness;
    Pixel foreground;
    Pixel background;
    Colormap colormap;
    XtVaGetValues(w,
		  XmNwidth, &width,
		  XmNhighlightThickness, &highlightThickness, 
		  XmNshadowThickness, &shadowThickness,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  XmNcolormap, &colormap,
		  NULL);

    points[0].x += width - highlightThickness - shadowThickness;
    points[0].y += highlightThickness + shadowThickness;

    Pixel color;
    XmGetColors(XtScreen(w), colormap, background, 0, 0, &color, 0);

    XGCValues xgc;
    xgc.foreground = color;
    xgc.background = background;
    GC gc = XtGetGC(w, GCForeground | GCBackground, &xgc);

    XFillPolygon(XtDisplay(w), XtWindow(w), gc, points, XtNumber(points), 
		 Convex, CoordModePrevious);
}



// Popup menu after some delay
struct subresource_values {
    int push_menu_popup_time;	// Delay before popping up menu
};

static XtResource subresources[] = {
    {
	XtNpushMenuPopupTime,
	XtCPushMenuPopupTime,
	XmRInt,
	sizeof(int),
	XtOffsetOf(subresource_values, push_menu_popup_time), 
	XmRImmediate,
	XtPointer(500)
    }
};

static void ArmPushMenuCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    subresource_values values;
    XtGetApplicationResources(w, &values, 
			      subresources, XtNumber(subresources), 
			      NULL, 0);

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    if (cbs && cbs->event)
	last_push_menu_event = *cbs->event;

    XtIntervalId id = XtAppAddTimeOut(XtWidgetToApplicationContext(w), 
				      values.push_menu_popup_time, 
				      PopupPushMenuCB, client_data);
    XtAddCallback(w, XmNdisarmCallback, RemoveTimeOutCB, XtPointer(id));
}

static void RedrawPushMenuCB(Widget w, XtPointer, XtPointer)
{
    XtCallActionProc(w, "decorate-push-menu", 0, 0, 0);
}


void set_sensitive(Widget w, bool state)
{
    if (w != 0)
    {
	XtSetSensitive(w, state);

	if (!state && w == active_button)
	{
	    // We won't get the LeaveWindow event, since W is now
	    // insensitive.  Flatten button explicitly.
	    flatten_button(w);
	}
    }
}
