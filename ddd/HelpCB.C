// $Id$  -*- C++ -*-
// Interactive Help Callbacks

// Copyright (C) 1995, 1996 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char HelpCB_rcsid[] =
    "$Id$";

#include "config.h"

#include "HelpCB.h"
#include "DestroyCB.h"
#include "findParent.h"
#include "longName.h"
#include "Agent.h"

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

#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

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

static Widget help_dialog = 0;
static Widget help_shell  = 0;
static Pixmap help_pixmap = 0;

static MString NoHelpText(Widget widget);
static MString NoTipText(Widget widget, XEvent *event);
static MString NoDocumentationText(Widget widget, XEvent *event);
static void _MStringHelpCB(Widget widget, 
			   XtPointer client_data, 
			   XtPointer call_data,
			   bool help_on_help = false,
			   Pixmap pixmap = 0);

MString helpOnVersionExtraText;

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

static MString get_tip_string(Widget widget, XEvent *event)
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

static MString get_documentation_string(Widget widget, XEvent *event)
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
}

void ImmediateHelpCB(Widget widget, XtPointer, XtPointer call_data)
{
    if (widget == 0)
	return;

    // Get help on this widget
    MString text = get_help_string(widget);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
}

void HelpOnThisCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (widget == 0)
	return;

    Widget w = (Widget)client_data;

    // Get help on this widget
    MString text = get_help_string(w);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
}

void HelpOnWindowCB(Widget widget, XtPointer, XtPointer call_data)
{
    // Get a shell window
    Widget shell = findTopLevelShellParent(widget);

    MString text = get_help_string(shell);
    MStringHelpCB(widget, XtPointer(text.xmstring()), call_data);
}

void HelpOnVersionCB(Widget widget, XtPointer, XtPointer call_data)
{
    // Get a shell window
    Widget shell = findTopLevelShellParent(widget);

    MString text = get_help_on_version_string(shell);
    text += helpOnVersionExtraText;

    // Get the pixmap
    Pixmap pixmap = 0;
    if (helpOnVersionPixmapProc)
	pixmap = helpOnVersionPixmapProc(widget);

    _MStringHelpCB(widget, XtPointer(text.xmstring()), call_data, 
		   false, pixmap);
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


Pixmap (*helpOnVersionPixmapProc)(Widget)    = 0;
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
			   bool help_on_help,
			   Pixmap pixmap)
{
    XmString text = (XmString)client_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNmessageString, text); arg++;

    Widget shell = findTopLevelShellParent(widget);
    if (shell == 0)
	shell = widget;

    if (help_dialog && (shell != help_shell || pixmap != help_pixmap))
    {
	DestroyWhenIdle(help_dialog);
	help_dialog = 0;
    }

    help_shell  = shell;
    help_pixmap = pixmap;

    if (help_dialog == 0)
    {
	// Build help_dialog
	XtSetArg(args[arg], XmNdeleteResponse, XmUNMAP); arg++;

	if (pixmap)
	{
	    XtSetArg(args[arg], XmNsymbolPixmap, pixmap); arg++;
	}

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
    XtSetSensitive(help_button, !help_on_help);

    // Popup help_dialog
    XtManageChild(help_dialog);
    raise_shell(help_dialog);
}


