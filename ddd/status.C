// $Id$ -*- C++ -*-
// Interpret GDB output

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char status_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "status.h"

#include "Delay.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "MString.h"
#include "commandQ.h"
#include "ddd.h"
#include "mydialogs.h"
#include "post.h"
#include "string-fun.h"
#include "verify.h"

#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>


//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// GDB selection dialog
static Widget gdb_selection_dialog;
static Widget gdb_selection_list_w;

// True if last cmd came from GDB window
bool gdb_keyboard_command = false;

// True if the next line is to be displayed in the status line
bool show_next_line_in_status = false;

//-----------------------------------------------------------------------------
// Prompt recognition
//-----------------------------------------------------------------------------

void set_buttons_from_gdb(Widget buttons, string& text)
{
    bool yn = text.contains("(y or n) ", -1) 
	|| text.contains("(yes or no) ", -1)
	|| (gdb->type() == XDB && text.contains("? ", -1));

    if (yn && !gdb_keyboard_command)
    {
	// Fetch previous output lines, in case this is a multi-line message.
	String s = XmTextGetString(gdb_w);
	string prompt(s);
	XtFree(s);

	char prompt_start = (gdb->type() == XDB ? '>' : '(');

	int pos = prompt.index(prompt_start, -1);
	if (pos >= 0)
	    pos = prompt.index('\n', pos) + 1;
	if (pos == 0)
	    pos = messagePosition;

	XmTextReplace(gdb_w, pos, XmTextGetLastPosition(gdb_w), "");

	prompt = prompt.from(pos);
	if (text.contains('('))
	    prompt += text.before('(', -1); // Don't repeat `(y or n)'
	else
	    prompt += text;

	post_gdb_yn(prompt);
	text = "";
	return;
    }

    if (buttons == 0)
	return;

    static bool last_yn = false;
    if (yn == last_yn)
	return;

    last_yn = yn;

    XtSetSensitive(buttons, false);

    WidgetList children;
    Cardinal num_children;

    XtVaGetValues(buttons,
		  XmNchildren, &children,
		  XmNnumChildren, &num_children,
		  NULL);

    int i;
    for (i = 0; i < int(num_children); i++)
	XtManageChild(children[i]);
    for (i = 0; i < int(num_children); i++)
    {
	
	Widget w = children[i];
	string name = XtName(w);

	if (yn == (name == "Yes" || name == "No"))
	    XtManageChild(w);
	else
	    XtUnmanageChild(w);
    }

    XtSetSensitive(buttons, true);
}

void SelectCB(Widget dialog, XtPointer, XtPointer)
{
    int *numbers = getDisplayNumbers(gdb_selection_list_w);
    string choice = itostring(numbers[0]);
    delete[] numbers;

    // clog << quote(choice) << "\n";

    _gdb_command(choice, dialog);
}

void set_selection_from_gdb(string& text)
{
    if (gdb_keyboard_command)
	return;

    int last_gt = text.index("\n> ", -1);
    if (last_gt < 0)
	return;

    // Fetch previous output lines, in case this is a multi-line message.
    String s = XmTextGetString(gdb_w);
    string prompt(s);
    XtFree(s);

    prompt = prompt.from(int(messagePosition)) + text.before(last_gt);

    XmTextReplace(gdb_w, messagePosition, 
		  XmTextGetLastPosition(gdb_w), "");
    text = "";

    if (gdb_selection_dialog == 0)
    {
	gdb_selection_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(gdb_w),
					   "gdb_selection_dialog", NULL, 0));
	Delay::register_shell(gdb_selection_dialog);

	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_APPLY_BUTTON));

	gdb_selection_list_w = XmSelectionBoxGetChild(gdb_selection_dialog, 
						      XmDIALOG_LIST);
	XtVaSetValues(gdb_selection_list_w,
		      XmNselectionPolicy, XmSINGLE_SELECT,
		      NULL);
	XtAddCallback(gdb_selection_dialog,
		      XmNokCallback, SelectCB, 0);
	XtAddCallback(gdb_selection_dialog,
		      XmNcancelCallback, gdbCommandCB, XtPointer("\003"));
	XtAddCallback(gdb_selection_dialog,
		      XmNhelpCallback, ImmediateHelpCB, 0);
    }

    int count       = prompt.freq('\n') + 1;
    string *choices = new string[count];
    bool *selected  = new bool[count];

    split(prompt, choices, count, '\n');

    for (int i = 0; i < count; i++)
	selected[i] = (get_positive_nr(choices[i]) == 1);

    setLabelList(gdb_selection_list_w, choices, selected, count);

    delete[] choices;
    delete[] selected;

    XtManageChild(gdb_selection_dialog);
}

//-----------------------------------------------------------------------------
// Status recognition
//-----------------------------------------------------------------------------

void set_status_from_gdb(const string& text)
{
    if (private_gdb_input)
	return;

    if (!show_next_line_in_status 
	&& (gdb->type() == XDB || !text.contains(") ", -1))
	&& (gdb->type() != XDB || !text.contains(">", -1)))
	return;

    // Fetch line before prompt in GDB window
    String s = XmTextGetString(gdb_w);
    string message = s + messagePosition;
    XtFree(s);

    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status && 
	(message == "" || message[message.length() - 1] != '\n'))
	return;

    // Skip prompt and uncomplete lines
    int idx = message.index('\n', -1);
    if (idx >= 0)
	message = message.before(idx);

    strip_final_newlines(message);
    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status)
    {
	messagePosition = XmTextGetLastPosition(gdb_w) + text.length();
	show_next_line_in_status = false;
	message.gsub('\n', ' ');
    }
    else
    {
	// Show first line only
	while (message != "" && message[0] == '\n')
	    message = message.after('\n');
	if (message.contains('\n'))
	    message = message.before('\n');
    }

    strip_final_newlines(message);
    if (message == "")
	return;

    set_status(message);
}

void set_status(const string& message)
{
    if (status_w == 0)
	return;

    string m = message;
    if (m != "" && !m.contains("=") && isascii(m[0]) && islower(m[0]))
	m[0] = toupper(m[0]);

    MString msg(m, "rm");
    XtVaSetValues(status_w,
		  XmNlabelString, msg.xmstring(),
		  NULL);
    XFlush(XtDisplay(status_w));
    XmUpdateDisplay(status_w);
}
