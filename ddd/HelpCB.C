// $Id$  -*- C++ -*-
// Interactive Help Callbacks

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

char HelpCB_rcsid[] =
    "$Id$";

#include "config.h"

#include "Agent.h"
#include "ComboBox.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MakeMenu.h"
#include "SmartC.h"
#include "TimeOut.h"
#include "ddd.h"		// process_pending_events()
#include "findParent.h"
#include "longName.h"
#include "toolbar.h"
#include "windows.h"		// set_scrolled_window_size()
#include "misc.h"		// min(), max()

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
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

#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>	// LessTif hacks
#include <X11/Shell.h>

// Misc DDD includes
#include "LessTifH.h"
#include "strclass.h"
#include "cook.h"
#include "events.h"
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
	XtNhelpString,
	XtCHelpString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(help_resource_values, helpString),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XtNhelpShowTitle,
	XtCHelpShowTitle,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(help_resource_values, showTitle),
	XtRImmediate,
	XtPointer(False)
    }
};

static XtResource help_on_version_subresources[] = {
    {
	XtNhelpOnVersionString,
	XtCHelpOnVersionString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(help_on_version_resource_values, helpOnVersionString),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XtNhelpShowTitle,
	XtCHelpShowTitle,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(help_on_version_resource_values, showTitle),
	XtRImmediate,
	XtPointer(False)
    }
};

static XtResource tip_subresources[] = {
    {
	XtNtipString,
	XtCTipString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(tip_resource_values, tipString), 
	XtRImmediate,
	XtPointer(0)
    }
};

static XtResource doc_subresources[] = {
    {
	XtNdocumentationString,
	XtCDocumentationString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(doc_resource_values, documentationString), 
	XtRImmediate,
	XtPointer(0)
    }
};

//-----------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------

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


//-----------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------

// A single `\n' means `no string'.  (An empty string causes the
// helpers to be called.)
static bool isNone(const MString& s)
{
    return s.isEmpty() && s.lineCount() > 1;
}

static MString get_help_string(Widget widget)
{
    // Get text
    help_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      help_subresources, XtNumber(help_subresources), 
			      NULL, 0);

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

static MString get_help_on_version_string(Widget widget)
{
    // Get text
    help_on_version_resource_values values;
    XtGetApplicationResources(widget, &values, 
			      help_on_version_subresources, 
			      XtNumber(help_subresources), 
			      NULL, 0);

    MString text(values.helpOnVersionString, true);
    if (text.isNull() || text.isEmpty())
	text = NoHelpText(widget);

    if (values.showTitle)
	text.prepend(rm("Help on version for ") + bf(cook(longName(widget)))
		     + rm(":") + cr() + cr());

    return text;
}

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
			      NULL, 0);

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