static void HelpIndexCB(Widget widget, XtPointer client_data, 
			XtPointer call_data)
{
    Delay delay;

    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    Widget help_man = Widget(client_data);
    int index = cbs->item_position;

    void *userData;
    XtVaGetValues(widget,
		  XmNuserData, &userData,
		  NULL);

    XmTextPosition *positions = (XmTextPosition *)userData;
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

static void FindCB(Widget w, XtPointer client_data, XtPointer call_data,
		   bool forward)
{
    Delay delay;

    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;

    FindInfo *fi = (FindInfo *)client_data;

    String key_s = XmTextFieldGetString(fi->key);
    string key(key_s);
    XtFree(key_s);

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
	XmTextSetSelection(fi->text,
			   next_occurrence,
			   next_occurrence + key.length(),
			   time(cbs->event));
	if (!forward)
	    XmTextSetInsertionPosition(fi->text, next_occurrence);
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

    void *userData;
    XtVaGetValues(list, XmNuserData, &userData, NULL);
    XmTextPosition *positions = (XmTextPosition *)userData;

    int pos = 0;
    while (positions[pos] <= cursor)
	pos++;

    ListSetAndSelectPos(list, pos);
}

// Return true iff TEXT contains a manual header line at pos
static bool has_header(const string& text, unsigned pos)
{
    if (pos < text.length() - 1
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

void ManualStringHelpCB(Widget widget, const MString& title,
			const string& unformatted_text)
{
    Delay delay;

    Arg args[10];
    Cardinal arg = 0;

    static Widget help_man     = 0;
    static Widget help_index   = 0;
    static Widget text_dialog  = 0;
    static Widget dialog_title = 0;

    if (help_man == 0)
    {
	// Build manual dialog
	Widget toplevel = findTheTopLevelShell(widget);
	if (toplevel == 0)
	    return;

	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmUNMAP); arg++;
	text_dialog = 
	    verify(XmCreatePromptDialog(toplevel, "manual_help", args, arg));
	Delay::register_shell(text_dialog);

	arg = 0;
	Widget form =
	    verify(XmCreateForm(text_dialog, "form", args, arg));

	arg = 0;
	XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
	dialog_title = verify(XmCreateLabel(form, "title", args, arg));
	XtManageChild(dialog_title);

	arg = 0;
 	XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_WIDGET);   arg++;
 	XtSetArg(args[arg], XmNtopWidget,        dialog_title);      arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
	Widget area = 
	    verify(XmCreatePanedWindow(form, "area", args, arg));
	XtManageChild(area);

	arg = 0;
	help_index = verify(XmCreateScrolledList(area, "index", args, arg));
	XtManageChild(help_index);
	
	arg = 0;
	XtSetArg(args[arg], XmNeditable, False);                    arg++;
	XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT);        arg++;
	XtSetArg(args[arg], XmNvalue,    unformatted_text.chars()); arg++;
	help_man = verify(XmCreateScrolledText(area, "text", args, arg));
	XtManageChild(help_man);

	arg = 0;
	Widget search = verify(XmCreateRowColumn(area, "search", args, arg));
	XtManageChild(search);

	Widget arg_label = create_arg_label(search);
	arg = 0;
	Widget key = verify(XmCreateTextField(search, "key", args, arg));
	XtManageChild(key);
	XtAddCallback(arg_label, XmNactivateCallback, 
		      ClearTextFieldCB, key);

	arg = 0;
	Widget findBackward = 
	    verify(XmCreatePushButton(search, "findBackward", args, arg));
	XtManageChild(findBackward);

	arg = 0;
	Widget findForward = 
	    verify(XmCreatePushButton(search, "findForward", args, arg));
	XtManageChild(findForward);

	XtWidgetGeometry size;
	size.request_mode = CWHeight;
	XtQueryGeometry(search, NULL, &size);
	unsigned char unit_type;
	XtVaGetValues(search, XmNunitType, &unit_type, NULL);
	int new_height = XmConvertUnits(search, XmVERTICAL, XmPIXELS, 
					size.height, unit_type);
	XtVaSetValues(search,
		      XmNpaneMaximum, new_height,
		      XmNpaneMinimum, new_height,
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

	XtAddCallback(key, XmNactivateCallback, ActivateCB, 
		      XtPointer(findForward));

	static FindInfo fi;
	fi.key  = key;
	fi.text = help_man;

	XtAddCallback(findForward, XmNactivateCallback, 
		      FindForwardCB, XtPointer(&fi));
	XtAddCallback(findBackward, XmNactivateCallback, 
		      FindBackwardCB, XtPointer(&fi));

	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_SELECTION_LABEL));

	XtVaSetValues(text_dialog, XmNdefaultButton, Widget(0), NULL);

	XtManageChild(form);
	InstallButtonTips(text_dialog);
    }

    // Set title
    if (!title.isNull())
	XtVaSetValues(dialog_title, XmNlabelString, title.xmstring(), NULL);

    string text(unformatted_text);
    bool manual = !text.contains("File:", 0);
    int i;

    if (manual)
    {
	// Manual page: strip manual headers and footers
	unsigned source = 0;
	int target = 0;

	for (;;)
	{
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

		source = text.index('\n', source);
		while (source < text.length() && text[source] == '\n')
		    source++;
	    }

	    if (source >= text.length())
		break;

	    text[target++] = text[source++];
	}
	text.from(target) = "";

	// Handle umlauts
	text.gsub("\"\ba", "\344");
	text.gsub("\"\bo", "\366");
	text.gsub("\"\bu", "\374");
	text.gsub("_\b\"\b_\ba", "_\b\344");
	text.gsub("_\b\"\b_\bo", "_\b\366");
	text.gsub("_\b\"\b_\bu", "_\b\374");
    }
    else
    {
	// Info file: strip menus
	unsigned source = 0;
	int target = 0;

	while (source < text.length())
	{
	    while (text.contains("* ", source)
		   && (source == 0 || text[source - 1] == '\n'))
	    {
		// Skip menu item
		while (text[source++] != '\n')
		    ;
	    }
	    text[target++] = text[source++];
	}
	text.from(target) = "";
    }

    {
	// Info and manual: kill multiple empty lines
	unsigned source = 0;
	int target = 0;
	while (source < text.length())
	{
	    if (text[source] == '\n')
	    {
		while (source < text.length() - 2
		       && text[source + 1] == '\n'
		       && text[source + 2] == '\n')
		    source++;
	    }
	    text[target++] = text[source++];
	}
	text.from(target) = "";
    }

    if (manual)
    {
	// Manual page: handle underlines

	int size = text.length();
	bool *underlined    = new bool[size];
	bool *doublestriked = new bool[size];
	for (i = 0; i < size; i++)
	    underlined[i] = doublestriked[i] = false;

	unsigned source = 0;
	int target = 0;

	while (source < text.length())
	{
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
	text.from(target) = "";

	// Set text
	XtVaSetValues(help_man, XmNvalue, text.chars(), NULL);

	// Set highlighting
	XmTextSetHighlight(help_man, 0, XmTextGetLastPosition(help_man), 
			   XmHIGHLIGHT_NORMAL);

	XmTextPosition underlining    = 0;
	XmTextPosition doublestriking = 0;
	for (i = 0; i < int(text.length()); i++)
	{
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

    // Find titles
    StringArray titles;
    IntArray positions;

    if (manual)
    {
	// Manual page
	int start_of_line = 0;

	for (unsigned source = 0; source < text.length(); source++)
	{
	    if (text[source] == '\n')
	    {
		if (source - start_of_line > 3)
		{
		    // Check manual title
		    bool is_title = false;
		
		    if (text(start_of_line, 2) == "  "
			&& text[start_of_line + 3] != ' ')
			is_title = true; // .SS title
		    else if (text[start_of_line] != ' '
			     && source - start_of_line < 60)
			is_title = true; // .SH title

		    if (is_title)
		    {
			titles += text(start_of_line,
						source - start_of_line);
			positions += start_of_line;
		    }
		}

		start_of_line = source + 1;
	    }
	}
    }
    else
    {
	// Info file
	int source = 0;
	while (source >= 0)
	{
	    assert(text.contains("File: ", source));

	    // Fetch title below `File: ' line
	    int start_of_title = text.index("\n\n", source) + 2;
	    int end_of_title   = text.index("\n", start_of_title);
	    string title = text(start_of_title, end_of_title - start_of_title);

	    // Fetch indent level, using the underline characters
	    int start_of_underline = text.index('\n', start_of_title) + 1;
	    static string underlines = "*=-.";
	    int indent = underlines.index(text[start_of_underline]);
	    if (indent < 0)
		indent = underlines.length();
	    if (indent == 0)
		title.upcase();

	    // Add title and position
	    titles += replicate(' ', indent * 2) + title;
	    positions += source;

	    // Strip `File: ' line
	    text.del(source, start_of_title - source);
	    
	    // Find next `File: ' line
	    source = text.index("File: ", source);
	}

	// Set text
	XtVaSetValues(help_man, XmNvalue, text.chars(), NULL);

	// Info: no highlighting
	XmTextSetHighlight(help_man, 0, XmTextGetLastPosition(help_man), 
			   XmHIGHLIGHT_NORMAL);
    }

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

    // Enable Text Window
    XtManageChild(text_dialog);
    raise_shell(text_dialog);
}


void TextHelpCB(Widget widget, XtPointer client_data, XtPointer)
{
    Delay delay;

    String text = (String)client_data;

    Arg args[10];
    Cardinal arg = 0;

    static Widget help_text   = 0;
    static Widget text_dialog = 0;

    if (help_text == 0)
    {
	// Build help_text
	Widget toplevel = findTheTopLevelShell(widget);
	if (toplevel == 0)
	    return;

	arg = 0;
	XtSetArg(args[arg], XmNdeleteResponse, XmUNMAP); arg++;
	text_dialog = 
	    verify(XmCreatePromptDialog(toplevel, "text_help", args, arg));
	Delay::register_shell(text_dialog);

	arg = 0;
	Widget form =
	    verify(XmCreateForm(text_dialog, "form", args, arg));

	arg = 0;
	XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
	Widget title = verify(XmCreateLabel(form, "title", args, arg));
	XtManageChild(title);

	arg = 0;
 	XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_WIDGET);   arg++;
 	XtSetArg(args[arg], XmNtopWidget,        title);             arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
	XtSetArg(args[arg], XmNeditable,         False);             arg++;
	XtSetArg(args[arg], XmNeditMode,         XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNvalue,            text); arg++;
	help_text = 
	    verify(XmCreateScrolledText(form, "text", args, arg));
	XtManageChild(help_text);

	XtAddCallback(text_dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));

	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_SELECTION_LABEL));

	XtManageChild(form);
	InstallButtonTips(text_dialog);
    }

    // Setup text for existing dialog
    arg = 0;
    XtSetArg(args[arg], XmNvalue, (String)text); arg++;
    XtSetValues(help_text, args, arg);

    // Enable Text Window
    XtManageChild(text_dialog);
    raise_shell(text_dialog);
}


void HelpOnContextCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Widget toplevel = findTheTopLevelShell(widget);
    if (toplevel == 0)
	return;

    Cursor cursor = XCreateFontCursor(XtDisplay(toplevel), XC_question_arrow);
    Widget item = XmTrackingLocate(toplevel, cursor, False);

    if (item)
	ImmediateHelpCB(item, client_data, call_data);
    else
	ImmediateHelpCB(toplevel, client_data, call_data);

    // Some Motif versions get confused if this function is invoked
    // via a menu accelerator; the keyboard remains grabbed. Hence, we
    // ungrab it explicitly.
    XtUngrabKeyboard(toplevel, XtLastTimestampProcessed(XtDisplay(widget)));
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
static void CancelTimeOut(Widget, XtPointer client_data, XtPointer)
{
    XtIntervalId timer = XtIntervalId(client_data);
    XtRemoveTimeOut(timer);
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
    XtRemoveCallback(w, XmNdestroyCallback, 
		     CancelTimeOut, XtPointer(*timer));

    MString tip = get_tip_string(w, &ti->event);
    if (tip.isNull() || isNone(tip) || tip.isEmpty())
	return;

    if (tip_shell == 0)
    {
	Arg args[10];
	int arg;

	arg = 0;
	XtSetArg(args[arg], XmNallowShellResize, true);             arg++;
	XtSetArg(args[arg], XmNx, WidthOfScreen(XtScreen(w)) + 1);  arg++;
	XtSetArg(args[arg], XmNy, HeightOfScreen(XtScreen(w)) + 1); arg++;
	tip_shell = verify(XtCreateWidget("tipShell",
					  overrideShellWidgetClass, 
					  findTheTopLevelShell(w), args, arg));

	arg = 0;
	XtSetArg(args[arg], XmNlabelString, tip.xmstring());        arg++;
	XtSetArg(args[arg], XmNrecomputeSize, true);                arg++;
	XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING);   arg++;
	tip_label = XmCreateLabel(tip_shell, "tipLabel", args, arg);
	XtManageChild(tip_label);

	// Simple hack to ensure shell is realized
	XtPopup(tip_shell, XtGrabNone);
	XtPopdown(tip_shell);
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
    XtRemoveCallback(ti->widget, XmNdestroyCallback, 
		     CancelTimeOut, XtPointer(*timer));

    if (DisplayDocumentation != 0 
	&& (XmIsText(ti->widget) ? text_docs_enabled : button_docs_enabled))
    {
	// Display documentation
	MString doc = get_documentation_string(ti->widget, &ti->event);
	DisplayDocumentation(doc);
    }
}

