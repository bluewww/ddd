// $Id$ -*- C++ -*-
// DDD buttons

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char buttons_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "buttons.h"

#include "AppData.h"
#include "Command.h"
#include "DataDisp.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "LessTifH.h"
#include "MakeMenu.h"
#include "SourceView.h"
#include "StringSA.h"
#include "TimeOut.h"
#include "args.h"
#include "bool.h"
#include "charsets.h"
#include "comm-manag.h"
#include "cook.h"
#include "ctrl.h"
#include "ddd.h"
#include "disp-read.h"
#include "editing.h"
#include "fortranize.h"
#include "question.h"
#include "regexps.h"
#include "select.h"
#include "settings.h"
#include "shorten.h"
#include "source.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/SelectioB.h>
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
// Version stuff
//-----------------------------------------------------------------------------

// Return true iff old button separators (`:') are used.  These were
// introduced in DDD 1997-10 or 2.1.
static bool old_button_format()
{
    if (app_data.dddinit_version == 0)
	return true;
    string v = app_data.dddinit_version;
    int major = atoi(v);

    if (major > 1900)
    {
	// YYYY-MM-DD format
	if (major <= 1996)
	    return true;	// 1996 or earlier
	if (major >= 1998)
	    return false;	// 1998 or later

	assert(major == 1997);
	string v2 = v.after('-');
	int minor = atoi(v2);
	if (minor <= 9)		// 1997-09 or earlier
	    return true;

	return false;		// 1997-10 or later
    }
    else
    {
	// MAJOR.MINOR format
	if (major <= 1)
	    return true;	// 1.x or earlier
	if (major >= 3)
	    return false;	// 3.x or later

	assert(major == 2);
	string v2 = v.after('.');
	int minor = atoi(v2);
	if (minor <= 1)
	    return true;	// 2.1 or earlier

	return false;		// 2.2 or later
    }
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
		set_status_mstring(saved_status_message, true);
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

	set_status_mstring(doc, true);
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
    strip_trailing_space(name);

    return name;
}

static string gdbHelp(string command)
{
    translate_command(command);

    if (gdb->type() == JDB && command == "next")
    {
	// JDB 1.1 has an undocumented `next' command.  Treat it like `step'.
	command = "step";
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

    if (help == NO_GDB_ANSWER && gdb->type() == JDB)
    {
	// JDB has a single static `help' command.
	string& all_help = help_cache["<ALL>"];
	if (all_help == NO_GDB_ANSWER || all_help == "")
	{
	    all_help = gdb_question("help", -1, true);
	    if (all_help == NO_GDB_ANSWER)
		return NO_GDB_ANSWER; // try again later
	}

	int index = all_help.index("\n" + command) + 1;
	if (all_help.contains(command, index))
	{
	    help = all_help.from(index);
	    help = help.after("--");
	    help = help.before('\n');
	}
	else
	{
	    help = "Undefined command: " + quote(command) 
		+ ".  Try \"help\"\n";
	}
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
	}
    }

    if (help == NO_GDB_ANSWER && gdb->type() != JDB)
    {
	// Ask debugger for help
	help = gdb_question("help " + command, help_timeout, true);
    }

    strip_space(help);

    if (help != NO_GDB_ANSWER)
	help_cache[command] = help;

    return help;
}


// Return DBX 3.2 one-liner help.
static string gdbTip(string command)
{
    static bool entered = false;
    if (entered)
	return NO_GDB_ANSWER;
    entered = true;

    string tip = NO_GDB_ANSWER;

    if (gdb->type() == DBX)
    {
	static string commands = NO_GDB_ANSWER;
	if (commands == NO_GDB_ANSWER)
	{
	    commands = gdb_question("commands", -1, true);
	    if (!is_known_command(commands))
		commands = "";
	}

	int start = 0;
	for (;;)
	{
	    int i = commands.index(command, start);
	    if (i < 0)
		break;
	    while (i > 0 
		   && commands[i - 1] != '\n' 
		   && isspace(commands[i - 1]))
		i--;
	    if (i == 0 || commands[i - 1] == '\n')
	    {
		tip = commands.from(i);
		tip = tip.before('\n');
		break;
	    }

	    start = i + 1;
	}
    }

    entered = false;
    return tip;
}


