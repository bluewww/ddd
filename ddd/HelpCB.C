// $Id$  -*- C++ -*-
// Interactive Help Callbacks

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001, 2004 Free Software Foundation, Inc.
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

// "DDD is for wimps."
//                  -- Robert Bresner <rbresner@olf.com>

char HelpCB_rcsid[] =
    "$Id$";

#include "config.h"

#include "Agent.h"
#if defined(IF_XM)
#include "ComboBox.h"
#endif
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MakeMenu.h"
#include "SmartC.h"
#include "TimeOut.h"
#if defined(IF_XM)
#include "TextSetS.h"
#endif
#include "ddd.h"		// process_pending_events()
#include "findParent.h"
#include "isid.h"
#include "longName.h"
#include "toolbar.h"
#include "windows.h"		// set_scrolled_window_size()
#include "misc.h"		// min(), max()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#if defined(IF_XM)
#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/PanedW.h>
#include <Xm/MenuShell.h>
#include <Xm/ToggleB.h>

#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>	// LessTif hacks
#include <X11/Shell.h>
#else
#include <GUI/Bipolar.h>
#include <GUI/Dialog.h>
#include <GUI/MenuItem.h>
#endif

// Misc DDD includes
#include "LessTifH.h"
#include "strclass.h"
#include "cook.h"
#include "events.h"
#include "exit.h"
#include "simpleMenu.h"
#include "verify.h"
#include "Delay.h"
#include "StringA.h"
#include "IntArray.h"
#include "wm.h"
#include "post.h"
#include "mydialogs.h"
#include "ArgField.h"


//-----------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------

// The help system supports five resources:
// helpString          - displayed in context-sensitive help.
// helpOnVersionString - displayed in `Help On Version'.
// tipString           - displayed in small windows when entering buttons
// documentationString - displayed in the status line
// helpShowTitle       - if set, include widget name in context-sensitive help

#if defined(IF_XM)
struct help_resource_values {
    XmString helpString;
    Boolean showTitle;
};

struct help_on_version_resource_values {
    XmString helpOnVersionString;
    Boolean showTitle;
};

struct tip_resource_values {
    XmString tipString;
};

struct doc_resource_values {
    XmString documentationString;
};

static XtResource help_subresources[] = {
    {
	XTRESSTR(XtNhelpString),
	XTRESSTR(XtCHelpString),
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(help_resource_values, helpString),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNhelpShowTitle),
	XTRESSTR(XtCHelpShowTitle),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(help_resource_values, showTitle),
	XtRImmediate,
	XtPointer(False)
    }
};

static XtResource help_on_version_subresources[] = {
    {
	XTRESSTR(XtNhelpOnVersionString),
	XTRESSTR(XtCHelpOnVersionString),
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(help_on_version_resource_values, helpOnVersionString),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNhelpShowTitle),
	XTRESSTR(XtCHelpShowTitle),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(help_on_version_resource_values, showTitle),
	XtRImmediate,
	XtPointer(False)
    }
};

static XtResource tip_subresources[] = {
    {
	XTRESSTR(XtNtipString),
	XTRESSTR(XtCTipString),
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(tip_resource_values, tipString), 
	XtRImmediate,
	XtPointer(0)
    }
};

static XtResource doc_subresources[] = {
    {
	XTRESSTR(XtNdocumentationString),
	XTRESSTR(XtCDocumentationString),
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(doc_resource_values, documentationString), 
	XtRImmediate,
	XtPointer(0)
    }
};
#endif

//-----------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------

#if defined(IF_XM)
static Widget help_dialog = 0;
static Widget help_shell  = 0;

static MString NoHelpText(Widget widget);
static MString NoTipText(Widget widget, XEvent *event);
static MString NoDocumentationText(Widget widget, XEvent *event);
static void _MStringHelpCB(Widget widget, 
			   XtPointer client_data, 
			   XtPointer call_data,
			   bool help_on_help = false);

MString helpOnVersionExtraText;
#else
static GUI::Dialog *help_dialog = 0;
static GUI::Widget *help_shell  = 0;

static MString NoHelpText(GUI::Widget *widget);
static MString NoTipText(GUI::Widget *widget, GUI::Event *event);
static MString NoDocumentationText(GUI::Widget *widget, GUI::Event *event);
static void _MStringHelpCB(GUI::Widget *widget, 
			   const GUI::String &text,
			   bool help_on_help = false);

GUI::String helpOnVersionExtraText("");
#endif


//-----------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------

// A single `\n' means `no string'.  (An empty string causes the
// helpers to be called.)
static bool isNone(const MString& s)
{
    return s.isEmpty() && s.lineCount() > 1;
}

#if defined(IF_XM)
static MString get_help_string(Widget widget)
{
    // Get text
    help_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      help_subresources, XtNumber(help_subresources), 
			      ArgList(0), 0);

    MString text(values.helpString, true);
    if ((text.isNull() || text.isEmpty()) && DefaultHelpText != 0)
	text = DefaultHelpText(widget);
    if (text.isNull())
	text = NoHelpText(widget);

    if (values.showTitle)
	text.prepend(rm("Help for ") + bf(cook(longName(widget)))
		     + rm(":") + cr() + cr());

    return text;
}
#else
static GUI::String get_help_string(GUI::Widget *widget)
{
    // Get text
    return GUI::String("No help for ") + widget->get_name();
}
#endif

#if defined(IF_XM)
static MString get_help_on_version_string(Widget widget)
{
    // Get text
    help_on_version_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      help_on_version_subresources, 
			      XtNumber(help_subresources), 
			      ArgList(0), 0);

    MString text(values.helpOnVersionString, true);
    if (text.isNull() || text.isEmpty())
	text = NoHelpText(widget);

    if (values.showTitle)
	text.prepend(rm("Help on version for ") + bf(cook(longName(widget)))
		     + rm(":") + cr() + cr());

    return text;
}
#else
static GUI::String get_help_on_version_string(GUI::Widget *widget)
{
    // Get text
    return GUI::String("No help on version for ") + widget->get_name();
}
#endif

#if defined(IF_XM)
static MString _get_tip_string(Widget widget, XEvent *event)
{
    if (XmIsText(widget))
    {
	if (DefaultTipText != 0)
	    return DefaultTipText(widget, event);
	return NoTipText(widget, event);
    }

    // Get text
    tip_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      tip_subresources, XtNumber(tip_subresources), 
			      ArgList(0), 0);

    MString text(values.tipString, true);
    if (text.isNull() || isNone(text))
	return NoTipText(widget, event);

    if (text.isEmpty())
    {
	if (DefaultTipText != 0)
	    return DefaultTipText(widget, event);
	return NoTipText(widget, event);
    }

    return text;
}
#else
static MString _get_tip_string(GUI::Widget *widget, GUI::Event *event)
{
    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(widget);
    if (text) {
	if (DefaultTipText != 0)
	    return DefaultTipText(widget, event);
	return NoTipText(widget, event);
    }
    GUI::String s = GUI::String("No tip for ") + widget->get_name();
    return MString(s, true);
}
#endif

#if defined(IF_XM)
static MString prepend_label_name(Widget widget, XEvent *event, 
				  MString (*get_string)(Widget, XEvent *))
{
    MString text = get_string(widget, event);

#if 0				// Experimental
    if (XtIsSubclass(widget, xmLabelWidgetClass))
    {
	unsigned char label_type = XmSTRING;
	XtVaGetValues(widget, XmNlabelType, &label_type, XtPointer(0));
	if (label_type == XmPIXMAP)
	{
	    XmString label = 0;
	    XtVaGetValues(widget, XmNlabelString, &label, XtPointer(0));
	    if (label != 0)
	    {
		text = MString(label, true) + rm(": ") + text;
		XmStringFree(label);
	    }
	}
    }
#endif

    return text;
}
#else
static MString prepend_label_name(GUI::Widget *widget, GUI::Event *event, 
				  MString (*get_string)(GUI::Widget *, GUI::Event *))
{
    return get_string(widget, event);
}
#endif

#if defined(IF_XM)
inline MString get_tip_string(Widget widget, XEvent *event)
{
    return prepend_label_name(widget, event, _get_tip_string);
}
#else
inline MString get_tip_string(GUI::Widget *widget, GUI::Event *event)
{
    return prepend_label_name(widget, event, _get_tip_string);
}
#endif


#if defined(IF_XM)
static MString _get_documentation_string(Widget widget, XEvent *event)
{
    if (XmIsText(widget))
    {
	if (DefaultDocumentationText != 0)
	    return DefaultDocumentationText(widget, event);
	return NoDocumentationText(widget, event);
    }

    // Get text
    doc_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      doc_subresources, XtNumber(doc_subresources), 
			      ArgList(0), 0);

    MString text(values.documentationString, true);
    if (text.isNull())
	return get_tip_string(widget, event);

    if (isNone(text))
	return NoDocumentationText(widget, event);

    if (text.isEmpty())
    {
	if (DefaultDocumentationText != 0)
	    return DefaultDocumentationText(widget, event);
	return NoDocumentationText(widget, event);
    }

    return text;
}
#else
static MString _get_documentation_string(GUI::Widget *widget, GUI::Event *event)
{
    if (dynamic_cast<GUI::ScrolledText *>(widget))
    {
	if (DefaultDocumentationText != 0)
	    return DefaultDocumentationText(widget, event);
	return NoDocumentationText(widget, event);
    }

    // Get text

    GUI::String text = GUI::String("No documentation for ") + widget->get_name();
    return MString(text, true);
}
#endif

#if defined(IF_XM)
inline MString get_documentation_string(Widget widget, XEvent *event)
{
    return prepend_label_name(widget, event, _get_documentation_string);
}
#else
inline MString get_documentation_string(GUI::Widget *widget, GUI::Event *event)
{
    return prepend_label_name(widget, event, _get_documentation_string);
}
#endif


#if defined(IF_XM)
static bool call_tracking_help(XtPointer call_data, bool key_only = false)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

    if (cbs == 0)
	return key_only;

    if (cbs->event == 0)
	return key_only;

    if (cbs->event->type != KeyPress && cbs->event->type != KeyRelease)
	return key_only;

    if ((cbs->event->xkey.state & ShiftMask) == 0)
	return false;

    return true;
}
#else
static bool call_tracking_help(GUI::Event *event, bool key_only = false)
{
    if (!event)
	return key_only;

    GUI::EventKey *evk = dynamic_cast<GUI::EventKey *>(event);
    if (!evk)
	return key_only;

    if ((evk->state & GUI::SHIFT_MASK) == 0)
	return false;

    return true;
}
#endif

#if defined(IF_XM)
static void nop1(Widget) {}
void (*PostHelpOnItemHook)(Widget) = nop1;
#else
static void nop1(GUI::Widget *) {}
void (*PostHelpOnItemHook)(GUI::Widget *) = nop1;
#endif


//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

#if defined(IF_XM)
void HelpOnHelpCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (help_dialog == 0)
    {
	// Make sure help dialog is created
	ImmediateHelpCB(widget, client_data, call_data);
    }

    // Get help on the help dialog
    MString text = get_help_string(help_dialog);
    _MStringHelpCB(widget, XtPointer(text.xmstring()), call_data, true);

    PostHelpOnItemHook(help_dialog);
}
#else
void HelpOnHelpCB(GUI::Widget *widget)
{
    std::cerr << "HelpOnHelpCB not implemented\n";
}
#endif

#if defined(IF_XM)
void ImmediateHelpCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (widget == 0)
	return;

    if (call_tracking_help(call_data))
    {
	HelpOnContextCB(widget, client_data, call_data);
	return;
    }

    Delay delay;

    // Get help on this widget
    MString text = get_help_string(widget);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
    PostHelpOnItemHook(widget);
}
#else
extern void ImmediateHelpCB(GUI::Widget *w)
{
    std::cerr << "ImmediateHelpCB not implemented\n";
}
#endif

#if defined(IF_XM)
void HelpOnThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (widget == 0)
	return;

    if (call_tracking_help(call_data))
    {
	HelpOnContextCB(widget, client_data, call_data);
	return;
    }

    Delay delay;

    Widget w = (Widget)client_data;
	
    // Get help on this widget
    MString text = get_help_string(w);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
    PostHelpOnItemHook(w);
}
#else
void HelpOnThisCB(GUI::Widget *widget, GUI::Event *event)
{
    if (call_tracking_help(event))
    {
	HelpOnContextCB(widget, event);
	return;
    }

    Delay delay;

    // Get help on this widget
    GUI::String text = get_help_string(widget);
    MStringHelpCB(widget, text);
    PostHelpOnItemHook(widget);
}
#endif

