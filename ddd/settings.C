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

char settings_rcsid[] = 
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
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <ctype.h>
#include <string.h>

#include "Assoc.h"
#include "AppData.h"
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
#include "VarArray.h"
#include "StringSA.h"
#include "SourceView.h"

static Widget settings_form  = 0;
static Widget reset_settings = 0;
static VarArray<Widget> entries;
static Assoc<Widget, string> values;
static Assoc<Widget, string> initial_values;

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

// Issue `set' command
static void gdb_set_command(string set_command, string value)
{
    if (value == "unlimited")
	value = "0";

    if (set_command == "dir" && value != "")
    {
	// `dir' in GDB works somewhat special: it prepends its
	// argument to the source path instead of simply setting it.
	// Hence, first reset `dir' to some initial value.

	string confirm_value = "on";
	Widget confirm_w = command_to_widget(settings_form, "set confirm");
	if (confirm_w)
	    confirm_value = values[confirm_w];

	if (confirm_value == "on")
	    gdb_command("set confirm off");
	gdb_command(set_command);
	if (confirm_value == "on")
	    gdb_command("set confirm on");
    }

    if (value != "")
	gdb_command(set_command + " " + value);
    else
	gdb_command(set_command);
}

// TextField reply
static void SetTextCB(Widget w, XtPointer client_data, XtPointer)
{
    String value_s = XmTextFieldGetString(w);
    string value(value_s);
    XtFree(value_s);

    gdb_set_command((String)client_data, value);
}

// OptionMenu reply
static void SetOptionCB(Widget w, XtPointer client_data, XtPointer)
{
    gdb_set_command((String)client_data, XtName(w));
}

// ToggleButton reply
static void SetBoolCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "on");
    else
	gdb_set_command((String)client_data, "off");
}

// ToggleButton reply
static void SetNumCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "1");
    else
	gdb_set_command((String)client_data, "0");
}
	
// Update state of `reset' button
static void update_reset_settings()
{
    for (int i = 0; i < entries.size(); i++)
    {
	Widget entry = entries[i];
	if (initial_values[entry] != values[entry])
	{
	    XtSetSensitive(reset_settings, True);
	    return;
	}
    }

    XtSetSensitive(reset_settings, False);
}

// Process output of `show' command
void process_show(string command, string value, bool init)
{
    if (settings_form == 0)
	return;

    if (value.contains('\n', -1))
	value = value.before('\n');

    if (!init)
	set_status(value);
	
    if (value.contains(".", -1))
    {
	value = value.after(" is ", -1);
	value = value.before(int(value.length()) - 1);
    }

    if (value.contains("\"auto;", 0))
	value = "auto";
    if (value.contains('"', 0))
	value = unquote(value);
    else if (value.contains(": "))
	value = value.after(": ");

    static string empty;
    value.gsub(gdb_out_ignore, empty);

    string set_command = command;
    if (!set_command.contains("set ", 0))
	set_command = "set " + set_command.after(rxwhite);

    Widget button = command_to_widget(settings_form, set_command);
    if (button == 0)
	button = command_to_widget(settings_form, command);
    if (button == 0)
	button = command_to_widget(settings_form, command.after(rxwhite));

    if (button != 0)
    {
	values[button] = value;
	if (init)
	    initial_values[button] = value;
    }

    if (!init)
	update_reset_settings();

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
    }
    else if (button != 0 && XtIsSubclass(button, xmToggleButtonWidgetClass))
    {
	bool set = value != "off" && value != "0" && value != "unlimited";
	XtVaSetValues(button, XmNset, set, NULL);
	return;
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

    cerr << "Warning: cannot set " << quote(set_command)
	 << " to " << quote(value) << "\n";
}



// Create settings form

enum EntryType
{
    BoolToggleButtonEntry,	// Create on/off toggle button
    NumToggleButtonEntry,	// Create 1/0 toggle button
    CheckOptionMenuEntry,       // Create `check' option menu
    OtherOptionMenuEntry,       // Create other option menu
    TextFieldEntry		// Create text field
};

