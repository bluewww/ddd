// $Id$
// create custom Motif Menus

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char MakeMenu_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "strclass.h"

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/Label.h>

#include "MakeMenu.h"
#include "bool.h"
#include "verify.h"

// Add items to shell
static void addItems(Widget /* parent */, Widget shell, MMDesc items[], 
		     bool ignore_seps = false)
{
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

	char subMenuName[200];
	Widget subMenu;

	switch(type & MMTypeMask) 
	{
	case MMPush:
	    // Create a PushButton
	    assert(subitems == 0);

	    arg = 0;
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

	    strcpy(subMenuName, string(name) + "Menu");
	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateCascadeButton(shell, name, args, arg));
	    break;

	case MMRadioMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    strcpy(subMenuName, string(name) + "Menu");
	    subMenu = MMcreateRadioPulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = verify(XmCreateCascadeButton(shell, name, args, arg));
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
    Widget shell = verify(XmCreatePulldownMenu(parent, name, args, arg));
    addItems(parent, shell, items);

    return shell;
}

// Create radio pulldown menu from items
Widget MMcreateRadioPulldownMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    XtSetArg(args[arg], XmNisHomogeneous, True); arg++;
    XtSetArg(args[arg], XmNentryClass, xmToggleButtonWidgetClass); arg++;
    XtSetArg(args[arg], XmNradioBehavior, True); arg++;	// 

    Widget shell = verify(XmCreatePulldownMenu(parent, name, args, arg));
    addItems(parent, shell, items);

    return shell;
}

// Create popup menu from items
Widget MMcreatePopupMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget shell = verify(XmCreatePopupMenu(parent, name, args, arg));
    addItems(parent, shell, items);

    return shell;
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
    Widget bar = XmCreateWorkArea(parent, name, args, arg);
    if (bar == 0)
    {
	// LessTif 0.1 doesn't have work areas
	bar = verify(XmCreateRowColumn(parent, name, args, arg));
    }
    addItems(parent, bar, items, true);
    XtManageChild(bar);

    return bar;
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
    
    if (callback.closure == 0)
	callback.closure = default_closure;

    switch(type & MMTypeMask) 
    {
    case MMPush:
	if (callback.callback != 0)
	    XtAddCallback(widget, 
			  XmNactivateCallback,
			  callback.callback, 
			  callback.closure);
	else
	    XtSetSensitive(widget, False);
	break;

    case MMToggle:
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
	Widget subMenu;
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
