// $Id$ -*- C++ -*-
// Modify debugger settings

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

const char settings_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "settings.h"

#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <ctype.h>

#include "Delay.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "commandQ.h"
#include "cook.h"
#include "ddd.h"
#include "question.h"
#include "status.h"
#include "verify.h"
#include "wm.h"

static void add_settings(Widget form, int& row, string command = "set");

// ToggleButton reply
static void SetBoolCB(Widget, XtPointer client_data, XtPointer call_data)
{
    String set_command = (String)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    String value = cbs->set ? "on" : "off";
    gdb_command(string(set_command) + " " + value);
}

// TextField reply
static void SetTextCB(Widget w, XtPointer client_data, XtPointer)
{
    String set_command = (String)client_data;
    String value_s = XmTextFieldGetString(w);
    string value(value_s);
    XtFree(value_s);

    if (value == "unlimited")
	value = "0";

    gdb_command(string(set_command) + " " + value);
}

// OptionMenu reply
static void SetOptionCB(Widget w, XtPointer client_data, XtPointer)
{
    String set_command = (String)client_data;
    gdb_command(string(set_command) + " " + XtName(w));
}


// Add single button
static void add_button(Widget form, string line, int& row)
{
    if (!line.contains("set ", 0))
	return;			// No `set' command

    string set_command  = line.before(" -- ");
    string doc          = line.after(" -- ");
    string base         = set_command.after("set ");
    string show_command = "show " + base;

    if (!doc.contains("Set ", 0))
    {
	// Generic command or `set variable' - list `set' subcommands
	add_settings(form, row, set_command);
	return;
    }

    string value = gdb_question(show_command);
    if (value.freq('\n') > 1)
    {
	// Generic command - list `set' subcommands
	add_settings(form, row, set_command);
	return;
    }

    value = value.after(" is ", -1);
    value = value.before(".\n");

    doc = doc.after("Set ");
    if (doc.contains("whether to ", 0))
	doc = doc.after("whether to ");
    if (doc.contains("the ", 0))
	doc = doc.after("the ");
    doc[0] = toupper(doc[0]);

    if (value.contains("\"auto;", 0))
	value = "\"auto\"";

    if (value == "")
	return;

    Arg args[10];
    int arg;

    // Add label
    MString labelString(doc);
    arg = 0;
    XtSetArg(args[arg], XmNlabelString,      labelString.xmstring()); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);          arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);      arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);                arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_BEGINNING);  arg++;
    Widget label = verify(XmCreateLabel(form, base, args, arg));
    XtManageChild(label);

    // Add help button
    arg = 0;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);      arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNtopPosition,      row);                arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);            arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_CENTER); arg++;
    Widget help = verify(XmCreatePushButton(form, "help", args, arg));
    XtManageChild(help);

    // Add entry
    Widget entry;
    String set_command_s = new char[set_command.length() + 1];
    strcpy(set_command_s, set_command);

    if (base.contains("check", 0))
    {
	// `set check'
	if (value.contains('"', 0))
	    value = unquote(value);

	arg = 0;
	Widget menu = 
	    verify(XmCreatePulldownMenu(form, set_command, args, arg));

	// Possible options are contained in the help string
	string options = gdb_question("help " + set_command);
	options = options.from('(');
	options = options.before(')');

	Widget active = 0;
	while (options != "")
	{
	    string option = options.after(0);
	    option = option.through(rxalpha);
	    options = options.after(rxalpha);

	    arg = 0;
	    Widget button = 
		verify(XmCreatePushButton(menu, option, args, arg));
	    XtManageChild(button);
	    XtAddCallback(button, XmNactivateCallback, SetOptionCB, 
			  set_command_s);

	    if (active == 0 || option == value)
		active = button;
	}

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	XtSetArg(args[arg], XmNmenuHistory,      active);            arg++;
	entry = verify(XmCreateOptionMenu(form, set_command, args, arg));
	XtManageChild(entry);
    }
    else if (base.contains("language", 0) || base.contains("demangle", 0))
    {
	// `set language / set demangle'
	if (value.contains('"', 0))
	    value = unquote(value);

	arg = 0;
	Widget menu = 
	    verify(XmCreatePulldownMenu(form, set_command, args, arg));

	// Possible options are listed upon `set language' without value
	string options = gdb_question("set " + base);

	Widget active = 0;
	while (options != "")
	{
	    string option = options.before('\n');
	    options = options.after('\n');

	    if (option.contains("  "))
	    {
		string label = option.after("  ");
		label = label.after(rxwhite);
		if (label.contains(" based on"))
		    label = label.before(" based on");
		if (label.contains("Use the "))
		    label = label.after("Use the ");
		if (label.contains(" demangling"))
		    label = label.before(" demangling");
		if (label.contains(" language"))
		    label = label.before(" language");

		if (option.contains(" auto"))
		    option = "auto";
		else
		    option = option.before(rxwhite);

		MString xmlabel(label);
		arg = 0;
		XtSetArg(args[arg], XmNlabelString, xmlabel.xmstring()); arg++;
		Widget button = 
		    verify(XmCreatePushButton(menu, option, args, arg));
		XtManageChild(button);
		XtAddCallback(button, XmNactivateCallback, SetOptionCB, 
			      set_command_s);

		if (active == 0 || option == value)
		    active = button;
	    }
	}

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	XtSetArg(args[arg], XmNmenuHistory,      active);            arg++;
	entry = verify(XmCreateOptionMenu(form, set_command, args, arg));
	XtManageChild(entry);
    }
    else if (value == "on" || value == "off")
    {
	// Boolean value
	bool set = (value == "on");

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNorientation,      XmHORIZONTAL);      arg++;
	entry = verify(XmCreateRadioBox(form, set_command, args, arg));
	XtManageChild(entry);

	arg = 0;
	XtSetArg(args[arg], XmNset, set); arg++;
	Widget on = verify(XmCreateToggleButton(entry, "on", args, arg));
	XtManageChild(on);

	arg = 0;
	XtSetArg(args[arg], XmNset, !set); arg++;
	Widget off = verify(XmCreateToggleButton(entry, "off", args, arg));
	XtManageChild(off);

	XtAddCallback(on, XmNvalueChangedCallback,
		      SetBoolCB, XtPointer(set_command_s));
    }
    else
    {
	// Some other value
	if (value.contains('"', 0))
	    value = unquote(value);

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNvalue,            value.chars());     arg++;
	entry = verify(XmCreateTextField(form, set_command, args, arg));
	XtManageChild(entry);
	XtAddCallback(entry, XmNactivateCallback, 
		      SetTextCB, XtPointer(set_command_s));
    }

    XtAddCallback(help, XmNactivateCallback, 
		  HelpOnThisCB, XtPointer(entry));

    row++;
}