#if defined(IF_XM)
void HelpOnWindowCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (call_tracking_help(call_data))
    {
	HelpOnContextCB(widget, client_data, call_data);
	return;
    }

    Delay delay;

    // Get help on the shell window
    Widget shell = findTopLevelShellParent(widget);

    MString text = get_help_string(shell);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
    PostHelpOnItemHook(shell);
}
#else
void HelpOnWindowCB(GUI::Widget *widget, GUI::Event *event)
{
    if (call_tracking_help(event))
    {
	HelpOnContextCB(widget, event);
	return;
    }

    Delay delay;

    // Get help on the shell window
    GUI::Widget *shell = findTopLevelShellParent(widget);

    GUI::String text = get_help_string(shell);
    MStringHelpCB(widget, text);
    PostHelpOnItemHook(shell);
}
#endif

#if defined(IF_XM)
void HelpOnVersionCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (call_tracking_help(call_data))
    {
	HelpOnContextCB(widget, client_data, call_data);
	return;
    }

    Delay delay;

    // Get a shell window
    Widget shell = findTopLevelShellParent(widget);

    MString text = get_help_on_version_string(shell);
    text += helpOnVersionExtraText;

    _MStringHelpCB(widget, XtPointer(text.xmstring()), call_data, false);

    // PostHelpOnItemHook(shell);
}
#else
void HelpOnVersionCB(GUI::Widget *widget, GUI::Event *event)
{
    if (call_tracking_help(event))
    {
	HelpOnContextCB(widget, event);
	return;
    }

    Delay delay;

    // Get a shell window
    GUI::Widget *shell = findTopLevelShellParent(widget);

    GUI::String text = get_help_on_version_string(shell);
    text += helpOnVersionExtraText;

    _MStringHelpCB(widget, text, false);

    // PostHelpOnItemHook(shell);
}
#endif

#if defined(IF_XM)
static void HelpDestroyCB(Widget, XtPointer client_data, XtPointer)
{
    Widget old_dialog = Widget(client_data);
    if (old_dialog == help_dialog)
    {
	help_dialog = 0;
	help_shell  = 0;
    }
}
#else
static bool HelpDestroyCB(GUI::Widget *old_dialog)
{
    if (old_dialog == help_dialog)
    {
	help_dialog = 0;
	help_shell  = 0;
    }
    return false;
}
#endif


#if defined(IF_XM)
// Default help, tip, and documentation strings
static MString NoHelpText(Widget widget)
{
    MString text = "No help available for \"";
    text += cook(longName(widget));
    text += "\"";

    return text;
}

static MString NoTipText(Widget, XEvent *)
{
    return MString(0, true);	// Empty string
}

static MString NoDocumentationText(Widget, XEvent *)
{
    return MString(0, true);	// Empty string
}
#else
// Default help, tip, and documentation strings
static MString NoHelpText(GUI::Widget *widget)
{
    GUI::String text = "No help available for \"";
    text += widget->get_name();
    text += "\"";

    return MString(text, true);
}

static MString NoTipText(GUI::Widget *, GUI::Event *)
{
    return MString("");	// Empty string
}

static MString NoDocumentationText(GUI::Widget *, GUI::Event *)
{
    return MString("");	// Empty string
}
#endif

#if defined(IF_XM)
static XmTextPosition NoTextPosOfEvent(Widget, XEvent *)
{
    return XmTextPosition(-1);
}
#else
static long NoTextPosOfEvent(GUI::ScrolledText *, GUI::Event *)
{
    return -1;
}
#endif

#if defined(IF_XM)
MString (*DefaultHelpText)(Widget)                    = NoHelpText;
MString (*DefaultTipText)(Widget, XEvent *)           = NoTipText;
MString (*DefaultDocumentationText)(Widget, XEvent *) = NoDocumentationText;
XmTextPosition (*TextPosOfEvent)(Widget, XEvent *)    = NoTextPosOfEvent;
#else
MString (*DefaultHelpText)(GUI::Widget *)                    = NoHelpText;
MString (*DefaultTipText)(GUI::Widget *, GUI::Event *)           = NoTipText;
MString (*DefaultDocumentationText)(GUI::Widget *, GUI::Event *) = NoDocumentationText;
long (*TextPosOfEvent)(GUI::ScrolledText *, GUI::Event *)    = NoTextPosOfEvent;
#endif


void (*DisplayDocumentation)(const MString&) = 0;

#if defined(IF_XM)
void StringHelpCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    MString text = (String)client_data;

    MStringHelpCB(widget, text.xmstring(), call_data);
}

void MStringHelpCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    _MStringHelpCB(widget, client_data, call_data);
}

static void _MStringHelpCB(Widget widget, 
			   XtPointer client_data, 
			   XtPointer,
			   bool help_on_help)
{
    XmString text = (XmString)client_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNmessageString, text); arg++;

    Widget shell = findTopLevelShellParent(widget);
    if (shell == 0)
	shell = widget;

    if (help_dialog && (shell != help_shell))
    {
	DestroyWhenIdle(help_dialog);
	help_dialog = 0;
    }

    help_shell  = shell;

    if (help_dialog == 0)
    {
	// Build help_dialog
	XtSetArg(args[arg], XmNdeleteResponse, XmUNMAP); arg++;

	help_dialog = 
	    verify(XmCreateInformationDialog(shell, 
					     XMST("help"), args, arg));
	Delay::register_shell(help_dialog);
	XtAddCallback(help_dialog, XmNhelpCallback,
		      HelpOnHelpCB, 0);
	XtAddCallback(help_dialog, XtNdestroyCallback, 
		      HelpDestroyCB, XtPointer(help_dialog));

	XtUnmanageChild(XmMessageBoxGetChild(help_dialog, 
					     XmDIALOG_CANCEL_BUTTON));
    }
    else
    {
	// Setup text for existing dialog
	XtSetValues(help_dialog, args, arg);
    }

    // If this is a recursive call, disable the help button
    Widget help_button = 
	XmMessageBoxGetChild(help_dialog, XmDIALOG_HELP_BUTTON);
    set_sensitive(help_button, !help_on_help);

    // Popup help_dialog
    manage_and_raise(help_dialog);
}
#else
void StringHelpCB(GUI::Widget *widget, const char *s)
{
    GUI::String text(s);

    MStringHelpCB(widget, text);
}

void MStringHelpCB(GUI::Widget *widget, const GUI::String &text)
{
    _MStringHelpCB(widget, text);
}

static void _MStringHelpCB(GUI::Widget *widget, 
			   const GUI::String &text,
			   bool help_on_help)
{
    GUI::Widget *shell = findTopLevelShellParent(widget);
    if (shell == 0)
	shell = widget;

    if (help_dialog && (shell != help_shell))
    {
	DestroyWhenIdle(help_dialog);
	help_dialog = 0;
    }

    help_shell  = shell;

    if (help_dialog == 0)
    {
	// Build help_dialog
	help_dialog = new GUI::Dialog(*shell, "help");

	Delay::register_shell(help_dialog);

#if defined(NAG_ME)
#warning Register help callback (HelpOnHelpCB)
#endif
	help_dialog->signal_delete_event().connect(sigc::hide(sigc::bind(sigc::ptr_fun(HelpDestroyCB), help_dialog)));

    }
    else
    {
	// Setup text for existing dialog
	GUI::Label *label = new GUI::Label(*help_dialog, GUI::PACK_SHRINK,
					   "help_text", text);
    }

    // If this is a recursive call, disable the help button

    // Popup help_dialog
    manage_and_raise(help_dialog);
}
#endif


#if defined(IF_XM)
static void HelpIndexCB(Widget widget, XtPointer client_data, 
			XtPointer call_data)
{
    Delay delay;

    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    Widget help_man = Widget(client_data);
    int index = cbs->item_position;

    void *userData = 0;
    XtVaGetValues(widget, XmNuserData, &userData, XtPointer(0));
    XmTextPosition *positions = (XmTextPosition *)userData;
    if (positions == 0)
	return;			// Not yet set

    XmTextPosition pos = positions[index - 1];

    XmTextSetInsertionPosition(help_man, pos);
    XmTextSetTopCharacter(help_man, pos);
    XmTextShowPosition(help_man, pos);
}
#else
static void HelpIndexCB(const GUI::String &contents)
{
    std::cerr << "HelpIndexCB called (" << contents.c_str() << ")\n";
}
#endif


#if defined(IF_XM)
// Activate the button given in CLIENT_DATA
static void ActivateCB(Widget, XtPointer client_data, 
		       XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
}
#else
// Activate the button given in CLIENT_DATA
static void ActivateCB(GUI::Button *b)
{
    b->activate();
}
#endif

#if defined(IF_XM)
struct FindInfo {
    Widget key;			// The text field holding the search key 
    Widget text;		// The text to be searched
};
#else
struct FindInfo {
    GUI::Widget *key;		// The text field holding the search key 
    GUI::Widget *text;		// The text to be searched
};
#endif

static bool lock_update_arg = false;

#if defined(IF_XM)
static void FindCB(Widget w, XtPointer client_data, XtPointer call_data,
		   bool forward)
{
    Delay delay;

    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

    FindInfo *fi = (FindInfo *)client_data;

    String key_s = XmTextFieldGetString(fi->key);
    string key(key_s);
    XtFree(key_s);

    static StringArray find_keys;

    int next_occurrence = -1;
    if (!key.empty())
    {
	find_keys += key;
	smart_sort(find_keys);
	uniq(find_keys);
	ComboBoxSetList(fi->key, find_keys);

	String text_s = XmTextGetString(fi->text);
	string text(text_s);
	XtFree(text_s);

	// If no uppercase letter is in KEY, make TEXT lowercase
	if (key == downcase(key))
	    text.downcase();

	XmTextPosition cursor = XmTextGetInsertionPosition(fi->text);

	if (forward)
	{
	    next_occurrence = text.index(key, cursor);
	    if (next_occurrence < 0)
		next_occurrence = text.index(key); // Wrap around
	}
	else
	{
	    next_occurrence = text.index(key, cursor - text.length() - 1);
	    if (next_occurrence < 0)
		next_occurrence = text.index(key, -1); // Wrap around
	}
    }

    if (next_occurrence < 0)
    {
	// FIXME: This should be in some status line.  Unfortunately,
	// we don't have any...
	post_warning(quote(key) + " not found.", "manual_find_error", w);
    }
    else
    {
	// LessTif 0.79 sometimes returns 0 in cbs->event.  Handle this.
	Time tm;
	if (cbs->event != 0)
	    tm = time(cbs->event);
	else
	    tm = XtLastTimestampProcessed(XtDisplay(fi->text));

	lock_update_arg = true;

	TextSetSelection(fi->text,
			 next_occurrence,
			 next_occurrence + key.length(),
			 tm);
	if (forward)
	{
	    XmTextSetInsertionPosition(fi->text, 
				       next_occurrence + key.length());
	}
	else
	{
	    XmTextSetInsertionPosition(fi->text, next_occurrence);
	}

	lock_update_arg = false;
    }
}
#else
static void FindCB(FindInfo *fi, bool forward)
{
    std::cerr << "FindCB\n";
}
#endif


#if defined(IF_XM)
// Find the next occurrence of the string contained in the widget 
// given in CLIENT_DATA
static void FindForwardCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    FindCB(w, client_data, call_data, true);
}

// Find the previous occurrence of the string contained in the widget 
// given in CLIENT_DATA
static void FindBackwardCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    FindCB(w, client_data, call_data, false);
}
#else
// Find the next occurrence of the string contained in the widget 
// given in CLIENT_DATA
static void FindForwardCB(FindInfo *fi)
{
    FindCB(fi, true);
}

// Find the previous occurrence of the string contained in the widget 
// given in CLIENT_DATA
static void FindBackwardCB(FindInfo *fi)
{
    FindCB(fi, false);
}
#endif

#if defined(IF_XM)
// Highlight current section after cursor motion
static void HighlightSectionCB(Widget, XtPointer client_data, 
			       XtPointer call_data)
{
    XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
    Widget list = Widget(client_data);

    XmTextPosition cursor = cbs->newInsert;

    void *userData = 0;
    XtVaGetValues(list, XmNuserData, &userData, XtPointer(0));
    XmTextPosition *positions = (XmTextPosition *)userData;
    if (positions == 0)
	return;			// Not yet set

    int pos = 0;
    while (positions[pos] <= cursor)
	pos++;

    ListSetAndSelectPos(list, pos);
}
#else
// Highlight current section after cursor motion
static void HighlightSectionCB(GUI::ScrolledText *text, GUI::ListView *list)
{
    std::cerr << "HighlightSectionCB.\n";
}
#endif

