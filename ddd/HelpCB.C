// $Id$  -*- C++ -*-
// Interactive Help Callbacks

// Copyright (C) 1995, 1996 Technische Universitaet Braunschweig, Germany.
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

#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Form.h>

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

// The help system supports three resources:
// helpString          - displayed in context-sensitive help.
// tipString           - displayed in small windows when entering buttons
// documentationString - displayed in the status line
struct resource_values {
    XmString helpString;
    XmString tipString;
    XmString documentationString;
    Boolean showTitle;
};

static XtResource subresources[] = {
    {
	XtNhelpString,
	XtCHelpString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(resource_values, helpString),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XtNtipString,
	XtCTipString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(resource_values, tipString), 
	XtRImmediate,
	XtPointer(0)
    },
    
    {
	XtNdocumentationString,
	XtCDocumentationString,
	XmRXmString,
	sizeof(XmString),
	XtOffsetOf(resource_values, documentationString), 
	XtRImmediate,
	XtPointer(0)
    },

    {
	XtNhelpShowTitle,
	XtCHelpShowTitle,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(resource_values, showTitle),
	XtRImmediate,
	XtPointer(False)
    }
};

static Widget help_dialog = 0;
static Widget help_shell  = 0;
static Pixmap help_pixmap = 0;

static MString _DefaultHelpText(Widget widget);
static MString _DefaultTipText(Widget widget, XEvent *event);
static MString _DefaultDocumentationText(Widget widget, XEvent *event);
static void _MStringHelpCB(Widget widget, 
			   XtPointer client_data, 
			   XtPointer call_data,
			   bool help_on_help = false,
			   Pixmap pixmap = 0);

MString helpOnVersionExtraText;

static MString get_help_string(Widget widget)
{
    // Get text
    resource_values values;
    XtGetApplicationResources(widget, &values, 
			      subresources, XtNumber(subresources), 
			      NULL, 0);

    MString text(values.helpString, true);
    if ((text.xmstring() == 0 || text.isEmpty()) && DefaultHelpText != 0)
	text = DefaultHelpText(widget);
    if (text.xmstring() == 0 || text.isEmpty())
	text = _DefaultHelpText(widget);

    if (values.showTitle)
	text = MString("Help for ") + cook(longName(widget)) 
	    + ":\n\n" + text;

    return text;
}


static MString get_tip_string(Widget widget, XEvent *event)
{
    if (XmIsText(widget))
    {
	if (DefaultTipText != 0)
	    return DefaultTipText(widget, event);
	return _DefaultTipText(widget, event);
    }

    // Get text
    resource_values values;
    XtGetApplicationResources(widget, &values, 
			      subresources, XtNumber(subresources), 
			      NULL, 0);

    MString text(values.tipString, true);
    if (text.xmstring() == 0)
	return _DefaultTipText(widget, event);

    if (text.isEmpty())
    {
	if (DefaultTipText != 0)
	    return DefaultTipText(widget, event);
	return _DefaultTipText(widget, event);
    }

    return text;
}

