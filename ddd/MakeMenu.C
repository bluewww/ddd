// $Id$
// create custom Motif Menus

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:10  zeller
// Initial revision
//
// Revision 1.20  1995/04/24  11:37:27  zeller
// New: radio menus
//
// Revision 1.19  1995/03/22  15:02:29  zeller
// New: MMInsensitive, MMUnmanages
//
// Revision 1.18  1995/03/22  02:01:17  zeller
// New: ignore separators in work areas
//
// Revision 1.17  1995/02/22  21:11:42  luetke
// Neu: MMcreateWorkArea.
//
// Revision 1.16  1994/07/19  22:08:46  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.15  1993/10/14  11:28:33  zeller
// Neu: Widgets statt Gadgets (soll angeblich schneller sein)
// Einrueckung ueberarbeitet
//
// Revision 1.14  1993/10/14  11:22:49  zeller
// Neu: Labels
//
// Revision 1.13  1993/10/14  08:53:29  zeller
// Neu: MMcreatePopupMenu()
//
// Revision 1.12  1993/05/22  20:09:47  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.11  1993/04/22  11:04:17  zeller
// Lizenz verbessert
// 
// Revision 1.10  1993/04/16  11:41:53  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.9  1992/12/08  12:42:06  zeller
// stdlib.h (fuer abort()) eingebunden
// 
// Revision 1.8  1992/11/20  15:25:18  zeller
// Neu: MMaddHelpCallback()
// 
// Revision 1.7  1992/11/11  16:51:24  zeller
// Fix: subMenuName korrekt aufgebaut
// 
// Revision 1.6  1992/11/11  15:56:12  zeller
// Neu: widgetptr erlaubt Ablage der Widget-ID an anderer Stelle
// 
// Revision 1.5  1992/11/04  11:53:40  zeller
// strclass statt "String.h"; callback entfernt
// 
// Revision 1.4  1992/03/02  16:42:53  zeller
// Neu: Callbacks separat setzen
// 
// Revision 1.3  1992/03/02  16:11:47  zeller
// Neu: default_closure
// 
// Revision 1.2  1992/02/19  13:37:43  zeller
// Alles auf MMaddItems() zurueckgefuehrt;
// Callbacks fuer CascadeButtons eingefuehrt;
// Sensitivity auf 0 gesetzt, wenn keine Callbacks definiert
// 
// Revision 1.1  1992/02/19  12:51:07  zeller
// Initial revision
// 

#include "assert.h"
#include "strclass.h"
#include "Delay.h"

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


// Add items to shell
static void addItems(Widget parent, Widget shell, MMDesc items[], 
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
	    widget = XmCreatePushButton(shell, name, args, arg);
	    break;

	case MMToggle:
	    // Create a ToggleButton
	    assert(subitems == 0);

	    arg = 0;
	    widget = XmCreateToggleButton(shell, name, args, arg);
	    break;

	case MMLabel:
	    // Create a Label
	    assert(subitems == 0);

	    arg = 0;
	    widget = XmCreateLabel(shell, name, args, arg);
	    break;

	case MMMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    strcpy(subMenuName, string(name) + "Menu");
	    subMenu = MMcreatePulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = XmCreateCascadeButton(shell, name, args, arg);
	    break;

	case MMRadioMenu:
	    // Create a CascadeButton and a new PulldownMenu
	    assert(subitems != 0);

	    strcpy(subMenuName, string(name) + "Menu");
	    subMenu = MMcreateRadioPulldownMenu(shell, subMenuName, subitems);

	    arg = 0;
	    XtSetArg(args[arg], XmNsubMenuId, subMenu); arg++;
	    widget = XmCreateCascadeButton(shell, name, args, arg);
	    break;

	case MMSeparator:
	    // Create a separator
	    assert(subitems == 0);

	    if (ignore_seps)
		continue;
	    arg = 0;
	    widget = XmCreateSeparator(shell, name, args, arg);
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
    Widget shell = XmCreatePulldownMenu(parent, name, args, arg);
    Delay::register_shell(shell);
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

    Widget shell = XmCreatePulldownMenu(parent, name, args, arg);
    Delay::register_shell(shell);
    addItems(parent, shell, items);

    return shell;
}

// Create popup menu from items
Widget MMcreatePopupMenu(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget shell = XmCreatePopupMenu(parent, name, args, arg);
    Delay::register_shell(shell);
    addItems(parent, shell, items);

    return shell;
}


// Create menu bar from items
Widget MMcreateMenuBar(Widget parent, String name, MMDesc items[])
{
    Arg args[10];
    int arg;

    arg = 0;
    Widget bar = XmCreateMenuBar(parent, name, args, arg);
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