#if defined(IF_XM)
static void SetSelectionCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    if (lock_update_arg)
	return;

    ArgField *arg_field = (ArgField *)client_data;

    string selection = "";
    String _selection = XmTextGetSelection(w);
    if (_selection != 0)
    {
	selection = _selection;
	XtFree(_selection);
    }
    else
    {
	// No selection - get word at cursor
	XmTextVerifyCallbackStruct *cbs = 
	    (XmTextVerifyCallbackStruct *)call_data;
	XmTextPosition cursor = cbs->newInsert;
	String text = XmTextGetString(w);
	XmTextPosition startpos, endpos;
	startpos = endpos = cursor;

	while (startpos > 0 && isid(text[startpos - 1]))
	    startpos--;
	while (text[endpos] != '\0' && isid(text[endpos]))
	    endpos++;

	if (endpos > startpos)
	    selection = string(text + startpos, endpos - startpos);
    }

    if (!selection.empty())
    {
	selection.downcase();
	while (selection.contains('\n'))
	    selection = selection.after('\n');
	arg_field->set_string(selection);
    }
}
#else
static void SetSelectionCB(GUI::ScrolledText *text, GUI::ComboBoxEntryText *arg)
{
    std::cerr << "SetSelectionCB.\n";
}
#endif

// Return true iff TEXT contains a manual header line at pos
static bool has_header(char *text, unsigned pos)
{
    if (text[pos] != '\0'
	&& !isspace(text[pos])
	&& (pos == 0 || text[pos - 1] == '\n')
	&& text[pos + 1] != '\b')
    {
	// Non-highlighted character in column 1
	return true;
    }

    return false;
}

#if defined(IF_XM)
// Note: assumes `man' or `info' format.
void ManualStringHelpCB(Widget widget, XtPointer client_data, 
			XtPointer)
{
    static MString null(0, true);
    string text(STATIC_CAST(char *,client_data));

    ManualStringHelpCB(widget, null, text);
}
#else
// Note: assumes `man' or `info' format.
void ManualStringHelpCB(GUI::Widget *widget, char *s)
{
    std::cerr << "ManualStringHelpCB: not implemented\n";
}
#endif


#if defined(IF_XM)
// Close action from menu
static void CloseCB(Widget w, XtPointer, XtPointer)
{
    Widget shell = findTopLevelShellParent(w);
    DestroyWhenIdle(shell);
}
#else
// Close action from menu
static void CloseCB(GUI::Widget *w)
{
    GUI::Shell *shell = findTopLevelShellParent(w);
    DestroyWhenIdle(shell);
}
#endif

#if defined(IF_XM)
// Create an empty dialog within a top-level-shell
// FIXME: This should be part of core DDD!
static Widget create_text_dialog(Widget parent, const _XtString name, 
				 Arg *args, int arg, Widget& bar)
{
    Widget shell = verify(XtCreateWidget(name, topLevelShellWidgetClass,
					 parent, args, arg));

    arg = 0;
    Widget w = XmCreateMainWindow(shell, XMST(name), args, arg);
    XtManageChild(w);

    MMDesc file_menu[] = 
    {
	GENTRYL("close", N_("close"), MMPush,
		BIND(CloseCB, shell),
		sigc::ptr_fun(CloseCB),
		0, 0),
	GENTRYL("exit", N_("exit"), MMPush,
		BIND(DDDExitCB, EXIT_SUCCESS),
		sigc::bind(sigc::ptr_fun(DDDExitCB), EXIT_SUCCESS),
		0, 0),
	MMEnd
    };

    MMDesc menubar[] = 
    {
	GENTRYL("file", N_("file"), MMMenu,
		MMNoCB, MDUMMY, file_menu, 0),
	GENTRYL("edit", N_("edit"), MMMenu,
		MMNoCB, MDUMMY, simple_edit_menu, 0),
	GENTRYL("help", N_("help"), MMMenu | MMHelp,
		MMNoCB, MDUMMY, simple_help_menu, 0),
	MMEnd
    };

    bar = MMcreateMenuBar(w, "menubar", menubar);

    // Don't add a callback to `Edit' menu
    menubar[1].items = 0;

    MMaddCallbacks(menubar);
    MMaddHelpCallback(menubar, ImmediateHelpCB);

    menubar[1].items = simple_edit_menu;

    Delay::register_shell(shell);
    InstallButtonTips(shell);

    return w;
}
#else
// Create an empty dialog within a top-level-shell
// FIXME: This should be part of core DDD!
static GUI::Dialog *create_text_dialog(GUI::Widget *parent, const char *name, 
				       GUI::MenuBar *&bar)
{
    GUI::Dialog *shell = new GUI::Dialog(*parent, name);

    MMDesc file_menu[] = 
    {
	GENTRYL("close", N_("close"), MMPush,
		BIND(CloseCB, shell),
		sigc::ptr_fun(CloseCB),
		0, 0),
	GENTRYL("exit", N_("exit"), MMPush,
		BIND(DDDExitCB, EXIT_SUCCESS),
		sigc::bind(sigc::ptr_fun(DDDExitCB), EXIT_SUCCESS),
		0, 0),
	MMEnd
    };

    MMDesc menubar[] = 
    {
	GENTRYL("file", N_("file"), MMMenu,
		MMNoCB, MDUMMY, file_menu, 0),
	GENTRYL("edit", N_("edit"), MMMenu,
		MMNoCB, MDUMMY, simple_edit_menu, 0),
	GENTRYL("help", N_("help"), MMMenu | MMHelp,
		MMNoCB, MDUMMY, simple_help_menu, 0),
	MMEnd
    };

    bar = MMcreateMenuBar(*shell, "menubar", menubar);

    // Don't add a callback to `Edit' menu
    menubar[1].items = 0;

    MMaddCallbacks(menubar);
    MMaddHelpCallback(menubar, sigc::ptr_fun(ImmediateHelpCB));

    menubar[1].items = simple_edit_menu;

    Delay::register_shell(shell);
    InstallButtonTips(shell);

    return shell;
}
#endif

#if defined(IF_XM)
static void DeleteFindInfoCB(Widget, XtPointer client_data, XtPointer)
{
    FindInfo *fi = (FindInfo *)client_data;
    delete fi;
}
#else
static void DeleteFindInfoCB(GUI::Widget *)
{
    std::cerr << "DeleteFindInfoCB not implemented yet.\n";
}
#endif

static int max_width(const char *text)
{
    int max_width = 0;
    int width     = 0;

    while (*text != '\0')
    {
	switch (*text++)
	{
	case '\b':
	    if (width > 0)
		width--;
	    break;

	case '\n':
	    max_width = max(max_width, width);
	    width = 0;
	    break;

	default:
	    width++;
	}
    }

    return max_width;
}

#if defined(IF_XM)
static void ToggleIndexCB(Widget w, XtPointer client_data, XtPointer)
{
    Widget child = Widget(client_data);

    if (XmToggleButtonGetState(w))
	manage_paned_child(child);
    else
	XtUnmanageChild(child);
}
#else
static void ToggleIndexCB(GUI::Bipolar *w, GUI::Widget *child)
{
    if (w->get_active())
	manage_paned_child(child);
    else
	unmanage_paned_child(child);
}
#endif

// Return manual
#if defined(IF_XM)
void ManualStringHelpCB(Widget widget, const MString& title,
			const string& unformatted_text)
#else
void ManualStringHelpCB(GUI::Widget *widget, const MString& title,
			const string& unformatted_text)
