// $Id$ -*- C++ -*-
// Simple `File', `Edit', and `Help' menus

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

char simpleMenu_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "simpleMenu.h"

#include "MakeMenu.h"
#include "HelpCB.h"
#include "WhatNextCB.h"
#include "events.h"
#include "findParent.h"
#include "tips.h"
#include "show.h"

#include <Xm/Text.h>
#include <Xm/TextF.h>


//-----------------------------------------------------------------------------
// Basic functions
//-----------------------------------------------------------------------------

static bool same_shell(Widget w1, Widget w2)
{
    if (w1 == 0 || w2 == 0)
	return false;

    Widget shell_1 = findTopLevelShellParent(w1);
    Widget shell_2 = findTopLevelShellParent(w2);

    return shell_1 == shell_2;
}

static Boolean cut(Widget w, Widget dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    if (XmIsText(dest))
	success = XmTextCut(dest, tm);
    else if (XmIsTextField(dest))
	success = XmTextFieldCut(dest, tm);

    return success;
}

static Boolean copy(Widget w, Widget dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    if (XmIsText(dest))
	success = XmTextCopy(dest, tm);
    else if (XmIsTextField(dest))
	success = XmTextFieldCopy(dest, tm);

    return success;
}

static Boolean paste(Widget w, Widget dest)
{
    if (!same_shell(w, dest))
	return False;

    Boolean editable = False;
    XtVaGetValues(dest, XmNeditable, &editable, NULL);
    if (!editable)
	return False;

    Boolean success = False;

    if (XmIsText(dest))
	success = XmTextPaste(dest);
    else if (XmIsTextField(dest))
	success = XmTextFieldPaste(dest);

    return success;
}

static void clear(Widget w, Widget dest)
{
    if (!same_shell(w, dest))
	return;

    Boolean editable = False;
    XtVaGetValues(dest, XmNeditable, &editable, NULL);
    if (!editable)
	return;

    if (XmIsText(dest))
	XmTextSetString(dest, "");
    else if (XmIsTextField(dest))
	XmTextFieldSetString(dest, "");
}


static Boolean select(Widget w, Widget dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    if (!success && XmIsText(dest))
    {
	XmTextSetSelection(dest, 0, XmTextGetLastPosition(dest), tm);
	success = True;
    }
    else if (!success && XmIsTextField(dest))
    {
	XmTextFieldSetSelection(dest, 0, 
				XmTextFieldGetLastPosition(dest), tm);
	success = True;
    }

    return success;
}

static void unselect(Widget w, Widget dest, Time tm)
{
    if (!same_shell(w, dest))
	return;

    if (XmIsText(dest))
	XmTextClearSelection(dest, tm);
    else if (XmIsTextField(dest))
	XmTextFieldClearSelection(dest, tm);
}

static Boolean remove(Widget w, Widget dest)
{
    if (!same_shell(w, dest))
	return False;

    Boolean editable = False;
    XtVaGetValues(dest, XmNeditable, &editable, NULL);
    if (!editable)
	return False;

    Boolean success = False;
    if (XmIsText(dest))
	success = XmTextRemove(dest);
    else if (XmIsTextField(dest))
	success = XmTextFieldRemove(dest);

    return success;
}


//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

static void UnselectAllCB(Widget w, XtPointer client_data,
			  XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win = Widget(client_data);

    // Do destination window
    unselect(w, dest, tm);

    // Do given window
    if (win != dest)
	unselect(w, win, tm);
}

static void CutCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win  = Widget(client_data);

    // Try destination window
    if (!success)
	success = cut(w, dest, tm);

    // Try given window
    if (!success && win != dest)
	success = cut(w, win, tm);

    if (success)
	UnselectAllCB(w, client_data, call_data);
}

static void CopyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);
    
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win = Widget(client_data);

    // Try destination window
    if (!success)
	success = copy(w, dest, tm);

    // Try given window
    if (!success && win != dest)
	success = copy(w, win, tm);
}

static void PasteCB(Widget w, XtPointer client_data, XtPointer)
{
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win = Widget(client_data);

    // Try destination window
    if (!success)
	success = paste(w, dest);

    // Try given window
    if (!success && win != dest)
	success = paste(w, win);
}

static void ClearAllCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    UnselectAllCB(w, client_data, call_data);

    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win  = Widget(client_data);

    // Clear destination window
    clear(w, dest);

    // Clear given window
    if (win != dest)
	clear(w, win);
}

static void SelectAllCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Time tm = time(cbs->event);

    Boolean success = false;
    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win  = (Widget)client_data;

    if (!success)
	success = select(w, dest, tm);
    if (!success && win != dest)
	success = select(w, win, tm);
}

static void RemoveCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Boolean success = False;
    Widget dest = XmGetDestination(XtDisplay(w));
    Widget win  = (Widget)client_data;

    // Try destination window
    if (!success)
	success = remove(w, dest);

    // Try given window
    if (!success && win != dest)
	success = remove(w, win);

    if (success)
	UnselectAllCB(w, client_data, call_data);
}


//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

// Edit menu
MMDesc simple_edit_menu[] =
{
    { "cut",       MMPush,  { CutCB       }},
    { "copy",      MMPush,  { CopyCB      }},
    { "paste",     MMPush,  { PasteCB     }},
    { "clearAll",  MMPush,  { ClearAllCB  }},
    { "delete",    MMPush,  { RemoveCB    }},
    MMSep,
    { "selectAll", MMPush,  { SelectAllCB }},
    MMEnd
};

// Help menu
MMDesc simple_help_menu[] = 
{
    {"onHelp",      MMPush, { HelpOnHelpCB }},
    MMSep,
    {"onItem",      MMPush, { HelpOnItemCB }},
    {"onWindow",    MMPush, { HelpOnWindowCB }},
    MMSep,
    {"whatNext",    MMPush, { WhatNextCB }},
    {"tipOfTheDay", MMPush, { TipOfTheDayCB }},
    MMSep,
    {"dddManual",   MMPush, { DDDManualCB }},
    {"news",        MMPush, { DDDNewsCB }},
    {"gdbManual",   MMPush, { GDBManualCB }},
    MMSep,
    {"license",     MMPush, { DDDLicenseCB }},
    {"www",         MMPush, { DDDWWWPageCB }},
    MMSep,
    {"onVersion",   MMPush, { HelpOnVersionCB }},
    MMEnd
};
