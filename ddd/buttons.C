// $Id$ -*- C++ -*-
// DDD buttons

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

char buttons_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "buttons.h"

#include "HelpCB.h"
#include "MString.h"
#include "bool.h"
#include "ctrl.h"
#include "ddd.h"
#include "editing.h"
#include "question.h"
#include "source.h"
#include "status.h"
#include "string-fun.h"
#include "disp-read.h"
#include "verify.h"
#include "GDBAgent.h"
#include "StringSA.h"

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------

static void YnButtonCB(Widget dialog, 
		       XtPointer client_data, 
		       XtPointer call_data)
{
    _gdb_out(string((char *)client_data) + '\n');
    gdbCommandCB(dialog, client_data, call_data);
    gdb_keyboard_command = true;
}


//-----------------------------------------------------------------------------
// Show documentation string in status line
//-----------------------------------------------------------------------------

static void showDocumentationInStatusLine(const MString& doc)
{
    static MString current_status_message(0, true);
    static MString saved_status_message(0, true);
    static bool doc_shown_in_status = false;

    if (doc.xmstring() == 0 || doc.isEmpty())
    {
	// Button has been left - restore previous message unless overwritten
	if (current_status_message.xmstring() != 0)
	{
	    // Button has been left
	    if (current_status_message == current_status())
	    {
		// Restore previous message
		set_status_mstring(saved_status_message);
	    }
	    else
	    {
		// Message has been overwritten.  This is a button effect,
		// so clear the message.
		set_status("");
	    }
	    current_status_message = MString(0, true);
	}

	doc_shown_in_status = false;
    }
    else
    {
	// Button has been entered - save old message
	if (!doc_shown_in_status)
	{
	    saved_status_message = current_status();
	    doc_shown_in_status = true;
	}

	set_status_mstring(doc);
	current_status_message = doc;
    }
}


//-----------------------------------------------------------------------------
// Default help texts (especially buttons)
//-----------------------------------------------------------------------------

static string gdbHelpName(Widget widget)
{
    string name = XtName(widget);
    name.gsub('_', ' ');
    strip_final_blanks(name);

    return name;
}

static string gdbHelp(string command)
{
    if (gdb->type() == XDB)
    {
	// XDB is too dumb to find out aliases
	if (command == "run")
	    command = "r";
	else if (command == "cont")
	    command = "c";
	else if (command == "next")
	    command = "S";
	else if (command == "step")
	    command = "s";
	else if (command == "quit")
	    command = "q";
    }

    string help = NO_GDB_ANSWER;

    if (is_graph_cmd(command))
    {
	// Use own help texts
	string cmd = command.after("graph ") + " dummy";
	if (is_refresh_cmd(cmd))
	    help = "Refresh data window.";
	else if (is_display_cmd(cmd))
	    help = "Display expression in the data window.";
    }

    static StringStringAssoc help_texts;
    if (help == NO_GDB_ANSWER)
    {
	// Lookup cache
	if (help_texts.has(command))
	    help = help_texts[command];
    }

    if (help == NO_GDB_ANSWER)
    {
	// Ask debugger for help
	help = gdb_question("help " + command, 0, true);
	strip_final_blanks(help);
    }

    if (help != NO_GDB_ANSWER)
	help_texts[command] = help;

    return help;
}

MString gdbDefaultHelp(Widget widget)
{
    string name = gdbHelpName(widget);
    string help = gdbHelp(name);
    if (help == NO_GDB_ANSWER)
    {
	help = "No help available now.\n"
	    "Please try again when the debugger is ready.";
    }

    return MString(name + "\n", "bf") +	MString(help, "rm");
}

MString gdbDefaultTip(Widget widget)
{
    string help = gdbHelp(gdbHelpName(widget));
    if (help == NO_GDB_ANSWER)
	return MString(0, true);

    if (help.contains('\n'))
	help = help.before('\n');
    if (help.contains('.'))
	help = help.before('.');

    static regex RXuppercase("[A-Z]");
    help = help.from(RXuppercase);

    return MString(help, "rm");
}

Widget make_buttons(Widget parent, const string& name, 
		    const string& button_list)
{
    if (button_list == "")
	return 0;

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
    Widget buttons = verify(XmCreateWorkArea(parent, name, args, arg));
    if (buttons == 0)
    {
	// Not available in LessTif 0.1
	buttons = verify(XmCreateRowColumn(parent, name, args, arg));
    }

    add_buttons(buttons, button_list);

    XtManageChild(buttons);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(buttons, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(buttons, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(buttons, XmVERTICAL, XmPIXELS, 
				    size.height, unit_type);
    XtVaSetValues(buttons,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    return buttons;
}

void add_buttons(Widget buttons, const string& button_list)
{
    int colons = button_list.freq(':') + 1;
    string *commands = new string[colons];
    split(button_list, commands, colons, ':');

    for (int i = 0; i < colons; i++)
    {
	XtCallbackProc callback = gdbCommandCB;

	string name = commands[i];
	string command = name;
	if (name.contains("..."))
	{
	    name = name.before("...");
	}
	else if (name.contains('^'))
	{
	    command = ctrl(name.from('^'));
	    name = name.before('^');
	}
	else if (name != "" && iscntrl(name[name.length() - 1]))
	{
	    command = string(name[name.length() - 1]);
	    name = name.before(-1);
	}

	// Make sure the widget name does not contain invalid characters
	string label_s = name;
	if (label_s != "")
	    label_s[0] = toupper(label_s[0]);
	MString label(label_s);
	static regex rxsep("[^-_a-zA-Z0-9]");
	name.gsub(rxsep, '_');

	Widget button = verify(XmCreatePushButton(buttons, name, 0, 0));
	XtManageChild(button);

	XmString xmlabel;
	XtVaGetValues(button, XmNlabelString, &xmlabel, NULL);
	MString foundLabel(xmlabel, true);
	XmStringFree(xmlabel);

	if (foundLabel.str() == name)
	{
	    // User did not specify a specific labelString - 
	    // use the specified button command as label
	    XtVaSetValues(button, XmNlabelString, label.xmstring(), NULL);
	}

	if (name == "Yes")
	{
	    command = "yes";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "No")
	{
	    command = "no";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "Prev")
	    callback = gdbPrevCB;
	else if (name == "Next")
	    callback = gdbNextCB;
	else if (name == "Clear")
	    callback = gdbClearCB;
	else if (name == "Complete")
	    callback = gdbCompleteCB;
	else if (name == "Apply")
	    callback = gdbApplyCB;
	else if (name == "Back")
	    callback = gdbGoBackCB;
	else if (name == "Forward")
	    callback = gdbGoForwardCB;
	else if (name == "Edit")
	    callback = gdbEditSourceCB;
	else if (name == "Reload")
	    callback = gdbReloadSourceCB;

	// We remove all callbacks to avoid popping down DialogShells
	XtRemoveAllCallbacks(button, XmNactivateCallback);
	XtAddCallback(button, XmNactivateCallback, callback,
		      (XtPointer)XtNewString((String)command));
    }
    delete[] commands;

    // Register default help command
    DefaultHelpText = gdbDefaultHelp;
    DefaultTipText  = gdbDefaultTip;

    DisplayDocumentation  = showDocumentationInStatusLine;
}