static MString prepend_label_name(Widget widget, XEvent *event, 
				  MString (*get_string)(Widget, XEvent *))
{
    MString text = get_string(widget, event);

#if 0				// Experimental
    if (XtIsSubclass(widget, xmLabelWidgetClass))
    {
	unsigned char label_type = XmSTRING;
	XtVaGetValues(widget, XmNlabelType, &label_type, NULL);
	if (label_type == XmPIXMAP)
	{
	    XmString label = 0;
	    XtVaGetValues(widget, XmNlabelString, &label, NULL);
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

inline MString get_tip_string(Widget widget, XEvent *event)
{
    return prepend_label_name(widget, event, _get_tip_string);
}


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
			      NULL, 0);

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

inline MString get_documentation_string(Widget widget, XEvent *event)
{
    return prepend_label_name(widget, event, _get_documentation_string);
}


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

static void nop1(Widget) {}
void (*PostHelpOnItemHook)(Widget) = nop1;


//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

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

static void HelpDestroyCB(Widget, XtPointer client_data, XtPointer)
{
    Widget old_dialog = Widget(client_data);
    if (old_dialog == help_dialog)
    {
	help_dialog = 0;
	help_shell  = 0;
    }
}


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

static XmTextPosition NoTextPosOfEvent(Widget, XEvent *)
{
    return XmTextPosition(-1);
}

MString (*DefaultHelpText)(Widget)                    = NoHelpText;
MString (*DefaultTipText)(Widget, XEvent *)           = NoTipText;
MString (*DefaultDocumentationText)(Widget, XEvent *) = NoDocumentationText;
XmTextPosition (*TextPosOfEvent)(Widget, XEvent *)    = NoTextPosOfEvent;


void (*DisplayDocumentation)(const MString&) = 0;

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
	    verify(XmCreateInformationDialog(shell, "help", args, arg));
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


static void HelpIndexCB(Widget widget, XtPointer client_data, 
			XtPointer call_data)
{
    Delay delay;

    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    Widget help_man = Widget(client_data);
    int index = cbs->item_position;

    void *userData = 0;
    XtVaGetValues(widget, XmNuserData, &userData, NULL);
    XmTextPosition *positions = (XmTextPosition *)userData;
    if (positions == 0)
	return;			// Not yet set

    XmTextPosition pos = positions[index - 1];

    XmTextSetInsertionPosition(help_man, pos);
    XmTextSetTopCharacter(help_man, pos);
    XmTextShowPosition(help_man, pos);
}


// Activate the button given in CLIENT_DATA
static void ActivateCB(Widget, XtPointer client_data, 
		       XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
}

struct FindInfo {
    Widget key;			// The text field holding the search key 
    Widget text;		// The text to be searched
};

static bool lock_update_arg = false;

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
    int next_occurrence = -1;

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

    if (next_occurrence < 0)
	post_warning(quote(key) + " not found.", "manual_find_error", w);
    else
    {
	// LessTif 0.79 sometimes returns NULL in cbs->event.  Handle this.
	Time tm;
	if (cbs->event != 0)
	    tm = time(cbs->event);
	else
	    tm = XtLastTimestampProcessed(XtDisplay(fi->text));

	lock_update_arg = true;

	XmTextSetSelection(fi->text,
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

// Highlight current section after cursor motion
static void HighlightSectionCB(Widget, XtPointer client_data, 
			       XtPointer call_data)
{
    XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
    Widget list = Widget(client_data);

    XmTextPosition cursor = cbs->newInsert;

    void *userData = 0;
    XtVaGetValues(list, XmNuserData, &userData, NULL);
    XmTextPosition *positions = (XmTextPosition *)userData;
    if (positions == 0)
	return;			// Not yet set

    int pos = 0;
    while (positions[pos] <= cursor)
	pos++;

    ListSetAndSelectPos(list, pos);
}

inline bool isid(char c)
{
    return isalnum(c) || c == '_' || c == '$';
}

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

    if (selection != "")
    {
	selection.downcase();
	arg_field->set_string(selection);
    }
}

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

// Note: assumes `man' or `info' format.
void ManualStringHelpCB(Widget widget, XtPointer client_data, 
			XtPointer)
{
    static MString null(0, true);
    string text((char *)client_data);

    ManualStringHelpCB(widget, null, text);
}

// Create an empty dialog within a top-level-shell
static Widget create_text_dialog(Widget parent, String name, 
				 Arg *args, int arg)
{
    string shell_name = string(name) + "_popup";
    Widget shell = verify(XtCreateWidget(shell_name.chars(), 
					 topLevelShellWidgetClass,
					 parent, args, arg));

    XtSetArg(args[arg], XmNdialogType, XmDIALOG_PROMPT); arg++;
    Widget w = XmCreateSelectionBox(shell, name, args, arg);
    XtManageChild(w);

    XtUnmanageChild(XmSelectionBoxGetChild(w, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(w, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(w, XmDIALOG_SELECTION_LABEL));

    XtAddCallback(w, XmNokCallback, DestroyThisCB, shell);

    return w;
}

static void DeleteFindInfoCB(Widget, XtPointer client_data, XtPointer)
{
    FindInfo *fi = (FindInfo *)client_data;
    delete fi;
}

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

// Return manual
void ManualStringHelpCB(Widget widget, const MString& title,
			const string& unformatted_text)
{
    // Delay delay;

    // Build manual dialog
    Widget toplevel = findTheTopLevelShell(widget);
    if (toplevel == 0)
	return;

    // Format text
    string the_text(unformatted_text);

    // For efficiency reasons, we access all data in-place via the TEXT ptr.
    char *text = (char *)the_text.chars();

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
		// Header line: strip line and surrounding blanks
		if (target > 0)
		    target--;
		while (target >= 0 && text[target] == '\n')
		    target--;
		target++;
		if (target > 0)
		    text[target++] = '\n';

		while (text[source] != '\n')
		    source++;
		while (text[source] == '\n')
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
		the_text(start_of_title, end_of_title - start_of_title);

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
	text = (char *)the_text.chars();
	len = the_text.length();
    }



    Arg args[15];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    Widget text_dialog = create_text_dialog(toplevel, "manual_help", 
					    args, arg);
    
    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_APPLY_BUTTON));

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget form =
	verify(XmCreateForm(text_dialog, "form", args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    Widget dialog_title = verify(XmCreateLabel(form, "title", args, arg));
    XtManageChild(dialog_title);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNmarginHeight,     0);                 arg++;
    XtSetArg(args[arg], XmNborderWidth,      0);                 arg++;
    XtSetArg(args[arg], XmNallowResize,      True);              arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNtopWidget,        dialog_title);      arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    Widget area = verify(XmCreatePanedWindow(form, "area", args, arg));
    XtManageChild(area);

    arg = 0;
    Widget help_index = verify(XmCreateScrolledList(area, "index", args, arg));
    XtManageChild(help_index);
    set_scrolled_window_size(help_index);

    int columns = max_width(text);
    columns = min(max(columns, 40), 80) + 1;

    arg = 0;
    XtSetArg(args[arg], XmNcolumns,  columns);           arg++;
    XtSetArg(args[arg], XmNeditable, False);             arg++;
    XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
    XtSetArg(args[arg], XmNvalue,    "");                arg++;
    Widget help_man = verify(XmCreateScrolledText(area, "text", args, arg));
    XtManageChild(help_man);
    set_scrolled_window_size(help_man);

    FindInfo *fi = new FindInfo;
    fi->text = help_man;
    XtAddCallback(text_dialog, XmNdestroyCallback,
		  DeleteFindInfoCB, XtPointer(fi));

    MMDesc items [] = 
    {
	{ "findBackward", MMPush, { FindBackwardCB, XtPointer(fi) } },
	{ "findForward",  MMPush, { FindForwardCB, XtPointer(fi) } },
	MMEnd
    };

    Widget arg_label;
    ArgField *arg_field;
    Widget toolbar = create_toolbar(area, "manual", items, 0, arg_label,
				    arg_field, XmPIXMAP);
    fi->key = arg_field->text();
    XtAddCallback(arg_label, XmNactivateCallback, 
		  ClearTextFieldCB, fi->key);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(toolbar, NULL, &size);
    XtVaSetValues(toolbar,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  NULL);

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

    XtVaSetValues(text_dialog, XmNdefaultButton, Widget(0), NULL);
    
    XtManageChild(form);
    InstallButtonTips(text_dialog);

    // Set title
    if (!title.isNull())
    {
	XtVaSetValues(dialog_title, XmNlabelString, title.xmstring(), NULL);
	wm_set_name(XtParent(text_dialog), title.str(), title.str());
    }

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
	XtVaSetValues(help_man, XmNvalue, text, NULL);

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
			    the_text(start_of_line, source - start_of_line);
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
	XtVaSetValues(help_man, XmNvalue, text, NULL);

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
	    XmStringCreateLtoR(titles[i], 
			       titles[i].contains(' ', 0) ? 
			       CHARSET_RM : CHARSET_BF);
    }

    XtVaSetValues(help_index,
		  XmNtopItemPosition,   1,
		  XmNselectedItemCount, 0,
		  XmNitems,             xmtitles,
		  XmNuserData,          xmpositions,
		  XmNitemCount,         titles.size(),
		  NULL);

    for (i = 0; i < titles.size(); i++)
	XmStringFree(xmtitles[i]);
    delete[] xmtitles;

    process_pending_events();

    // Enable Text Window
    Delay::register_shell(XtParent(text_dialog));
    manage_and_raise(text_dialog);
}


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
    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    Widget text_dialog = create_text_dialog(toplevel, name, args, arg);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_APPLY_BUTTON));

    arg = 0;
    Widget form = verify(XmCreateForm(text_dialog, "form", args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    Widget title = verify(XmCreateLabel(form, "title", args, arg));
    XtManageChild(title);

    int columns = max_width(text);
    columns = min(max(columns, 40), 80) + 1;

    arg = 0;
    XtSetArg(args[arg], XmNcolumns,          columns);           arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNtopWidget,        title);             arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNeditable,         False);             arg++;
    XtSetArg(args[arg], XmNeditMode,         XmMULTI_LINE_EDIT); arg++;
    XtSetArg(args[arg], XmNvalue,            text); arg++;
    Widget help_text = verify(XmCreateScrolledText(form, "text", args, arg));
    XtManageChild(help_text);
    set_scrolled_window_size(help_text);

    XtAddCallback(text_dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));

    XtManageChild(form);
    InstallButtonTips(text_dialog);

    // Enable Text Window
    XtRealizeWidget(XtParent(text_dialog));
    Delay::register_shell(XtParent(text_dialog));
    XtPopup(XtParent(text_dialog), XtGrabNone);
    manage_and_raise(text_dialog);
}


