// $Id$  -*- C++ -*-
// Nora Help Callback

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
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/List.h>
#include <Xm/Text.h>

#include <X11/cursorfont.h>
#include <X11/StringDefs.h>

#include "strclass.h"
#include "cook.h"
#include "verify.h"
#include "Delay.h"
#include "StringA.h"
#include "IntArray.h"

struct resource_values {
    XmString helpString;
    Boolean showTitle;
};

static XtResource subresources[] = {
    {"contextHelpString",    "ContextHelpString", 
     XmRXmString, sizeof(XmString),
     XtOffsetOf(resource_values, helpString), XtRImmediate, XtPointer(0) },
    {"contextHelpShowTitle", "ContextHelpShowTitle", 
     XmRBoolean, sizeof(Boolean),
     XtOffsetOf(resource_values, showTitle), XtRImmediate, XtPointer(False) },
};

static Widget help_dialog = 0;
static Widget help_shell  = 0;
static Widget text_dialog = 0;
static Pixmap help_pixmap = 0;

static MString _DefaultHelpText(Widget widget);
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


// Get the top-level shell window
static MString _DefaultHelpText(Widget widget)
{
    MString text = "No help available for \"";
    text += cook(longName(widget));
    text += "\"";

    return text;
}

MString (*DefaultHelpText)(Widget widget) = _DefaultHelpText;
Pixmap (*helpOnVersionPixmapProc)(Widget) = 0;

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

    static Widget help_man   = 0;
    static Widget help_index = 0;

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
}


void FileHelpCB(Widget widget, XtPointer client_data, XtPointer)
{
    Delay delay;

    String filename = (String)client_data;

    char buffer[BUFSIZ];
    String text = (String)XtMalloc(1);
    text[0] = '\0';

    FILE *fp = fopen(filename, "r");
    if (fp == 0)
    {
	perror(filename);
	return;
    }

    while (!feof(fp))
    {
	int nitems = fread(buffer, sizeof(char), BUFSIZ, fp);
	buffer[nitems] = '\0';
	text = (String)XtRealloc(text, strlen(text) + strlen(buffer) + 1);
	strcat(text, buffer);
    }

    if (fclose(fp) == EOF)
    {
	perror(filename);
	XtFree(text);
	return;
    }

    Arg args[10];
    Cardinal arg = 0;

    static Widget help_text = 0;
    if (help_text == 0)
    {
	// Build help_text
	Widget toplevel = findTheTopLevelShell(widget);
	if (toplevel == 0)
	{
	    XtFree(text);
	    return;
	}
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
}


void CommandHelpCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    Delay d;

    string tmpfile = tmpnam(NULL);
    string command = 
	string("/bin/sh -c '( ") + (String)client_data + " ) > " 
	+ tmpfile + " 2>&1'";

    Agent agent(command);
    agent.start();
    agent.wait();

    FileHelpCB(widget, (String)tmpfile, call_data);
    unlink(tmpfile);
}


void HelpManualCB(Widget widget, XtPointer, XtPointer call_data)
{
    Widget toplevel = widget;
    while (XtParent(toplevel))
	toplevel = XtParent(toplevel);

    string app_name = XtName(toplevel);

    static string command;
    command = "man " + app_name;

    char buffer[BUFSIZ];
    String text = (String)XtMalloc(1);
    text[0] = '\0';

    Agent agent(command);
    agent.start();

    FILE *fp = agent.inputfp();
    if (fp == 0)
	return;

    while (!feof(fp))
    {
	int nitems = fread(buffer, sizeof(char), BUFSIZ, fp);
	buffer[nitems] = '\0';
	text = (String)XtRealloc(text, strlen(text) + strlen(buffer) + 1);
	strcat(text, buffer);
    }

    ManualStringHelpCB(widget, (String)text, call_data);
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