// Return TRUE if doc begins with a boolean verb
static bool is_verb(const string& doc)
{
    if (doc.contains("whether to"))
	return true;
    int ing = doc.index("ing");
    if (ing < 0)
	return false;
    int first_space  = doc.index(' ');
    int second_space = doc.index(' ', first_space + 1);
    if (ing < first_space || ing < second_space || second_space < 0)
	return true;

    return false;
}

// Determine entry type
static EntryType entry_type(const string& base, 
			    const string& doc,
			    const string& value)
{
    if (base.contains("check", 0))
	return CheckOptionMenuEntry;
    else if (base.contains("language", 0) || base.contains("demangle", 0))
	return OtherOptionMenuEntry;
    else if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
	return BoolToggleButtonEntry;
    else if ((value.contains("0.\n", -1) || value.contains("1.\n", -1))
	     && (is_verb(doc)))
	return NumToggleButtonEntry;
    else
	return TextFieldEntry;
}

static StringStringAssoc gdb_question_cache;

static string cached_gdb_question(const string& question)
{
    string& answer = gdb_question_cache[question];
    if (answer == "")
	answer = gdb_question(question);
    return answer;
}

static void clear_gdb_question_cache()
{
    static StringStringAssoc empty;
    gdb_question_cache = empty;
}

static void add_settings(int& row, EntryType entry_filter,
			 string command = "set");