//-----------------------------------------------------------------------------
// Context-sensitive help
//-----------------------------------------------------------------------------

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
		  NULL);

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
	cerr << "TrackingEvent: Could not grab pointer\n";
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

// Hook before help on context
static void nop2(Widget, XtPointer, XtPointer) {}
void (*PreHelpOnContextHook)(Widget w, XtPointer client_data, 
			     XtPointer call_data) = nop2;

void HelpOnContextCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget item = 0;
    Widget toplevel = findTheTopLevelShell(widget);

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
    if (lesstif_version < 1000)
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
			  XmNnumChildren, &numChildren, NULL);
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
			      NULL);

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

// The timer used for the delay until the tip is cleared.
static XtIntervalId clear_tip_timer   = 0;

// The timer used for the delay until the documentation is shown.
static XtIntervalId raise_doc_timer   = 0;

// The timer used for the delay until the documentation is cleared.
static XtIntervalId clear_doc_timer   = 0;

// Delay times (in ms)
int help_button_tip_delay = 750;  // Delay before raising button tip
int help_value_tip_delay  = 750;  // Delay before raising value tip
int help_button_doc_delay = 0;    // Delay before showing button doc
int help_value_doc_delay  = 0;    // Delay before showing value doc
int help_clear_doc_delay  = 1000; // Delay before clearing doc
int help_clear_tip_delay  = 50;   // Delay before clearing tip


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