#endif
{
#if defined(IF_XM)
    // Delay delay;

    // Build manual dialog
    Widget toplevel = findTheTopLevelShell(widget);
    if (toplevel == 0)
	return;

    // Format text
    string the_text(unformatted_text);

    // For efficiency reasons, we access all data in-place via the TEXT ptr.
    char *text = CONST_CAST(char *,the_text.chars());

    // Set i > 0 if TEXT contains more than one newline
    int i = the_text.index('\n');
    if (i >= 0)
	i = the_text.index('\n', i + 1);

    bool manual = !the_text.contains("File:", 0) && i > 0;
    bool info   =  the_text.contains("File:", 0) && i > 0;

    int len = the_text.length();

    if (manual)
    {
	// Manual page: strip manual headers and footers
	int source = 0;
	int target = 0;

	for (;;)
	{
	    if (target % 100 == 0)
		process_pending_events();

	    while (has_header(text, source))
	    {
		// Header line: strip line and surrounding blanks.
		// At least using GNU nroff, each header line
		// has three blank lines before and two afterwards.
		if (target > 0)
		    target--;
		if (target >= 0 && text[target] == '\n')
		    target--;
		if (target >= 0 && text[target] == '\n')
		    target--;
		if (target >= 0 && text[target] == '\n')
		    target--;
		if (target >= 0 && text[target] == '\n')
		    target--;
		target++;
		if (target > 0)
		    text[target++] = '\n';

		while (text[source] != '\n')
		    source++;
		if (text[source] == '\n')
		    source++;
		if (text[source] == '\n')
		    source++;
		if (text[source] == '\n')
		    source++;
	    }
	    if (text[source] == '\0')
		break;

	    text[target++] = text[source++];
	}

	text[target] = '\0';
	len = target;
	while (target < int(the_text.length()))
	    text[target++] = '\0';
    }
    else if (info)
    {
	// Info file: strip menus
	int source = 0;
	int target = 0;

	while (text[source] != '\0')
	{
	    if (target % 100 == 0)
		process_pending_events();

	    while (text[source] == '*' && 
		   text[source + 1] == ' ' && 
		   (source == 0 || text[source - 1] == '\n'))
	    {
		// Skip menu item
		while (text[source++] != '\n')
		    ;
	    }

	    text[target++] = text[source++];
	}

	text[target] = '\0';
	len = target;
	while (target < int(the_text.length()))
	    text[target++] = '\0';
    }

    if (info || manual)
    {
	// Info and manual: kill multiple empty lines
	int source = 0;
	int target = 0;

	while (text[source] != '\0')
	{
	    if (target % 100 == 0)
		process_pending_events();

	    if (text[source] == '\n')
	    {
		while (text[source] != '\0'
		       && text[source + 1] == '\n'
		       && text[source + 2] == '\n')
		    source++;
	    }
	    text[target++] = text[source++];
	}
	text[target] = '\0';
	len = target;
	while (target < int(the_text.length()))
	    text[target++] = '\0';
    }

    // Find titles
    StringArray titles;
    IntArray positions;

    if (info)
    {
	// Info file
	int source = 0;
	while (source >= 0)
	{
	    process_pending_events();

	    assert(the_text.contains("File: ", source));

	    // Fetch title below `File: ' line
	    int start_of_title = the_text.index("\n\n", source) + 2;
	    int end_of_title   = the_text.index("\n", start_of_title);
	    string title = 
		the_text.at(start_of_title, end_of_title - start_of_title);

	    // Fetch indent level, using the underline characters
	    int start_of_underline = the_text.index('\n', start_of_title) + 1;
	    static string underlines = "*=-.";
	    int indent = underlines.index(the_text[start_of_underline]);
	    if (indent < 0)
		indent = underlines.length();
	    if (indent == 0)
		title.upcase();

	    // Add title and position
	    titles += replicate(' ', indent * 2) + title;
	    positions += source;

	    // Strip `File: ' line
	    the_text.del(source, start_of_title - source);
	    
	    // Find next `File: ' line
	    source = the_text.index("File: ", source);
	}
	text = CONST_CAST(char *,the_text.chars());
	len = the_text.length();
    }



    Arg args[15];
    Cardinal arg = 0;
    Widget menubar;
    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    Widget text_dialog = create_text_dialog(toplevel, "manual_help", 
					    args, arg, menubar);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget form = verify(XmCreateForm(text_dialog, XMST("form"), args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNmarginHeight,     0);                 arg++;
    XtSetArg(args[arg], XmNborderWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNallowResize,      True);              arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    Widget area = verify(XmCreatePanedWindow(form, 
					     XMST("help_area"), args, arg));
    XtManageChild(area);

    FindInfo *fi = new FindInfo;
    XtAddCallback(text_dialog, XmNdestroyCallback,
		  DeleteFindInfoCB, XtPointer(fi));

    MMDesc items [] = 
    {
	GENTRYL("findBackward", N_("findBackward"), MMPush, 
		BIND(FindBackwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindBackwardCB), fi)),
		0, 0),
	GENTRYL("findForward", N_("findForward"),   MMPush, 
		BIND(FindForwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindForwardCB), fi)),
		0, 0),
	MMEnd
    };

    Widget arg_label;
    ArgField *arg_field;
    Widget toolbar = create_toolbar(area, "toolbar", items, 0, arg_label,
				    arg_field, XmPIXMAP);
    fi->key = arg_field->text();
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, fi->key);
    MMaddCallbacks(simple_edit_menu, XtPointer(fi->key));

    arg = 0;
    Widget help_index = 
	verify(XmCreateScrolledList(area, XMST("index"), args, arg));
    XtManageChild(help_index);
    if (lesstif_version < 90)
	set_scrolled_window_size(help_index);

    Widget view_index;
    MMDesc manual_menu[] = 
    {
	GENTRYL("findForward", N_("findForward"),   MMPush, 
		BIND(FindForwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindForwardCB), fi)),
		0, 0),
	GENTRYL("findBackward", N_("findBackward"), MMPush, 
		BIND(FindBackwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindBackwardCB), fi)),
		0, 0),
	MMSep,
	GENTRYL("viewIndex", N_("viewIndex"),       MMToggle,
		BIND(ToggleIndexCB, XtParent(help_index)), 
		sigc::bind(sigc::ptr_fun(ToggleIndexCB), help_index->get_parent()), 
		0, &view_index),
	MMEnd
    };

    MMDesc more_menubar[] = 
    {
	// This is called `source' such that we can re-use the Find
	// specs from the DDD `Source' meny
	GENTRYL("source", N_("source"), MMMenu, MMNoCB, MDUMMY, manual_menu, 0),
	MMEnd
    };

    MMaddItems(menubar, more_menubar);
    MMaddCallbacks(more_menubar);
    XtVaSetValues(view_index, XmNset, True, XtPointer(0));

    int columns = max_width(text);
    columns = min(max(columns, 40), 80) + 1;

    arg = 0;
    XtSetArg(args[arg], XmNcolumns,  columns);           arg++;
    XtSetArg(args[arg], XmNeditable, False);             arg++;
    XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
    XtSetArg(args[arg], XmNvalue,    "");                arg++;
    Widget help_man = 
	verify(XmCreateScrolledText(area, XMST("text"), args, arg));
    XtManageChild(help_man);
    if (lesstif_version < 90)
	set_scrolled_window_size(help_man);
    fi->text = help_man;

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(toolbar, (XtWidgetGeometry *)0, &size);
    XtVaSetValues(toolbar,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  XtPointer(0));

    XtAddCallback(help_index, XmNsingleSelectionCallback,
		  HelpIndexCB, XtPointer(help_man));
    XtAddCallback(help_index, XmNmultipleSelectionCallback,
		  HelpIndexCB, XtPointer(help_man));
    XtAddCallback(help_index, XmNbrowseSelectionCallback,
		  HelpIndexCB, XtPointer(help_man));
    XtAddCallback(help_index, XmNdefaultActionCallback,
		  HelpIndexCB, XtPointer(help_man));
    XtAddCallback(text_dialog, XmNhelpCallback,
		  ImmediateHelpCB, XtPointer(help_man));

    XtAddCallback(help_man, XmNmotionVerifyCallback,
		  HighlightSectionCB, XtPointer(help_index));
    XtAddCallback(help_man, XmNmotionVerifyCallback,
		  SetSelectionCB, XtPointer(arg_field));

    XtAddCallback(fi->key, XmNactivateCallback, ActivateCB,
		  XtPointer(items[1].widget));

    XtVaSetValues(text_dialog, XmNdefaultButton, Widget(0), XtPointer(0));
    
    XtManageChild(form);
    InstallButtonTips(text_dialog);

    // Set title
    if (!title.isNull())
	wm_set_name(XtParent(text_dialog), title.str(), title.str());

    if (manual)
    {
	// Manual page: handle underlines

	bool *underlined    = new bool[len];
	bool *doublestriked = new bool[len];
	for (i = 0; i < len; i++)
	    underlined[i] = doublestriked[i] = false;

	int source = 0;
	int target = 0;

	while (text[source] != '\0')
	{
	    if (target % 100 == 0)
		process_pending_events();

	    char c = text[target++] = text[source++];

	    if (c == '\b' && target >= 2)
	    {
		target -= 2;
		if (text[target] == '_')
		    underlined[target] = true;
		if (text[target] == text[source])
		    doublestriked[target] = true;
	    }
	}
	text[target] = '\0';
	len = target;
	while (target < int(the_text.length()))
	    text[target++] = '\0';

	// Set text
	XtVaSetValues(help_man, XmNvalue, text, XtPointer(0));

	// Set highlighting
	XmTextSetHighlight(help_man, 0, XmTextGetLastPosition(help_man), 
			   XmHIGHLIGHT_NORMAL);

	XmTextPosition underlining    = 0;
	XmTextPosition doublestriking = 0;
	for (i = 0; i < len; i++)
	{
	    if (i % 100 == 0)
		process_pending_events();

	    if (doublestriked[i] && !doublestriking)
	    {
		doublestriking = i;
	    }
	    else if (!doublestriked[i] && doublestriking)
	    {
		// There is no bold face in XmText, normal selection
		// clutters the screen and secondary selection is already used.
		// So what shall we do here?  Use the Motif 2.0 CSText?
#if 0
		XmTextSetHighlight(help_man, doublestriking, i, 
				   XmHIGHLIGHT_SELECTED);
		process_pending_events();
#endif
		doublestriking = 0;
	    }

	    if (underlined[i] && !underlining)
	    {
		underlining = i;
	    }
	    else if (!underlined[i] && underlining)
	    {
		XmTextSetHighlight(help_man, underlining, i, 
				   XmHIGHLIGHT_SECONDARY_SELECTED);
		underlining = 0;
	    }
	}

	delete[] underlined;
	delete[] doublestriked;
    }

    if (manual)
    {
	// Set titles for manual page
	int start_of_line = 0;

	for (int source = 0; source < len; source++)
	{
	    if (source % 100 == 0)
		process_pending_events();

	    if (text[source] == '\n')
	    {
		if (source - start_of_line > 3)
		{
		    // Check manual title
		    bool is_title = false;
		
		    if (text[start_of_line] == ' ' &&
			text[start_of_line + 1] == ' ' &&
			text[start_of_line + 2] != '\0' &&
			text[start_of_line + 3] != ' ')
			is_title = true; // .SS title
		    else if (text[start_of_line] != ' '
			     && source - start_of_line < 60)
			is_title = true; // .SH title

		    if (is_title)
		    {
			titles += 
			    the_text.at(start_of_line, source - start_of_line);
			positions += start_of_line;
		    }
		}

		start_of_line = source + 1;
	    }
	}
    }
    else
    {
	// Info file, or something else: titles are already set

	// Set text
	XtVaSetValues(help_man, XmNvalue, text, XtPointer(0));

	// No highlighting
	XmTextSetHighlight(help_man, 0, XmTextGetLastPosition(help_man), 
			   XmHIGHLIGHT_NORMAL);
    }

    process_pending_events();

    // Set titles in selection list
    XmTextPosition *xmpositions = new XmTextPosition[titles.size() + 1];
    for (i = 0; i < titles.size(); i++)
	xmpositions[i] = positions[i];
    xmpositions[i] = INT_MAX;

    XmStringTable xmtitles = new XmString[titles.size()];

    for (i = 0; i < titles.size(); i++)
    {
	xmtitles[i] = 
	    XmStringCreateLtoR(XMST(titles[i].chars()),
			       titles[i].contains(' ', 0) ?
			       XMST(CHARSET_RM) :
			       XMST(CHARSET_BF));
    }

    XtVaSetValues(help_index,
		  XmNtopItemPosition,   1,
		  XmNselectedItemCount, 0,
		  XmNitems,             xmtitles,
		  XmNuserData,          xmpositions,
		  XmNitemCount,         titles.size(),
		  XtPointer(0));

    for (i = 0; i < titles.size(); i++)
	XmStringFree(xmtitles[i]);
    delete[] xmtitles;

    process_pending_events();

    // Enable Text Window
    Delay::register_shell(XtParent(text_dialog));
    InstallButtonTips(XtParent(text_dialog));
    manage_and_raise(text_dialog);
#else
#ifdef NAG_ME
#warning ManualStringCB not implemented
#endif
    std::cerr << "ManualStringHelpCB: not implemented\n";
#endif
}


#if defined(IF_XM)
void TextHelpCB(Widget widget, XtPointer client_data, XtPointer)
{
    // Delay delay;

    string name = "text_help";

    // If CLIENT_DATA starts with @FOO@, use FOO as dialog name
    String text = (String)client_data;
    if (text[0] == '@')
    {
	name = text + 1;
	name = name.before('@');
	text += name.length() + 2;
    }

    // Build help_text
    Widget toplevel = findTheTopLevelShell(widget);
    if (toplevel == 0)
	return;

    Arg args[15];
    Cardinal arg = 0;
    Widget menubar;
    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    Widget text_dialog = create_text_dialog(toplevel, name.chars(), args, arg, 
					    menubar);

    arg = 0;
    Widget form = verify(XmCreateForm(text_dialog, XMST("form"), args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNmarginHeight,     0);                 arg++;
    XtSetArg(args[arg], XmNborderWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNallowResize,      True);              arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    Widget area = verify(
	XmCreatePanedWindow(form, XMST("help_area"), args, arg));
    XtManageChild(area);

    FindInfo *fi = new FindInfo;
    XtAddCallback(text_dialog, XmNdestroyCallback,
		  DeleteFindInfoCB, XtPointer(fi));

    MMDesc items [] = 
    {
	GENTRYL("findBackward", N_("findBackward"), MMPush, 
		BIND(FindBackwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindBackwardCB), fi)),
		0, 0),
	GENTRYL("findForward", N_("findForward"),   MMPush, 
		BIND(FindForwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindForwardCB), fi)),
		0, 0),
	MMEnd
    };

    Widget arg_label;
    ArgField *arg_field;
    Widget toolbar = create_toolbar(area, "toolbar", items, 0, arg_label,
				    arg_field, XmPIXMAP);
    fi->key = arg_field->text();
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, fi->key);
    MMaddCallbacks(simple_edit_menu, XtPointer(fi->key));

    MMDesc manual_menu[] = 
    {
	GENTRYL("findForward", N_("findForward"),   MMPush, 
		BIND(FindForwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindForwardCB), fi)),
		0, 0),
	GENTRYL("findBackward", N_("findBackward"), MMPush,
		BIND(FindBackwardCB, fi),
		sigc::hide(sigc::bind(sigc::ptr_fun(FindBackwardCB), fi)),
		0, 0),
	MMEnd
    };

    MMDesc more_menubar[] = 
    {
	// This is called `source' such that we can re-use the Find
	// specs from the DDD `Source' meny
	GENTRYL("source", N_("source"), MMMenu, MMNoCB, MDUMMY, manual_menu, 0),
	MMEnd
    };

    MMaddItems(menubar, more_menubar);
    MMaddCallbacks(more_menubar);

    int columns = max_width(text);
    columns = min(max(columns, 40), 80) + 1;

    arg = 0;
    XtSetArg(args[arg], XmNcolumns,          columns);           arg++;
    XtSetArg(args[arg], XmNeditable,         False);             arg++;
    XtSetArg(args[arg], XmNeditMode,         XmMULTI_LINE_EDIT); arg++;
    XtSetArg(args[arg], XmNvalue,            text); arg++;
    Widget help_text = verify(
	XmCreateScrolledText(area, XMST("text"), args, arg));
    XtManageChild(help_text);
    if (lesstif_version < 90)
	set_scrolled_window_size(help_text);
    fi->text = help_text;

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(toolbar, (XtWidgetGeometry *)0, &size);
    XtVaSetValues(toolbar,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  XtPointer(0));

    XtAddCallback(text_dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));

    XtAddCallback(help_text, XmNmotionVerifyCallback,
		  SetSelectionCB, XtPointer(arg_field));

    XtAddCallback(fi->key, XmNactivateCallback, ActivateCB,
		  XtPointer(items[1].widget));

    XtManageChild(form);
    InstallButtonTips(text_dialog);

    // Enable Text Window
    XtRealizeWidget(XtParent(text_dialog));
    Delay::register_shell(XtParent(text_dialog));
    InstallButtonTips(XtParent(text_dialog));
    XtPopup(XtParent(text_dialog), XtGrabNone);
    manage_and_raise(text_dialog);
}
#else
void TextHelpCB(GUI::Widget *widget, const char *s)
{
    std::cerr << "TextHelpCB: Not implemented yet.\n";
}
#endif


//-----------------------------------------------------------------------------
// Context-sensitive help
//-----------------------------------------------------------------------------

#if defined(IF_XM)
// Return the widget related to the mouse event EV
static Widget EventToWidget(Widget widget, XEvent *ev)
{
    // If the button was clicked outside of this programs windows, the
    // widget that grabbed the pointer will get the event.  So, we
    // check the bounds of the widget against the coordinates of the
    // event.  If they're outside, we return 0.  Otherwise we
    // return the widget in which the event occured.

    switch (ev->type)
    {
    case KeyPress:
    case KeyRelease:
    case ButtonPress:
    case ButtonRelease:
	break;

    default:
	return 0;		// No window
    }

    Position x, y;
    Dimension width, height;
    XtVaGetValues(widget, XmNx, &x, XmNy, &y,
		  XmNwidth, &width, XmNheight, &height,
		  XtPointer(0));

    if (ev->xbutton.window == XtWindow(widget) &&
	(ev->xbutton.x < x ||
	 ev->xbutton.y < y ||
	 ev->xbutton.x > x + width ||
	 ev->xbutton.y > y + height))
    {
	return 0;
    }
    else
    {
	return XtWindowToWidget(XtDisplay(widget),
				ev->xbutton.window);
    }
}