// Add single button
static void add_button(int& row, EntryType entry_filter, string line)
{
    if (!line.contains(" -- "))
	return;			// No help line

    string set_command  = line.before(" -- ");
    string doc          = line.after(" -- ");
    string base         = set_command.after("set ");
    if (base == "")
	base = set_command;
    string show_command = "show " + base;

    if (base == "args")
	return;			// Already handled in `Run...' editor

    if (base == "radix")
	return;			// Already handled in input- and output-radix

    bool is_set = doc.contains("Set ", 0);
    bool is_add = doc.contains("Add ", 0);

    if (!is_set && !is_add)
    {
	// Generic command or `set variable' - list `set' subcommands
	add_settings(row, entry_filter, set_command);
	return;
    }

    string value = cached_gdb_question(show_command);
    if (is_set && value.freq('\n') > 1)
    {
	// Generic command - list `set' subcommands
	add_settings(row, entry_filter, set_command);
	return;
    }

    if (is_set && !value.contains(".\n", -1))
	return;

    if (is_set)
	doc = doc.after("Set ");
    else if (is_add)
	doc = doc.after("Add ");

    EntryType e_type = entry_type(base, doc, value);
    if (e_type != entry_filter)
	return;

    if (is_add && doc.contains("of "))
	doc = doc.after("of ");
    if (doc.contains("whether to ", 0))
	doc = doc.after("whether to ");
    if (doc.contains("the ", 0))
	doc = doc.after("the ");
    doc[0] = toupper(doc[0]);

    Arg args[10];
    int arg;

    // Add label
    Widget label = 0;
    Widget entry = 0;
    String set_command_s = new char[set_command.length() + 1];
    strcpy(set_command_s, set_command);

    MString labelString(doc);
    arg = 0;
    XtSetArg(args[arg], XmNlabelString,      labelString.xmstring()); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);          arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);      arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);                arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_BEGINNING);  arg++;
    if (e_type != BoolToggleButtonEntry && e_type != NumToggleButtonEntry)
    {
	if (is_set)
	    label = verify(XmCreateLabel(settings_form, base, args, arg));
	else
	    label = verify(XmCreateLabel(settings_form, "the" + base, 
					 args, arg));

	XtManageChild(label);
    }
    else
    {
	entry = label = 
	    verify(XmCreateToggleButton(settings_form, set_command, 
					args, arg));
	XtManageChild(label);

	if (e_type == BoolToggleButtonEntry)
	    XtAddCallback(entry, XmNvalueChangedCallback,
			  SetBoolCB, XtPointer(set_command_s));
	else
	    XtAddCallback(entry, XmNvalueChangedCallback,
			  SetNumCB, XtPointer(set_command_s));
    }

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
    switch (e_type)
    {
    case BoolToggleButtonEntry:
    case NumToggleButtonEntry:
	// All is done
	break;

    case CheckOptionMenuEntry:
	{
	    // `set check'
	    arg = 0;
	    Widget menu = 
		verify(XmCreatePulldownMenu(settings_form, "menu", args, arg));

	    // Possible options are contained in the help string
	    string options = cached_gdb_question("help " + set_command);
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

	    MString empty;
	    arg = 0;
	    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	    XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	    XtSetArg(args[arg], XmNlabelString,      empty.xmstring());  arg++;
	    XtSetArg(args[arg], XmNmarginWidth,      0);                 arg++;
	    XtSetArg(args[arg], XmNmarginHeight,     0);                 arg++;
	    XtSetArg(args[arg], XmNspacing,          0);                 arg++;
	    XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	    entry = verify(XmCreateOptionMenu(settings_form, 
					      set_command, args, arg));
	    XtManageChild(entry);
	}
	break;

    case OtherOptionMenuEntry:
	{
	    // `set language / set demangle'
	    arg = 0;
	    Widget menu = 
		verify(XmCreatePulldownMenu(settings_form, "menu", args, arg));

	    // Possible options are listed upon `set language' without value
	    string options = cached_gdb_question("set " + base);

	    while (options != "")
	    {
		string option = options.before('\n');
		options = options.after('\n');

		if (option.contains("  "))
		{
		    string label = option.after("  ");
		    label = label.after(rxwhite);

		    if (option.contains(" auto"))
			option = "auto";
		    else
			option = option.before(rxwhite);

		    MString xmlabel(label);
		    arg = 0;
		    XtSetArg(args[arg], XmNlabelString, 
			     xmlabel.xmstring()); arg++;
		    Widget button = 
			verify(XmCreatePushButton(menu, option, args, arg));
		    XtManageChild(button);
		    XtAddCallback(button, XmNactivateCallback, SetOptionCB, 
				  set_command_s);
		}
	    }

	    MString empty;
	    arg = 0;
	    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	    XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	    XtSetArg(args[arg], XmNlabelString,      empty.xmstring());  arg++;
	    XtSetArg(args[arg], XmNmarginWidth,      0);                 arg++;
	    XtSetArg(args[arg], XmNmarginHeight,     0);                 arg++;
	    XtSetArg(args[arg], XmNspacing,          0);                 arg++;
	    XtSetArg(args[arg], XmNsubMenuId,        menu);              arg++;
	    entry = 
		verify(XmCreateOptionMenu(settings_form, 
					  set_command, args, arg));
	    XtManageChild(entry);
	}
	break;

    case TextFieldEntry:
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

    Widget rightmost = help;
    if (entry != label)
	rightmost = entry;

    // Add leader
    arg = 0;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNleftWidget,       label);             arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNrightWidget,      rightmost);         arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
    Widget leader = 
	verify(XmCreateSeparator(settings_form, "leader", args, arg));
    XtManageChild(leader);

    // Make entry insensitive, if part of initialization commands.
    string init = app_data.gdb_initial_cmds;
    int idx = init.index(set_command);
    if (idx == 0 || idx > 0 && init[idx - 1] == '\n')
    {
	XtSetSensitive(entry,  False);
	XtSetSensitive(label,  False);
	XtSetSensitive(leader, False);
    }

    // Initialize button
    process_show(show_command, value, true);

    XtAddCallback(help, XmNactivateCallback, 
		  HelpOnThisCB, XtPointer(entry));
    entries += entry;

    row++;
}

// Add buttons
static void add_settings(int& row, EntryType entry_filter, string command)
{
    string commands = cached_gdb_question("help " + command);

    while (commands != "")
    {
	string line = commands.before('\n');
	commands = commands.after('\n');
	add_button(row, entry_filter, line);
    }
}

// Add separator
static void add_separator(int& row)
{
    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
    Widget sep = verify(XmCreateSeparator(settings_form, "sep", args, arg));
    XtManageChild(sep);
    row++;
}

// Reset settings
static void ResetSettingsCB (Widget, XtPointer, XtPointer)
{
    for (int i = 0; i < entries.size(); i++)
    {
	Widget entry = entries[i];
	if (initial_values[entry] != values[entry])
	    gdb_set_command(XtName(entry), initial_values[entry]);
    }
}

