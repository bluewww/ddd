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

static Widget settings_form   = 0;

static void add_settings(int& row, string command = "set");

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

// ToggleButton reply
static void SetBoolCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	SetOptionCB(w, client_data, call_data);
}

// Find widget for command COMMAND
static Widget command_to_widget(Widget ref, string command)
{
    Widget found = 0;
    while (command != "" && (found = XtNameToWidget(ref, command)) == 0)
    {
	// Strip last word (command argument)
	int index = command.index(rxwhite, -1);
	command   = command.before(index);
    }

    return found;
}
	

// Process output of `show' command
void process_show(string command, string value)
{
    if (settings_form == 0)
	return;

    if (value.contains(".\n", -1))
    {
	value = value.after(" is ", -1);
	value = value.before(".\n");
    }
    if (value.contains("\"auto;", 0))
	value = "auto";
    if (value.contains('"', 0))
	value = unquote(value);

    string set_command = command;
    if (!set_command.contains("set ", 0))
	set_command = string("set ") + set_command.after(rxwhite);

    Widget button = command_to_widget(settings_form, set_command);

    if (button != 0 && XtIsSubclass(button, xmRowColumnWidgetClass))
    {
	Widget menu = 0;
	XtVaGetValues(button, XmNsubMenuId, &menu, NULL);
	if (menu != 0)
	{
	    // Option menu
	    Widget active = XtNameToWidget(menu, value);
	    if (active != 0)
	    {
		XtVaSetValues(button, XmNmenuHistory, active, NULL);
		return;
	    }
	}

	Widget on  = XtNameToWidget(button, "on");
	Widget off = XtNameToWidget(button, "off");
	if (on != 0 && off != 0)
	{
	    if (value == "on")
	    {
		XmToggleButtonSetState(on,  True,  False);
		XmToggleButtonSetState(off, False, False);
		return;
	    }
	    else if (value == "off")
	    {
		XmToggleButtonSetState(on,  False, False);
		XmToggleButtonSetState(off, True,  False);
		return;
	    }
	}
    }
    else if (button != 0 && XtIsSubclass(button, xmTextFieldWidgetClass))
    {
	XtVaSetValues(button, 
		      XmNvalue,                 value.chars(),
		      XmNcursorPosition,        0,
		      XmNcursorPositionVisible, True,
		      NULL);
	return;
    }

    cerr << "Warning: cannot " << set_command 
	 << " to " << quote(value) << "\n";
}


// Add single button
static void add_button(string line, int& row)
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
	add_settings(row, set_command);
	return;
    }

    string value = gdb_question(show_command);
    if (value.freq('\n') > 1)
    {
	// Generic command - list `set' subcommands
	add_settings(row, set_command);
	return;
    }

    if (!value.contains(".\n", -1))
	return;

    doc = doc.after("Set ");
    if (doc.contains("whether to ", 0))
	doc = doc.after("whether to ");
    if (doc.contains("the ", 0))
	doc = doc.after("the ");
    doc[0] = toupper(doc[0]);

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
    Widget label = verify(XmCreateLabel(settings_form, base, args, arg));
    XtManageChild(label);

    // Add help button
    arg = 0;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);      arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNtopPosition,      row);                arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);            arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_CENTER); arg++;
    Widget help = verify(XmCreatePushButton(settings_form, "help", args, arg));
    XtManageChild(help);

    // Add entry
    Widget entry;
    String set_command_s = new char[set_command.length() + 1];
    strcpy(set_command_s, set_command);

    if (base.contains("check", 0))
    {
	// `set check'
	arg = 0;
	Widget menu = 
	    verify(XmCreatePulldownMenu(settings_form, "menu", args, arg));

	// Possible options are contained in the help string
	string options = gdb_question("help " + set_command);
	options = options.from('(');
	options = options.before(')');

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
	}

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	entry = 
	    verify(XmCreateOptionMenu(settings_form, set_command, args, arg));
	XtManageChild(entry);
    }
    else if (base.contains("language", 0) || base.contains("demangle", 0))
    {
	// `set language / set demangle'
	arg = 0;
	Widget menu = 
	    verify(XmCreatePulldownMenu(settings_form, "menu", args, arg));

	// Possible options are listed upon `set language' without value
	string options = gdb_question("set " + base);

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
	    }
	}

	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	entry = 
	    verify(XmCreateOptionMenu(settings_form, set_command, args, arg));
	XtManageChild(entry);
    }
    else if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
    {
	// Boolean value
	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	XtSetArg(args[arg], XmNorientation,      XmHORIZONTAL);      arg++;
	entry = 
	    verify(XmCreateRadioBox(settings_form, set_command, args, arg));
	XtManageChild(entry);

	arg = 0;
	Widget on = verify(XmCreateToggleButton(entry, "on", args, arg));
	XtManageChild(on);

	arg = 0;
	Widget off = verify(XmCreateToggleButton(entry, "off", args, arg));
	XtManageChild(off);

	XtAddCallback(on, XmNvalueChangedCallback,
		      SetBoolCB, XtPointer(set_command_s));
	XtAddCallback(off, XmNvalueChangedCallback,
		      SetBoolCB, XtPointer(set_command_s));
    }
    else
    {
	// Some other value
	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	entry = 
	    verify(XmCreateTextField(settings_form, set_command, args, arg));
	XtManageChild(entry);
	XtAddCallback(entry, XmNactivateCallback, 
		      SetTextCB, XtPointer(set_command_s));
    }

    // Initialize button
    process_show(show_command, value);

    XtAddCallback(help, XmNactivateCallback, 
		  HelpOnThisCB, XtPointer(entry));

    row++;
}

// Add buttons
static void add_settings(int& row, string command = "set")
{
    string commands = gdb_question("help " + command);

    while (commands != "")
    {
	string line = commands.before('\n');
	commands = commands.after('\n');
	add_button(line, row);
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
	settings_form = verify(XmCreateForm(scroll, "form", args, arg));

	// Add setting buttons to the button box.
	int row = 0;
	add_settings(row);
	XtVaSetValues(settings_form, XmNfractionBase, row, NULL);
	XtManageChild(settings_form);
	XtManageChild(scroll);
    }

    XtManageChild(settings_dialog);
    raise_shell(settings_dialog);
}