// Add buttons
static void add_settings(Widget form, int& row, string command = "set")
{
    string commands = gdb_question("help " + command);

    while (commands != "")
    {
	string line = commands.before('\n');
	commands = commands.after('\n');
	add_button(form, line, row);
    }
}


// Popup editor for debugger settings
void dddPopupSettingsCB (Widget, XtPointer, XtPointer)
{
    static Widget settings_dialog = 0;

    if (settings_dialog == 0)
    {
	StatusDelay delay("Retrieving debugger settings");

	Arg args[10];
	int arg;

	arg = 0;
	settings_dialog = 
	    verify(XmCreatePromptDialog(find_shell(), "settings", args, arg));
	Delay::register_shell(settings_dialog);

	// Remove old prompt and cancel button
	Widget text = 
	    XmSelectionBoxGetChild(settings_dialog, XmDIALOG_TEXT);
	XtUnmanageChild(text);
	Widget old_label = 
	    XmSelectionBoxGetChild(settings_dialog, XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(old_label);
	Widget cancel = 
	    XmSelectionBoxGetChild(settings_dialog, XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild(cancel);
	XtAddCallback(settings_dialog, XmNhelpCallback, ImmediateHelpCB, 0);
	XtAddCallback(settings_dialog, XmNokCallback, UnmanageThisCB, 
		      XtPointer(settings_dialog));
	XtVaSetValues(settings_dialog, XmNdefaultButton, Widget(0), NULL);

	// Add a scrolled window...
	arg = 0;
	Widget scroll = verify(XmCreateScrolledWindow(settings_dialog, 
						      "scroll", args, arg));

	// ...and a form.
	arg = 0;
	Widget form = verify(XmCreateForm(scroll, "form", args, arg));

	// Add setting buttons to the button box.
	int row = 0;
	add_settings(form, row);
	XtVaSetValues(form, XmNfractionBase, row, NULL);
	XtManageChild(form);
	XtManageChild(scroll);
    }

    XtManageChild(settings_dialog);
    raise_shell(settings_dialog);
}
