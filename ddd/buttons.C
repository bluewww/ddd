// $Id$ -*- C++ -*-
// DDD buttons

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char buttons_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "buttons.h"

#include "AppData.h"
#include "Delay.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "SourceView.h"
#include "StringSA.h"
#include "bool.h"
#include "charsets.h"
#include "comm-manag.h"
#include "ctrl.h"
#include "ddd.h"
#include "disp-read.h"
#include "editing.h"
#include "question.h"
#include "select.h"
#include "settings.h"
#include "shorten.h"
#include "source.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <ctype.h>


//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Maximum length of value in value tip and in status line
int max_value_tip_length = 20;
int max_value_doc_length = 128;


//-----------------------------------------------------------------------------
// Button callbacks
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

    if (doc.isNull() || doc.isEmpty())
    {
	// Button has been left - restore previous message unless overwritten
	if (!current_status_message.isNull())
	{
	    // Button has been left
	    if (current_status_message == current_status())
	    {
		// Restore previous message
		set_status_mstring(saved_status_message);
	    }
	    else
	    {
		// Message has been overwritten.
#if 0 
		// This is a button effect, hence clear the message.
		set_status("");
#endif
	    }
	    static MString empty(0, true);
	    current_status_message = empty;
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

const int help_timeout = 2;	// Timeout for short queries (in s)

static StringStringAssoc help_cache;

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

    if (command == "graph" || command.contains("graph ", 0))
	help = "Data window operation.";

    if (help == NO_GDB_ANSWER)
    {
	// Lookup cache
	if (help_cache.has(command))
	    help = help_cache[command];
    }

    if (help == NO_GDB_ANSWER && gdb->type() == DBX)
    {
	string cmd  = command.before(rxwhite);
	string base = command.after(rxwhite);
	base.gsub(' ', '_');

	if (cmd == "set" || cmd == "dbxenv")
	{
	    // Ask DBX for help on DBXENV command
	    help = get_dbx_help(cmd, base);
	    strip_final_blanks(help);
	}
    }

    if (help == NO_GDB_ANSWER)
    {
	// Ask debugger for help
	help = gdb_question("help " + command, help_timeout, true);
	strip_final_blanks(help);
    }

    if (help != NO_GDB_ANSWER)
	help_cache[command] = help;

    return help;
}

static MString gdbDefaultHelpText(Widget widget)
{
    string name = gdbHelpName(widget);
    MString msg = bf(name) + cr();

    string help = gdbHelp(name);
    if (help == NO_GDB_ANSWER)
    {
	msg += rm("No help available now.") + cr();
	msg += rm("Please try again when " +  gdb->title() + " is ready.");
    }
    else
    {
	msg += rm(help);
    }

    return msg;
}


static StringStringAssoc value_cache;

void clear_value_cache()
{
    static StringStringAssoc empty;
    value_cache = empty;
}

static string gdbValue(const string& expr)
{
    string value = NO_GDB_ANSWER;
    if (value == NO_GDB_ANSWER)
    {
	// Lookup cache
	if (value_cache.has(expr))
	    value = value_cache[expr];
    }

    if (value == NO_GDB_ANSWER)
    {
	// Ask debugger for value.  In case of secondary prompts, use
	// the default choice.
	gdb->removeHandler(ReplyRequired, gdb_selectHP);
	value = gdb_question(gdb->print_command(expr), help_timeout);
	gdb->addHandler(ReplyRequired, gdb_selectHP);

	read_leading_blanks(value);
	strip_final_blanks(value);
    }

    if (value != NO_GDB_ANSWER)
	value_cache[expr] = value;

    return value;
}

static void strip_through(string& s, string key)
{
    int key_index = s.index(key);
    int nl_index  = s.index('\n');

    if (key_index >= 0 && (nl_index < 0 || key_index < nl_index))
	s = s.from(int(key_index + key.length()));
}

static XmTextPosition textPosOfEvent(Widget widget, XEvent *event)
{
    XmTextPosition startpos, endpos;
    string expr = 
	source_view->get_word_at_event(widget, event, startpos, endpos);

#if 0				// We might point at a text breakpoint
    if (expr == "")
	return XmTextPosition(-1);
#endif

    return startpos;
}

// Get tip string for text widget WIDGET.
static MString gdbDefaultValueText(Widget widget, XEvent *event, 
				   bool for_documentation)
{
    assert (XmIsText(widget));

    XmTextPosition startpos, endpos;
    string expr = 
	source_view->get_word_at_event(widget, event, startpos, endpos);

    // If we're at a breakpoint, return appropriate help
    MString bp_help = 
	source_view->help_on_pos(widget, startpos, for_documentation);
    if (bp_help.xmstring() != 0)
	return bp_help;

    if (expr == "")
	return MString(0, true); // Nothing pointed at

    // Don't invoke the debugger if EXPR is not an identifier.
    // Otherwise, we might point at `i++' or `f()' and have weird side
    // effects.
    MString clear = for_documentation ? rm(" ") : MString(0, true);
    if (!expr.matches(rxidentifier))
	return clear;

    // Change EVENT such that the popup tip will remain at the same
    // position
    Position x, y;
    if (XmTextPosToXY(widget, endpos, &x, &y))
    {
	switch (event->type)
	{
	case MotionNotify:
	    event->xmotion.x = x;
	    event->xmotion.y = y;
	    break;

	case EnterNotify:
	case LeaveNotify:
	    event->xcrossing.x = x;
	    event->xcrossing.y = y;
	    break;
	}
    }

    // Get value of ordinary variable
    string tip = gdbValue(expr);
    if (tip == NO_GDB_ANSWER)
	return MString(0, true);

    if (is_invalid(tip) && widget == source_view->code())
    {
	// Get register value - look up `$pc' when pointing at `pc'
	expr.prepend("$");
	tip = gdbValue(expr);
	if (tip == NO_GDB_ANSWER)
	    return MString(0, true);

	if (tip.matches(rxint))
	{
	    // Show hex value as well.  We don't do a local
	    // conversion here, but ask GDB instead, since the hex
	    // format may be language-dependent.
	    string hextip = gdbValue("/x " + expr);
	    if (hextip != NO_GDB_ANSWER)
		tip = hextip + " (" + tip + ")";
	}
    }
	    
    if (is_invalid(tip))
	return clear;

    tip = get_disp_value_str(tip, gdb);
    if (tip == "void")
	return clear;		// Empty variable

    if (for_documentation)
    {
	shorten(tip, max_value_doc_length - expr.length());

	// The status line also shows the name we're pointing at
	MString mtip = tt(tip);
	mtip.prepend(rm(expr + " = "));
	return mtip;
    }
    else
    {
	// The value tip just shows the value
	shorten(tip, max_value_tip_length);
	return tt(tip);
    }
}

// Get tip string for button widget WIDGET.
static MString gdbDefaultButtonText(Widget widget, XEvent *, 
				    bool for_documentation)
{
    MString bp_help = source_view->help_on_glyph(widget, for_documentation);
    if (bp_help.xmstring() != 0)
	return bp_help;

    string help_name = gdbHelpName(widget);
    string tip = gdbHelp(help_name);
    if (tip == NO_GDB_ANSWER)
	return MString(0, true);
    if (tip.contains(help_name, 0))
	tip = tip.after(help_name);
    strip_through(tip, " # ");
    strip_through(tip, " - ");

    tip = tip.from(rxalpha);
    if (tip.length() > 0)
	tip = toupper(tip[0]) + tip.after(0);

    if (tip.contains('\n'))
	tip = tip.before('\n');
    if (tip.contains('.'))
	tip = tip.before('.');

    return rm(tip);
}


static MString gdbDefaultText(Widget widget, XEvent *event, 
			      bool for_documentation)
{
    if (XmIsText(widget))
	return gdbDefaultValueText(widget, event, for_documentation);
    else
	return gdbDefaultButtonText(widget, event, for_documentation);
}

static MString gdbDefaultTipText(Widget widget, XEvent *event)
{
    return gdbDefaultText(widget, event, false);
}

static MString gdbDefaultDocumentationText(Widget widget, XEvent *event)
{
    return gdbDefaultText(widget, event, true);
}


//-----------------------------------------------------------------------------
// Button Verification
//-----------------------------------------------------------------------------

// Buttons to be verified
static WidgetArray buttons_to_be_verified;

static void VerifyButtonWorkProc(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    XtIntervalId& verify_id = *((XtIntervalId *)client_data);
    assert(*id == verify_id);
    verify_id = 0;

    for (int i = 0; i < buttons_to_be_verified.size(); i++)
    {
	Widget& button = buttons_to_be_verified[i];
	if (button == 0)
	    continue;

	XtCallbackList callbacks = 0;
	XtVaGetValues(button, 
		      XmNactivateCallback, &callbacks,
		      NULL);

	for (int j = 0; callbacks != 0 && callbacks[j].callback != 0; j++)
	{
	    if (callbacks[j].callback == gdbCommandCB)
	    {
		string cmd = String(callbacks[j].closure);
		cmd = cmd.through(rxidentifier);
		if (cmd != "")
		{
		    int next_invocation = 0;
		    XtAppContext app_context = 
			XtWidgetToApplicationContext(button);

		    string answer = gdbHelp(cmd);
		    if (answer == NO_GDB_ANSWER)
		    {
			// No answer - try again in 20ms
			next_invocation = 20;
		    }
		    else
		    {
			XtSetSensitive(button, is_known_command(answer));
			button = 0;          // Don't process this one again
			next_invocation = 5; // Process next button in 5ms
		    }

		    verify_id = XtAppAddTimeOut(app_context, next_invocation,
						VerifyButtonWorkProc, 
						client_data);
		    return;
		}
	    }
	}
    }
}

static void DontVerifyButtonCB(Widget w, XtPointer, XtPointer)
{
    // W is being destroyed - remove all references
    for (int i = 0; i < buttons_to_be_verified.size(); i++)
	if (buttons_to_be_verified[i] == w)
	    buttons_to_be_verified[i] = 0;
}

// Make BUTTON insensitive if it is not supported
void verify_button(Widget button)
{
    if (!app_data.verify_buttons)
	return;
    if (button == 0)
	return;
    if (!XtIsSubclass(button, xmPushButtonWidgetClass))
	return;

#if 0
    XtSetSensitive(button, False);
#endif

    buttons_to_be_verified += button;
    XtAddCallback(button, XtNdestroyCallback, 
		  DontVerifyButtonCB, XtPointer(0));

    // Procedure id
    static XtIntervalId verify_id = 0;
    if (verify_id == 0)
    {
	verify_id = XtAppAddTimeOut(XtWidgetToApplicationContext(button),
				    0, VerifyButtonWorkProc, 
				    XtPointer(&verify_id));
    }
}


//-----------------------------------------------------------------------------
// Button Creation
//-----------------------------------------------------------------------------

// Create a button work area from BUTTON_LIST named NAME
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

	// Verify later whether the button actually exists
	verify_button(button);

	// We remove all callbacks to avoid popping down DialogShells
	XtRemoveAllCallbacks(button, XmNactivateCallback);
	XtAddCallback(button, XmNactivateCallback, callback,
		      (XtPointer)XtNewString((String)command));
    }
    delete[] commands;

    // Register default help command
    DefaultHelpText           = gdbDefaultHelpText;
    DefaultTipText            = gdbDefaultTipText;
    DefaultDocumentationText  = gdbDefaultDocumentationText;
    TextPosOfEvent            = textPosOfEvent;

    DisplayDocumentation      = showDocumentationInStatusLine;
}