// In LessTif, XmTrackingEvent() is somewhat broken - it returns on
// KeyRelease events, and it does not return the event.  Here's an
// improved implementation.
static Widget
TrackingEvent(Widget widget, Cursor cursor,
	      Boolean confine_to, XEvent *event_return)
{
    Window confine_to_this;
    XEvent ev;
    Boolean key_pressed = False;
    Time time;

    if (confine_to)
    {
	confine_to_this = XtWindow(widget);
    }
    else
    {
	confine_to_this = None;
    }

    time = XtLastTimestampProcessed(XtDisplay(widget));
    if (XtGrabPointer(widget, True,
		      ButtonReleaseMask | ButtonPressMask,
		      GrabModeAsync, GrabModeAsync,
		      confine_to_this, cursor, time) != GrabSuccess)
    {
	std::cerr << "TrackingEvent: Could not grab pointer\n";
	return 0;
    }

    while (True)
    {
	XtAppNextEvent(XtWidgetToApplicationContext(widget), &ev);
	time = XtLastTimestampProcessed(XtDisplay(widget));

	if (ev.type == KeyPress)
	{
	    // Avoid exiting upon releasing the key that caused
	    // XmTrackingEvent() to be invoked
	    key_pressed = True;
	}
	else if ((ev.type == KeyRelease && key_pressed) ||
		 (ev.type == ButtonRelease && ev.xbutton.button == 1))
	{
	    if (event_return != 0)
		*event_return = ev;

	    XtUngrabPointer(widget, time);

	    return EventToWidget(widget, &ev);
	}
    }
}
#endif

#if defined(IF_XM)
// Hook before help on context
static void nop2(Widget, XtPointer, XtPointer) {}
void (*PreHelpOnContextHook)(Widget w, XtPointer client_data, 
			     XtPointer call_data) = nop2;
#else
// Hook before help on context
static void nop2(GUI::Widget *, GUI::Event *) {}
void (*PreHelpOnContextHook)(GUI::Widget *, GUI::Event *) = nop2;
#endif

#if defined(IF_XM)
void HelpOnContextCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget item = 0;
    Widget toplevel = findTopLevelShellParent(widget);

    if (toplevel == 0)
    {
	HelpOnWindowCB(widget, client_data, call_data);
	return;
    }
	   
    PreHelpOnContextHook(widget, client_data, call_data);

    static Cursor cursor = 
	XCreateFontCursor(XtDisplay(toplevel), XC_question_arrow);

    XEvent ev;
#if XmVersion < 1002
    // No XmTrackingEvent() in Motif 1.1
    item = TrackingEvent(toplevel, cursor, False, &ev);
#else
    if (lesstif_version <= 84)
	item = TrackingEvent(toplevel, cursor, False, &ev);
    else
	item = XmTrackingEvent(toplevel, cursor, False, &ev);
#endif

    if (item != 0)
	ImmediateHelpCB(item, client_data, 0);
    else
	ImmediateHelpCB(toplevel, client_data, 0);

    // Some Motif versions get confused if this function is invoked
    // via a menu accelerator; the keyboard remains grabbed. Hence, we
    // ungrab it explicitly.
    XtUngrabKeyboard(toplevel,
		     XtLastTimestampProcessed(XtDisplay(widget)));
}
#else
void HelpOnContextCB(GUI::Widget *widget, GUI::Event *event)
{
    GUI::Widget *item = 0;
    GUI::Shell *toplevel = findTopLevelShellParent(widget);

    if (toplevel == 0)
    {
	HelpOnWindowCB(widget, event);
	return;
    }
	   
    PreHelpOnContextHook(widget, event);

    std::cerr << "Should set \"?\" cursor here\n";
    std::cerr << "XmTrackingEvent?\n";

    if (item != 0)
	ImmediateHelpCB(item);
    else
	ImmediateHelpCB(toplevel);

}
#endif


#if defined(IF_XM)
// Return the child widget (or gadget) EX/EY is in, starting with WIDGET.
static Widget GetWidgetAt(Widget w, int ex, int ey)
{
    if (w == 0)
	return 0;

    bool once_again = true;
    while (once_again)
    {
	WidgetList children  = 0;
	Cardinal numChildren = 0;

	if (XtIsComposite(w))
	{
	    XtVaGetValues(w, XmNchildren, &children,
			  XmNnumChildren, &numChildren, XtPointer(0));
	}

	once_again = false;

	// Look for position within children.
	// In a form, we may have multiple children overlapping each
	// other.  Hence, search later children first, as they will be
	// on top of earlier ones.
	for (int m = int(numChildren) - 1; m >= 0; m--)
	{
	    Widget child = children[m];
	    if (XtIsRectObj(child) && XtIsManaged(child))
	    {
		Dimension cx, cy, cw, ch;
		XtVaGetValues(child,
			      XmNx, &cx, XmNy, &cy,
			      XmNwidth, &cw, XmNheight, &ch,
			      XtPointer(0));

		if (cx <= ex && ex <= (cx + cw) &&
		    cy <= ey && ey <= (cy + ch))
		{
		    // Position is within CHILD - restart search
		    once_again = true;
		    w = child;
		    ex -= cx;
		    ey -= cy;
		    break;
		}
	    }
	}
    }

    return w;
}

// Returns the widget the pointer is over, or 0 if it cannot be determined.
static Widget GetWidgetAt(XKeyEvent *e)
{
    return GetWidgetAt(XtWindowToWidget(e->display, e->window), e->x, e->y);
}
#endif

#if defined(IF_XM)
void HelpOnItemCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget toplevel = findTheTopLevelShell(widget);

    if (call_tracking_help(call_data, true) || toplevel == 0)
    {
	HelpOnContextCB(widget, client_data, call_data);
	return;
    }

    Delay delay;		// Finding the widget may take time

    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

    Widget item = GetWidgetAt(&cbs->event->xkey);
    if (item == 0)
	item = toplevel;

    ImmediateHelpCB(item, client_data, 0);
}
#else
void HelpOnItemCB(GUI::Widget *widget, GUI::Event *event)
{
    GUI::Widget *toplevel = findTheTopLevelShell(widget);

    if (call_tracking_help(event, true) || toplevel == 0)
    {
	HelpOnContextCB(widget, event);
	return;
    }

    Delay delay;		// Finding the widget may take time

    std::cerr << "HelpOnItemCB: Not complete.\n";
#if 0
    Widget item = GetWidgetAt(&event->key);
    if (item == 0)
	item = toplevel;

    ImmediateHelpCB(item, client_data, 0);
#endif
}
#endif


//-----------------------------------------------------------------------------
// Button tips
//-----------------------------------------------------------------------------

// True iff button tips are enabled.
static bool button_tips_enabled       = true;

// True iff text tips are enabled.
static bool text_tips_enabled         = true;

// True iff button docs are enabled.
static bool button_docs_enabled       = true;

// True iff text docs are enabled.
static bool text_docs_enabled         = true;

#if defined(IF_XM)
// The shell containing the tip label.
static Widget tip_shell               = 0;

// The tip label.
static Widget tip_label               = 0;

// The tip row; a RowColumn widget surrounding the label.
static Widget tip_row                 = 0;

// True if the button tip shell is raised.
static bool tip_popped_up             = false;

// The timer used for the delay until the tip is raised.
static XtIntervalId raise_tip_timer   = 0;
#else
// The shell containing the tip label.
static GUI::Window *tip_shell = NULL;

// The tip label.
static GUI::Label *tip_label = NULL;

// The tip row; a RowColumn widget surrounding the label.
static GUI::Widget *tip_row = NULL;

// True if the button tip shell is raised.
static bool tip_popped_up = false;

// The timer used for the delay until the tip is raised.
static GUI::connection raise_tip_timer;
#endif

#if defined(IF_XM)
// The timer used for the delay until the tip is cleared.
static XtIntervalId clear_tip_timer   = 0;
#else
// The timer used for the delay until the tip is cleared.
static GUI::connection clear_tip_timer;
#endif

#if defined(IF_XM)
// The timer used for the delay until the documentation is shown.
static XtIntervalId raise_doc_timer   = 0;

// The timer used for the delay until the documentation is cleared.
static XtIntervalId clear_doc_timer   = 0;
#else
// The timer used for the delay until the documentation is shown.
static GUI::connection raise_doc_timer;

// The timer used for the delay until the documentation is cleared.
static GUI::connection clear_doc_timer;
#endif

// Delay times (in ms)
int help_button_tip_delay = 750;  // Delay before raising button tip
int help_value_tip_delay  = 750;  // Delay before raising value tip
int help_button_doc_delay = 0;    // Delay before showing button doc
int help_value_doc_delay  = 0;    // Delay before showing value doc
int help_clear_doc_delay  = 1000; // Delay before clearing doc
int help_clear_tip_delay  = 50;   // Delay before clearing tip


#if defined(IF_XM)
// Helper: cancel the timer given in CLIENT_DATA
static void CancelTimer(Widget, XtPointer client_data, XtPointer)
{
    XtIntervalId timer = XtIntervalId(client_data);
    XtRemoveTimeOut(timer);
}

// Helper: cancel RAISE_DOC_TIMER
static void CancelRaiseDoc(Widget = 0, XtPointer = 0, XtPointer = 0)
{
    if (raise_doc_timer)
    {
	XtRemoveTimeOut(raise_doc_timer);
	raise_doc_timer = 0;
    }
}
#else
// Helper: cancel the timer given in CLIENT_DATA
static void *CancelTimer(void *client_data)
{
    GUI::connection *timer = static_cast<GUI::connection *>(client_data);
    timer->disconnect();
}

// Helper: cancel RAISE_DOC_TIMER
static void *CancelRaiseDoc(void *)
{
    if (raise_doc_timer)
    {
	raise_doc_timer.disconnect();
    }
    return NULL;
}
#endif

#if defined(IF_XM)
// Helper: cancel RAISE_TIP_TIMER
static void CancelRaiseTip(Widget = 0, XtPointer = 0, XtPointer = 0)
{
    if (raise_tip_timer)
    {
	XtRemoveTimeOut(raise_tip_timer);
	raise_tip_timer = 0;
    }
}
#else
// Helper: cancel RAISE_TIP_TIMER
static void *CancelRaiseTip(void *)
{
    if (raise_tip_timer)
    {
	raise_tip_timer.disconnect();
    }
}
#endif

#if defined(IF_XM)
// Event information passed through timeouts, etc.
struct TipInfo {
    XEvent event;		// The event structure
    Widget widget;		// The widget the event occurred in
};

