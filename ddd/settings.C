// $Id$ -*- C++ -*-
// Modify debugger settings

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000-2001 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003, 2004, 2005, 2008 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char settings_rcsid[] = 
    "$Id$";

#include "settings.h"


#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/DialogS.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/MwmUtil.h>
#include <Xm/Separator.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "AppData.h"
#include "Assoc.h"
#include "ComboBox.h"
#include "Command.h"
#include "DataDisp.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "DispBox.h"
#include "EntryType.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "LessTifH.h"
#include "MakeMenu.h"
#include "SmartC.h"
#include "SourceView.h"
#include "StringSA.h"
#include "UndoBuffer.h"
#include "ThemeP.h"
#include "VarArray.h"
#include "WidgetSA.h"
#include "basename.h"
#include "buttons.h"
#include "cook.h"
#include "comm-manag.h"
#include "ddd.h"
#include "editing.h"
#include "glob.h"
#include "mydialogs.h"
#include "logo.h"
#include "options.h"
#include "post.h"
#include "question.h"
#include "regexps.h"
#include "session.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "vsldoc.h"
#include "wm.h"
#include "charsets.h"

#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
#endif

//-----------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------

const Dimension EXTRA_SPACE     = 10;   // Minimum space between label / entry
const Dimension EXTRA_WIDTH     =  6;   // Additional space for safety
const Dimension MAX_HEIGHT     = 300;   // Maximum height of window

enum SettingsType { SETTINGS, INFOS, SIGNALS, THEMES };


//-----------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------

static Widget            settings_panel = 0;
static Widget            settings_form  = 0;
static Widget            reset_settings_button = 0;
static Widget            apply_settings_button = 0;
static WidgetArray       settings_entries;
static EntryTypeArray    settings_entry_types;
static WidgetStringAssoc settings_values;
static WidgetStringAssoc settings_initial_values;
static bool              need_reload_settings = false;

static Widget            signals_panel = 0;
static Widget            signals_form  = 0;
static Widget            reset_signals_button = 0;
static WidgetArray       signals_entries;
static WidgetStringAssoc signals_values;
static WidgetStringAssoc signals_initial_values;
static bool              need_reload_signals = false;

static Widget            themes_panel = 0;
static Widget            reset_themes_button = 0;
static Widget            apply_themes_button = 0;
static WidgetArray       themes_entries;
static WidgetArray       themes_labels;

static Widget            infos_panel        = 0;
static Widget            reset_infos_button = 0;
static WidgetArray       infos_entries;



//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

static void get_setting(std::ostream& os, DebuggerType type,
			const string& base, string value,
			unsigned long flags = SAVE_DEFAULT | SAVE_SESSION);
static void set_arg();


// Find widget for command COMMAND
static Widget command_to_widget(Widget ref, string command)
{
    Widget found = 0;
    while (!command.empty() && (found = XtNameToWidget(ref, command.chars())) == 0)
    {
	// Strip last word (command argument)
	int index = command.index(rxwhite, -1);
	command   = command.before(index);
    }

    return found;
}

// Issue `set' command
static void gdb_set_command(const string& set_command, string value)
{
    if (gdb->type() == GDB && value == "unlimited")
	value = "0";

    if (gdb->type() == GDB && set_command == "dir" && !value.empty())
    {
	// `dir' in GDB works somewhat special: it prepends its
	// argument to the source path instead of simply setting it.
	// Hence, first reset `dir' to some initial value.

	string confirm_value = "on";
	Widget confirm_w = command_to_widget(settings_form, "set confirm");
	if (confirm_w)
	    confirm_value = settings_values[confirm_w];

	if (confirm_value == "on")
	    gdb_command("set confirm off");
	gdb_command(set_command);
	if (confirm_value == "on")
	    gdb_command("set confirm on");
    }

    if (set_command.contains("o ", 0))
    {
	if (value.contains('\'', 0) && value.contains('\'', -1))
	    value = unquote(value);
// removed for perl-5.8.x
//	if (value == "N/A")
//	    value = "";

	gdb_command(set_command + "=" + value); // Perl
    }
    else if (set_command.contains("set $", 0) && !set_command.contains(" = "))
    {
	gdb_command(set_command + " = " + value); // DBX
    }
    else if (!value.empty())
    {
	gdb_command(set_command + " " + value); // GDB
    }
    else
    {
	gdb_command(set_command);
    }
}

// OptionMenu reply
static void SetOptionCB(Widget w, XtPointer client_data, XtPointer)
{
    gdb_set_command((const _XtString)client_data, XtName(w));
}

// ToggleButton reply
static void SetOnOffCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_set_command((const _XtString)client_data, cbs->set ? "on":"off");
}

// ToggleButton reply
static void SetTrueFalseCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_set_command((const _XtString)client_data, cbs->set ? "true":"false");
}

// ToggleButton reply
static void SetSensitiveCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_set_command((const _XtString)client_data,
		    cbs->set ? "sensitive":"insensitive");
}

// ToggleButton reply
static void SetNumCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_set_command((const _XtString)client_data, cbs->set ? "1":"0");
}

// ToggleButton reply
static void SetNoNumCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_set_command((const _XtString)client_data, cbs->set ? "0":"1");
}

// ToggleButton reply
static void SetDisplayCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
      data_disp->new_user_display((const _XtString)client_data);
    else
      data_disp->delete_user_display((const _XtString)client_data);
}

// ToggleButton reply
static void SendSignalCB(Widget, XtPointer client_data, XtPointer)
{
    gdb_command(string("signal ") + XtName(Widget(client_data)));
}


static string handle_command(Widget w, bool set)
{
    string sig    = string(XtName(w)).before('-');
    string action = string(XtName(w)).after('-');
    if (!set)
	action.prepend("no");

    if (action == "nopass")
	action = "ignore";
    else if (action == "noignore")
	action = "pass";

    return "handle " + sig + " " + action;
}

// ToggleButton reply
static void SignalCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    gdb_command(handle_command(w, cbs->set));
}

// Get help on signal - using `info' on `libc'
static void HelpOnSignalCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    Widget label = Widget(client_data);
    string sig = XtName(label);

    string s = "";

    if (sig == "all")
    {
	HelpOnThisCB(w, XtPointer(label), call_data);
	return;
    }

    StatusDelay delay("Retrieving Signal Documentation");

    // libc info has one common node for SIGUSR1 and SIGUSR2
    string sigindex = sig;
    if (sigindex == "SIGUSR1")
	sigindex = "SIGUSR2";

    const string cmd = "info -f libc -n 'Variable Index' " + sigindex + " -o -";

    const string s1 = sh_command(cmd); 
    FILE *fp = popen(s1.chars(), "r");
    if (fp != 0)
    {
	std::ostringstream info;
	int c;
	while ((c = getc(fp)) != EOF)
	    info << char(c);
	pclose(fp);
	
	s = info;
	int start = s.index("- Macro: int " + sig);
	start = s.index('\n', start);
	s = s.after(start);
	int end = s.index("\n - ");
	if (end >= 0)
	    s = s.before(end);

	s.gsub("ee\n*Note\n", "ee\n`");
	s.gsub("ee *Note\n",  "ee\n`");
	s.gsub("ee\n*Note ",  "ee\n`");
	s.gsub("ee *Note ",   "ee `");
	s.gsub("*Note ",      "See `");
	s.gsub("*Note\n",     "See\n`");
	s.gsub("::", "' in libc info");
	s.gsub("`C-", "`Ctrl+");

	strip_space(s);
	while (s.contains("  "))
	    s.gsub("  ", " ");
	s.gsub("\n ", "\n");
    }

    if (s.empty())
	s = "No help available on this signal.";

    XmString xmdoc = 0;
    XtVaGetValues(label, XmNlabelString, &xmdoc, XtPointer(0));
    MString doc(xmdoc, true);
    XmStringFree(xmdoc);

    MString text = bf(sig + ": ") + doc;
    text += cr() + cr() + rm(s);

    MStringHelpCB(w, XtPointer(text.xmstring()), call_data);
}


// Update state of `reset' button
static void update_reset_settings_button()
{
    if (reset_settings_button == 0)
	return;

    check_options_file();

    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];

	string value = settings_values[entry];
	if (settings_entry_types[i] == TextFieldEntry)
	{
	    String value_s = XmTextFieldGetString(entry);
	    value = value_s;
	    XtFree(value_s);

	    if (value != settings_values[entry])
	    {
		set_sensitive(reset_settings_button, true);
		return;
	    }
	}

	if (value != settings_initial_values[entry])
	{
	    set_sensitive(reset_settings_button, true);
	    return;
	}
    }

    set_sensitive(reset_settings_button, false);
}

static void update_apply_settings_button()
{
    if (apply_settings_button == 0)
	return;

    for (int i = 0; i < settings_entries.size(); i++)
    {
	if (settings_entry_types[i] != TextFieldEntry)
	    continue;

	Widget entry = settings_entries[i];

	String value_s = XmTextFieldGetString(entry);
	string value(value_s);
	XtFree(value_s);

	if (value != settings_values[entry])
	{
	    set_sensitive(apply_settings_button, true);
	    return;
	}
    }

    set_sensitive(apply_settings_button, false);
}

static void update_reset_signals_button()
{
    if (reset_signals_button == 0)
	return;

    check_options_file();

    for (int i = 0; i < signals_entries.size(); i++)
    {
	Widget entry = signals_entries[i];
	if (signals_initial_values[entry] != signals_values[entry])
	{
	    set_sensitive(reset_signals_button, true);
	    return;
	}
    }

    set_sensitive(reset_signals_button, false);
}

static void update_themes_buttons()
{
    if (apply_themes_button == 0 || reset_themes_button == 0)
	return;

    bool apply_is_sensitive = false;
    bool reset_is_sensitive = false;

    int i;

    ThemeManager old_tm(app_data.themes);

    for (i = 0; i < themes_entries.size(); i++)
    {
	Widget entry = themes_entries[i];
	string theme = basename(XtName(entry));
	ThemePattern p;

	if (DispBox::theme_manager.has_pattern(theme))
	    p = DispBox::theme_manager.pattern(theme);

	std::ostringstream os;
	os << p;
	string current_value = string(os);

	String value_s = XmTextFieldGetString(entry);
	string value(value_s);
	XtFree(value_s);

	string old_value;

	bool have_pattern = old_tm.has_pattern(theme);
	if (have_pattern)
	{
	    ThemePattern p = old_tm.pattern(theme);
	    std::ostringstream os;
	    os << p;
	    old_value = string(os);
	}

	if (value != current_value)
	    apply_is_sensitive = True;

	if (value != old_value)
	    reset_is_sensitive = True;

	if (apply_is_sensitive && reset_is_sensitive)
	    break;
    }

    if (!reset_is_sensitive)
    {
	for (i = 0; i < themes_labels.size(); i++)
	{
	    Widget button = themes_labels[i];
	    string theme = basename(XtName(button));
	    bool old_set = old_tm.has_pattern(theme);

	    if (old_set)
	    {
		ThemePattern p = old_tm.pattern(theme);
		if (!p.active())
		    old_set = false;
	    }

	    if (XmToggleButtonGetState(button) != old_set)
	    {
		reset_is_sensitive = True;
		break;
	    }
	}
    }

    set_sensitive(apply_themes_button, apply_is_sensitive);
    set_sensitive(reset_themes_button, reset_is_sensitive);
}



// Update states of `info' buttons
void update_infos()
{
    bool have_info = false;

    for (int i = 0; i < infos_entries.size(); i++)
    {
	Widget button = infos_entries[i];
	bool set = data_disp->have_user_display(XtName(button));
	have_info = have_info || set;
	XtVaSetValues(button, XmNset, set, XtPointer(0));
    }

    if (reset_infos_button != 0)
	set_sensitive(reset_infos_button, have_info);
}

static void UpdateSettingsButtonsNowCB(XtPointer, XtIntervalId *)
{
    update_apply_settings_button();
    update_reset_settings_button();
}

// TextField reply
static void UpdateSettingsButtonsCB(Widget w, XtPointer client_data, 
				    XtPointer)
{
    // The TextField value has not yet changed.  Call again later.
    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0,
		    UpdateSettingsButtonsNowCB, client_data);
}


static void UpdateThemesButtonsNowCB(XtPointer, XtIntervalId *)
{
    update_themes_buttons();
}

