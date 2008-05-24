// $Id$ -*- C++ -*-
// Simple `File', `Edit', and `Help' menus

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

char simpleMenu_rcsid[] = 
    "$Id$";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "simpleMenu.h"

#include "config.h"
#include "MakeMenu.h"
#include "HelpCB.h"
#include "TextSetS.h"
#include "WhatNextCB.h"
#include "events.h"
#include "exit.h"
#include "findParent.h"
#include "tips.h"
#include "show.h"

#if defined(IF_XM)
#include <Xm/Text.h>
#include <Xm/TextF.h>
#endif

#include "gtk_wrapper.h"


//-----------------------------------------------------------------------------
// Basic functions
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
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
    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
    if (!editable)
	return;

    if (XmIsText(dest))
	XmTextSetString(dest, XMST(""));
    else if (XmIsTextField(dest))
	XmTextFieldSetString(dest, XMST(""));
}
#else
static bool same_shell(GUI::Widget *w1, GUI::Widget *w2)
{
    if (w1 == 0 || w2 == 0)
	return false;

    GUI::Shell *shell_1 = findTopLevelShellParent(w1);
    GUI::Shell *shell_2 = findTopLevelShellParent(w2);

    return shell_1 == shell_2;
}

static Boolean cut(GUI::Widget *w, GUI::Widget *dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    std::cerr << "cut() not implemented yet\n";
//    if (XmIsText(dest))
//	success = XmTextCut(dest, tm);
//    else if (XmIsTextField(dest))
//	success = XmTextFieldCut(dest, tm);

    return success;
}

static Boolean copy(GUI::Widget *w, GUI::Widget *dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    std::cerr << "copy() not implemented yet\n";
//    if (XmIsText(dest))
//	success = XmTextCopy(dest, tm);
//    else if (XmIsTextField(dest))
//	success = XmTextFieldCopy(dest, tm);

    return success;
}

static Boolean paste(GUI::Widget *w, GUI::Widget *dest)
{
    if (!same_shell(w, dest))
	return False;

    std::cerr << "paste() not implemented yet\n";

//    Boolean editable = False;
//    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
//    if (!editable)
//	return False;

    Boolean success = False;

//    if (XmIsText(dest))
//	success = XmTextPaste(dest);
//    else if (XmIsTextField(dest))
//	success = XmTextFieldPaste(dest);

    return success;
}

static void clear(GUI::Widget *w, GUI::Widget *dest)
{
    if (!same_shell(w, dest))
	return;

    Boolean editable = False;

    std::cerr << "clear() not implemented yet\n";
//    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
//    if (!editable)
//	return;

//    if (XmIsText(dest))
//	XmTextSetString(dest, XMST(""));
//    else if (XmIsTextField(dest))
//	XmTextFieldSetString(dest, XMST(""));
}
#endif