static string gdbSettingsValue(string command)
{
    switch (gdb->type())
    {
    case GDB:
	if (command.contains("set ", 0))
	{
	    string value = gdb_question("show " + command.after(rxwhite));
	    if (!value.contains("current"))
		value.gsub(" is ", " is currently ");
	    strip_trailing_space(value);
	    return value;
	}
	break;

    case DBX:
    case XDB:
    case JDB:
	return NO_GDB_ANSWER;		// FIXME
    }

    return NO_GDB_ANSWER;
}

static MString gdbDefaultHelpText(Widget widget)
{
    const string name = gdbHelpName(widget);

    MString msg;
    if (name != "" && islower(name[0]))
	msg = bf(toupper(name[0]) + name.after(0));
    else
	msg = bf(name);
    msg += cr();
    msg += cr();

    string help = gdbHelp(name);
    if (help == NO_GDB_ANSWER)
    {
	msg += rm("No help available now.") + cr();
	msg += rm("Please try again when " +  gdb->title() + " is ready.");
    }
    else
    {
	msg += rm(help);

	// Add current settings state, if any
	string state = gdbSettingsValue(name);
	if (state != NO_GDB_ANSWER)
	{
	    msg += cr();
	    msg += cr();
	    msg += rm(state);
	}
    }

    return msg;
}


static StringStringAssoc value_cache;

void clear_value_cache()
{
    static StringStringAssoc empty;
    value_cache = empty;
}

string gdbValue(const string& expr)
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

	strip_space(value);
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
	source_view->help_on_pos(widget, startpos, endpos, for_documentation);

    if (bp_help.xmstring() == 0 && expr == "")
	return MString(0, true); // Nothing pointed at

#if RUNTIME_REGEX
    static regex rxchain("[-a-zA-Z0-9_>.]+");
#endif

    // Don't invoke the debugger if EXPR is not an identifier.
    // Otherwise, we might point at `i++' or `f()' and have weird side
    // effects.
    MString clear = for_documentation ? rm(" ") : MString(0, true);
    if (bp_help.xmstring() == 0 && !expr.matches(rxchain))
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

    if (bp_help.xmstring() != 0)
	return bp_help;

    // Get value of ordinary variable
    string name = fortranize(expr);
    string tip = gdbValue(name);
    if (tip == NO_GDB_ANSWER)
	return MString(0, true);

    if (is_invalid(tip) && widget == source_view->code())
    {
	// Get register value - look up `$pc' when pointing at `pc'
	name = expr;
	name.prepend("$");
	tip = gdbValue(name);
	if (tip == NO_GDB_ANSWER)
	    return MString(0, true);

	if (tip.matches(rxint))
	{
	    // Show hex value as well.  We don't do a local
	    // conversion here, but ask GDB instead, since the hex
	    // format may be language-dependent.
	    string hextip = gdbValue("/x " + name);
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
	shorten(tip, max_value_doc_length - name.length());

	// The status line also shows the name we're pointing at
	MString mtip = tt(tip);
	mtip.prepend(rm(name + " = "));
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
    if (!bp_help.isNull())
	return bp_help;

    MString shortcut_help = data_disp->shortcut_help(widget);
    if (!shortcut_help.isNull())
	return shortcut_help;

    string help_name = gdbHelpName(widget);

    string tip = NO_GDB_ANSWER;
    if (tip == NO_GDB_ANSWER)
	tip = gdbTip(help_name);
    if (tip == NO_GDB_ANSWER)
	tip = gdbHelp(help_name);
    if (tip == NO_GDB_ANSWER)
	return MString(0, true);

    // Sun DBX 3.2 sometimes forgets the newline after the 80th character
    if (tip.length() > 80)
	tip = tip.before(80);

    strip_leading_space(tip);
    if (tip.contains(help_name, 0))
    {
	string t = tip.after(help_name);
	if (t != "" && !isalpha(t[0]))
	{
	    tip = t;
	    strip_leading_space(tip);
	}
    }
    
    strip_through(tip, " # ");
    strip_through(tip, " - ");

    if (gdb->type() == XDB)
    {
	// Get rid of XXX [number] as in `S [number] Single step, step...'
	// Bob Wiegand <robert.e.wiegand.1@gsfc.nasa.gov>
	strip_through(tip, "]");
    }

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
	    string cmd = "";

	    if (callbacks[j].callback == gdbCommandCB)
	    {
		cmd = (String)(callbacks[j].closure);
		cmd = cmd.through(rxidentifier);
	    }

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
		    button = 0;	           // Don't process this one again
		    next_invocation = 50;  // Process next button in 50ms
		}

		verify_id = XtAppAddTimeOut(app_context, next_invocation,
					    VerifyButtonWorkProc, 
					    client_data);
		return;
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
		    String button_list)
{
    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
    XtSetArg(args[arg], XmNuserData, XtPointer(""));   arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0);            arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0);           arg++;
    XtSetArg(args[arg], XmNspacing, 2);                arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);            arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0);     arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0);        arg++;
    Widget buttons = verify(XmCreateWorkArea(parent, name, args, arg));
    if (buttons == 0)
    {
	// Not available in LessTif 0.1
	buttons = verify(XmCreateRowColumn(parent, name, args, arg));
    }

    set_buttons(buttons, button_list);

    if (XtIsManaged(buttons))
    {
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
    }

    return buttons;
}