// TextField reply
static void UpdateThemesButtonsCB(Widget w, XtPointer client_data, 
				  XtPointer)
{
    // The TextField value has not yet changed.  Call again later.
    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0,
		    UpdateThemesButtonsNowCB, client_data);
}

static void HelpOnThemeCB(Widget w, XtPointer client_data, 
			  XtPointer call_data)
{
    // Fetch text from file
    string file = XtName((Widget)client_data);
    string text = vsldoc(file, DispBox::vsllib_path);
    if (text.empty())
	text = "No help available on this theme.";

    MString mtext = bf(basename(file.chars())) + cr() + cr() + rm(text);
    MStringHelpCB(w, XtPointer(mtext.xmstring()), call_data);
}

static void ApplyThemesCB(Widget, XtPointer, XtPointer)
{
    Delay d;

    ThemeManager t;

    for (int i = 0; i < themes_entries.size(); i++)
    {
	Widget button = themes_labels[i];
	bool active = XmToggleButtonGetState(button);

	Widget entry  = themes_entries[i];
	String value_s = XmTextFieldGetString(entry);
	string value = value_s;
	XtFree(value_s);

	t.add(basename(XtName(entry)), ThemePattern(value, active));
    }

    data_disp->set_theme_manager(t);
    update_themes_buttons();
}

static void ResetThemesCB(Widget w, XtPointer client_data, 
			  XtPointer call_data)
{
    CommandGroup cg;

    data_disp->set_theme_manager(ThemeManager(app_data.themes));
    update_themes();
    ApplyThemesCB(w, client_data, call_data);
}


// Register additional info button
void register_info_button(Widget w)
{
    infos_entries += w;
}

// Save `settings' state
void save_settings_state()
{
    if (settings_form == 0)
	return;

    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	settings_initial_values[entry] = settings_values[entry];
    }

    update_reset_settings_button();
    set_need_save_defines(false);
}

// Save `signals' state
void save_signals_state()
{
    if (signals_form == 0)
	return;

    for (int i = 0; i < signals_entries.size(); i++)
    {
	Widget entry = signals_entries[i];
	signals_initial_values[entry] = signals_values[entry];
    }

    update_reset_signals_button();
}

// Process output of `show' command
void process_show(const string& command, string value, bool init)
{
    if (settings_form == 0)
	return;

    if (gdb->type() == DBX && value.freq('\n') > 1)
    {
	// In DBX, we always get all values at once.  Find the
	// appropriate line.
	string base = command.after(rxwhite);
	if (base.contains(rxwhite))
	    base = base.before(rxwhite);

	int column = 0;
	int start_value = -1;
	for (int i = 0; i < int(value.length()); i++)
	{
	    if (column == 0
		&& value.contains(base, i) 
		&& value.contains(rxwhite, i + base.length()))
	    {
		start_value = i;
		break;
	    }

	    if (value[i] == '\n')
		column = 0;
	    else
		column++;
	}

	if (start_value < 0)
	{
#if 0
	    std::cerr << "Warning: cannot set " << quote(base)
		 << " to " << quote(value) << "\n";
#endif
	    return;
	}

	value = value.from(start_value);
	value = value.after(base);
	if (value.contains('\n'))
	    value = value.before('\n');
	strip_space(value);
    }

    if (value.contains('\n'))
	value = value.before('\n');

    if (!init)
	set_status(value);

    if ( gdb->type() == BASH || gdb->type() == DBG || gdb->type() == GDB
	 || gdb->type() == MAKE || gdb->type() == PYDB )
    {
	if (value.contains(" is "))
	    value = value.after(" is ", -1);
        if (value.contains(" is: "))
            value = value.after(" is: ", -1);
	if (value.contains(".", -1))
	    value = value.before(int(value.length()) - 1);
	if (value.contains(". "))
	    value = value.before(". ");

	if (value.contains("assumed to be "))
	    value = value.after("assumed to be ");

	if (value.contains("\"auto;", 0) || 
	    value.contains("set automatically", 0) ||
	    value.contains("auto-detected", 0) ||
            value.contains("auto -", 0) ||
            value.contains("\"auto\"", 0))
	{
	    value = "auto";
	}

	if (value.contains('"', 0))
	    value = unquote(value);
	else if (value.contains(": "))
	    value = value.after(": ");
    }

    if (gdb->type() == PERL)
    {
	if (value.contains('\''))
	{
	    value = value.after('\'');
	    value = value.before('\'');
	}
    }

    static string empty;
    value.gsub(gdb_out_ignore, empty);

    string set_command = command;
    if (set_command.contains("o ", 0))
    {
	// Perl
	if (set_command.contains('?'))
	    set_command = set_command.before('?');
	else if (set_command.contains('='))
	    set_command = set_command.before('=');
    }
    else if (!set_command.contains("set ", 0))
    {
	// GDB
	set_command = "set " + set_command.after(rxwhite);
    }

    Widget button = command_to_widget(settings_form, set_command);
    if (button == 0)
	button = command_to_widget(settings_form, command);
    if (button == 0)
	button = command_to_widget(settings_form, command.after(rxwhite));

    if (button != 0)
    {
	if (!init)
	{
	    // Save current state in undo buffer
	    std::ostringstream command;
	    get_setting(command, gdb->type(), XtName(button), 
			settings_values[button]);
	    undo_buffer.add_command(string(command));
	}

	settings_values[button] = value;
	if (init)
	    settings_initial_values[button] = value;
    }

    if (!init)
	update_reset_settings_button();

    if (button != 0 && XmIsRowColumn(button))
    {
	Widget menu = 0;
	XtVaGetValues(button, XmNsubMenuId, &menu, XtPointer(0));
	if (menu != 0)
	{
	    // Option menu
	    Widget active = XtNameToWidget(menu, value.chars());
	    if (active != 0)
	    {
		XtVaSetValues(button, XmNmenuHistory, active, XtPointer(0));
		return;
	    }
	}
    }
    else if (button != 0 && XmIsToggleButton(button))
    {
	bool set = value != "off" && value != "0" && value != "unlimited" 
	    && value != "false" && value != "insensitive";

	for (int i = 0; i < settings_entries.size(); i++)
	{
	    if (settings_entries[i] == button
		&& settings_entry_types[i] == NoNumToggleButtonEntry)
	    {
		set = !set;
		break;
	    }
	}

	XtVaSetValues(button, XmNset, set, XtPointer(0));
	return;
    }
    else if (button != 0 && XmIsTextField(button))
    {
	XtVaSetValues(button, 
		      XmNvalue,                 value.chars(),
		      XmNcursorPosition,        0,
		      XmNcursorPositionVisible, True,
		      XtPointer(0));
	return;
    }

#if 0
    std::cerr << "Warning: cannot set " << quote(set_command)
	 << " to " << quote(value) << "\n";
#endif
}

// Process output of `handle' command
void process_handle(string output, bool init)
{
    if (signals_form == 0)
	return;

    string undo_command = "";

    while (!output.empty())
    {
	string line = output;
	if (line.contains('\n'))
	    line = line.before('\n');
	output = output.after('\n');

	string base = line.before(rxwhite);
	if (!base.contains("SIG", 0))
	    continue;

	line = line.after(rxwhite);
	static const _XtString titles[3] = { "stop", "print", "pass" };

	for (int word = 0; word <= 2; word++)
	{
	    string value = line.before(rxwhite);
	    line = line.after(rxwhite);

	    value.downcase();
	    bool set = (value == "yes");
	    string name = base + "-" + titles[word];
	    Widget w = XtNameToWidget(signals_form, name.chars());

	    if (w == 0)
	    {
#if 0
		std::cerr << "Warning: cannot set " << base << " " << titles[word]
		     << " to " << quote(value) << "\n";
#endif
		continue;
	    }

	    if (!init && signals_values[w] != value)
	    {
		// Save undoing command
		if (!undo_command.empty())
		    undo_command += '\n';
		undo_command += handle_command(w, !set);
	    }

	    XtVaSetValues(w, XmNset, set, XtPointer(0));

	    signals_values[w] = value;
	    if (init)
		signals_initial_values[w] = value;
	}
    }

    if (!undo_command.empty())
	undo_buffer.add_command(undo_command);

    update_reset_signals_button();
}



// Create settings form

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
    int third_space  = doc.index(' ', second_space + 1);

    if (ing < third_space || third_space < 0)
	return true;

    return false;
}

// Determine entry type
static EntryType entry_type(DebuggerType type,
			    const string& base, 
			    const string& doc,
			    const string& value)
{
#if RUNTIME_REGEX
    static regex rxnonzero1("no[nt][ -]?(0|zero|null)");
    static regex rxnonzero2("!= *(0|zero|null)");
    static regex rxfixednum("[0-9]+(\\.[0-9]+)?");
    static regex rxstring("\".*\"");
#endif

    switch (type)
    {
    case GDB:
	if (base.contains("check", 0))
	    return CheckOptionMenuEntry;
	if (base.contains("follow-fork-mode", 0))
	    return ForkOptionMenuEntry;
	if (base.contains("endian", 0) ||
	    base.contains("architecture", 0))
	    return TargetOptionMenuEntry;
	if (base.contains("disassembly-flavor", 0))
	    return DisassemblyOptionMenuEntry;
	if (base.contains("scheduler-locking", 0))
	    return SchedulerOptionMenuEntry;
	if (base.contains("language", 0) || 
	    base.contains("demangle", 0)
#if GDB_ABI_OPTIONS
            || base.contains("cp-abi", 0)
            || base.contains("osabi", 0)
#endif
	    )
	    return OtherOptionMenuEntry;
	if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
	    return OnOffToggleButtonEntry;
	if ((value.contains("0.\n", -1) || value.contains("1.\n", -1))
	    && is_verb(doc))
	    return NumToggleButtonEntry;
	break;

    case BASH:
    case MAKE:
    case PYDB:
      {
	string field = value.after(" is ");
	field = field.before(".\n", -1);

        if (field.matches(rxstring))
	    return TextFieldEntry;
        if (field.matches(rxfixednum))
	    return TextFieldEntry;
	if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
	    return OnOffToggleButtonEntry;
	if ((value.contains("0.\n", -1) || value.contains("1.\n", -1))
	    && is_verb(doc))
	    return NumToggleButtonEntry;
	break;
       }
    case DBX:
	if ((value == "0" || value == "1") &&
	    !doc.contains("number") && !doc.contains(" > 1"))
	{
	    if (doc.contains(" 1") && !doc.contains(" 2"))
		return NumToggleButtonEntry;
	    if (doc.contains(rxnonzero1) || doc.contains(rxnonzero2))
		return NumToggleButtonEntry;
	}
	if (value.contains("on", 0) || value.contains("off", 0))
	    return OnOffToggleButtonEntry;
	if (value.contains("true", 0) || value.contains("false", 0))
	    return TrueFalseToggleButtonEntry;
	if (value.contains("sensitive", 0) 
	    || value.contains("insensitive", 0))
	    return SensitiveToggleButtonEntry;
	if (has_nr(value))
	    return TextFieldEntry;
	if (base.contains("version")
	    || base.contains("run_io")
	    || base.contains("follow_fork_mode"))
	    return OtherOptionMenuEntry;
	break;

    case DBG:
	if (base.contains("mode", 0))
	    return OnOffToggleButtonEntry;
    	break;

    case XDB:
    case JDB:
    case PERL:
	break;			// FIXME
    }

    // When all else fails, display a text
    return TextFieldEntry;
}


// The GDB question cache
static StringStringAssoc gdb_question_cache;

static string cached_gdb_question(const string& question, int timeout = 0)
{
    string& answer = gdb_question_cache[question];
    if (answer.empty())
	answer = gdb_question(question, timeout);
    return answer;
}

static void clear_gdb_question_cache()
{
    static StringStringAssoc empty;
    gdb_question_cache = empty;
}

static void strip_leading(string& doc, const char *key)
{
    if (doc.contains(key, 0))
	doc = doc.after(key);
    strip_leading_space(doc);
}

static void strip_from(string& doc, const char *key)
{
    if (doc.contains(key))
	doc = doc.before(key);
    strip_leading_space(doc);
}