// Clear the documentation
static void ClearDocumentation(XtPointer client_data, XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == clear_doc_timer);
    clear_doc_timer = 0;

    TipInfo *ti = (TipInfo *)client_data;

    if (DisplayDocumentation != 0 
	&& (XmIsText(ti->widget) ? text_docs_enabled : button_docs_enabled))
    {
	// Clear documentation
	static MString empty(0, true);
	DisplayDocumentation(empty);
    }
}

// Clear tips and documentation
static void ClearTip(Widget w, XEvent *event)
{
    if (raise_tip_timer)
    {
	XtRemoveTimeOut(raise_tip_timer);
	raise_tip_timer = 0;
    }

    if (tip_popped_up)
    {
	XtPopdown(tip_shell);
	tip_popped_up = false;
    }

    if (raise_doc_timer)
    {
	XtRemoveTimeOut(raise_doc_timer);
	raise_doc_timer = 0;
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
	// want some flashing documentation string.

	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	clear_doc_timer =
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    help_clear_doc_delay, 
			    ClearDocumentation, XtPointer(&ti));
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

	raise_doc_timer =
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    doc_delay,
			    ShowDocumentation, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelTimeOut, 
		      XtPointer(raise_doc_timer));
    }

    if (XmIsText(w) ? text_tips_enabled : button_tips_enabled)
    {
	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	int tip_delay = 
	    XmIsText(w) ? help_value_tip_delay : help_button_tip_delay;

	raise_tip_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    tip_delay,
			    PopupTip, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelTimeOut, 
		      XtPointer(raise_tip_timer));
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
#if 0
	    if (event->xcrossing.mode != NotifyNormal)
	    {
		// Ignore grab/ungrab event
		break;
	    }