void set_buttons(Widget buttons, String _button_list, bool manage)
{
    XtPointer user_data;
    XtVaGetValues(buttons, XmNuserData, &user_data, NULL);

    if (user_data != 0)
    {
	String s = (String)user_data;
	if (s == _button_list || string(s) == string(_button_list))
	    return;		// Unchanged value
    }

    // Destroy all existing children (= buttons)
    WidgetList children   = 0;
    Cardinal num_children = 0;

    XtVaGetValues(buttons,
		  XtNchildren, &children,
		  XtNnumChildren, &num_children,
		  NULL);

    int i;
    for (i = 0; i < int(num_children); i++)
    {
	XtUnmanageChild(children[i]);
	DestroyWhenIdle(children[i]);
    }

    // Add new buttons
    string button_list = _button_list;

    if (button_list.contains(':') && old_button_format())
    {
	// DDD 2.1 and earlier used `:' to separate buttons
	button_list.gsub(':', '\n');

	cerr << "Warning: converting " << quote(_button_list) << "\n"
	     << "to new format " << quote(button_list) << "\n";
    }

    int lines = button_list.freq('\n') + 1;
    string *commands = new string[lines];
    split(button_list, commands, lines, '\n');

    int number_of_buttons = 0;
    for (i = 0; i < lines; i++)
    {
	XtCallbackProc callback = gdbCommandCB;

	string name = commands[i];
	strip_space(name);

	if (name == "")
	    continue;

	MString label(0, true);
	if (name.contains(app_data.label_delimiter))
	{
	    string label_s = name.after(app_data.label_delimiter);
	    name = name.before(app_data.label_delimiter);
	    strip_space(label_s);
	    strip_space(name);
	    label = MString(label_s);
	}

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

	if (label.isNull())
	{
	    // Create default label from name
	    string label_s = name;
	    if (label_s != "")
		label_s[0] = toupper(label_s[0]);
	    label = MString(label_s);
	}

	// Make sure the widget name does not contain invalid characters
#if RUNTIME_REGEX
	static regex rxsep("[^-_a-zA-Z0-9]");
#endif
	name.gsub(rxsep, '_');

#if 0
	Widget button = verify(create_flat_button(buttons, name));
#else
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNborderWidth, 0);        arg++;
	XtSetArg(args[arg], XmNhighlightThickness, 1); arg++;
	Widget button = verify(XmCreatePushButton(buttons, name, args, arg));
#endif
	XtManageChild(button);
	number_of_buttons++;

	// A user-specified labelString overrides the given label
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
	else if (name == "Make")
	    callback = gdbMakeAgainCB;
	else if (name == "Reload")
	    callback = gdbReloadSourceCB;

	// Be sure to verify whether the button actually exists
	verify_button(button);

	// We remove all callbacks to avoid popping down DialogShells
	XtRemoveAllCallbacks(button, XmNactivateCallback);
	XtAddCallback(button, XmNactivateCallback, callback,
		      (XtPointer)XtNewString(command.chars()));

	// Add a help callback
	XtAddCallback(button, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
    }
    delete[] commands;

    if (manage)
    {
	if (number_of_buttons > 0)
	    XtManageChild(buttons);
	else
	    XtUnmanageChild(buttons); // No buttons at all
    }

    XtVaSetValues(buttons, XmNuserData, XtPointer(_button_list), NULL);

    // Register default help command
    DefaultHelpText           = gdbDefaultHelpText;
    DefaultTipText            = gdbDefaultTipText;
    DefaultDocumentationText  = gdbDefaultDocumentationText;
    TextPosOfEvent            = textPosOfEvent;

    DisplayDocumentation      = showDocumentationInStatusLine;

    // Install tips
    InstallButtonTips(buttons);
}