static void munch_doc(string& doc)
{
    strip_leading_space(doc);

    // Sun DBX 3.0
    strip_leading(doc, "#");
    strip_leading(doc, "If on,");
    strip_leading(doc, "If true,");
    strip_leading(doc, "When on,");
    strip_leading(doc, "When `on',");
    strip_leading(doc, "Set ");
    strip_leading(doc, "Sets ");
    strip_leading(doc, "Governs ");
    strip_leading(doc, "Limit ");
    strip_leading(doc, "Enable/disable ");
    strip_leading(doc, "Allow/disallow ");
    strip_leading(doc, "whether the ");
    strip_leading(doc, "debugger will ");
    strip_leading(doc, "Automatically ");
    strip_leading(doc, "Name of ");

    // SGI DBX
    strip_leading(doc, "-");

    if (doc.contains("0 =>"))
	doc = "Don't" + doc.after("0 =>");

    // DEC and SGI DBX
    strip_leading(doc, "$");
    strip_leading(doc, "if");
    strip_leading(doc, "If");
    strip_leading(doc, "when");
    strip_leading(doc, "When");
    strip_leading(doc, "this ");
    strip_leading(doc, "is ");
    strip_leading(doc, "==");
    strip_leading(doc, "!=");
    strip_leading(doc, "0");
    strip_leading(doc, "1");
    strip_leading(doc, "set");
    strip_leading(doc, "zero");
    strip_leading(doc, "non-zero");
    strip_leading(doc, "non-0");
    strip_leading(doc, "(default)");
    strip_leading(doc, "(the default)");
    strip_leading(doc, ",");
    strip_leading(doc, "=>");
    strip_leading(doc, "implies ");
    strip_leading(doc, "then ");
    strip_leading(doc, "contains ");
    strip_leading(doc, "the ");
    strip_leading(doc, "name of ");
    strip_leading(doc, "specify ");
    strip_leading(doc, "which ");
    strip_leading(doc, "after ");
    strip_leading(doc, "String with ");

    // GDB
    strip_leading(doc, "whether to ");
    strip_leading(doc, "the ");

    // More DEC DBX
    strip_from(doc, "we're looking at");
    strip_from(doc, "we're debugging");
    strip_from(doc, "Default:");
    strip_from(doc, ", default");
    strip_from(doc, " to $DBX");
    strip_from(doc, " entirely");
    strip_from(doc, " rather than");
    strip_from(doc, ", else");
    strip_from(doc, " next to");
    strip_from(doc, " used to");
    strip_from(doc, " that was");
    strip_from(doc, " accessing");

    // More SGI DBX
    strip_from(doc, " if none");
    strip_from(doc, " (init");
    strip_from(doc, " (default");
    strip_from(doc, " ($");
    strip_from(doc, " ( $");
    strip_from(doc, " (with");
    strip_from(doc, " However ");
    strip_from(doc, " This ");
    strip_from(doc, " Defaults ");
    strip_from(doc, " $");

    doc.gsub(" " + downcase(gdb->title()), " " + gdb->title());
    if (doc.contains(downcase(gdb->title()), 0))
	doc = gdb->title() + doc.after(downcase(gdb->title()));

    if (doc.length() > 0)
	doc[0] = toupper(doc[0]);

    if (gdb->type() == GDB)
	doc.gsub('_', ' ');

    if (doc.length() > 60)
	doc = doc.before(57) + "...";
}


// Get DBX documentation
static string _get_dbx_help(const string& dbxenv, const string& base)
{
    string dbx_help;
    if (dbxenv == "dbxenv")
	dbx_help = cached_gdb_question("help dbxenv", -1);
    if (dbx_help.freq('\n') <= 2)
	dbx_help = cached_gdb_question("help $variable", -1);
    if (dbx_help.freq('\n') <= 2)
	dbx_help = cached_gdb_question("help variable", -1);
    if (dbx_help.freq('\n') <= 2)
	dbx_help = cached_gdb_question("help $variables", -1);

    // Find documentation in DBX_HELP
    int column = 0;
    int start_help = -1;
    int i;
    for (i = 0; i < int(dbx_help.length()); i++)
    {
	if (column == 0 
	    && (dbx_help.contains(base, i) 
		|| dbx_help.contains(dbxenv + " " + base, i)))
	{
	    start_help = i;
	    break;
	}

	if (dbx_help[i] == '\n')
	    column = 0;
	else
	    column++;
    }

    if (start_help < 0)
    {
	dbx_help = base;
	dbx_help.gsub('_', ' ');
	return dbx_help;		// Simple default
    }

    int end_help = -1;
    for (i++, column++; i < int(dbx_help.length()); i++)
    {
	if (column == 0 && !isspace(dbx_help[i]))
	{
	    end_help = i;
	    break;
	}

	if (dbx_help[i] == '\n')
	    column = 0;
	else
	    column++;
    }

    if (end_help >= 0)
	dbx_help = dbx_help.at(start_help, end_help - start_help);
    else
	dbx_help = dbx_help.from(start_help);

    return dbx_help;
}

static void compress_spaces(string& s, int width)
{
    char *buffer = new char[s.length() + 1];
    char *t = buffer;

    int column = 0;
    for (int i = 0; i < int(s.length()); i++)
    {
	if (isspace(s[i]))
	{
	    while (i < int(s.length()) && isspace(s[i]))
		i++;

	    if (width > 0 && column >= width)
	    {
		*t++ = '\n';
		column = 0;
	    }
	    else
	    {
		*t++ = ' ';
		column++;
	    }
	}

	if (i < int(s.length()))
	{
	    *t++ = s[i];
	    column++;
	}
    }
    *t++ = '\0';

    s = buffer;
    delete[] buffer;
}

// Same, but for usage in help messages
static string get_dbx_help(const string& dbxenv, const string& base, int width)
{
    string dbx_help = _get_dbx_help(dbxenv, base);
    dbx_help = dbx_help.after(base);
    strip_leading_space(dbx_help);

    if (dbx_help.contains("  - "))
    {
	dbx_help = dbx_help.after("  - ");
	strip_leading_space(dbx_help);
    }
    else if (dbx_help.contains(" # "))
    {
	dbx_help = dbx_help.after(" # ");
	strip_leading_space(dbx_help);
    }

    // Remove remaining `# ' prefixes (Solaris DBX)
    dbx_help.gsub("  # ", "");
    dbx_help.gsub(" # ", " number ");

    // Reduce spaces (Solaris DBX & DEC DBX)
    compress_spaces(dbx_help, width);

    return dbx_help;
}

string get_dbx_help(const string& dbxenv, const string& base)
{
    return get_dbx_help(dbxenv, base, 60);
}

struct DBXTranslation {
    const _XtString base;
    const _XtString doc;
};

static DBXTranslation const dbx_translations[] = 
{
    {"$addrfmt64", "C format for address printing (64bit)"},
    {"$assignverify", "Verify assignments"},
    {"$byteaccess", "Access memory items less than 4 bytes"},
    {"$ctypenames", "Support C type names"},
    {"$dispix", "Display pixie instructions"},
    {"$fp_precise", "Floating point precise mode"},
    {"$framereg", "Registers are with respect to the current frame"},
    {"$groupforktoo", "Group fork too"},
    {"$hexdoubles", "Display float values as float and hex"},
    {"$newevent", "Last new event number"},
    {"$newpgrpevent", "Last new pgrp event number"},
    {"$newrecord", "Last record number"},
    {"$nonstop", "Stop the debugged process"},
    {"$pager", "Pager"},
    {"$pendingtraps", "Pending traps"},
    {"$piaddtohist", "Add playback commands to history"},
    {"$print_exception_frame", "Print exception frames"},
    {"$shellparameters", "Shell parameters"},
    {"$showbreakaddrs", "Show the address of placed breakpoints"},
    {"$showfilename", "Show file name when stopping"},
    {"$sourcepathrule", "Rule for accessing source paths"},
    {"$stacktracelimit", "Stack trace limit"},
    {"$stdc", "Support Standard C"},
    {"$stepintoall", "Step into all procedures"},
    {"$stopformat", "Stop format"},
    {"$whereisdsolimit", "Whereis limit"},
    {"disassembler_version", "Disassembler version"},
    {"follow_fork_inherit", "When following child, inherit events"},
    {"follow_fork_mode", "When process forks"},
    {"output_list_size", "Lines to print in the `list' command"},
    {"rtc_biu_at_exit", "Produce memory report at exit"},
    {"run_autostart", "Let `step' and `next' implicitly start the program"},
    {"run_io", "Redirect I/O to"},
    {"suppress_startup_message", "Suppress startup message below release"}
};

static string get_dbx_doc(const string& dbxenv, const string& base)
{
    // Some specials
    for (int i = 0; i < int(XtNumber(dbx_translations)); i++)
    {
	if (base == dbx_translations[i].base)
	    return dbx_translations[i].doc;
    }
    
    // Generic help
    string dbx_doc = get_dbx_help(dbxenv, base, -1);
    strip_leading_space(dbx_doc);

    dbx_doc.gsub("etc. ", "etc ");
    if (dbx_doc.contains(". "))
	dbx_doc = dbx_doc.before(". ");
    if (dbx_doc.contains("; "))
	dbx_doc = dbx_doc.before("; ");
    dbx_doc.gsub("etc ", "etc. ");

#if 0
    if (dbx_doc.empty())
    {
	dbx_doc = base;
	dbx_doc.gsub('_', ' ');
    }
#endif

    munch_doc(dbx_doc);

    return dbx_doc;
}

static Dimension preferred_width(Widget w)
{
    if (w == 0)
	return 0;

    XtWidgetGeometry size;
    size.request_mode = CWWidth;
    XtQueryGeometry(w, (XtWidgetGeometry *)0, &size);

    int left_offset  = 0;
    int right_offset = 0;
    XtVaGetValues(w, XmNleftOffset, &left_offset, 
		  XmNrightOffset, &right_offset, XtPointer(0));

    return size.width + left_offset + right_offset;
}

static Dimension preferred_height(Widget w)
{
    if (w == 0)
	return 0;

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(w, (XtWidgetGeometry *)0, &size);
    return size.height;
}

static void add_settings(Widget form, int& row, Dimension& max_width,
			 DebuggerType type, EntryType entry_filter, 
			 const string& gdb_class = "set");

static Widget create_signal_button(Widget label,
				   const string& name,
				   int row,
				   Widget rightmost = 0)
{
    Arg args[15];
    Cardinal arg = 0;

    MString lbl(capitalize(name));
    XtSetArg(args[arg], XmNlabelString, lbl.xmstring()); arg++;

    if (rightmost != 0)
    {
	XtSetArg(args[arg], XmNrightAttachment, XmATTACH_WIDGET); arg++;
	XtSetArg(args[arg], XmNrightWidget,     rightmost);       arg++;
    }
    else
    {
	XtSetArg(args[arg], XmNrightAttachment, XmATTACH_FORM); arg++;
    }

    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNtopPosition,      row);                arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);            arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_CENTER); arg++;

    Widget w = 0;
    if (name == "send")
    {
	w = verify(XmCreatePushButton(XtParent(label), XMST(name.chars()), args, arg));
	XtManageChild(w);

	XtAddCallback(w, XmNactivateCallback, SendSignalCB, XtPointer(label));
    }
    else
    {
	string fullname = string(XtName(label)) + "-" + name;
	w = verify(XmCreateToggleButton(XtParent(label), XMST(fullname.chars()), args, arg));
	XtManageChild(w);

	XtAddCallback(w, XmNvalueChangedCallback, SignalCB, XtPointer(label));
    }

    return w;
}

// Get `show' command for settings command CMD
string show_command(const string& cmd, DebuggerType type)
{
    string show = "";
    switch (type)
    {
    case BASH:
    case DBG:
    case GDB:
    case MAKE:
    case PYDB:
	show = "show ";
	if (cmd.contains("set ", 0))
	    show += cmd.after("set ");
	else if (cmd.contains("directory ", 0))
	    show += "directories";
	else if (cmd.contains("path ", 0))
	    show += "paths";
	else
	    show += cmd;

	if (show.freq(' ') >= 2)
	{
	    // Strip last argument from `show' command
	    int index = show.index(' ', -1);
	    show = show.before(index);
	}
	break;

    case DBX:
	show = cmd.before(rxwhite);
	break;

    case PERL:
	if (cmd.contains('='))
	    show = cmd.before('=') + "?";
	else
	    show = cmd + "?";
	break;

    case JDB:
    case XDB:
	break;
    }

    return show;
}