// Raise button tip near the widget given in CLIENT_DATA
static void PopupTip(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == raise_tip_timer);
    raise_tip_timer = 0;

    TipInfo *ti = (TipInfo *)client_data;
    Widget& w = ti->widget;

    if (w == 0)
	return;

    XtRemoveCallback(w, XmNdestroyCallback, CancelRaiseTip, 0);

    MString tip = get_tip_string(w, &ti->event);
    if (tip.isNull() || isNone(tip) || tip.isEmpty())
	return;

    if (!XtIsRealized(w))
	return;

    if (tip_shell == 0)
    {
	Arg args[10];
	int arg;

	arg = 0;
	XtSetArg(args[arg], XmNallowShellResize, true);             arg++;
	XtSetArg(args[arg], XmNx, WidthOfScreen(XtScreen(w)) + 1);  arg++;
	XtSetArg(args[arg], XmNy, HeightOfScreen(XtScreen(w)) + 1); arg++;
	XtSetArg(args[arg], XmNwidth,            10);               arg++;
	XtSetArg(args[arg], XmNheight,           10);               arg++;
	tip_shell = verify(XmCreateMenuShell(findTheTopLevelShell(w),
					     XMST("tipShell"), args, arg));

	arg = 0;
	XtSetArg(args[arg], XmNmarginWidth, 0);                     arg++;
	XtSetArg(args[arg], XmNmarginHeight, 0);                    arg++;
	XtSetArg(args[arg], XmNresizeWidth, True);                  arg++;
	XtSetArg(args[arg], XmNresizeHeight, True);                 arg++;
	XtSetArg(args[arg], XmNborderWidth, 0);                     arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0);                 arg++;
	tip_row = verify(
	    XmCreateRowColumn(tip_shell, XMST("tipRow"), args, arg));
	XtManageChild(tip_row);

	arg = 0;
	XtSetArg(args[arg], XmNlabelString, tip.xmstring());        arg++;
	XtSetArg(args[arg], XmNrecomputeSize, true);                arg++;
	XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING);   arg++;
	tip_label = XmCreateLabel(tip_row, XMST("tipLabel"), args, arg);
	XtManageChild(tip_label);

	// Simple hack to ensure shell is realized
	XtPopup(tip_shell, XtGrabNone);
	XtPopdown(tip_shell);
    }

    if (lesstif_version <= 87)
    {
	// LessTif 0.87 and earlier fails to resize the shell properly
	// - the border width is zero.  Use this hack instead.
	XmFontList font_list;
	XtVaGetValues(tip_label, XmNfontList, &font_list, XtPointer(0));
    
	Dimension tip_width  = tip.width(font_list)  + 6;
	Dimension tip_height = tip.height(font_list) + 6;

	XtResizeWidget(tip_label, tip_width, tip_height, 0);
	XtResizeWidget(tip_row,   tip_width, tip_height, 0);
	XtResizeWidget(tip_shell, tip_width, tip_height, 1);
    }

    XtVaSetValues(tip_label, XmNlabelString, tip.xmstring(), XtPointer(0));

    // Find a possible place for the tip.  Consider the alignment of
    // the parent composite as well as the distance to the screen edge.

    //            TopLeft TopRight
    // LeftTop    XXXXXXXXXXXXXXXX RightTop
    //            XXXXXXXXXXXXXXXX
    // LeftBottom XXXXXXXXXXXXXXXX RightBottom
    //         BottomLeft BottomRight

    enum Placement { LeftTop, RightTop,
		     LeftBottom, RightBottom,
		     BottomLeft, BottomRight,
		     TopLeft, TopRight };

    static Placement last_placement = BottomRight;
    static Widget last_parent = 0;

    Widget parent = XtParent(w);
    if (parent != last_parent || XmIsText(w))
	last_placement = BottomRight;

    // Use 18 runs to find out the best position:
    // Run 0: try last placement in same alignment
    // Run 1-8: try at various sides of W.  Don't hide other widgets
    //          and don't move tip off the screen.
    // Run 9-17: same as 0-8, but don't care for hiding other widgets.
    for (int run = 0; run < 18; run++)
    {
	Placement placement = last_placement;
	switch (run % 9)
	{
	case 0: placement = last_placement; break;
	case 1: placement = BottomRight; break;
	case 2: placement = RightBottom; break;
	case 3: placement = RightTop;    break;
	case 4: placement = TopRight;    break;
	case 5: placement = BottomLeft;  break;
	case 6: placement = LeftBottom;  break;
	case 7: placement = LeftTop;     break;
	case 8: placement = TopLeft;     break;
	}

	bool ok = false;

	if (XmIsRowColumn(parent))
	{
	    // We're part of a button box: if vertically aligned, place
	    // tip on the right; otherwise, place it at the bottom.
	    unsigned char orientation = XmHORIZONTAL;
	    XtVaGetValues(parent, XmNorientation, &orientation, XtPointer(0));

	    switch (placement)
	    {
	    case BottomLeft:
	    case BottomRight:
	    case TopLeft:
	    case TopRight:
		if (orientation == XmHORIZONTAL)
		    ok = true;
		break;

	    case LeftBottom:
	    case LeftTop:
	    case RightBottom:
	    case RightTop:
		if (orientation == XmVERTICAL)
		    ok = true;
		break;
	    }
	}
	else if (XmIsForm(parent))
	{
	    // We're part of a form: try to place the tip beyond a form
	    // boundary.

	    int fraction_base = 100;
	    XtVaGetValues(parent, 
			  XmNfractionBase, &fraction_base,
			  XtPointer(0));
	    if (fraction_base == 100)
	    {
		// Simple form
		ok = true;
	    }
	    else
	    {
		// Command tool or likewise
		unsigned char left_attachment   = XmATTACH_NONE;
		unsigned char right_attachment  = XmATTACH_NONE;
		unsigned char top_attachment    = XmATTACH_NONE;
		unsigned char bottom_attachment = XmATTACH_NONE;

		XtVaGetValues(w, 
			      XmNleftAttachment,   &left_attachment,
			      XmNrightAttachment,  &right_attachment,
			      XmNtopAttachment,    &top_attachment,
			      XmNbottomAttachment, &bottom_attachment,
			      XtPointer(0));

		int left_position   = 0;
		int right_position  = 0;
		int top_position    = 0;
		int bottom_position = 0;

		XtVaGetValues(w, 
			      XmNleftPosition,   &left_position,
			      XmNrightPosition,  &right_position,
			      XmNtopPosition,    &top_position,
			      XmNbottomPosition, &bottom_position,
			      XtPointer(0));

		switch (placement)
		{
		case BottomLeft:
		case BottomRight:
		    if (bottom_attachment == XmATTACH_POSITION
			&& bottom_position >= fraction_base)
			ok = true;
		    if (bottom_attachment == XmATTACH_FORM
			|| top_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case RightBottom:
		case RightTop:
		    if (right_attachment == XmATTACH_POSITION
			&& right_position >= fraction_base)
			ok = true;
		    if (right_attachment == XmATTACH_FORM
			|| left_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case TopLeft:
		case TopRight:
		    if (top_attachment == XmATTACH_POSITION
			&& top_position == 0)
			ok = true;
		    if (top_attachment == XmATTACH_FORM
			|| bottom_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case LeftBottom:
		case LeftTop:
		    if (left_attachment == XmATTACH_POSITION
			&& left_position == 0)
			ok = true;
		    if (left_attachment == XmATTACH_FORM
			|| right_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;
		}
	    }
	}
	else
	{
	    // Any other alignment
	    ok = true;
	}

	if (!ok && run <= 8)
	    continue;

	// Don't move tip off the screen
	XWindowAttributes w_attributes;
	XGetWindowAttributes(XtDisplay(w), XtWindow(w), &w_attributes);

	if (XmIsText(w))
	{
	    w_attributes.width  = 0;
	    w_attributes.height = 0;
	}

	XtWidgetGeometry tip_geometry;
	tip_geometry.request_mode = CWHeight | CWWidth;
	XtQueryGeometry(tip_shell, (XtWidgetGeometry *)0, &tip_geometry);

	int x_offset = 5;
	int y_offset = 5;

	int dx = 0;
	switch (placement)
	{
	case LeftBottom:
	case LeftTop:
	    dx = -(tip_geometry.width + x_offset);
	    break;
	case RightBottom:
	case RightTop:
	    dx = w_attributes.width + x_offset;
	    break;
	case BottomLeft:
	case TopLeft:
	    dx = w_attributes.width / 2 - tip_geometry.width;
	    break;
	case BottomRight:
	case TopRight:
	    dx = w_attributes.width / 2;
	    break;
	}

	int dy = 0;
	switch (placement)
	{
	case LeftBottom:
	case RightBottom:
	    dy = w_attributes.height / 2;
	    break;
	case LeftTop:
	case RightTop:
	    dy = w_attributes.height / 2 - tip_geometry.height;
	    break;
	case BottomLeft:
	case BottomRight:
	    dy = w_attributes.height + y_offset;
	    break;
	case TopLeft:
	case TopRight:
	    dy = -(tip_geometry.height + y_offset);
	    break;
	}

	if (XmIsText(w))
	{
	    BoxPoint pos = point(&ti->event);
	    dx += pos[X];
	    dy += pos[Y];
	}

	// Don't move tip off the screen
	Window w_child;
	int x, y;
	XTranslateCoordinates(XtDisplay(w), XtWindow(w), w_attributes.root,
			      dx, dy, &x, &y, &w_child);
	if (x < 0)
	    continue;
	if (y < 0)
	    continue;
	if (x + tip_geometry.width >= WidthOfScreen(XtScreen(w)))
	    continue;
	if (y + tip_geometry.height >= HeightOfScreen(XtScreen(w)))
	    continue;

	// Move tip to X, Y...
	XtVaSetValues(tip_shell,
		      XmNx, x,
		      XmNy, y,
		      XtPointer(0));
	
	// and pop it up.
	XtPopup(tip_shell, XtGrabNone);
	tip_popped_up = true;
	last_placement = placement;
	last_parent    = parent;

	return;
    }
}

// Show the documentation for the widget given in CLIENT_DATA
static void ShowDocumentation(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == raise_doc_timer);
    raise_doc_timer = 0;

    TipInfo *ti = (TipInfo *)client_data;
    XtRemoveCallback(ti->widget, XmNdestroyCallback, CancelRaiseDoc, 0);

    if (DisplayDocumentation != 0 
	&& (XmIsText(ti->widget) ? text_docs_enabled : button_docs_enabled))
    {
	// Display documentation
	MString doc = get_documentation_string(ti->widget, &ti->event);
	DisplayDocumentation(doc);
    }
}

// Clear the documentation
static void ClearDocumentationNow(Widget w);

static void CancelClearDocumentation(Widget w, XtPointer, XtPointer)
{
    if (clear_doc_timer == 0)
	return;

    XtRemoveTimeOut(clear_doc_timer);
    clear_doc_timer = 0;
    ClearDocumentationNow(w);
}

static void ClearDocumentationNow(Widget w)
{
    if (DisplayDocumentation != 0 
	&& (XmIsText(w) ? text_docs_enabled : button_docs_enabled))
    {
	// Clear documentation
	static MString empty(0, true);
	DisplayDocumentation(empty);
    }
}

static void ClearDocumentation(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == clear_doc_timer);
    clear_doc_timer = 0;

    TipInfo *ti = (TipInfo *)client_data;

    XtRemoveCallback(ti->widget, XmNdestroyCallback, 
		     CancelClearDocumentation, 0);
    ClearDocumentationNow(ti->widget);
}
#else
// Event information passed through timeouts, etc.
struct TipInfo {
    GUI::RefPtr<GUI::Event> event;		// The event structure
    GUI::Widget *widget;	// The widget the event occurred in
};

#define CANCEL_RAISE_DOC (void *)1
#define CANCEL_RAISE_TIP (void *)2
#define CANCEL_CLEAR_DOCUMENTATION (void *)3


// Raise button tip near the widget given in CLIENT_DATA
static bool PopupTip(TipInfo *ti)
{
    raise_tip_timer.disconnect();

    GUI::Widget *w = ti->widget;

    if (w == 0)
	return false;

    w->remove_destroy_notify_callback(CANCEL_RAISE_TIP);

    MString tip = get_tip_string(w, &*ti->event);
    if (tip.isNull() || isNone(tip) || tip.isEmpty())
	return false;

    if (!w->is_realized())
	return false;

    if (tip_shell == 0)
    {
	tip_shell = new GUI::Window("tipshell", "tipshell", GUI::WINDOW_POPUP);
	tip_label = new GUI::Label(*tip_shell, GUI::PACK_EXPAND_WIDGET,
				   "testbutton", _("Testbutton"));
#if 0
	tip_shell->property_allow_shrink() = true;
	std::cerr << "ALLOW SHRINK = " << tip_shell->property_allow_shrink()
		  << "\n";
#endif
	tip_label->show();
    }

    tip_label->set_text(tip.xmstring());
    // FIXME: The GTK shell seems to grow, but will not shrink again.
    GUI::Requisition req = tip_label->size_request();
    std::cerr << "TIP REQUISITION " << req.width << " " << req.height << "\n";
    tip_shell->resize(req.width, req.height);

    // Find a possible place for the tip.  Consider the alignment of
    // the parent composite as well as the distance to the screen edge.

    //            TopLeft TopRight
    // LeftTop    XXXXXXXXXXXXXXXX RightTop
    //            XXXXXXXXXXXXXXXX
    // LeftBottom XXXXXXXXXXXXXXXX RightBottom
    //         BottomLeft BottomRight

    enum Placement { LeftTop, RightTop,
		     LeftBottom, RightBottom,
		     BottomLeft, BottomRight,
		     TopLeft, TopRight };

    static Placement last_placement = BottomRight;
    static GUI::Widget *last_parent = NULL;

    GUI::Widget *parent = w->get_parent();
    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
    if (parent != last_parent || text)
	last_placement = BottomRight;

    // Use 18 runs to find out the best position:
    // Run 0: try last placement in same alignment
    // Run 1-8: try at various sides of W.  Don't hide other widgets
    //          and don't move tip off the screen.
    // Run 9-17: same as 0-8, but don't care for hiding other widgets.
    for (int run = 0; run < 18; run++)
    {
	std::cerr << "Placing tip, run " << run << "\n";
	Placement placement = last_placement;
	switch (run % 9)
	{
	case 0: placement = last_placement; break;
	case 1: placement = BottomRight; break;
	case 2: placement = RightBottom; break;
	case 3: placement = RightTop;    break;
	case 4: placement = TopRight;    break;
	case 5: placement = BottomLeft;  break;
	case 6: placement = LeftBottom;  break;
	case 7: placement = LeftTop;     break;
	case 8: placement = TopLeft;     break;
	}

	bool ok = false;

	GUI::HBox *hbox = dynamic_cast<GUI::HBox *>(parent);
	GUI::VBox *vbox = dynamic_cast<GUI::VBox *>(parent);
	if (hbox || vbox)
	{
	    switch (placement)
	    {
	    case BottomLeft:
	    case BottomRight:
	    case TopLeft:
	    case TopRight:
		if (hbox)
		    ok = true;
		break;

	    case LeftBottom:
	    case LeftTop:
	    case RightBottom:
	    case RightTop:
		if (vbox)
		    ok = true;
		break;
	    }
	}
#if 0
	else if (XmIsForm(parent))
	{
	    // We're part of a form: try to place the tip beyond a form
	    // boundary.

	    int fraction_base = 100;
	    XtVaGetValues(parent, 
			  XmNfractionBase, &fraction_base,
			  XtPointer(0));
	    if (fraction_base == 100)
	    {
		// Simple form
		ok = true;
	    }
	    else
	    {
		// Command tool or likewise
		unsigned char left_attachment   = XmATTACH_NONE;
		unsigned char right_attachment  = XmATTACH_NONE;
		unsigned char top_attachment    = XmATTACH_NONE;
		unsigned char bottom_attachment = XmATTACH_NONE;

		XtVaGetValues(w, 
			      XmNleftAttachment,   &left_attachment,
			      XmNrightAttachment,  &right_attachment,
			      XmNtopAttachment,    &top_attachment,
			      XmNbottomAttachment, &bottom_attachment,
			      XtPointer(0));

		int left_position   = 0;
		int right_position  = 0;
		int top_position    = 0;
		int bottom_position = 0;

		XtVaGetValues(w, 
			      XmNleftPosition,   &left_position,
			      XmNrightPosition,  &right_position,
			      XmNtopPosition,    &top_position,
			      XmNbottomPosition, &bottom_position,
			      XtPointer(0));

		switch (placement)
		{
		case BottomLeft:
		case BottomRight:
		    if (bottom_attachment == XmATTACH_POSITION
			&& bottom_position >= fraction_base)
			ok = true;
		    if (bottom_attachment == XmATTACH_FORM
			|| top_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case RightBottom:
		case RightTop:
		    if (right_attachment == XmATTACH_POSITION
			&& right_position >= fraction_base)
			ok = true;
		    if (right_attachment == XmATTACH_FORM
			|| left_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case TopLeft:
		case TopRight:
		    if (top_attachment == XmATTACH_POSITION
			&& top_position == 0)
			ok = true;
		    if (top_attachment == XmATTACH_FORM
			|| bottom_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;

		case LeftBottom:
		case LeftTop:
		    if (left_attachment == XmATTACH_POSITION
			&& left_position == 0)
			ok = true;
		    if (left_attachment == XmATTACH_FORM
			|| right_attachment == XmATTACH_OPPOSITE_FORM)
			ok = true;
		    break;
		}
	    }
	}
#endif
	else
	{
	    // Any other alignment
	    ok = true;
	}

	if (!ok && run <= 8)
	    continue;

	GUI::Allocation alloc = w->get_allocation();
	std::cerr << "Placing tip, alloc = "
		  << alloc.x << " " << alloc.y << " "
		  << alloc.width << " " << alloc.height << "\n";
	// Don't move tip off the screen
	int width = alloc.width;
	int height = alloc.height;
	
	if (text)
	{
	    width  = 0;
	    height = 0;
	}

	int tip_x, tip_y, tip_width, tip_height;
	tip_shell->get_position(tip_x, tip_y);
	tip_shell->get_size(tip_width, tip_height);
	std::cerr << "Placing tip, tip_pos = " << tip_x << " " << tip_y << "\n";
	std::cerr << "Placing tip, tip_size = " << tip_width << " " << tip_height << "\n";

	int x_offset = 5;
	int y_offset = 5;

	int dx = 0;
	switch (placement)
	{
	case LeftBottom:
	case LeftTop:
	    std::cerr << "Placing tip, LB/LT\n";
	    dx = -(tip_width + x_offset);
	    break;
	case RightBottom:
	case RightTop:
	    std::cerr << "Placing tip, RB/RT\n";
	    dx = width + x_offset;
	    break;
	case BottomLeft:
	case TopLeft:
	    std::cerr << "Placing tip, BL/TL\n";
	    dx = width / 2 - tip_width;
	    break;
	case BottomRight:
	case TopRight:
	    std::cerr << "Placing tip, BR/TR\n";
	    dx = width / 2;
	    break;
	}

	int dy = 0;
	switch (placement)
	{
	case LeftBottom:
	case RightBottom:
	    std::cerr << "Placing tip, LB/RB\n";
	    dy = height / 2;
	    break;
	case LeftTop:
	case RightTop:
	    std::cerr << "Placing tip, LT/RT\n";
	    dy = height / 2 - tip_height;
	    break;
	case BottomLeft:
	case BottomRight:
	    std::cerr << "Placing tip, BL/BR\n";
	    dy = height + y_offset;
	    break;
	case TopLeft:
	case TopRight:
	    std::cerr << "Placing tip, TL/TR\n";
	    dy = -(tip_height + y_offset);
	    break;
	}

	if (text)
	{
	    BoxPoint pos = point(ti->event);
	    std::cerr << "Placing tip, event pos = " << pos[X] << " " << pos[Y] << "\n";
	    dx += pos[X];
	    dy += pos[Y];
	}

	// Don't move tip off the screen
	GUI::RefPtr<GUI::XWindow> xwin = w->get_window();
	int xt, yt, xo, yo;
	// get_position is said to be relative to parent (but that
	// doesn't seem to be true).
	// xwin->get_position(xo, yo);
	xwin->get_origin(xo, yo);
	xo += alloc.x;
	yo += alloc.y;
	std::cerr << "Placing tip, origin = " << xo << " " << yo << "\n";

	xt = xo + dx;
	yt = yo + dy;
	if (xt < 0)
	    continue;
	if (yt < 0)
	    continue;
	GUI::RefPtr<GUI::Screen> screen = w->get_screen();
	if (xt + tip_width >= screen->get_width())
	    continue;
	if (yt + tip_height >= screen->get_height())
	    continue;

	// Move tip to X, Y...
	tip_shell->move(xt, yt);
	
	// and pop it up.
	// XtPopup(tip_shell, XtGrabNone);
	tip_shell->show();
	tip_popped_up = true;
	last_placement = placement;
	last_parent    = parent;

	return false;
    }

    tip_shell->show();
    tip_popped_up = true;
    return false;
}

// Show the documentation for the widget given in CLIENT_DATA
static bool ShowDocumentation(TipInfo *ti)
{
    // raise_doc_timer.disconnect(); // Just return false

    ti->widget->remove_destroy_notify_callback(CANCEL_RAISE_DOC);

    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(ti->widget);

    if (DisplayDocumentation != 0 
	&& (text ? text_docs_enabled : button_docs_enabled))
    {
	// Display documentation
	MString doc = get_documentation_string(ti->widget, &*ti->event);
	DisplayDocumentation(doc);
    }
    return false;
}

// Clear the documentation
static void ClearDocumentationNow(GUI::Widget *w);

static void *CancelClearDocumentation(void *client_data)
{
    if (!clear_doc_timer)
	return NULL;

    clear_doc_timer.disconnect();
    GUI::Widget *w = static_cast<GUI::Widget *>(client_data);
    ClearDocumentationNow(w);
    return NULL;
}

static void ClearDocumentationNow(GUI::Widget *w)
{
    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
    if (DisplayDocumentation != 0 
	&& (text ? text_docs_enabled : button_docs_enabled))
    {
	// Clear documentation
	static MString empty(0, true);
	DisplayDocumentation(empty);
    }
}

static bool ClearDocumentation(TipInfo *ti)
{
    // clear_doc_timer.disconnect(); // Just return false

    ti->widget->remove_destroy_notify_callback(CANCEL_CLEAR_DOCUMENTATION);

    ClearDocumentationNow(ti->widget);
    return false;
}
#endif

#if defined(IF_XM)
// Clear tips and documentation
static void ClearTip(Widget w, XEvent *event)
{
    CancelRaiseTip();
    CancelRaiseDoc();

    if (tip_popped_up)
    {
	XtPopdown(tip_shell);
	tip_popped_up = false;
    }

    if (clear_doc_timer)
    {
	XtRemoveTimeOut(clear_doc_timer);
	clear_doc_timer = 0;
    }

    if (DisplayDocumentation != 0 
	&& (XmIsText(w) ? text_docs_enabled : button_docs_enabled))
    {
	// We don't clear the documentation immediately, since the
	// user might be moving over to another button, and we don't
	// want flashing documentation strings.

	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	clear_doc_timer =
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    help_clear_doc_delay, 
			    ClearDocumentation, XtPointer(&ti));

	// Should the button be destroyed beforehand, cancel timeout
	XtRemoveCallback(w, XmNdestroyCallback, CancelClearDocumentation, 0);
	XtAddCallback   (w, XmNdestroyCallback, CancelClearDocumentation, 0);
    }
}
#else
// Clear tips and documentation
static void ClearTip(GUI::Widget *w, GUI::Event *event)
{
    CancelRaiseTip(NULL);
    CancelRaiseDoc(NULL);

    std::cerr << "tip_popped_up = " << tip_popped_up << "\n";
    if (tip_popped_up)
    {
	tip_shell->hide();
	tip_popped_up = false;
    }

    if (clear_doc_timer)
    {
	clear_doc_timer.disconnect();
    }

    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
    if (DisplayDocumentation != 0 
	&& (text ? text_docs_enabled : button_docs_enabled))
    {
	// We don't clear the documentation immediately, since the
	// user might be moving over to another button, and we don't
	// want flashing documentation strings.

	static TipInfo ti;
	ti.event  = event->clone();
	ti.widget = w;

	clear_doc_timer =
	    GUI::signal_timeout().connect(sigc::bind(sigc::ptr_fun(ClearDocumentation), &ti),
					  help_clear_doc_delay);

	// Should the button be destroyed beforehand, cancel timeout
	w->remove_destroy_notify_callback(w);
	w->add_destroy_notify_callback(w, CancelClearDocumentation);
    }
}
#endif

#if defined(IF_XM)
// Raise tips and documentation
static void RaiseTip(Widget w, XEvent *event)
{
    if (DisplayDocumentation != 0
	&& (XmIsText(w) ? text_docs_enabled : button_docs_enabled))
    {
	// No need to clear the documentation
	if (clear_doc_timer)
	{
	    XtRemoveTimeOut(clear_doc_timer);
	    clear_doc_timer = 0;
	}

	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	int doc_delay = 
	    XmIsText(w) ? help_value_doc_delay : help_button_doc_delay;

	CancelRaiseDoc();

	raise_doc_timer =
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    doc_delay,
			    ShowDocumentation, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelRaiseDoc, 0);
    }

    if (XmIsText(w) ? text_tips_enabled : button_tips_enabled)
    {
	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	int tip_delay = 
	    XmIsText(w) ? help_value_tip_delay : help_button_tip_delay;

	CancelRaiseTip();

	raise_tip_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    tip_delay,
			    PopupTip, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelRaiseTip, 0);
    }
}
#else
// Raise tips and documentation
static void RaiseTip(GUI::Widget *w, GUI::Event *event)
{
    std::cerr << "RaiseTip " << w << " " << event << "\n";
    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
    if (DisplayDocumentation != 0
	&& (text ? text_docs_enabled : button_docs_enabled))
    {
	// No need to clear the documentation
	if (clear_doc_timer)
	{
	    clear_doc_timer.disconnect();
	}

	static TipInfo ti;
	ti.event  = event->clone();
	ti.widget = w;

	int doc_delay = 
	    text ? help_value_doc_delay : help_button_doc_delay;

	CancelRaiseDoc(NULL);

	raise_doc_timer =
	    GUI::signal_timeout().connect(sigc::bind(sigc::ptr_fun(ShowDocumentation), &ti), doc_delay);

	// Should W be destroyed beforehand, cancel timeout
	w->add_destroy_notify_callback(CANCEL_RAISE_DOC, CancelRaiseDoc);
    }

    if (text ? text_tips_enabled : button_tips_enabled)
    {
	static TipInfo ti;
	ti.event  = event;
	ti.widget = w;

	int tip_delay = 
	    text ? help_value_tip_delay : help_button_tip_delay;

	CancelRaiseTip(NULL);

	raise_tip_timer = 
	    GUI::signal_timeout().connect(sigc::bind(sigc::ptr_fun(PopupTip), &ti), tip_delay);

	// Should W be destroyed beforehand, cancel timeout
	w->add_destroy_notify_callback(CANCEL_RAISE_TIP, CancelRaiseTip);
    }
}
#endif

#if defined(IF_XM)
static void DoClearTip(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == clear_tip_timer);
    clear_tip_timer = 0;

    TipInfo& ti = *((TipInfo *)client_data);
    ClearTip(ti.widget, &ti.event);
}
#else
static bool DoClearTip(TipInfo *ti)
{
    std::cerr << "CLEAR TIP\n";
    // clear_tip_timer = 0; // Just return false

    ClearTip(ti->widget, ti->event);
    return false;
}
#endif

