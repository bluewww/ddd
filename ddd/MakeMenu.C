// $Id$
// Create custom Motif Menus

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/MenuShell.h>
#include <X11/Xutil.h>

#include "LessTifH.h"
#include "bool.h"
#include "verify.h"
#include "findParent.h"
#include "windows.h"

// Pushmenu callbacks
static void ArmPushMenuCB(Widget, XtPointer, XtPointer);
static void RedrawPushMenuCB(Widget, XtPointer, XtPointer);
static void PopupPushMenuAct(Widget w, XEvent* e, String *, Cardinal *);
static void DecoratePushMenuAct(Widget w, XEvent* e, String *, Cardinal *);

static XtActionsRec actions [] = {
    {"popup-push-menu",            PopupPushMenuAct },
    {"decorate-push-menu",         DecoratePushMenuAct },
};

static char pushMenuTranslations[] = 
    "None<Btn3Down>:	popup-push-menu()\n"
    "<Expose>:          decorate-push-menu()\n"
;


// Make sure menu stays on top.  This prevents conflicts with
// auto-raise windows which would otherwise hide menu panels.
static void StayOnTopEH(Widget w, XtPointer, XEvent *event, Boolean *)
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

static void stay_on_top(Widget shell)
{
    assert(XmIsMenuShell(shell));

    XtAddEventHandler(shell, VisibilityChangeMask, False,
		      StayOnTopEH, XtPointer(0));
}

// Add items to shell
static void addItems(Widget /* parent */, Widget shell, MMDesc items[], 
		     bool ignore_seps = false)
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
	Widget &widget          = item->widget;
	Widget *widgetptr       = item->widgetptr;
	MMDesc *subitems        = item->items;

	string subMenuName = string(name) + "Menu";
	string panelName   = string(name) + "Panel";
	Widget subMenu  = 0;
	Widget label    = 0;

	switch(type & MMTypeMask) 
	{
	case MMPush:
	    // Create a PushButton
	    if (subitems != 0)
		subMenu = MMcreatePopupMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNuserData, subMenu); arg++;
	    widget = verify(XmCreatePushButton(shell, name, args, arg));
	    break;

	case MMToggle:
	    // Create a ToggleButton
	    assert(subitems == 0);

	    arg = 0;
	    widget = verify(XmCreateToggleButton(shell, name, args, arg));
	    break;

	case MMLabel:
	    // Create a Label
	    assert(subitems == 0);

	    arg = 0;
	    widget = verify(XmCreateLabel(shell, name, args, arg));
	    break;

	case MMMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateCascadeButton(shell, name, args, arg));

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
	    widget = verify(XmCreateCascadeButton(shell, name, args, arg));
	    break;

	case MMOptionMenu:
	    // Create an option menu
	    assert(subitems != 0);

	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateOptionMenu(shell, name, args, arg));
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

	    arg = 0;
	    label = verify(XmCreateLabel(widget, name, args, arg));
	    XtManageChild(label);

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

	if (type & MMInsensitive)
	    XtSetSensitive(widget, False);

	if (!(type & MMUnmanaged))
	    XtManageChild(widget);

	if (widgetptr)
	    *widgetptr = widget;
    }
}



// Create pulldown menu from items
Widget MMcreatePulldownMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget menu = verify(XmCreatePulldownMenu(parent, name, args, arg));
    addItems(parent, menu, items);
    stay_on_top(XtParent(menu));

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
    addItems(parent, menu, items);
    stay_on_top(XtParent(menu));

    return menu;
}

// Create popup menu from items
Widget MMcreatePopupMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget menu = verify(XmCreatePopupMenu(parent, name, args, arg));
    addItems(parent, menu, items);

    // Don't auto-raise popup menus.
    // 1. There are conflicts with nested popups.
    // 2. During a popup, the pointer is grabbed such that we won't
    //    have an auto-raised window anyway.
    // stay_on_top(XtParent(menu));

    return menu;
}


// Create menu bar from items
Widget MMcreateMenuBar(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget bar = verify(XmCreateMenuBar(parent, name, args, arg));
    addItems(parent, bar, items);
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
    addItems(parent, bar, items, true);
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
    addItems(parent, panel, items);
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
    addItems(parent, panel, items);
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
    addItems(parent, panel, items);
    XtManageChild(panel);

    return panel;
}