// In Perl, make these options insensitive
static const _XtString const perl_taboos[] = 
{
    "TTY",
    "noTTY",
    "ReadLine",
    "NonStop",
    "ornaments",
    "compactDump",
    "veryCompact",
    "hashDepth",
    "arrayDepth",
    "dumpDepth"
};

// Add single button
static void add_button(Widget form, int& row, Dimension& max_width,
		       DebuggerType type, EntryType entry_filter,
		       string line)
{
    if (line.empty())
	return;

    string set_command;		// Command to create the setting
    string show_command;	// Command to display the value
    string doc;			// Documentation string
    string base;		// Common base of set_command and show_command
    string value;		// Current variable value
    EntryType e_type;		// Type of the entry

    bool is_set = true;		// Command sets a value
    bool is_add = false;	// Command adds a value

    if (entry_filter == ThemeEntry)
    {
	set_command = show_command = line;
	e_type = entry_filter;
	doc = vsldoc(line, DispBox::vsllib_path);
	if (doc.empty())		   // No documentation:
	    doc = basename(line.chars());  // Use base name of file instead
	else if (doc.contains("."))
	    doc = doc.before("."); // Use first sentence only
    }
    else
    {
	switch (type)
	{
	case BASH:
	case DBG:
	case GDB:
	case MAKE:
	case PYDB:
	{
	    if (!line.contains(" -- ") && 
		(entry_filter != SignalEntry || (!line.contains("SIG", 0) &&
						 !line.contains("all", 0))))
		return;			// No help line

	    set_command  = line.before(" -- ");
	    doc          = line.after(" -- ");
	    base         = set_command.after(' ');
	    if (base.empty())
		base = set_command;
	    show_command = "show " + base;

#if GDB_AMBIGUOUS_SHOW_PATH
	    if (base == "path")
               show_command = "show paths";
#endif

	    if (entry_filter == SignalEntry)
	    {
		if (!line.contains("all", 0) && !line.contains("SIG", 0))
		    return;		// No signal line

		// Special handling for signal tables
		base = line.before(rxwhite);
		doc  = line.after(rxwhite);
		doc  = doc.after(rxwhite);
		doc  = doc.after(rxwhite);
		doc  = doc.after(rxwhite);
		e_type = SignalEntry;
	    }
	    else if (entry_filter != DisplayToggleButtonEntry)
	    {
		if (base == "args")
		    return; // Already handled in `Run...' editor

		if (base == "radix")
		    return; // Already handled in input- and output-radix

#if GDB_BROKEN_SET_INTERPRETER
                // Terry Teague <terry_teague@users.sourceforge.net> reports
		// that GDB 5.x on Mac OS X (prior to 10.4) chokes on
		// "set interpreter console" when sourcing the gdbSettings on
		// initialization.
                if (base == "interpreter")
                    return;
#endif

		// GDB 4.18 provides `set extension-language', but not the
		// equivalent `show extension-language'.
		if (base == "extension-language")
		    return;

		if (doc.contains("deprecated"))
		    return;	// Won't support this

		is_set = doc.contains("Set ", 0);
		is_add = doc.contains("Add ", 0);

		if (!is_set && !is_add)
		{
		    // Generic command or `set variable' - list `set'
		    // subcommands
		    add_settings(form, row, max_width, 
				 type, entry_filter, set_command);
		    return;
		}

		value = cached_gdb_question(show_command);
		if (is_set && value.freq('\n') > 1 && type != MAKE)
		{
		    // Generic command - list `set' subcommands
		    add_settings(form, row, max_width,
				 type, entry_filter, set_command);
		    return;
		}

		if (is_set && 
		    !value.contains(" is ") && 
		    !value.contains(".\n", -1))
		    return;

		e_type = entry_type(type, base, doc, value);
		if (e_type != entry_filter)
		    return;

		if (is_set)
		    doc = doc.after("Set ");
		else if (is_add)
		    doc = doc.after("Add ");

		if (is_add && doc.contains("of "))
		    doc = doc.after("of ");
	    }
	    else
	    {
		if (base == "signals")
		    return;	// We already have `info handle'.
		if (base == "watchpoints")
		    return;	// We already have `info breakpoints'.
		if (base == "target")
		    return;	// We already have `info files'.
		if (base == "address")
		    return;	// `info address' requires an argument.
		if (base == "line")
		    return;	// `info line' requires an argument.

		// These infos produce too much output for the data window
		// to be of any use.  Even worse, a data display as large
		// as the one generated by `info functions' causes my X
		// terminal to crash. - AZ
		if (base == "types")
		    return;
		if (base == "functions")
		    return;
		if (base == "variables")
		    return;

		e_type = DisplayToggleButtonEntry;
		if (e_type != entry_filter)
		    return;

		strip_leading(doc, "Show ");
		strip_leading(doc, "Print ");
		strip_leading(doc, "out ");
		strip_leading(doc, "the ");
	    }
	    munch_doc(doc);
	    break;
	}

	case DBX:
	{
	    if (!line.contains("$", 0) 
		&& !(line.contains(rxidentifier, 0) && islower(line[0])))
		return;			// No help line

	    base = line.before(rxwhite);
	    if (base.empty())
		return;
	    if (base == "run_savetty")
		return; // Makes no sense under a GUI

	    value = line.after(rxwhite);
	    if (value.contains('=', 0))
	    {
		// Ladebug `set' output is `$VAR = VALUE'
		value = value.after('=');
		value = value.after(rxwhite);
	    }


	    string dbxenv;
	    if (base[0] == '$')
		dbxenv = "set";
	    else
		dbxenv = "dbxenv";

	    e_type = entry_type(type, base, 
				get_dbx_help(dbxenv, base), value);
	    if (e_type != entry_filter)
		return;

	    set_command = dbxenv + " " + base;
	    show_command = set_command + " " + value;
	    doc = get_dbx_doc(dbxenv, base);

#if RUNTIME_REGEX
	    static regex rxdont("Do ?n['o]t");
#endif
	    if (doc.contains(rxdont, 0) && e_type == NumToggleButtonEntry)
	    {
		doc = doc.after(rxdont);
		munch_doc(doc);
		e_type = NoNumToggleButtonEntry;
	    }
	    break;
	}

	case JDB:
	{
	    // All we have is the `use' command.
	    e_type = TextFieldEntry;
	    set_command  = "use";
	    show_command = "use";
	    doc          = "Class path";
	    value = source_view->class_path();
	    break;
	}

	case XDB:
	    return;			// FIXME

	case PERL:
	{
	    e_type = TextFieldEntry;
	    base  = line.before(" = ");
	    strip_space(base);

	    value = unquote(line.after(" = "));

// removed for perl-5.8.x
//	    if (value == "N/A")
//		value = "";

	    set_command  = "o " + base;
	    show_command = "o " + base + "?";
	    doc = base;
	    break;
	}
	}

	if (e_type != entry_filter)
	    return;

	if (doc.empty())
	    return;		// No need to support undocumented stuff
    }

    Arg args[15];
    int arg;

    // Add label
    Widget label = 0;
    Widget entry = 0;

    String set_command_s = new char[set_command.length() + 1];
    strcpy(set_command_s, set_command.chars());

    MString labelString(doc);
    arg = 0;
    XtSetArg(args[arg], XmNlabelString,      labelString.xmstring()); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);          arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);      arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);                arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_BEGINNING);  arg++;

    XtCallbackProc callback = 0;

    switch (e_type)
    {
    case OnOffToggleButtonEntry:
	callback = SetOnOffCB;
	break;

    case TrueFalseToggleButtonEntry:
	callback = SetTrueFalseCB;
	break;

    case SensitiveToggleButtonEntry:
	callback = SetSensitiveCB;
	break;

    case NumToggleButtonEntry:
	callback = SetNumCB;
	break;

    case NoNumToggleButtonEntry:
	callback = SetNoNumCB;
	break;

    case DisplayToggleButtonEntry:
	callback = SetDisplayCB;
	break;

    case ThemeEntry:
	callback = ApplyThemesCB;
	break;

    default:
	callback = 0;
	break;
    }

    if (callback == 0)
    {
	if (is_set)
	    label = verify(XmCreateLabel(form, XMST(base.chars()), args, arg));
	else {
	  const string s1 = string("the") + base;
	  label = verify(XmCreateLabel(form, XMST(s1.chars()), args, arg));
	}

	XtManageChild(label);
    }
    else
    {
	entry = label = 
	    verify(XmCreateToggleButton(form, XMST(set_command.chars()), args, arg));
	XtManageChild(label);

	XtAddCallback(entry, XmNvalueChangedCallback,
		      callback, XtPointer(set_command_s));
    }

    // Add help button
    arg = 0;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);      arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNtopPosition,      row);                arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION);  arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);            arg++;
    XtSetArg(args[arg], XmNalignment,        XmALIGNMENT_CENTER); arg++;
    Widget help = verify(XmCreatePushButton(form, XMST("help"), args, arg));
    XtManageChild(help);

    Widget send  = 0;
    Widget pass  = 0;
    Widget print = 0;
    Widget stop  = 0;
    if (e_type == SignalEntry)
    {
	send  = create_signal_button(label, "send",  row, help);
	pass  = create_signal_button(label, "pass",  row, send);
	print = create_signal_button(label, "print", row, pass);
	stop  = create_signal_button(label, "stop",  row, print);

	if (base == "all")
	{
	    set_sensitive(send, false);
	    XtVaSetValues(pass,  XmNset, True, XtPointer(0));
	    XtVaSetValues(print, XmNset, True, XtPointer(0));
	    XtVaSetValues(stop,  XmNset, True, XtPointer(0));
	}
    }

    // Add entry
    switch (e_type)
    {
    case OnOffToggleButtonEntry:
    case TrueFalseToggleButtonEntry:
    case SensitiveToggleButtonEntry:
    case NumToggleButtonEntry:
    case NoNumToggleButtonEntry:
    case DisplayToggleButtonEntry:
    case SignalEntry:
	// All is done
	break;

    case CheckOptionMenuEntry:
    {
	// `set check'
	arg = 0;
	Widget menu = verify(XmCreatePulldownMenu(form, 
						  XMST("menu"), args, arg));

	// Possible options are contained in the help string
	string options = cached_gdb_question("help " + set_command);
	options = options.from('(');
	options = options.before(')');

	while (!options.empty())
	{
	    string option = options.after(0);
	    option = option.through(rxalpha);
	    options = options.after(rxalpha);

	    arg = 0;
	    Widget button = 
		verify(XmCreatePushButton(menu, XMST(option.chars()), args, arg));
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
	entry = verify(XmCreateOptionMenu(form, XMST(set_command.chars()), args, arg));
	XtManageChild(entry);

	Widget option_label = XmOptionLabelGadget(entry);
	XtUnmanageChild(option_label);
	break;
    }

    case OtherOptionMenuEntry:
    case TargetOptionMenuEntry:
    case ForkOptionMenuEntry:
    case DisassemblyOptionMenuEntry:
    case SchedulerOptionMenuEntry:
    {
#if GDB_ABI_OPTIONS
        // set cp-abi / set osabi /
#endif
	// set language / set demangle / set architecture / set endian /
	// set follow-fork-mode / set disassembly-flavor / 
	// set scheduler-locking
	arg = 0;
	Widget menu = verify(XmCreatePulldownMenu(form, 
						  XMST("menu"), args, arg));

	string options;
	char separator = '\n';

	switch (gdb->type())
	{
	case GDB:
	    options = cached_gdb_question("set " + base);
	    if((base == "architecture")
	    || (base == "demangle-style")
	    || (base == "disassembly-flavor")
	    || (base == "endian")
	    || (base == "follow-fork-mode")
	    || (base == "scheduler-locking")
#if GDB_ABI_OPTIONS
            || (base == "osabi")
#endif
	       )
	    {
		// First look for the reponse in the format of GDB-5.x or
		// newer. Possible options are listed upon `set endian'
		// (or whatever) and separated by `,'.
		if (options.contains("Requires an argument. Valid arguments are"))
		{
		    strip_leading(options, "Requires an argument. Valid arguments are");
		    // remove trailing .\n (if any)
		    if (options.contains(".\n"))
		    {
			options = options.before(".\n");
		    }
		    separator = ',';
		}
                else if (options.contains("Requires an argument. Valid values are"))
                {
                    strip_leading(options, "Requires an argument. Valid values are");
                    // remove trailing .\n (if any)
                    if (options.contains(".\n"))
                    {
                        options = options.before(".\n");
                    }
                    separator = ',';
                }
		// OK, so it doesn't match GDB-5.x output; go with 4.x format
		else if (base == "architecture")
		{
		    // Possible options are listed upon `info architecture'
		    // and separated by ` '.
		    options = cached_gdb_question("info " + base);
		    options = "auto" + options.from('\n');
		    separator = ' ';
		}
		else if (base == "disassembly-flavor")
		{
		    // Hardwired options
		    options = "intel\natt\n";
		}
		else if (base == "endian")
		{
		    // Hardwired options
		    options = "auto\nbig endian\nlittle endian\n";
		}
		else if (base == "follow-fork-mode")
		{
		    // Hardwired options
		    options = "parent\nchild\nask\n";
		}
		else if (base == "scheduler-locking")
		{
		    // Hardwired options
		    options = "off\non\nstep\n";
		}
#if GDB_ABI_OPTIONS
                else if (base == "cp-abi")
                {
                    strip_leading(options, "The available C++ ABIs are:");
                }
#endif
	    }
	    break;

	case DBX:
	    options = _get_dbx_help("dbxenv", base);
	    options = options.after('<');
	    options = options.before('>');

	    if (options.empty())
	    {
		if (base == "follow_fork_mode")
		    options = "parent|child|both|ask";
	    }
	    separator = '|';
	    break;

	case BASH:
	case DBG:
	case JDB:
	case MAKE:
	case PERL:
	case PYDB:
	case XDB:
	    return;		// FIXME
	}

	while (!options.empty())
	{
	    string option = options;
	    if (option.contains(separator))
		option = option.before(separator);
	    options = options.after(separator);

	    strip_space(option);

	    string label = option;
	    if (gdb->type() == GDB && option.contains("  "))
	    {
		label = option.after("  ");
		label = label.after(rxwhite);

		if (option.contains(" auto"))
		    option = "auto";
		else
		    option = option.before(rxwhite);
	    }

#if GDB_ABI_OPTIONS
            else if (gdb->type() == GDB && option.contains(" - "))
            {
                 label = option.after(" - ");
                 option = option.before(" - ");
                 if (option.contains("auto"))
		    label = "auto";
            }
#endif

	    if (option.empty() || option.contains(':', -1))
		continue;

	    MString xmlabel(label);
	    arg = 0;
	    if (gdb->type() == GDB)
	    {
		XtSetArg(args[arg], XmNlabelString, 
			 xmlabel.xmstring()); arg++;
	    }
	    Widget button = 
		verify(XmCreatePushButton(menu, XMST(option.chars()), args, arg));
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
	entry = verify(XmCreateOptionMenu(form, XMST(set_command.chars()), args, arg));
	XtManageChild(entry);

	Widget option_label = XmOptionLabelGadget(entry);
	XtUnmanageChild(option_label);
	break;
    }

    case ThemeEntry:
    case TextFieldEntry:
    {
	// Some other value
	arg = 0;
	XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
	XtSetArg(args[arg], XmNrightWidget,      help);              arg++;
	XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
	XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
	entry = verify(XmCreateTextField(form, XMST(set_command.chars()), args, arg));
	XtManageChild(entry);

	if (e_type == TextFieldEntry)
	    XtAddCallback(entry, XmNvalueChangedCallback, 
			  UpdateSettingsButtonsCB, XtPointer(0));
	else // ThemeEntry
	    XtAddCallback(entry, XmNvalueChangedCallback, 
			  UpdateThemesButtonsCB, XtPointer(0));
    }
    }

    Widget rightmost = help;
    if (entry != label)
	rightmost = entry;
    if (stop != 0)
        rightmost = stop;

    Dimension width = preferred_width(label);
    if (entry != label)
	width += preferred_width(entry);
    width += preferred_width(send);
    width += preferred_width(stop);
    width += preferred_width(print);
    width += preferred_width(pass);
    width += preferred_width(help);

    // Add leader

    // Align leader with label font baseline
    XmFontList font_list   = 0;
    XmString xmlabel       = 0;
    Dimension marginHeight = 0;
    Dimension marginTop    = 0;
    XtVaGetValues(label,
		  XmNfontList,     &font_list,
		  XmNlabelString,  &xmlabel,
		  XmNmarginHeight, &marginHeight,
		  XmNmarginTop,    &marginTop,
		  XtPointer(0));

    Dimension baseline = XmStringBaseline(font_list, xmlabel);
    static const Dimension leaderHeight = 2;

    Dimension top_offset = baseline + marginHeight + marginTop - leaderHeight;
    XmStringFree(xmlabel);

    arg = 0;
    XtSetArg(args[arg], XmNseparatorType,    XmSINGLE_DASHED_LINE); arg++;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNleftWidget,       label);             arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_WIDGET);   arg++;
    XtSetArg(args[arg], XmNrightWidget,      rightmost);         arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET); arg++;
    XtSetArg(args[arg], XmNtopWidget,        label);             arg++;
    XtSetArg(args[arg], XmNtopOffset,        top_offset);        arg++;
    Widget leader = verify(XmCreateSeparator(form, 
					     XMST("leader"), args, arg));
    XtManageChild(leader);

    // Add help callback
    switch (e_type)
    {
    case SignalEntry:
	XtAddCallback(help, XmNactivateCallback, 
		      HelpOnSignalCB, XtPointer(label));
	break;

    case ThemeEntry:
	XtAddCallback(help, XmNactivateCallback, 
		      HelpOnThemeCB, XtPointer(label));
	break;

    default:
	XtAddCallback(help, XmNactivateCallback, 
		      HelpOnThisCB, XtPointer(entry));
	break;
    }

    if (e_type == SignalEntry)
    {
	if (base != "all")
	{
	    // Initialize button
	    process_handle(line, true);

	    // Register buttons
	    signals_entries += stop;
	    signals_entries += print;
	    signals_entries += pass;
	}
    }
    else if (e_type == ThemeEntry)
    {
	// Register entry
	themes_entries += entry;
	themes_labels  += label;
    }
    else if (e_type != DisplayToggleButtonEntry)
    {
	// Make entry insensitive if part of initialization commands.
	string init = app_data.gdb_init_commands;
	int idx = init.index(set_command);
	bool insensitive = (idx == 0 || (idx > 0 && init[idx - 1] == '\n'));

	// Make entry insensitive if one of Perl taboos
	if (!insensitive && gdb->type() == PERL)
	{
	    for (int i = 0; i < int(XtNumber(perl_taboos)); i++)
		if (base == perl_taboos[i])
		{
		    insensitive = true;
		    break;
		}
	}

	if (insensitive)
	{
	    set_sensitive(entry,  false);
	    set_sensitive(label,  false);
	    set_sensitive(leader, false);
	}

	// Initialize button
	process_show(show_command, value, true);

	// Register entry
	settings_entries     += entry;
	settings_entry_types += e_type;
    }
    else
    {
	// Register entry
	infos_entries += entry;
    }

    max_width = max(width, max_width);

    row++;
}