static MString get_documentation_string(Widget widget, XEvent *event)
{
    if (XmIsText(widget))
    {
	if (DefaultDocumentationText != 0)
	    return DefaultDocumentationText(widget, event);
	return _DefaultDocumentationText(widget, event);
    }

    // Get text
    resource_values values;
    XtGetApplicationResources(widget, &values, 
			      subresources, XtNumber(subresources), 
			      NULL, 0);

    MString text(values.documentationString, true);
    if (text.xmstring() == 0)
    {
	return get_tip_string(widget, event);
    }
    else if (text.isEmpty())
    {
	if (DefaultDocumentationText != 0)
	    return DefaultDocumentationText(widget, event);
	else
	    return _DefaultDocumentationText(widget, event);
    }
    else
    {
	return text;
    }
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
    // Get the top-level shell window
    Widget shell = widget;
    while (XtParent(shell))
	shell = XtParent(shell);

    MString text = get_help_string(shell);
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
static MString _DefaultHelpText(Widget widget)
{
    MString text = "No help available for \"";
    text += cook(longName(widget));
    text += "\"";

    return text;
}

static MString _DefaultTipText(Widget, XEvent *)
{
    return MString(0, true);	// empty string
}

static MString _DefaultDocumentationText(Widget, XEvent *)
{
    return MString(0, true);	// empty string
}

static XmTextPosition _TextPosOfEvent(Widget, XEvent *)
{
    return XmTextPosition(-1);
}

MString (*DefaultHelpText)(Widget)
    = _DefaultHelpText;
MString (*DefaultTipText)(Widget, XEvent *) 
    = _DefaultTipText;
MString (*DefaultDocumentationText)(Widget, XEvent *) 
    = _DefaultDocumentationText;
XmTextPosition (*TextPosOfEvent)(Widget widget, XEvent *event)
    = _TextPosOfEvent;


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


// Note: this assumes `groff -Tascii -man' format.
void ManualStringHelpCB(Widget widget, XtPointer client_data, 
			XtPointer)
{
    Delay delay;

    String text = (String)client_data;

    Arg args[10];
    Cardinal arg = 0;

    static Widget help_man    = 0;
    static Widget help_index  = 0;
    static Widget text_dialog = 0;

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
	Widget form = verify(XmCreateWorkArea(text_dialog, "area", args, arg));

	arg = 0;
	help_index = verify(XmCreateScrolledList(form, "index", args, arg));
	XtManageChild(help_index);
	
	arg = 0;
	XtSetArg(args[arg], XmNeditable, False); arg++;
	XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNvalue, text); arg++;
	help_man = verify(XmCreateScrolledText(form, "text", args, arg));
	XtManageChild(help_man);

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

	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_SELECTION_LABEL));
	XtManageChild(form);
    }

    string stripped_text(text);


    // Strip manual headers and footers
    unsigned source = 0;
    int target = 0;
    int line = 0;

    while (source < stripped_text.length())
    {
	char c = stripped_text[source++];
	if ((line % 66 + 1) >= 7 && (line % 66 + 1) <= 60)
	    stripped_text[target++] = c;

	if (c == '\n')
	    line++;
    }
    stripped_text.from(target) = "";


    // Handle underlines via selections
    int size = stripped_text.length();
    bool *underlined    = new bool[size];
    bool *doublestriked = new bool[size];
    int i;
    for (i = 0; i < size; i++)
	underlined[i] = doublestriked[i] = false;

    source = 0;
    target = 0;

    while (source < stripped_text.length())
    {
	char c = stripped_text[target++] = stripped_text[source++];

	if (c == '\b' && target >= 2)
	{
	    target -= 2;
	    if (stripped_text[target] == '_')
		underlined[target] = true;
	    if (stripped_text[target] == stripped_text[source])
		doublestriked[target] = true;
	}
    }
    stripped_text.from(target) = "";

    // Find titles
    StringArray titles;
    IntArray positions;

    int start_of_line = 0;

    for (source = 0; source < stripped_text.length(); source++)
    {
	if (stripped_text[source] == '\n')
	{
	    if (source - start_of_line > 3)
	    {
		bool is_title = false;
		
		if (stripped_text(start_of_line, 2) == "  "
		    && stripped_text[start_of_line + 3] != ' ')
		    is_title = true; // .SS title
		else if (stripped_text[start_of_line] != ' '
			 && source - start_of_line < 60)
		    is_title = true; // .SH title

		if (is_title)
		{
		    titles += stripped_text(start_of_line,
					    source - start_of_line);
		    positions += start_of_line;
		}
	    }

	    start_of_line = source + 1;
	}
    }

    // Set titles in selection list
    XmTextPosition *xmpositions = new XmTextPosition[titles.size()];
    for (i = 0; i < titles.size(); i++)
	xmpositions[i] = positions[i];

    XmStringTable xmtitles = new XmString[titles.size()];

    for (i = 0; i < titles.size(); i++)
	xmtitles[i] = XmStringCreateLtoR(titles[i], 
					 titles[i][0] == ' ' ? "rm" : "bf");

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
    

    // Setup text for existing dialog
    arg = 0;
    XtSetArg(args[arg], XmNvalue, (String)stripped_text); arg++;
    XtSetValues(help_man, args, arg);

    // Highlight underlined and doublestriked text
    XmTextSetHighlight(help_man, 0, stripped_text.length() - 1, 
		       XmHIGHLIGHT_NORMAL);

    XmTextPosition underlining    = 0;
    XmTextPosition doublestriking = 0;
    for (i = 0; i < int(stripped_text.length()); i++)
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
	XtSetArg(args[arg], XmNeditable, False); arg++;
	XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
	XtSetArg(args[arg], XmNvalue, text); arg++;
	help_text = 
	    verify(XmCreateScrolledText(text_dialog, "text", args, arg));
	XtManageChild(help_text);

	XtAddCallback(text_dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));

	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_CANCEL_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(text_dialog,
					       XmDIALOG_SELECTION_LABEL));
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
static XtIntervalId pending_tip_timer = 0;