#if defined(IF_XM)
// Widget W has been entered or left.  Handle event.
static void HandleTipEvent(Widget w,
			   XtPointer /* client_data */,
			   XEvent *event, 
			   Boolean * /* continue_to_dispatch */)
{
    static Widget last_left_widget = 0;

    switch (event->type)
    {
    case EnterNotify:
    {
	if (clear_tip_timer != 0)
	{
	    XtRemoveTimeOut(clear_tip_timer);
	    clear_tip_timer  = 0;

	    if (w != last_left_widget)
	    {
		// Entered other widget within HELP_CLEAR_TIP_DELAY.
		ClearTip(w, event);
		last_left_widget = 0;
	    }
	    else
	    {
		// Re-entered same widget -- ignore.
		last_left_widget = 0;
		break;
	    }
	}

	if (!XmIsText(w))
	{
	    // Clear and re-raise tip
	    ClearTip(w, event);
	    RaiseTip(w, event);
	}
    }
    break;

    case LeaveNotify:
    {
	last_left_widget = w;
	if (clear_tip_timer != 0)
	{
	    XtRemoveTimeOut(clear_tip_timer);
	    clear_tip_timer = 0;
	}

	// We don't clear the tip immediately, because the DDD ungrab
	// mechanism may cause the pointer to leave a button and
	// re-enter it immediately.
	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	clear_tip_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    help_clear_tip_delay, 
			    DoClearTip, &ti);
    }
    break;

    case KeyPress:
    case KeyRelease:
    case ButtonPress:
    case ButtonRelease:
	ClearTip(w, event);
	break;

    case MotionNotify:
	if (XmIsText(w))
	{
	    static Widget last_motion_widget           = 0;
	    static XmTextPosition last_motion_position = XmTextPosition(-1);
	    XmTextPosition pos = TextPosOfEvent(w, event);

	    if (w != last_motion_widget || pos != last_motion_position)
	    {
		last_motion_widget   = w;
		last_motion_position = pos;

		ClearTip(w, event);
		if (pos != XmTextPosition(-1))
		    RaiseTip(w, event);
	    }
	}
        break;
    }
}
#else
// Widget W has been entered or left.  Handle event.
static bool HandleTipEvent(GUI::Widget *w, GUI::Event *event)
{
    static GUI::Widget *last_left_widget = NULL;

    GUI::EventMotion *motion;
    GUI::EventCrossing *crossing;
    GUI::EventKey *key;
    GUI::EventButton *button;
    if ((crossing = dynamic_cast<GUI::EventCrossing *>(event))) {
	if (crossing->type == GUI::ENTER_NOTIFY) {
	    if (clear_tip_timer)
	    {
		clear_tip_timer.disconnect();

		if (w != last_left_widget)
		{
		    // Entered other widget within HELP_CLEAR_TIP_DELAY.
		    ClearTip(w, event);
		    last_left_widget = NULL;
		}
		else
		{
		    // Re-entered same widget -- ignore.
		    last_left_widget = NULL;
		    return false;
		}
	    }
	    GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
	    if (text)
		std::cerr << "TEXT WIDGET\n";
	    if (!text)
	    {
		// Clear and re-raise tip
		ClearTip(w, event);
		RaiseTip(w, event);
	    }
	}
	else if (crossing->type == GUI::LEAVE_NOTIFY) {
	    last_left_widget = w;
	    if (clear_tip_timer)
	    {
		clear_tip_timer.disconnect();
	    }

	    // We don't clear the tip immediately, because the DDD ungrab
	    // mechanism may cause the pointer to leave a button and
	    // re-enter it immediately.
	    static TipInfo ti;
	    ti.event  = event;
	    ti.widget = w;

	    clear_tip_timer = 
		GUI::signal_timeout().connect(sigc::bind(sigc::ptr_fun(DoClearTip), &ti),
					      help_clear_tip_delay);
	}
    }
    if ((key = dynamic_cast<GUI::EventKey *>(event))) {
	ClearTip(w, event);
    }
    if ((button = dynamic_cast<GUI::EventButton *>(event))) {
	ClearTip(w, event);
    }
    if ((motion = dynamic_cast<GUI::EventMotion *>(event))) {
	GUI::ScrolledText *text = dynamic_cast<GUI::ScrolledText *>(w);
	if (text) {
	    static GUI::ScrolledText *last_motion_widget           = NULL;
	    static long last_motion_position = -1;
	    int x, y;
	    // N.B. If we receive a hint, we must call get_pointer().
	    // Otherwise we will get no more events.
	    if (motion->is_hint) {
		w->get_pointer(x, y);
	    }
	    else {
		x = motion->x;
		y = motion->y;
	    }
	    long pos = TextPosOfEvent(text, event);
	    std::cerr << "EVENT AT " << x << " " << y << " " << pos << "\n";
	    if (w != last_motion_widget || pos != last_motion_position)
	    {
		last_motion_widget   = text;
		last_motion_position = pos;

		std::cerr << "HandleTipEvent event = " << event
			  << " " << ((GUI::EventMotion *)event)->x << " "
			  << ((GUI::EventMotion *)event)->y << "\n";
		ClearTip(w, event);
		if (pos != -1)
		    RaiseTip(w, event);
	    }
	}
    }
    return false;
}
#endif