// Add separator
static void add_separator(Widget form, int& row)
{
    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_POSITION); arg++;
    XtSetArg(args[arg], XmNtopPosition,      row);               arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
    Widget sep = verify(XmCreateSeparator(form, XMST("sep"), args, arg));
    XtManageChild(sep);
    row++;
}

// Add buttons
static void add_settings(Widget form, int& row, Dimension& max_width,
			 DebuggerType type, EntryType entry_filter,
			 const string& gdb_class)
{
    string commands;

    switch (type)
    {
    case GDB:
    case BASH:
    case MAKE:
    case PYDB:
    case DBG:
	if (entry_filter == SignalEntry)
	    commands = cached_gdb_question("info handle");
	else
	    commands = cached_gdb_question("help " + gdb_class);
	break;

    case XDB:
	commands = cached_gdb_question("I");
	break;

    case DBX:
    {
	// Some DBXes know `dbxenv'; others know `set'.  We cannot use
	// is_valid() or likewise here, since the output may contain
	// `error' strings that would cause is_valid() to fail.  Just
	// try both and use the longer reply (may take time).

	// Exception: on Sun DBX, `set' causes a dump of all
	// environment variables.  Don't use this.
	string dbxenv = cached_gdb_question("dbxenv", -1);
	string set    = cached_gdb_question("set", -1);
	if (dbxenv.length() > set.length() ||
	    set.contains(DDD_NAME "=" ddd_NAME))
	{
	    commands = dbxenv;
	}
	else
	{
	    // Ladebug may echo `set'
	    if (set.contains("set ", 0))
		set = set.after("set ");
	    strip_leading_space(set);
	    commands = set;
	}
	break;
    }

    case JDB:
    {
	// In JDB, all we have is the `use' command.
	add_button(form, row, max_width, type, entry_filter, "use");
	break;
    }

    case PERL:
	commands = cached_gdb_question("o");
	break;
    }

    if (type == GDB && entry_filter == SignalEntry)
    {
	// Add an entry for `handle all'
	add_button(form, row, max_width, type, entry_filter,
		   "all\tYes\tYes\tYes\tAll Signals");
	// add_separator(form, row);
    }

    if (commands.contains('\n'))
    {
	while (!commands.empty())
	{
	    string line = commands.before('\n');
	    commands    = commands.after('\n');
	    add_button(form, row, max_width, type, entry_filter, line);
	}
    }
    else
    {
	// Ladebug gives us all settings in one line.
	commands += '$';
	while (!commands.empty())
	{
	    commands = commands.from('$');
	    int idx = commands.index('$');
	    idx = commands.index('$', idx + 1);
	    string line = commands.before(idx);
	    commands    = commands.after(line);

	    add_button(form, row, max_width, type, entry_filter, line);
	}
    }
}

// Reload all settings
static void reload_all_settings()
{
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	string cmd = string(XtName(entry)) + " dummy";
	string show = show_command(cmd, gdb->type());
	string value = gdb_question(show);
	if (value != NO_GDB_ANSWER)
	    process_show(show, value, true);
    }
}

void update_settings()
{
    if (settings_panel != 0 && XtIsManaged(settings_panel))
    {
	reload_all_settings();
	need_reload_settings = false;
    }
}

// Reset settings
static void ResetSettingsCB(Widget, XtPointer, XtPointer)
{
    CommandGroup cg;

    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];

	string value = settings_values[entry];
	if (settings_entry_types[i] == TextFieldEntry)
	{
	    String value_s = XmTextFieldGetString(entry);
	    value = value_s;
	    XtFree(value_s);

	    if (value != settings_values[entry])
	    {
		value = settings_values[entry];
		XmTextFieldSetString(entry, XMST(value.chars()));
	    }
	}

	if (value != settings_initial_values[entry])
	    gdb_set_command(XtName(entry), settings_initial_values[entry]);
    }
}

// Reset signals
static void ResetSignalsCB(Widget, XtPointer, XtPointer)
{
    CommandGroup cg;

    string command = "";

    for (int i = 0; i < signals_entries.size(); i++)
    {
	Widget entry = signals_entries[i];
	if (signals_initial_values[entry] != signals_values[entry])
	{
	    bool set = (signals_initial_values[entry] == "yes");
	    command += handle_command(entry, set) + "\n";
	}
    }

    gdb_command(command);
}

// Apply settings
static void ApplySettingsCB(Widget, XtPointer, XtPointer)
{
    CommandGroup cg;

    for (int i = 0; i < settings_entries.size(); i++)
    {
	if (settings_entry_types[i] != TextFieldEntry)
	    continue;

	Widget entry = settings_entries[i];

	String value_s = XmTextFieldGetString(entry);
	string value(value_s);
	XtFree(value_s);

	if (value != settings_values[entry])
	    gdb_set_command(XtName(entry), value);
    }
}

// Delete all infos
static void DeleteAllInfosCB (Widget, XtPointer, XtPointer)
{
    for (int i = 0; i < infos_entries.size(); i++)
    {
	Widget entry = infos_entries[i];
	XmToggleButtonSetState(entry, False, True);
    }
}

// Fetch help for specific COMMAND
static string get_help_line(const string& command, DebuggerType /*type*/)
{
    string reply = cached_gdb_question("help " + command);
    reply = reply.before('\n');
    if (reply.contains('.'))
	reply = reply.before('.');
    reply = command + " -- " + reply;
    return reply;
}