// Perform proc on items
void MMonItems(MMDesc items[], MMItemProc proc, XtPointer closure)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	proc(item, closure);

	if (item->items)
	    MMonItems(item->items, proc, closure);
    }
}


// Add callbacks to items
static void addCallback(MMDesc *item, XtPointer default_closure)
{
    Arg args[10];
    int arg;

    MMType type             = item->type;
    Widget widget           = item->widget;
    XtCallbackRec callback  = item->callback;
    Widget subMenu          = 0;
    
    if (callback.closure == 0)
	callback.closure = default_closure;

    switch(type & MMTypeMask) 
    {
    case MMPush:
	arg = 0;
	XtSetArg(args[arg], XmNuserData, &subMenu); arg++;
	XtGetValues(widget, args, arg);

	if (subMenu != 0)
	{
	    // A 'push menu' is a menu associated with a push button.
	    // It pops up after pressing the button a certain time.
	    XtAddCallback(widget, XmNarmCallback,    ArmPushMenuCB, subMenu);
	    XtAddCallback(widget, XmNarmCallback,    RedrawPushMenuCB, 0);
	    XtAddCallback(widget, XmNdisarmCallback, RedrawPushMenuCB, 0);
	    
	    static XtTranslations translations = 
		XtParseTranslationTable(pushMenuTranslations);
	    XtAugmentTranslations(widget, translations);
	}

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

    case MMMenu:
    case MMRadioMenu:
    case MMOptionMenu:
	arg = 0;
	XtSetArg(args[arg], XmNsubMenuId, &subMenu); arg++;
	XtGetValues(widget, args, arg);

	if (callback.callback != 0)
	    XtAddCallback(subMenu, 
			  XmNmapCallback,
			  callback.callback, 
			  callback.closure);
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

// Handle PushButton menu popups

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
    Widget w = (Widget)client_data;

    XtRemoveCallback(w, XmNdisarmCallback, RemoveTimeOutCB, XtPointer(*id));

    // Popup the menu
    XtCallActionProc(w, "popup-push-menu", &last_push_menu_event, 0, 0);

    // Unactivate pushbutton
    XtCallActionProc(w, "Disarm",          &last_push_menu_event, 0, 0);

#if XmVersion < 1002
    // In Motif 1.1, the PushButton does not redisplay after being disarmed
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
#endif
}

void PopupPushMenuAct(Widget w, XEvent *event, String *, Cardinal *)
{
    if (!XmIsPushButton(w))
	return;

    Widget subMenu;
    XtVaGetValues(w, XmNuserData, &subMenu, XtPointer(0));
    if (subMenu == 0)
	return;

    Widget shell = XtParent(subMenu);

    // Attempt to place menu below button
    Position button_x, button_y;
    XtTranslateCoords(w, 0, 0, &button_x, &button_y);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    unsigned char unit_type;

    XtQueryGeometry(w, NULL, &size);
    XtVaGetValues(w, XmNunitType, &unit_type, NULL);
    Dimension button_height = XmConvertUnits(w, XmVERTICAL, XmPIXELS,
					     size.height, unit_type);
    Position x = button_x;
    Position y = button_y + button_height;

    event->xbutton.x_root = x;
    event->xbutton.y_root = y;
    XmMenuPosition(subMenu, &event->xbutton);

    XtManageChild(subMenu);
    XtPopup(shell, XtGrabNone);
}

void DecoratePushMenuAct(Widget w, XEvent */* event */, String *, Cardinal *)
{
    if (!XmIsPushButton(w))
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

static void ArmPushMenuCB(Widget w, XtPointer, XtPointer call_data)
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
				      PopupPushMenuCB, w);
    XtAddCallback(w, XmNdisarmCallback, RemoveTimeOutCB, XtPointer(id));
}

static void RedrawPushMenuCB(Widget w, XtPointer, XtPointer)
{
    XtCallActionProc(w, "decorate-push-menu", 0, 0, 0);
}