//-----------------------------------------------------------------------------
// Button Editor
//-----------------------------------------------------------------------------

// Remove garbage from S
static string normalize(string s)
{
    if (s.contains(':') && old_button_format())
    {
	// DDD 2.1 and earlier used `:' to separate buttons
	s.gsub(':', '\n');
    }

    int lines = s.freq('\n') + 1;
    string *commands = new string[lines];
    split(s, commands, lines, '\n');

    string ret = "";
    for (int i = 0; i < lines; i++)
    {
	string& cmd = commands[i];
	strip_space(cmd);
	if (cmd == "")
	    continue;

	if (ret.length() > 0)
	    ret += '\n';
	ret += cmd;
    }

    return ret;
}

static Widget buttons_dialog = 0;
static Widget button_box     = 0;
static Widget shortcut_label = 0;
static Widget console_w, shortcut_w;

struct ChangeTextInfo {
    String *str;
    Widget dialog;
    Widget text;
    Widget vfy;
    bool shortcuts;
};

static ChangeTextInfo *active_info = 0;

static void SetTextCB(Widget, XtPointer, XtPointer)
{
    if (active_info == 0)
	return;

    String _str = XmTextGetString(active_info->text);
    string str(_str);
    XtFree(_str);

    str = normalize(str);

    XmTextSetString(active_info->text, str);

    *active_info->str = (String)XtNewString(str.chars());
    update_user_buttons();
}

static void ResetTextCB(Widget, XtPointer, XtPointer)
{
    if (active_info == 0)
	return;

    XmTextSetString(active_info->text, *active_info->str);
    update_user_buttons();
}

static void ChangeTextCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    ChangeTextInfo *info = (ChangeTextInfo *)client_data;

    if (cbs->set)
    {
	// When changing, treat like `Apply'
	SetTextCB(w, XtPointer(0), XtPointer(0));

	active_info = info;

	string str = normalize(*info->str);
	XmTextSetString(info->text, (char *)str.chars());
	XtAddCallback(info->dialog, XmNhelpCallback, 
		      HelpOnThisCB, XtPointer(w));

	if (info->shortcuts)
	    XtUnmanageChild(info->vfy);
	else
	    XtManageChild(info->vfy);
    }
    else
    {
	XtRemoveCallback(info->dialog, XmNhelpCallback, 
			 HelpOnThisCB, XtPointer(w));
    }
}

static void SetVerifyButtonsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.verify_buttons = cbs->set;
}


static Widget add_button(String name, 
			 Widget dialog, Widget buttons, 
			 Widget text, Widget vfy,
			 String& str, bool shortcuts = false)
{
    Arg args[10];
    Cardinal arg = 0;
    Widget button = XmCreateToggleButton(buttons, name, args, arg);
    XtManageChild(button);

    ChangeTextInfo *info = new ChangeTextInfo;
    info->dialog    = dialog;
    info->str       = &str;
    info->text      = text;
    info->vfy       = vfy;
    info->shortcuts = shortcuts;

    XtAddCallback(button, XmNvalueChangedCallback, ChangeTextCB, 
		  XtPointer(info));

    return button;
}

static void create_buttons_dialog(Widget parent)
{
    if (buttons_dialog != 0)
	return;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNvisibleItemCount, 0); arg++;
    buttons_dialog = 
	verify(XmCreatePromptDialog(find_shell(parent), 
				    "edit_buttons", args, arg));

    XtAddCallback(buttons_dialog, XmNokCallback,     SetTextCB, 0);
    XtAddCallback(buttons_dialog, XmNapplyCallback,  SetTextCB, 0);
    XtAddCallback(buttons_dialog, XmNcancelCallback, ResetTextCB, 0);

#if XmVersion >= 1002
    XtManageChild(XmSelectionBoxGetChild(buttons_dialog, 
					 XmDIALOG_APPLY_BUTTON));