// ClipWindow translation stuff
static void ClipDo(Widget w, XEvent *event, 
		   String *params, Cardinal *num_params)
{
    Widget scroll = XtParent(w);
    Widget scrollbar = 0;
    XtVaGetValues(scroll, XmNverticalScrollBar, &scrollbar, XtPointer(0));
    XtCallActionProc(scrollbar, params[0], event, params + 1, *num_params - 1);
}

static void fix_clip_window_translations(Widget scroll)
{
    // The scrolled clip window grabs translations for osfBeginLine
    // and osfEndLine, which we need in our text fields.  Since we
    // cannot selectively disable these translations, we disable all
    // translations ...

    Widget clip = 0;
    XtVaGetValues(scroll, XmNclipWindow, &clip, XtPointer(0));
//  XtUninstallTranslations(clip);

    // ... and provide a minimal set instead.
    static bool have_clip_actions = false;
    if (!have_clip_actions)
    {
	static XtActionsRec clip_actions[] = {
	    {XTARECSTR("clip-do"), ClipDo}
	};

	XtAppAddActions(XtWidgetToApplicationContext(scroll), 
			clip_actions, XtNumber(clip_actions));
	have_clip_actions = true;
    }

    const _XtString const clip_translations =
	"<Key>osfPageUp:         clip-do(PageUpOrLeft, 0)\n"
	"<Key>osfPageDown:       clip-do(PageDownOrRight, 0)\n"
	"Ctrl <Key>osfBeginLine: clip-do(TopOrBottom)\n"
	"Ctrl <Key>osfEndLine:   clip-do(TopOrBottom)\n"
        "<Key>osfUp:             clip-do(IncrementUpOrLeft, 0)\n"
        "<Key>osfDown:           clip-do(IncrementDownOrRight, 0)\n";


    XtTranslations tr = XtParseTranslationTable(clip_translations);
    XtOverrideTranslations(clip, tr);
}


// Themes

// Return all available themes
void get_themes(StringArray& themes)
{
    StringArray bases;

    string path = DispBox::vsllib_path;
    int n = path.freq(':');
    string *dirs = new string[n + 1];
    split(path, dirs, n + 1, ':');

    for (int i = 0; i < n; i++)
    {
	string mask = dirs[i] + "/*.[vV][sS][lL]";
	char **files = glob_filename(mask.chars());
	if (files == (char **)0)
	{
	    std::cerr << mask << ": glob failed\n";
	}
	else if (files == (char **)-1)
	{
	    if (errno != 0)
		post_warning(string(mask) + ": " + strerror(errno), 
			     "no_themes_warning");
	}
	else
	{
	    int count;
	    for (count = 0; files[count] != 0; count++)
		;
	    smart_sort(files, count);

	    for (int i = 0; i < count; i++)
	    {
		string base = basename(files[i]);

		bool duplicate = false;
		for (int j = 0; !duplicate && j < bases.size(); j++)
		    if (bases[j] == base)
			duplicate = true;

		if (!duplicate)
		{
		    themes += files[i];
		    bases  += base;
		}

		free(files[i]);
	    }
	    free((char *)files);
	}
    }

    delete[] dirs;
}

static void add_themes(Widget form, int& row, Dimension& max_width)
{
    clear_vsldoc_cache();
    StringArray themes;
    get_themes(themes);

    for (int i = 0; i < themes.size(); i++)
	add_button(form, row, max_width, gdb->type(), ThemeEntry, themes[i]);
}


// Create settings or infos editor
static Widget create_panel(DebuggerType type, SettingsType stype)
{
    string title_msg;
    string dialog_name;
    switch (stype)
    {
    case SETTINGS:
	title_msg = gdb->title() + " Settings";
	dialog_name = "settings";
	break;

    case INFOS:
	title_msg = gdb->title() + " Status Displays";
	dialog_name = "infos";
	break;

    case SIGNALS:
	title_msg = gdb->title() + " Signal Handling";
	dialog_name = "signals";
	break;

    case THEMES:
	title_msg   = "Themes";
	dialog_name = "themes";
	break;
    }

    StatusDelay delay("Retrieving " + title_msg);

    Arg args[10];
    int arg=0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    Widget panel = verify(XmCreatePromptDialog(find_shell(), 
					       CONST_CAST(char*,dialog_name.chars()), args, arg));
    Delay::register_shell(panel);

    Widget apply_button = XmSelectionBoxGetChild(panel, XmDIALOG_OK_BUTTON);
    set_sensitive(apply_button, False);

    Widget reset_button = XmSelectionBoxGetChild(panel, XmDIALOG_APPLY_BUTTON);
    XtManageChild(reset_button);

    // Remove old prompt
    XtUnmanageChild(XmSelectionBoxGetChild(panel, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(panel, XmDIALOG_SELECTION_LABEL));

    XtAddCallback(panel, XmNhelpCallback, ImmediateHelpCB, 0);
    XtAddCallback(panel, XmNcancelCallback, UnmanageThisCB, XtPointer(panel));

    switch (stype)
    {
    case SETTINGS:
	XtAddCallback(panel, XmNokCallback, ApplySettingsCB, 0);
	XtAddCallback(panel, XmNapplyCallback, ResetSettingsCB, 0);
	apply_settings_button = apply_button;
	break;

    case INFOS:
	XtAddCallback(panel, XmNapplyCallback, DeleteAllInfosCB, 0);
	XtUnmanageChild(apply_button); // No text entries
	break;

    case SIGNALS:
	XtAddCallback(panel, XmNapplyCallback, ResetSignalsCB, 0);
	XtUnmanageChild(apply_button); // No text entries
	break;

    case THEMES:
	XtAddCallback(panel, XmNokCallback,    ApplyThemesCB, 0);
	XtAddCallback(panel, XmNapplyCallback, ResetThemesCB, 0);
	apply_themes_button = apply_button;
	break;
    }

    // Add a rowcolumn widget
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNspacing,      0); arg++;
    Widget column =
        verify(XmCreateRowColumn(panel, CONST_CAST(char *,"column"), args, arg));
    XtManageChild(column);

    // Add a label
    arg = 0;
    MString xmtitle(title_msg);
    XtSetArg(args[arg], XmNlabelString, xmtitle.xmstring()); arg++;
    Widget title = verify(XmCreateLabel(column, CONST_CAST(char *,"title"), args, arg));
    XtManageChild(title);

    // Add a scrolled window.
    arg = 0;
    XtSetArg(args[arg], XmNvisualPolicy, XmCONSTANT); arg++;
    XtSetArg(args[arg], XmNscrollingPolicy, XmAUTOMATIC); arg++;
    Widget scroll = 
	verify(XmCreateScrolledWindow(column, CONST_CAST(char *,"scroll"), args, arg));
    fix_clip_window_translations(scroll);

    // Add a form.
    arg = 0;
    Widget form = verify(XmCreateForm(scroll,
                            XMST("form"), args, arg));

    switch (stype)
    {
    case SETTINGS:
	settings_form  = form;
	break;

    case SIGNALS:
	signals_form = form;
	break;

    case INFOS:
    case THEMES:
	break;
    }

    // Add setting buttons to the button box.
    Dimension max_width = 0;
    int row = 0;
    int last_row = row;

    switch (stype)
    {
    case SETTINGS:
	add_settings(form, row, max_width, type, OnOffToggleButtonEntry);
	add_settings(form, row, max_width, type, TrueFalseToggleButtonEntry);
	add_settings(form, row, max_width, type, SensitiveToggleButtonEntry);
	add_settings(form, row, max_width, type, NumToggleButtonEntry);
	add_settings(form, row, max_width, type, NoNumToggleButtonEntry);
	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, max_width, type, TargetOptionMenuEntry);
	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, max_width, type, OtherOptionMenuEntry);
	add_settings(form, row, max_width, type, DisassemblyOptionMenuEntry);
	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, max_width, type, ForkOptionMenuEntry);
	add_settings(form, row, max_width, type, SchedulerOptionMenuEntry);
	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, max_width, type, CheckOptionMenuEntry);
	if (row != last_row)
	    add_separator(form, row);

	if (type == GDB)
	{
	    last_row = row;
	    add_button(form, row, max_width, type, TextFieldEntry, 
		       get_help_line("dir", type));
	    add_button(form, row, max_width, type, TextFieldEntry, 
		       get_help_line("path", type));
	    if (row != last_row)
		add_separator(form, row);
	}

	add_settings(form, row, max_width, type, TextFieldEntry);
	break;

    case INFOS:
	add_settings(form, row, max_width, type, 
		     DisplayToggleButtonEntry, "info");
	break;

    case SIGNALS:
	add_settings(form, row, max_width, type, SignalEntry, "signals");
	break;

    case THEMES:
	add_themes(form, row, max_width);
	break;
    }

    // Clean up cached documentation stuff
    clear_gdb_question_cache();

    // Setup values
    switch (stype)
    {
    case SETTINGS:
	reset_settings_button = reset_button;
	update_reset_settings_button();
	break;

    case INFOS:
	reset_infos_button = reset_button;
	update_infos();
	break;

    case SIGNALS:
	reset_signals_button = reset_button;
	update_reset_signals_button();
	break;

    case THEMES:
	reset_themes_button = reset_button;
	update_themes();
	update_themes_buttons();
	break;
    }

    // Set number of rows
    if (row > 0)
	XtVaSetValues(form, XmNfractionBase, row, XtPointer(0));
    XtManageChild(form);

    // Set sizes
    max_width += EXTRA_SPACE;
    XtVaSetValues(form, 
		  XmNwidth, max_width, XtPointer(0));

#if XmVersion > 1001
    Dimension height = preferred_height(form);
#else
    // OSF/Motif 1.1 returns a bad value in PREFERRED_HEIGHT
    (void) preferred_height;
    Dimension height = MAX_HEIGHT + 1;
#endif

    Widget vertical_scroll_bar = 0;
    Dimension spacing = 4;
    XtVaGetValues(scroll, 
		  XmNverticalScrollBar, &vertical_scroll_bar,
		  XmNspacing, &spacing,
		  XtPointer(0));


    // Ask Xt for the main shell's height;
    // we'll make our dialog no taller than 0.75 of this value
    Dimension recommended_height;
    XtVaGetValues(find_shell(), XmNheight, &recommended_height, XtPointer(0));
    recommended_height = recommended_height * 3 / 4;
    
    if (height + spacing > recommended_height )
    {
	// Form must be scrolled
	Dimension scrollbar_width = 15;   // Additional space for scrollbar

	if (vertical_scroll_bar != 0)
	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth;
	    XtQueryGeometry(vertical_scroll_bar, (XtWidgetGeometry *)0, &size);
	    scrollbar_width = size.width;
	}
	
	XtVaSetValues(scroll,
		      XmNheight, recommended_height,
		      XmNwidth, max_width + 
		                spacing + scrollbar_width + EXTRA_WIDTH,
		      XtPointer(0));
    }
    else
    {
	// Form need not be scrolled
	XtVaSetValues(scroll,
		      XmNheight, height + spacing,
		      XmNwidth, max_width + spacing,
		      XtPointer(0));
    }

    XtVaSetValues(scroll,
                  XmNtopAttachment,    XmATTACH_WIDGET,
                  XmNtopWidget,        title,
                  XmNleftAttachment,   XmATTACH_FORM,
                  XmNrightAttachment,  XmATTACH_FORM,
                  XmNbottomAttachment, XmATTACH_FORM,
                  XtPointer(0));

    XtManageChild(scroll);

    // prevent the action area from growing when the window is resized
    Dimension h, w, x, y;

    // prevent the window from being made smaller or bigger than it initially is
    XtVaGetValues(panel,
                  XmNwidth,  &w,
                  XmNheight, &h,
                  XtPointer(0));

    // XtGetValues sometimes returns rubbish in the higher-order bits
    w &= 0x7fff;
    h &= 0x7fff;

    XtVaSetValues(panel,
                  XmNminWidth,  w,
                  XmNminHeight, h,
                  XmNmaxWidth,  w,
                  XmNmaxHeight, h,
                  XtPointer(0));

    // Center this dialog window in it's parent widget
    XtVaGetValues(XtParent(panel),
                  XmNwidth,  &x,
                  XmNheight, &y,
                  XtPointer(0));

    x &= 0x7fff;  // "32767 pixels should be enough for just about anybody"
    y &= 0x7fff;  //                            -- Andrew Gaylard, 2003/10/13

    XtVaSetValues(panel,
                  XmNx, x/2 - w/2,
                  XmNy, y/2 - h/2,
                  XtPointer(0));

    InstallButtonTips(panel);

    return panel;
}