#if defined(IF_XM)
// (Un)install toolbar tips for W
static void InstallButtonTipEvents(Widget w, bool install)
{
    // Install handler only if either `tipString' or
    // `documentationString' resource is specified
    tip_resource_values tip_values;
    XtGetApplicationResources(w, &tip_values, 
			      tip_subresources, XtNumber(tip_subresources), 
			      ArgList(0), 0);

    doc_resource_values doc_values;
    XtGetApplicationResources(w, &doc_values, 
			      doc_subresources, XtNumber(doc_subresources), 
			      ArgList(0), 0);

    if (tip_values.tipString != 0 || doc_values.documentationString != 0)
    {
	EventMask event_mask = 
	    EnterWindowMask | LeaveWindowMask | ButtonPress | ButtonRelease 
	    | KeyPress | KeyRelease;
	if (install)
	{
	    XtAddEventHandler(w, event_mask, False, 
			      HandleTipEvent, XtPointer(0));

	}
	else
	{
	    XtRemoveEventHandler(w, event_mask, False, 
				 HandleTipEvent, XtPointer(0));
	}
    }

    // CvtStringToXmString and friends should be responsible for
    // the memory management.
    // XmStringFree(tip_values.tipString);
    // tip_values.tipString = 0;
    // XmStringFree(doc_values.documentationString);
    // doc_values.documentationString = 0;
}
#endif


#if defined(IF_XM)
// (Un)install toolbar tips for W and all its descendants
static void InstallButtonTipsNow(Widget w, bool install)
{
    if (w == 0 || !XtIsWidget(w))
	return;

    // Install tips for this widget
    InstallButtonTipEvents(w, install);

    if (XtIsComposite(w))
    {
	// Traverse widget tree
	WidgetList children   = 0;
	Cardinal num_children = 0;

	XtVaGetValues(w,
		      XtNchildren, &children,
		      XtNnumChildren, &num_children,
		      XtPointer(0));

	if (children != 0)
	    for (int i = 0; i < int(num_children); i++)
		InstallButtonTipsNow(children[i], install);
    }

    if (XmIsCascadeButton(w))
    {
	// Traverse the menu associated with this button
	Widget subMenuId = 0;
	XtVaGetValues(w, XmNsubMenuId, &subMenuId, XtPointer(0));
	if (subMenuId != 0)
	    InstallButtonTipsNow(subMenuId, install);
    }

    if (XmIsRowColumn(w))
    {
	// Traverse the menu associated with this option menu
	unsigned char rowColumnType = XmWORK_AREA;
	Widget subMenuId = 0;
	XtVaGetValues(w, 
		      XmNsubMenuId, &subMenuId,
		      XmNrowColumnType, &rowColumnType,
		      XtPointer(0));
	if (rowColumnType == XmMENU_OPTION && subMenuId != 0)
	    InstallButtonTipsNow(subMenuId, install);
    }
}
#else
void InstallButtonTipsNow(GUI::Widget *w, bool install)
{
    std::cerr << "InstallButtonTipsNow not implemented yet.\n";
}
#endif

#if defined(IF_XM)
// Callback funtion: install tips now.
static void InstallButtonTipsTimeOut(XtPointer client_data,
				     XtIntervalId *timer)
{
    Widget w = Widget(client_data);
    XtRemoveCallback(w, XmNdestroyCallback, 
		     CancelTimer, XtPointer(*timer));
    InstallButtonTipsNow(w, true);
}

// Callback funtion: uninstall tips now.
static void UnInstallButtonTipsTimeOut(XtPointer client_data, 
				       XtIntervalId *timer)
{
    Widget w = Widget(client_data);
    XtRemoveCallback(w, XmNdestroyCallback, 
		     CancelTimer, XtPointer(*timer));
    InstallButtonTipsNow(w, false);
}
#endif

#if defined(IF_XM)
// (Un)install tips for W and all its descendants.
// We do this as soon as we are back in the event loop, since we
// assume all children have been created until then.
void InstallButtonTips(Widget w, bool install)
{
    XtIntervalId timer;

    if (install)
	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0,
				InstallButtonTipsTimeOut, XtPointer(w));
    else
	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0,
				UnInstallButtonTipsTimeOut, XtPointer(w));

    // Should W be destroyed beforehand, cancel installation.
    XtAddCallback(w, XmNdestroyCallback, CancelTimer, XtPointer(timer));
}
#else
// (Un)install tips for W and all its descendants.
// We do this as soon as we are back in the event loop, since we
// assume all children have been created until then.
void InstallButtonTips(GUI::Widget *w, bool install)
{
    std::cerr << "InstallButtonTips not implemented yet.\n";
}
#endif

// Enable or disable button tips
void EnableButtonTips(bool enable)
{
    button_tips_enabled = enable;
}

// Enable or disable button docs
void EnableButtonDocs(bool enable)
{
    button_docs_enabled = enable;
}



//-----------------------------------------------------------------------------
// Text tips.
//-----------------------------------------------------------------------------

#if defined(IF_XM)
// (Un)install text tips for W.
void InstallTextTips(Widget w, bool install)
{
    EventMask event_mask = EnterWindowMask | LeaveWindowMask 
	| ButtonPress | ButtonRelease | PointerMotionMask
	| KeyPress | KeyRelease;

    if (install)
    {
	XtAddEventHandler(w, event_mask, False, 
			  HandleTipEvent, XtPointer(0));

    }
    else
    {
	XtRemoveEventHandler(w, event_mask, False, 
			     HandleTipEvent, XtPointer(0));
    }
}
#else
// (Un)install text tips for W.
void InstallTextTips(GUI::Widget *w, bool install)
{
    GUI::EventMask event_mask = GUI::EventMask(GUI::ENTER_NOTIFY_MASK
					       | GUI::LEAVE_NOTIFY_MASK 
					       | GUI::BUTTON_PRESS_MASK
					       | GUI::BUTTON_RELEASE_MASK
					       | GUI::POINTER_MOTION_MASK
					       | GUI::POINTER_MOTION_HINT_MASK
					       | GUI::KEY_PRESS_MASK
					       | GUI::KEY_RELEASE_MASK);

    if (install)
    {
	// Note: signal handler will trap all events in widget's event
	// mask, not only those we set here.
	w->add_event_mask(event_mask);
	w->signal_event().connect(sigc::bind<0>(sigc::ptr_fun(HandleTipEvent), w));
    }
    else
    {
	std::cerr << "\n\nHow to remove handler for InstallTextTips?\n";
	std::cerr << "We have no connection id.\n\n";
    }
}
#endif

// Enable or disable text tips
void EnableTextTips(bool enable)
{
    text_tips_enabled = enable;
}

// Enable or disable text docs
void EnableTextDocs(bool enable)
{
    text_docs_enabled = enable;
}
