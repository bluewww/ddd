// $Id$ -*- C++ -*-
// Select from a list of choices presented by GDB

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char select_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "select.h"

#include "Delay.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "Command.h"
#include "ddd.h"
#include "editing.h"
#include "mydialogs.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>

Widget gdb_selection_dialog = 0;
static Widget gdb_selection_list_w = 0;

static void SelectCB(Widget, XtPointer client_data, XtPointer)
{
    string& reply = *((string *)client_data);

    IntArray numbers;
    getDisplayNumbers(gdb_selection_list_w, numbers);
    if (numbers.size() > 0)
	reply = itostring(numbers[0]) + "\n";
}

static void CancelCB(Widget, XtPointer client_data, XtPointer)
{
    string& reply = *((string *)client_data);
    reply = "\003";
}


// Answer GDB question
static void select_from_gdb(string& question, string& reply)
{
    int count       = question.freq('\n') + 1;
    string *choices = new string[count];
    bool *selected  = new bool[count];

    split(question, choices, count, '\n');

    // Highlight choice #1 by default
    for (int i = 0; i < count; i++)
    {
	if (!has_nr(choices[i]))
	{
	    // Choice has no number (prompt) - remove it
	    for (int j = i; j < count - 1; j++)
		choices[j] = choices[j + 1];
	    count--;
	    i--;
	}
	else
	{
	    selected[i] = (get_positive_nr(choices[i]) == 1);
	}
    }

    if (count < 2)
    {
	// Nothing to choose from
	if (count == 1)
	{
	    // Take the first choice.
	    reply = itostring(atoi(choices[0])) + "\n";
	}
	
	delete[] choices;
	delete[] selected;
	return;
    }

    // Popup selection dialog
    static string selection_reply;

    if (gdb_selection_dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;

	gdb_selection_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(gdb_w),
					   "gdb_selection_dialog", args, arg));
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
		      XmNokCallback, SelectCB, &selection_reply);
	XtAddCallback(gdb_selection_dialog,
		      XmNcancelCallback, CancelCB, &selection_reply);
	XtAddCallback(gdb_selection_dialog,
		      XmNhelpCallback, ImmediateHelpCB, 0);
    }

    setLabelList(gdb_selection_list_w, choices, selected, count, false, false);

    delete[] choices;
    delete[] selected;

    manage_and_raise(gdb_selection_dialog);

    selection_reply = "";
    while (selection_reply == "" 
	   && gdb->running() && !gdb->isReadyWithPrompt())
	XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), XtIMAll);

    // Found a reply - return
    reply = selection_reply;
}

void gdb_selectHP(Agent *, void *, void *call_data)
{
    ReplyRequiredInfo *info = (ReplyRequiredInfo *)call_data;

#if 0
    if (gdb_keyboard_command)
    {
	// Use the GDB console to answer this query
	info->reply = "";
	return;
    }
#endif

    // Fetch previous output lines, in case this is a multi-line message.
    String s = XmTextGetString(gdb_w);
    string prompt(s);
    XtFree(s);
    prompt = prompt.from(int(messagePosition)) + info->question;

    // Issue prompt right now
    _gdb_out(info->question);
    info->question = "";

    // Set and issue reply
    select_from_gdb(prompt, info->reply);
    _gdb_out(info->reply);
}