// Create settings editor
static Widget create_settings(DebuggerType type)
{
    check_options_file();

    if (settings_panel == 0 && can_do_gdb_command() && gdb->type() == type)
    {
	// We place a delay here such that we show only one delay for
	// both getting the settings and the command definitions.
	StatusDelay delay("Retrieving " + gdb->title() + " Settings");

	settings_panel = create_panel(type, SETTINGS);

	// Get the command definitions, too.  These must be included
	// in saving GDB state.
	(void) get_defines(type);
    }
    else if (settings_panel != 0 && need_reload_settings)
    {
	reload_all_settings();
	need_reload_settings = false;
    }

    return settings_panel;
}

// Create infos editor
static Widget create_infos(DebuggerType type)
{
    check_options_file();

    if (infos_panel == 0 && can_do_gdb_command() && gdb->type() == type)
	infos_panel = create_panel(type, INFOS);

    return infos_panel;
}

// Reload all signals
static void reload_all_signals()
{
    check_options_file();

    string info = gdb_question("info handle");
    if (info != NO_GDB_ANSWER)
	process_handle(info, true);
}

// Create signal editor
static Widget create_signals(DebuggerType type)
{
    check_options_file();

    if (signals_panel == 0 && can_do_gdb_command() && gdb->type() == type)
    {
	signals_panel = create_panel(type, SIGNALS);
    }
    else if (signals_panel != 0 && need_reload_signals)
    {
	reload_all_signals();
	need_reload_signals = false;
    }

    return signals_panel;
}

void update_signals()
{
    if (signals_panel != 0 && XtIsManaged(signals_panel))
    {
	reload_all_signals();
	need_reload_signals = false;
    }
}

// Create themes editor
static Widget create_themes(DebuggerType type)
{
    check_options_file();

    if (themes_panel != 0)
	XtDestroyWidget(themes_panel);

    // Reset variables
    static const WidgetArray empty;
    themes_panel        = 0;
    reset_themes_button = 0;
    apply_themes_button = 0;
    themes_entries = empty;
    themes_labels  = empty;

    themes_panel = create_panel(type, THEMES);

    update_themes();
    return themes_panel;
}

// Update themes
void update_themes()
{
    assert(themes_entries.size() == themes_labels.size());

    for (int i = 0; i < themes_entries.size(); i++)
    {
	Widget entry  = themes_entries[i];
	Widget button = themes_labels[i];
	string theme = basename(XtName(entry));
	ThemePattern p;
	bool set = false;

	if (DispBox::theme_manager.has_pattern(theme))
	{
	    p = DispBox::theme_manager.pattern(theme);
	    set = DispBox::theme_manager.pattern(theme).active();
	}

	std::ostringstream os;
	os << p;
	string value = string(os);
	if (value.empty())
	{
	    value = "*";
	    set = false;
	}

	XmTextFieldSetString(entry, XMST(value.chars()));
	XtVaSetValues(button, XmNset, set, XtPointer(0));
    }
}



// Popup editor for debugger settings
void dddPopupSettingsCB (Widget, XtPointer, XtPointer)
{
    Widget settings = create_settings(gdb->type());
    if (settings == 0)
	return;

    manage_and_raise(settings);
}

// Popup editor for debugger infos
void dddPopupInfosCB (Widget, XtPointer, XtPointer)
{
    Widget infos = create_infos(gdb->type());
    if (infos == 0)
	return;

    manage_and_raise(infos);
}

// Popup editor for debugger infos
void dddPopupSignalsCB (Widget, XtPointer, XtPointer)
{
    Widget signals = create_signals(gdb->type());
    if (signals == 0)
	return;

    manage_and_raise(signals);
}

// Popup editor for display themes
void dddPopupThemesCB (Widget, XtPointer, XtPointer)
{
    Widget themes = create_themes(gdb->type());
    if (themes == 0)
	return;

    manage_and_raise(themes);
}

// True iff settings might have changed
bool need_settings()
{
    return settings_panel != 0;
}

// Reset it all
void reset_settings()
{
    if (settings_panel != 0)
	need_reload_settings = true;
}

// True iff signals might have changed
bool need_signals()
{
    return signals_panel != 0;
}

// Reset it all
void reset_signals()
{
    if (signals_panel != 0)
	need_reload_signals = true;
}

static void get_setting(std::ostream& os, DebuggerType type,
			const string& base, string value,
			unsigned long flags)
{
    if (value == "unlimited")
	value = "0";

    switch (type)
    {
    case DBX:
    case GDB:
	if (base == "dbxenv disassembler_version" ||
	    base == "dbxenv rtc_error_log_file_name" ||
	    base == "dbxenv output_log_file_name")
	{
	    // Do nothing (DBX) - dependent on the current machine etc.
	    break;
	}

	if (base == "set remotelogfile" && value.empty())
	{
	    // This is the default setting - do nothing (GDB)
	    break;
	}

	if (base == "set remotedevice" && value.empty())
	{
	    // This is the default setting - do nothing (GDB)
	    break;
	}

	if (base == "set targetdebug")
	{
	    // We cannot set this without a debuggee (GDB 5.0).
	    break;
	}

	if (base == "set scheduler-locking")
	{
	    // Architecture-dependent (GDB 5.0).
	    break;
	}

	if (base == "set solib-absolute-prefix" && value.empty())
	{
	    // GDB 4.17 bug: `set solib-absolute-prefix' without arg
	    // does not work.  Just leave it as default setting.
	    break;
	}

	if (base.contains("set $cur", 0) ||
	    base.contains("set $new", 0) ||
	    base.contains("set $pid", 0))
	{
	    // Do nothing - dependent on the current file (DEC DBX)
	    break;
	}

	if (base == "set $defaultin" ||
	    base == "set $defaultout" ||
	    base == "set $historyevent")
	{
	    // Do nothing - dependent on the current state (SGI DBX)
	    break;
	}

	if (base.contains("set $", 0))
	{
	    // Add setting (DBX).
	    os << base << " = " << value << '\n';
	    break;
	}

	if (base.contains("set ", 0))
	{
	    // Add setting (GDB).

	    // Jonathan Edwards <edwards@intranet.com> states:
	    // DDD gets all confused with gdb radix settings. When
	    // it configures gdb it assumes a decimal radix. But
	    // if you specify a non-decimal radix, then all
	    // settings made after that are incorrect.  I would
	    // suggest prefixing "0d" to all decimal numbers.
	    if (value.matches(rxint) && atoi(value.chars()) > 1)
		value.prepend("0d");

	    os << base << " " << value << '\n';
	    break;
	}

	if (base.contains("dbxenv ", 0))
	{
	    // Add setting (DBX).
	    os << base << " " << value << '\n';
	    break;
	}

	if (base == "dir")
	{
	    if (flags & SAVE_SESSION)
	    {
		// `dir' values are only saved within a session.

		os << base << "\n"	// Clear old value
		   << base << " " << value << '\n';
	    }
	    break;
	}

#if 0				// FIXME: How do we reset the path within GDB?
	if (base == "path")
	{
	    if (flags & SAVE_SESSION)
	    {
		// `path' values are only saved within a session.

		os << base << "\n"	// Clear old value
		   << base << " " << value << '\n';
	    }
	    break;
	}
#endif

	// Unknown setting -- ignore
	break;

    case PERL:
    {
// removed for perl-5.8.x
//	if (value == "N/A")
//	    value = "";

	string option = base.after(rxwhite);
	bool taboo = false;
	for (int i = 0; !taboo && i < int(XtNumber(perl_taboos)); i++)
	    taboo = (option == perl_taboos[i]);

	if (!taboo)
	    os << base << '=' << value << '\n';

	break;
    }

    case JDB:
	if (base == "use")
	{
	    if (flags & SAVE_SESSION)
	    {
		// `use' values are only saved within a session.
		os << base << ' ' << value << '\n';
	    }
	    break;
	}

	// Any other setting (which ones?)
	os << base << ' ' << value << '\n';
	break;

    case BASH:
    case DBG:
    case MAKE:
    case PYDB:
    case XDB:
	// Add setting
	os << base << ' ' << value << '\n';
	break;
    }
    
}

// Fetch GDB settings string
string get_settings(DebuggerType type, unsigned long flags)
{
    Widget settings = create_settings(type);
    if (settings == 0)
	return "";

    std::ostringstream command;
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	string value = settings_values[entry];

	get_setting(command, type, XtName(entry), value, flags);
    }

    return string(command);
}

// Fetch GDB signal handling string
string get_signals(DebuggerType type, unsigned long /* flags */)
{
    if (type != GDB)
	return "";		// Not supported yet

    create_signals(type);
    string commands = "";

    for (int i = 0; i < signals_entries.size(); i++)
    {
	Widget entry = signals_entries[i];
	bool set = (signals_values[entry] == "yes");
	commands += handle_command(entry, set) + "\n";
    }

    return commands;
}


//-----------------------------------------------------------------------
// Trace Command Definitions
//-----------------------------------------------------------------------

static bool save_defines_needed = false;
static bool load_defines_needed = true;

// Call this function if command definitions have changed
void set_need_save_defines(bool val)
{
    save_defines_needed = val;
}

bool need_save_defines()
{
    return save_defines_needed;
}

void set_need_load_defines(bool val)
{
    load_defines_needed = val;
}

bool need_load_defines()
{
    return load_defines_needed;
}

static StringStringAssoc defs;

static bool update_define(const string& command, bool undo = false)
{
    // We have a new command - update user buttons.
    clear_help_cache(command);
    update_user_buttons();

    string text = gdb_question("show user " + command);
    if (text == NO_GDB_ANSWER)
	return false;

    if (undo)
    {
	// Add `undoing' command
	undo_buffer.add_command("define " + command + "\n" + 
				defs[command] + "end");
    }

    // `Undefined' means command is undefined; `Not a user command'
    // means this is a built-in command.
    if (text.contains("Undefined", 0) || text.contains("Not a", 0))
    {
	defs.remove(command);
	return true;
    }

    string def  = "";

    while (!text.empty())
    {
	string line = text.before('\n');
	text        = text.after('\n');

	if (line.length() > 0 && isspace(line[0]))
	{
	    line = line.after(rxwhite);
	    strip_auto_command_prefix(line);
	    def += line + "\n";
	}
    }

    defs[command] = def;
    return true;
}

static void update_defines()
{
    if (!need_load_defines())
	return;

    StatusDelay delay("Retrieving Command Definitions");

    string commands = gdb_question("help user-defined");
    if (commands == NO_GDB_ANSWER)
	return;

    while (!commands.empty())
    {
	string line = commands.before('\n');
	commands    = commands.after('\n');

	if (!line.contains(" -- "))
	    continue;			// No help line

	string command = line.before(" -- ");
	bool ok = update_define(command);
	if (!ok)
	    return;
    }

    set_need_load_defines(false);
}

// Get current definitions
string get_defines(DebuggerType type, unsigned long /* flags */)
{
    if (type != GDB)
	return "";		// Not supported yet

    update_defines();

    // We have to get the current value of the `confirm' setting
    create_settings(type);

    string defines = "";

    // We have to turn off confirmation during command (re-)definition
    string confirm_value = "on";
    if (settings_form != 0)
    {
	Widget confirm_w = command_to_widget(settings_form, "set confirm");
	if (confirm_w != 0)
	    confirm_value = settings_values[confirm_w];
    }
    if (confirm_value == "on")
	defines += "set confirm off\n";

    for (StringStringAssocIter iter(defs); iter.ok(); ++iter)
    {
	string cmd = iter.key();
	cmd.downcase();		// GDB 5.0 makes all commands lower case

	string def = iter.value();
	if (def.empty())
	{
	    // We don't save empty definitions, such that users have a
	    // way to get rid of them.
	}
	else
	{
	    defines += "define " + cmd + "\n" + def + "end\n";
	}
    }

    if (confirm_value == "on")
	defines += "set confirm on\n";

    return defines;
}

// True iff COMMAND is a defined command
bool is_defined_cmd(const string& command)
{
    if (gdb->type() != GDB)
	return false;		// Not supported yet

    update_defines();

    string cmd = command;
    strip_space(cmd);
    if (cmd.contains(' '))
	cmd = cmd.before(' ');
    cmd.downcase();		// GDB 5.0 makes all commands lower case

    return defs.has(cmd);
}