#endif

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
#if 0
		    clog << "Wow!  Left and entered " << XtName(w) << " within " 
			 << help_clear_tip_delay << "ms!\n";
#endif

		    last_left_widget = 0;
		    break;
		}
	    }

#if 0
	    // Enabling this would inhibit tips on pull-down menus
	    const int any_button_mask = Button1Mask | Button2Mask 
	        | Button3Mask | Button4Mask | Button5Mask;
	    if (event->xcrossing.state & any_button_mask)
	    {
		// Some button is pressed - clear tip
		ClearTip(w, event);
	    }
	    else 
#endif
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
#if 0
	    if (event->xcrossing.mode != NotifyNormal)
	    {
		// Ignore grab/ungrab event
		break;
	    }
#endif

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

    case ButtonPress:
    case ButtonRelease:
	ClearTip(w, event);
	break;

    case MotionNotify:
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

#if 0
    clog << (install ? "Installing" : "Uninstalling")
	 << " button tip handler for " << cook(longName(w)) << "\n";
#endif

    EventMask event_mask = 
	EnterWindowMask | LeaveWindowMask | ButtonPress | ButtonRelease;
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
		     CancelTimeOut, XtPointer(*timer));
    InstallButtonTipsNow(w, true);
}

// Callback funtion: uninstall tips now.
static void UnInstallButtonTipsTimeOut(XtPointer client_data, 
				       XtIntervalId *timer)
{
    Widget w = Widget(client_data);
    XtRemoveCallback(w, XmNdestroyCallback, 
		     CancelTimeOut, XtPointer(*timer));
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
    XtAddCallback(w, XmNdestroyCallback, CancelTimeOut, XtPointer(timer));
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
#if 0
    clog << (install ? "Installing" : "Uninstalling")
	 << " button tip handler for " << cook(longName(w)) << "\n";
#endif

    EventMask event_mask = EnterWindowMask | LeaveWindowMask 
	| ButtonPress | ButtonRelease | PointerMotionMask;

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