// Helper: cancel RAISE_TIP_TIMER
static void CancelRaiseTip(Widget = 0, XtPointer = 0, XtPointer = 0)
{
    if (raise_tip_timer)
    {
	XtRemoveTimeOut(raise_tip_timer);
	raise_tip_timer = 0;
    }
}

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
					     "tipShell", args, arg));

	arg = 0;
	XtSetArg(args[arg], XmNmarginWidth, 0);                     arg++;
	XtSetArg(args[arg], XmNmarginHeight, 0);                    arg++;
	XtSetArg(args[arg], XmNresizeWidth, True);                  arg++;
	XtSetArg(args[arg], XmNresizeHeight, True);                 arg++;
	XtSetArg(args[arg], XmNborderWidth, 0);                     arg++;
	XtSetArg(args[arg], XmNshadowThickness, 0);                 arg++;
	tip_row = verify(XmCreateRowColumn(tip_shell, "tipRow", args, arg));
	XtManageChild(tip_row);

	arg = 0;
	XtSetArg(args[arg], XmNlabelString, tip.xmstring());        arg++;
	XtSetArg(args[arg], XmNrecomputeSize, true);                arg++;
	XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING);   arg++;
	tip_label = XmCreateLabel(tip_row, "tipLabel", args, arg);
	XtManageChild(tip_label);

	// Simple hack to ensure shell is realized
	XtPopup(tip_shell, XtGrabNone);
	XtPopdown(tip_shell);
    }

    if (lesstif_version < 1000)
    {
	// LessTif fails to resize the shell properly.  Use this hack instead.
	XmFontList font_list;
	XtVaGetValues(tip_label, XmNfontList, &font_list, NULL);
    
	Dimension tip_width  = tip.width(font_list)  + 6;
	Dimension tip_height = tip.height(font_list) + 6;

	XtResizeWidget(tip_label, tip_width, tip_height, 0);
	XtResizeWidget(tip_row,   tip_width, tip_height, 0);
	XtResizeWidget(tip_shell, tip_width, tip_height, 1);
    }

    XtVaSetValues(tip_label, XmNlabelString, tip.xmstring(), NULL);

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
	    XtVaGetValues(parent, XmNorientation, &orientation, NULL);

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
	    XtVaGetValues(parent, XmNfractionBase, &fraction_base, NULL);
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
			      NULL);

		int left_position   = 0;
		int right_position  = 0;
		int top_position    = 0;
		int bottom_position = 0;

		XtVaGetValues(w, 
			      XmNleftPosition,   &left_position,
			      XmNrightPosition,  &right_position,
			      XmNtopPosition,    &top_position,
			      XmNbottomPosition, &bottom_position,
			      NULL);

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
	XtQueryGeometry(tip_shell, NULL, &tip_geometry);

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
		      NULL);
	
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

static void DoClearTip(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == clear_tip_timer);
    clear_tip_timer = 0;

    TipInfo& ti = *((TipInfo *)client_data);
    ClearTip(ti.widget, &ti.event);
}


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


// (Un)install toolbar tips for W
static void InstallButtonTipEvents(Widget w, bool install)
{
    // If neither `tipString' nor `documentationString' resource is
    // specified, don't install handler
    tip_resource_values tip_values;
    XtGetApplicationResources(w, &tip_values, 
			      tip_subresources, XtNumber(tip_subresources), 
			      NULL, 0);
    doc_resource_values doc_values;
    XtGetApplicationResources(w, &doc_values, 
			      doc_subresources, XtNumber(doc_subresources), 
			      NULL, 0);
    if (tip_values.tipString == 0 && doc_values.documentationString == 0)
	return;

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
		      NULL);

	if (children != 0)
	    for (int i = 0; i < int(num_children); i++)
		InstallButtonTipsNow(children[i], install);
    }

    if (XmIsCascadeButton(w))
    {
	// Traverse the menu associated with this button
	Widget subMenuId = 0;
	XtVaGetValues(w, XmNsubMenuId, &subMenuId, NULL);
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
		      NULL);
	if (rowColumnType == XmMENU_OPTION && subMenuId != 0)
	    InstallButtonTipsNow(subMenuId, install);
    }
}

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