//-----------------------------------------------------------------------
// Edit Command Definitions
//-----------------------------------------------------------------------

// Data

static Widget name_w;		// Name of defined command
static Widget arg_w;		// `()' toggle
static Widget record_w;		// `Record' button
static Widget end_w;		// `End' button
static Widget edit_w;		// `Edit>>' button
static Widget editor_w;		// Command definition editor
static Widget apply_w;		// `Apply' button

static string current_name()
{
    String name_s = XmTextFieldGetString(name_w);
    string name(name_s);
    XtFree(name_s);
    strip_space(name);
    return name;
}


// Button stuff

// Name of GDB argument
static const string arg0 = "$arg0";

static bool is_arg_command(const string& name)
{
    return defs.has(name) && defs[name].contains(arg0);
}

static void add_button(string name, const _XtString& menu)
{
    if (XmToggleButtonGetState(arg_w) || is_arg_command(name))
	name += " ()";

    string s = menu;
    if (!s.empty() && !s.contains('\n', -1))
	s += '\n';
    s += name + "\n";
    menu = (String)XtNewString(s.chars());
}

static void remove_button(const string& name, const _XtString& menu)
{
    string s = string("\n") + menu;
    s.gsub("\n" + name + "\n", string("\n"));
    s.gsub("\n" + name + " ()\n", string("\n"));
    menu = (String)XtNewString(s.chars() + 1);
}

enum ButtonTarget { ConsoleTarget, SourceTarget, DataTarget };

static const _XtString &target_string(ButtonTarget t)
{
    switch (t)
    {
    case ConsoleTarget:
	return app_data.console_buttons;

    case SourceTarget:
	return app_data.source_buttons;

    case DataTarget:
	return app_data.data_buttons;
    }

    static const _XtString null = 0;
    return null;
}

static void ToggleButtonCB(Widget, XtPointer client_data, XtPointer call_data)
{
    string name = current_name();
    ButtonTarget target = (ButtonTarget) (long) client_data;

    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    const _XtString& str = target_string(target);

    if (info->set)
    {
	add_button(name, str);
    }
    else
    {
	remove_button(name, str);
    }

    update_user_buttons();
}

static MMDesc button_menu[] =
{
    { "console", MMToggle, 
      { ToggleButtonCB, XtPointer(ConsoleTarget) }, 0, 0, 0, 0},
    { "source",  MMToggle, 
      { ToggleButtonCB, XtPointer(SourceTarget) }, 0, 0, 0, 0},
    { "data",    MMToggle, 
      { ToggleButtonCB, XtPointer(DataTarget) }, 0, 0, 0, 0},
    MMEnd
};

static void refresh_toggle(ButtonTarget t)
{
    Widget& w = button_menu[t].widget;
    string s = string("\n") + target_string(t);
    string name = current_name();

    Boolean old_state;
    XtVaGetValues(w, XmNset, &old_state, XtPointer(0));

    Boolean new_state = 
	s.contains("\n" + name + "\n") || s.contains("\n" + name + " ()\n");
    if (old_state != new_state)
	XtVaSetValues(w, XmNset, new_state, XtPointer(0));

#if 1
    set_sensitive(w, !name.empty());
#else
    string answer = gdbHelp(name);
    if (answer != NO_GDB_ANSWER)
    {
	set_sensitive(w, is_known_command(answer));
    }
#endif
}

static void refresh_toggles()
{
    refresh_toggle(SourceTarget);
    refresh_toggle(DataTarget);
    refresh_toggle(ConsoleTarget);
}

static void refresh_combo_box()
{
    if (name_w == 0)
	return;			// Not yet created

    // Refresh combo box
    StringArray commands;
    for (StringStringAssocIter iter(defs); iter.ok(); ++iter)
	commands += iter.key();
    smart_sort(commands);
    ComboBoxSetList(name_w, commands);
}

// Editing stuff

// Text field has changed -- update buttons
void UpdateDefinePanelCB(Widget w, XtPointer, XtPointer)
{
    if (name_w == 0)
	return;			// Not yet created

    string name = current_name();

    set_sensitive(record_w, !gdb->recording() && !name.empty());
    set_sensitive(apply_w,  !gdb->recording() && defs.has(name));
    set_sensitive(end_w,    gdb->recording());
    set_sensitive(edit_w,   !gdb->recording() && !name.empty());

    set_sensitive(name_w, !gdb->recording());
    set_sensitive(XtParent(name_w), !gdb->recording());
    set_sensitive(editor_w, !gdb->recording());

    set_arg();

    if (w != 0 && !gdb->recording() && defs.has(name))
	XmToggleButtonSetState(arg_w, is_arg_command(name), False);

    refresh_toggles();
}

static void update_defineHP(Agent *, void *client_data, void *call_data)
{
    bool gdb_ready = bool(call_data);
    if (gdb_ready && !gdb->recording())
    {
	char *c = (char *)client_data;
	bool ok = update_define(c, true);

	if (ok)
	{
	    // Update buttons
	    UpdateDefinePanelCB();
	    refresh_combo_box();

	    // Don't get called again
	    gdb->removeHandler(ReadyForQuestion, update_defineHP, client_data);
	    delete[] c;
	}
    }
    else
    {
	UpdateDefinePanelCB();
    }
}

// This one is called by send_gdb_command() when seeing `define'
void update_define_later(const string& command)
{
    char *c = new char[command.length() + 1];
    strcpy(c, command.chars());

    gdb->addHandler(ReadyForQuestion, update_defineHP, (void *)c);

    UpdateDefinePanelCB();
}

static void RecordCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    string name = current_name();
    gdb_command("define " + name, w);
}

static void EndCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    if (gdb->recording())
	gdb_command("end", w);
}

static void DoneEditCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    if (!XtIsManaged(XtParent(editor_w)))
	return;

    string name = current_name();

    XtUnmanageChild(XtParent(editor_w));
    set_sensitive(name_w, true);
    set_sensitive(XtParent(name_w), true);

    MString label = "Edit " + MString(">>", CHARSET_SMALL);
    set_label(edit_w, label);

    String _commands = XmTextGetString(editor_w);
    string cmd = _commands;
    XtFree(_commands);

    if (!cmd.contains('\n', -1))
	cmd += '\n';

    if ((!cmd.empty() && !defs.has(name)) || cmd != defs[name])
    {
	StringArray commands;
	while (!cmd.empty())
	{
	    string c = cmd.before('\n');
	    if (!c.empty())
		commands += c;
	    cmd = cmd.after('\n');
	}

	// This might require confirmation.  Don't change anything.
	set_sensitive(edit_w,   false);
	set_sensitive(record_w, false);
	set_sensitive(end_w,    false);

	gdb_command("define " + name, w);
	for (int j = 0; j < commands.size(); j++)
	    gdb_command(commands[j], w);
	gdb_command("end", w);

	update_define_later(name);
    }
}

static void EditCommandDefinitionCB(Widget, XtPointer, XtPointer)
{
    if (XtIsManaged(XtParent(editor_w)))
	return;

    string name = current_name();

    // update_define(name);
    set_sensitive(name_w, false);
    set_sensitive(XtParent(name_w), false);

    string def = "";
    if (defs.has(name))
	def = defs[name];

    XmTextSetString(editor_w, XMST(def.chars()));

    XtManageChild(XtParent(editor_w));
    MString label = "Edit " + MString("<<", CHARSET_SMALL);
    set_label(edit_w, label);
}

static void ToggleEditCommandDefinitionCB(Widget w, XtPointer client_data, 
					  XtPointer call_data)
{
    if (XtIsManaged(XtParent(editor_w)))
	DoneEditCommandDefinitionCB(w, client_data, call_data);
    else
	EditCommandDefinitionCB(w, client_data, call_data);
}

// Apply the given command
static void ApplyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    if (gdb->recording())
	EndCommandDefinitionCB(w, client_data, call_data);

    DoneEditCommandDefinitionCB(w, client_data, call_data);

    string cmd = current_name();
    if (!cmd.empty())
    {
	if (XmToggleButtonGetState(arg_w))
	    cmd += " " + source_arg->get_string();

	gdb_command(cmd, w);
    }
}

// Force argument to `()'
static void ForceArg0HP(void *, void *, void *)
{
    static bool called = false;
    if (called)
	return;

    called = true;
    set_arg();
    called = false;
}

static void set_arg()
{
    static string saved_arg;
    static bool have_saved_arg = false;

    if (gdb->recording() && XmToggleButtonGetState(arg_w))
    {
	if (!have_saved_arg)
	{
	    saved_arg = source_arg->get_string();
	    have_saved_arg = true;

	    source_arg->set_string(arg0);

	    reset_status_lock();
	    MString msg = 
		rm("Using ") + bf("()") + rm(" as symbolic argument");
	    set_status_mstring(msg);
	}

	source_arg->addHandler(Changed, ForceArg0HP);
	source_arg->lock();
    }
    else
    {
	source_arg->removeHandler(Changed, ForceArg0HP);
	source_arg->lock(false);

	if (have_saved_arg)
	{
	    source_arg->set_string(saved_arg);
	    have_saved_arg = false;

	    reset_status_lock();
	    MString msg = 
		rm("Using ") + bf("()") + rm(" as literal argument");
	    set_status_mstring(msg);
	}
    }
}

static void ToggleArgCB(Widget, XtPointer, XtPointer)
{
    set_arg();
}

static MMDesc commands_menu[] =
{
    { "record", MMPush, 
      { RecordCommandDefinitionCB, 0 }, 0, &record_w, 0, 0 },
    { "end",    MMPush | MMInsensitive, 
      { EndCommandDefinitionCB, 0 }, 0, &end_w, 0, 0},
    { "edit",   MMPush, 
      { ToggleEditCommandDefinitionCB, 0 }, 0, &edit_w, 0, 0 },
    MMEnd
};

static MMDesc name_menu[] =
{
    { "name",     MMComboBox | MMUnmanagedLabel, 
      { UpdateDefinePanelCB, 0 }, 0, &name_w, 0, 0 },
    { "arg",      MMToggle, { ToggleArgCB, 0 }, 0, &arg_w, 0, 0 },
    MMEnd
};

static MMDesc panel_menu[] = 
{
    { "name",     MMButtonPanel, MMNoCB, name_menu, 0, 0, 0 },
    { "commands", MMButtonPanel, MMNoCB, commands_menu, 0, 0, 0 },
    { "button",   MMButtonPanel, MMNoCB, button_menu, 0, 0, 0 },
    MMEnd
};


// Define command
void dddDefineCommandCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;

    if (dialog == 0)
    {
	Arg args[10];
	int arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	dialog = verify(XmCreatePromptDialog(find_shell(w),
					     XMST("define_command"),
					     args, arg));

	// Remove old prompt
	Widget text = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
	XtUnmanageChild(text);
	Widget old_label = 
	    XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(old_label);

	apply_w = XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON);
	XtVaSetValues(dialog, XmNdefaultButton, apply_w, XtPointer(0));
	XtManageChild(apply_w);

	XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_OK_BUTTON));

	Delay::register_shell(dialog);

	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
	Widget form = XmCreateRowColumn(dialog, XMST("form"), args, arg);
	XtManageChild(form);

	Widget panel = MMcreatePanel(form, "panel", panel_menu);

	XtVaSetValues(panel,
		      XmNmarginWidth,    0,
		      XmNmarginHeight,   0,
		      XtPointer(0));

	arg = 0;
	XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
        editor_w = XmCreateScrolledText(form, XMST("text"), args, arg);
	XtUnmanageChild(XtParent(editor_w));
	XtManageChild(editor_w);

	MMaddCallbacks(panel_menu);
	InstallButtonTips(panel);

	MMadjustPanel(panel_menu);

	XtAddCallback(dialog, XmNokCallback, UnmanageThisCB, 
		      XtPointer(dialog));
	XtAddCallback(dialog, XmNokCallback, DoneEditCommandDefinitionCB, 
		      XtPointer(0));

	XtAddCallback(dialog, XmNapplyCallback,
		      ApplyCB, XtPointer(0));
	XtAddCallback(dialog, XmNcancelCallback, 
		      EndCommandDefinitionCB, XtPointer(0));
	XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		      XtPointer(dialog));

	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));

	set_need_load_defines(true);
	update_defines();
    }

    UpdateDefinePanelCB();
    refresh_combo_box();
    manage_and_raise(dialog);
}