#if defined(IF_XM)
static Boolean select(Widget w, Widget dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    if (!success && XmIsText(dest))
    {
	TextSetSelection(dest, 0, XmTextGetLastPosition(dest), tm);
	success = True;
    }
    else if (!success && XmIsTextField(dest))
    {
	TextFieldSetSelection(dest, 0, 
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
    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
    if (!editable)
	return False;

    Boolean success = False;
    if (XmIsText(dest))
	success = XmTextRemove(dest);
    else if (XmIsTextField(dest))
	success = XmTextFieldRemove(dest);

    return success;
}
#else
static Boolean select(GUI::Widget *w, GUI::Widget *dest, Time tm)
{
    if (!same_shell(w, dest))
	return False;

    Boolean success = False;

    std::cerr << "select() not implemented yet\n";
//    if (!success && XmIsText(dest))
//    {
//	TextSetSelection(dest, 0, XmTextGetLastPosition(dest), tm);
//	success = True;
//    }
//    else if (!success && XmIsTextField(dest))
//    {
//	TextFieldSetSelection(dest, 0, 
//			      XmTextFieldGetLastPosition(dest), tm);
//	success = True;
//    }

    return success;
}

static void unselect(GUI::Widget *w, GUI::Widget *dest, Time tm)
{
    if (!same_shell(w, dest))
	return;

    std::cerr << "unselect() not implemented yet\n";
//    if (XmIsText(dest))
//	XmTextClearSelection(dest, tm);
//    else if (XmIsTextField(dest))
//	XmTextFieldClearSelection(dest, tm);
}

static Boolean remove(GUI::Widget *w, GUI::Widget *dest)
{
    if (!same_shell(w, dest))
	return False;

    std::cerr << "remove() not implemented yet\n";
//    Boolean editable = False;
//    XtVaGetValues(dest, XmNeditable, &editable, XtPointer(0));
//    if (!editable)
//	return False;

    Boolean success = False;
//    if (XmIsText(dest))
//	success = XmTextRemove(dest);
//    else if (XmIsTextField(dest))
//	success = XmTextFieldRemove(dest);

    return success;
}
#endif

//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
#else
static void UnselectAllCB(GUI::Widget *w)
{
    std::cerr << "UnselectAllCB not implemented yet\n";
//    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
//    Time tm = time(cbs->event);

//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win = Widget(client_data);

//    // Do destination window
//    unselect(w, dest, tm);

//    // Do given window
//    if (win != dest)
//	unselect(w, win, tm);
}
#endif

#if defined(IF_XM)
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
#else
static void CutCB(GUI::Widget *w)
{
    std::cerr << "CutCB not implemented yet\n";
//    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
//    Time tm = time(cbs->event);

//    Boolean success = False;
//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win  = Widget(client_data);

//    // Try destination window
//    if (!success)
//	success = cut(w, dest, tm);

//    // Try given window
//    if (!success && win != dest)
//	success = cut(w, win, tm);

//    if (success)
//	UnselectAllCB(w, client_data, call_data);
}

static void CopyCB(GUI::Widget *w)
{
    std::cerr << "CopyCB not implemented yet\n";
//    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
//    Time tm = time(cbs->event);
    
//    Boolean success = False;
//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win = Widget(client_data);

//    // Try destination window
//    if (!success)
//	success = copy(w, dest, tm);

//    // Try given window
//    if (!success && win != dest)
//	success = copy(w, win, tm);
}

static void PasteCB(GUI::Widget *w)
{
    std::cerr << "PasteCB not implemented yet\n";
//    Boolean success = False;
//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win = Widget(client_data);

//    // Try destination window
//    if (!success)
//	success = paste(w, dest);

//    // Try given window
//    if (!success && win != dest)
//	success = paste(w, win);
}
#endif

#if defined(IF_XM)
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

#else
static void ClearAllCB(GUI::Widget *w)
{
    std::cerr << "ClearAllCB not implemented yet\n";
//    UnselectAllCB(w, client_data, call_data);

//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win  = Widget(client_data);

//    // Clear destination window
//    clear(w, dest);

//    // Clear given window
//    if (win != dest)
//	clear(w, win);
}

static void SelectAllCB(GUI::Widget *w)
{
    std::cerr << "SelectAllCB not implemented yet\n";
//    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
//    Time tm = time(cbs->event);

//    Boolean success = false;
//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win  = (Widget)client_data;

//    if (!success)
//	success = select(w, dest, tm);
//    if (!success && win != dest)
//	success = select(w, win, tm);
}

static void RemoveCB(GUI::Widget *w)
{
    std::cerr << "RemoveCB not implemented yet\n";
//    Boolean success = False;
//    Widget dest = XmGetDestination(XtDisplay(w));
//    Widget win  = (Widget)client_data;

//    // Try destination window
//    if (!success)
//	success = remove(w, dest);

//    // Try given window
//    if (!success && win != dest)
//	success = remove(w, win);

//    if (success)
//	UnselectAllCB(w, client_data, call_data);
}
#endif

//-----------------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------------

// Edit menu
MMDesc simple_edit_menu[] =
{
    GENTRYL("cut", "cut", MMPush,
	    BIND(CutCB, 0),
	    sigc::ptr_fun(CutCB),
	    0, 0),
    GENTRYL("copy", "copy", MMPush,
	    BIND(CopyCB, 0),
	    sigc::ptr_fun(CopyCB),
	    0, 0),
    GENTRYL("paste", "paste", MMPush,
	    BIND(PasteCB, 0),
	    sigc::ptr_fun(PasteCB),
	    0, 0),
    GENTRYL("clearAll", "clearAll", MMPush,
	    BIND(ClearAllCB, 0),
	    sigc::ptr_fun(ClearAllCB),
	    0, 0),
    GENTRYL("delete", "delete", MMPush,
	    BIND(RemoveCB, 0),
	    sigc::ptr_fun(RemoveCB),
	    0, 0),
    MMSep,
    GENTRYL("selectAll", "selectAll", MMPush,
	    BIND(SelectAllCB, 0),
	    sigc::ptr_fun(SelectAllCB),
	    0, 0),
    MMEnd
};

// Help menu
MMDesc simple_help_menu[] = 
{
#if defined(IF_XM)
    GENTRYL("onHelp", "onHelp", MMPush,
	    BIND(HelpOnHelpCB, 0),
	    sigc::ptr_fun(HelpOnHelpCB),
	    0, 0),
    MMSep,
    GENTRYL("onItem", "onItem", MMPush,
	    BIND(HelpOnItemCB, 0),
	    sigc::ptr_fun(HelpOnItemCB),
	    0, 0),
    GENTRYL("onWindow", "onWindow", MMPush,
	    BIND(HelpOnWindowCB, 0),
	    sigc::ptr_fun(HelpOnWindowCB),
	    0, 0),
    MMSep,
    GENTRYL("whatNext", "whatNext", MMPush,
	    BIND(WhatNextCB, 0),
	    sigc::ptr_fun(WhatNextCB),
	    0, 0),
    GENTRYL("tipOfTheDay", "tipOfTheDay", MMPush,
	    BIND(TipOfTheDayCB, 0),
	    sigc::ptr_fun(TipOfTheDayCB),
	    0, 0),
    MMSep,
    GENTRYL("dddManual", "dddManual", MMPush,
	    BIND(DDDManualCB, 0),
	    sigc::ptr_fun(DDDManualCB),
	    0, 0),
    GENTRYL("news", "news", MMPush,
	    BIND(DDDNewsCB, 0),
	    sigc::ptr_fun(DDDNewsCB),
	    0, 0),
    GENTRYL("gdbManual", "gdbManual", MMPush,
	    BIND(GDBManualCB, 0),
	    sigc::ptr_fun(GDBManualCB),
	    0, 0),
    MMSep,
    GENTRYL("license", "license", MMPush,
	    BIND(DDDLicenseCB, 0),
	    sigc::ptr_fun(DDDLicenseCB),
	    0, 0),
    GENTRYL("www", "www", MMPush,
	    BIND(DDDWWWPageCB, 0),
	    sigc::ptr_fun(DDDWWWPageCB),
	    0, 0),
    MMSep,
    GENTRYL("onVersion", "onVersion", MMPush,
	    BIND(HelpOnVersionCB, 0),
	    sigc::ptr_fun(HelpOnVersionCB),
	    0, 0),
#endif
    MMEnd
};