// Fetch help for specific COMMAND
static string get_help_line(string command)
{
    string reply = cached_gdb_question("help " + command);
    reply = reply.before('\n');
    if (reply.contains('.'))
	reply = reply.before('.');
    reply = command + " -- " + reply;
    return reply;
}

// Create settings editor
static Widget create_gdb_settings()
{
    static Widget settings = 0;
    if (settings != 0 || !gdb->isReadyWithPrompt() || gdb->type() != GDB)
	return settings;

    StatusDelay delay("Retrieving debugger settings");

    Arg args[10];
    int arg;

    arg = 0;
    settings = verify(XmCreatePromptDialog(find_shell(), "settings", 
    				       args, arg));
    Delay::register_shell(settings);

    // Remove old prompt and cancel button
    XtUnmanageChild(XmSelectionBoxGetChild(settings, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(settings, 
    				       XmDIALOG_SELECTION_LABEL));
    XtAddCallback(settings, XmNhelpCallback, ImmediateHelpCB, 0);
    XtAddCallback(settings, XmNokCallback, UnmanageThisCB, 
    	      XtPointer(settings));

    reset_settings = 
        XmSelectionBoxGetChild(settings, XmDIALOG_CANCEL_BUTTON);
    XtRemoveAllCallbacks(reset_settings, XmNactivateCallback);
    XtAddCallback(reset_settings, XmNactivateCallback, ResetSettingsCB, 0);
    XtVaSetValues(settings, XmNdefaultButton, Widget(0), NULL);

    // Add a rowcolumn widget
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNspacing,      0); arg++;
    Widget column =
        verify(XmCreateRowColumn(settings, "column", args, arg));
    XtManageChild(column);

    // Add a label
    arg = 0;
    Widget title =
        verify(XmCreateLabel(column, "title", args, arg));
    XtManageChild(title);

    // Add a scrolled window...
    arg = 0;
    Widget scroll = 
        verify(XmCreateScrolledWindow(column, "scroll", args, arg));

    // ...and a form.
    arg = 0;
    settings_form = verify(XmCreateForm(scroll, "form", args, arg));

    // Add setting buttons to the button box.
    int row = 0;
    add_settings(row, BoolToggleButtonEntry);
    // add_separator(row);
    add_settings(row, NumToggleButtonEntry);
    add_separator(row);
    add_settings(row, OtherOptionMenuEntry);
    add_separator(row);
    add_settings(row, CheckOptionMenuEntry);
    add_separator(row);
    add_button(row, TextFieldEntry, get_help_line("dir"));
    add_button(row, TextFieldEntry, get_help_line("path"));
    add_separator(row);
    add_settings(row, TextFieldEntry);
    clear_gdb_question_cache();
    update_reset_settings();

    XtVaSetValues(settings_form, XmNfractionBase, row, NULL);
    XtManageChild(settings_form);
    XtManageChild(scroll);

    return settings;
}

// Popup editor for debugger settings
void dddPopupSettingsCB (Widget, XtPointer, XtPointer)
{
    Widget settings = create_gdb_settings();
    if (settings == 0)
	return;

    XtManageChild(settings);
    raise_shell(settings);
}

// Fetch GDB settings string
string get_gdb_settings()
{
    Widget settings = create_gdb_settings();
    if (settings == 0)
	return "";

    string command = "";
    for (int i = 0; i < entries.size(); i++)
    {
	Widget entry = entries[i];
	string value = values[entry];
	if (value == "unlimited")
	    value = "0";

	string base = XtName(entry);
	if (base == "set remotelogfile" && value == "")
	{
	    // This is the default setting - do nothing
	}
	else if (base == "set remotedevice" && value == "")
	{
	    // This is the default setting - do nothing
	}
	else if (base.contains("set ", 0))
	{
	    // Add setting.
	    command += base + ' ' + value + '\n';
	}
	else
	{
	    // `dir' and `path' values are not saved, since they are
	    // dependent on the current machine and the current
	    // executable.
	}
    }

    return command;
}
