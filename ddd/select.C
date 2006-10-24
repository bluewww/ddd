// $Id$ -*- C++ -*-
// Select from a list of choices presented by GDB

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char select_rcsid[] = 
    "$Id$";

#include "select.h"

#include "Delay.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "Command.h"
#include "ddd.h"
#include "editing.h"
#include "file.h"
#include "mydialogs.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#ifdef IF_MOTIF
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#endif // IF_MOTIF

DIALOG_P gdb_selection_dialog = 0;
static TREEVIEW_P gdb_selection_list_w = 0;

static void SelectCB(CB_ALIST_2(XtP(string *) client_data))
{
    string& reply = *((string *)client_data);

    IntArray numbers;
    getItemNumbers(gdb_selection_list_w, numbers);
    if (numbers.size() > 0)
	reply = itostring(numbers[0]) + "\n";
}

static void CancelCB(CB_ALIST_2(XtP(string *) client_data))
{
    string& reply = *((string *)client_data);
    reply = "\003";
}


// Answer GDB question
static void select_from_gdb(const string& question, string& reply)
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
	    reply = itostring(atoi(choices[0].chars())) + "\n";
	}
	
	delete[] choices;
	delete[] selected;
	return;
    }

    // Popup selection dialog
    static string selection_reply;

    if (gdb_selection_dialog == 0)
    {
#ifdef IF_MOTIF
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;

	gdb_selection_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(gdb_w),
					   XMST("gdb_selection_dialog"),
					   args, arg));
#else // NOT IF_MOTIF
	gdb_selection_dialog = 
	    new Gtk::Dialog(XMST("gdb_selection_dialog"), *find_shell(gdb_w));
#endif // IF_MOTIF
	Delay::register_shell(gdb_selection_dialog);

#ifdef IF_MOTIF
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog,
					       XmDIALOG_TEXT));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(gdb_selection_dialog, 
					       XmDIALOG_APPLY_BUTTON));
#endif // IF_MOTIF

#ifdef IF_MOTIF
	gdb_selection_list_w = XmSelectionBoxGetChild(gdb_selection_dialog, 
						      XmDIALOG_LIST);
#else // NOT IF_MOTIF
	gdb_selection_list_w = new Gtk::TreeView();
	gdb_selection_dialog->get_vbox()->pack_start(*gdb_selection_list_w, Gtk::PACK_EXPAND_WIDGET);
	gdb_selection_list_w->show();
#endif // IF_MOTIF
#ifdef IF_MOTIF
	XtVaSetValues(gdb_selection_list_w,
		      XmNselectionPolicy, XmSINGLE_SELECT,
		      XtPointer(0));
#else // NOT IF_MOTIF
	gdb_selection_list_w->get_selection()->set_mode(Gtk::SELECTION_SINGLE);
#endif // IF_MOTIF

#ifdef IF_MOTIF
	XtAddCallback(gdb_selection_dialog,
		      XmNokCallback, SelectCB, &selection_reply);
	XtAddCallback(gdb_selection_dialog,
		      XmNcancelCallback, CancelCB, &selection_reply);
	XtAddCallback(gdb_selection_dialog,
		      XmNhelpCallback, ImmediateHelpCB, 0);
#else // NOT IF_MOTIF
	Gtk::Button *button;
	button = gdb_selection_dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(SelectCB), &selection_reply));
	button = gdb_selection_dialog->add_button(XMST("Cancel"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(CancelCB), &selection_reply));
#endif // IF_MOTIF
    }

    setLabelList(gdb_selection_list_w, choices, selected, count, false, false);

    delete[] choices;
    delete[] selected;

    manage_and_raise(gdb_selection_dialog);

    selection_reply = "";
    while (selection_reply.empty() 
	   && gdb->running() && !gdb->isReadyWithPrompt()) {
#ifdef IF_MOTIF
	XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), XtIMAll);
#else // NOT IF_MOTIF
	Glib::MainContext::get_default()->iteration(false);
#endif // IF_MOTIF
    }

    // Found a reply - return
    reply = selection_reply;
}

// Select a file
static void select_file(const string& /* question */, string& reply)
{
    gdbOpenFileCB(CB_ARGS_1(find_shell()));

    open_file_reply = "";
    while (open_file_reply.empty() 
	   && gdb->running() && !gdb->isReadyWithPrompt()) {
#ifdef IF_MOTIF
	XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), XtIMAll);
#else // NOT IF_MOTIF
	Glib::MainContext::get_default()->iteration(false);
#endif // IF_MOTIF
    }

    // Found a reply - return
    reply = open_file_reply + "\n";
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
#ifdef IF_MOTIF
    String s = XmTextGetString(gdb_w);
    string prompt(s);
    XtFree(s);
    prompt = prompt.from(int(messagePosition)) + info->question;
#else // NOT IF_MOTIF
    string prompt = string(gdb_w->get_text(messagePosition, -1).c_str()) + info->question;
#endif // IF_MOTIF

    // Issue prompt right now
    _gdb_out(info->question);
    info->question = "";

    // Set and issue reply
    if (prompt.contains("file name"))
    {
	// File selection
	select_file(prompt, info->reply);
    }
    else
    {
	// Option selection
	select_from_gdb(prompt, info->reply);
    }

    _gdb_out(info->reply);
}