// The timer used for the delay until the documentation is shown.
static XtIntervalId pending_doc_timer = 0;

// The timer used for the delay until the documentation is cleared.
static XtIntervalId pending_clr_timer = 0;

// Delay times (in ms)
int help_button_tip_delay = 750;  // Delay before raising button tip
int help_value_tip_delay  = 750;  // Delay before raising value tip
int help_button_doc_delay = 0;    // Delay before showing button doc
int help_value_doc_delay  = 0;    // Delay before showing value doc
int help_clear_doc_delay  = 1000; // Delay before clearing doc


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
    assert(*timer == pending_tip_timer);
    pending_tip_timer = 0;

    TipInfo *ti = (TipInfo *)client_data;
    Widget& w = ti->widget;
    XtRemoveCallback(w, XmNdestroyCallback, 
		     CancelTimeOut, XtPointer(*timer));

    MString tip = get_tip_string(w, &ti->event);
    if (tip.xmstring() == 0 || tip.isEmpty())
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
	tip_label = XmCreateLabel(tip_shell, "tipLabel", args, arg);
	XtManageChild(tip_label);

	// Simple hack to ensure shell is realized
	XtPopup(tip_shell, XtGrabNone);
	XtPopdown(tip_shell);
    }

    XtVaSetValues(tip_label,
		  XmNlabelString, tip.xmstring(),
		  NULL);


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
    assert(*timer == pending_doc_timer);
    pending_doc_timer = 0;

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
static void ClearDocumentation(XtPointer* client_data, 
			       XtIntervalId *timer)
{
    (void) timer;
    assert(*timer == pending_clr_timer);
    pending_clr_timer = 0;

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
    (void) w;

    if (pending_tip_timer)
    {
	XtRemoveTimeOut(pending_tip_timer);
	pending_tip_timer = 0;
    }

    if (tip_popped_up)
    {
	XtPopdown(tip_shell);
	tip_popped_up = false;
    }

    if (pending_doc_timer)
    {
	XtRemoveTimeOut(pending_doc_timer);
	pending_doc_timer = 0;
    }

    if (pending_clr_timer)
    {
	XtRemoveTimeOut(pending_clr_timer);
	pending_clr_timer = 0;
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

	pending_clr_timer =
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
	if (pending_clr_timer)
	{
	    XtRemoveTimeOut(pending_clr_timer);
	    pending_clr_timer = 0;
	}

	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	int doc_delay = 
	    XmIsText(w) ? help_value_doc_delay : help_button_doc_delay;

	pending_doc_timer =
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    doc_delay,
			    ShowDocumentation, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelTimeOut, 
		      XtPointer(pending_doc_timer));
    }

    if (XmIsText(w) ? text_tips_enabled : button_tips_enabled)
    {
	static TipInfo ti;
	ti.event  = *event;
	ti.widget = w;

	int tip_delay = 
	    XmIsText(w) ? help_value_tip_delay : help_button_tip_delay;

	pending_tip_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			    tip_delay,
			    PopupTip, XtPointer(&ti));

	// Should W be destroyed beforehand, cancel timeout
	XtAddCallback(w, XmNdestroyCallback, CancelTimeOut, 
		      XtPointer(pending_tip_timer));
    }
}

static int tips_ignore_leave = 0;

// Ignore next N enter/leave events.  This is required for temporary
// pointer grabs.
void TipsIgnoreLeave(int n)
{
    if (tip_popped_up || pending_tip_timer || pending_doc_timer)
	tips_ignore_leave = n;
}

// Widget W has been entered or left.  Handle event.
static void HandleTipEvent(Widget w,
			   XtPointer /* client_data */,
			   XEvent *event, 
			   Boolean * /* continue_to_dispatch */)
{
    switch (event->type)
    {
    case EnterNotify:
	if (!XmIsText(w))
	{
	    ClearTip(w, event);
	    RaiseTip(w, event);
	}
	break;

    case LeaveNotify:
	if (tips_ignore_leave > 0)
	{
	    tips_ignore_leave--;
	    break;
	}

	// FALL THROUGH
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
    resource_values values;
    XtGetApplicationResources(w, &values, 
			      subresources, XtNumber(subresources), 
			      NULL, 0);
    if (values.tipString == 0 && values.documentationString == 0)
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