#endif

    XtUnmanageChild(XmSelectionBoxGetChild(buttons_dialog,
					   XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(buttons_dialog, 
					   XmDIALOG_TEXT));
    Delay::register_shell(buttons_dialog);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNadjustMargin, False); arg++;
    Widget box = 
	verify(XmCreateRowColumn(buttons_dialog, "box", args, arg));
    XtManageChild(box);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING); arg++;
    shortcut_label = verify(XmCreateLabel(box, "shortcuts", args, arg));
    XtManageChild(shortcut_label);

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNorientation,  XmHORIZONTAL); arg++;
    button_box = 
	verify(XmCreateRadioBox(box, "buttons", args, arg));
    XtManageChild(button_box);

    arg = 0;
    XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
    Widget text = verify(XmCreateScrolledText(box, "text", args, arg));
    XtManageChild(text);

    arg = 0;
    XtSetArg(args[arg], XmNset, app_data.verify_buttons); arg++;
    Widget vfy = verify(XmCreateToggleButton(box, "verify", args, arg));
    XtManageChild(vfy);
    XtAddCallback(vfy, XmNvalueChangedCallback, SetVerifyButtonsCB, 0);

    console_w = 
	add_button("console", buttons_dialog, button_box, text, vfy,
		   app_data.console_buttons);
    Widget source_w = 
	add_button("source", buttons_dialog, button_box, text, vfy,
		   app_data.source_buttons);
    Widget data_w = 
	add_button("data", buttons_dialog, button_box, text, vfy,
		   app_data.data_buttons);
    shortcut_w = 
	add_button("shortcuts", buttons_dialog, button_box, text, vfy,
		   app_data.display_shortcuts, true);

    XmToggleButtonSetState(source_w, True, False);
    (void) data_w;
}

// We use one single editor for both purposes, since this saves space.
void dddEditButtonsCB(Widget w, XtPointer, XtPointer)
{
    create_buttons_dialog(w);
    XtUnmanageChild(buttons_dialog);

    XtManageChild(button_box);
    XtManageChild(shortcut_w);

    XmToggleButtonSetState(console_w, Boolean(True), True);
    ResetTextCB(w, 0, 0);

    XtManageChild(button_box);
    XtUnmanageChild(shortcut_w);
    XtUnmanageChild(shortcut_label);

    XtVaSetValues(XtParent(buttons_dialog), XmNtitle, 
		  DDD_NAME ": Button Editor", NULL);

    manage_and_raise(buttons_dialog);
}

void dddEditShortcutsCB(Widget w, XtPointer, XtPointer)
{
    create_buttons_dialog(w);
    XtUnmanageChild(buttons_dialog);

    XtManageChild(button_box);
    XtManageChild(shortcut_w);

    XmToggleButtonSetState(shortcut_w, Boolean(True), True);
    ResetTextCB(w, 0, 0);

    XtManageChild(shortcut_label);
    XtUnmanageChild(button_box);

    XtVaSetValues(XtParent(buttons_dialog), XmNtitle, 
		  DDD_NAME ": Shortcut Editor", NULL);

    manage_and_raise(buttons_dialog);
}

void refresh_button_editor()
{
    StringArray exprs;
    StringArray labels;

    data_disp->get_shortcut_menu(exprs, labels);
    string expr;
    for (int i = 0; i < exprs.size(); i++)
    {
	if (i > 0)
	    expr += '\n';
	expr += exprs[i];
	if (labels[i] != "")
	    expr += string('\t') + app_data.label_delimiter + ' ' + labels[i];
    }

    app_data.display_shortcuts = (String)XtNewString(expr.chars());

    if (active_info != 0 && active_info->str == &app_data.display_shortcuts)
	XmTextSetString(active_info->text, app_data.display_shortcuts);
}


//-----------------------------------------------------------------------------
// Flat Buttons
//-----------------------------------------------------------------------------

static void nop(Widget, XtPointer, XtPointer) {}

static MMDesc desc[] = 
{
    { "", MMFlatPush, { nop }},
    MMEnd
};

// Create a flat PushButton named NAME
Widget create_flat_button(Widget parent, const string& name)
{
    desc[0].name = (char *)name;
    MMaddItems(parent, desc);
    MMaddCallbacks(desc);
    return desc[0].widget;
}
