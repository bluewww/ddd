// $Id$ -*- C++ -*-
// Modify debugger settings

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
#include "EntryType.h"
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
#include "WidgetSA.h"
#include "SourceView.h"
#include "string-fun.h"
#include "DataDisp.h"
#include "LessTifH.h"

static Widget            settings_form  = 0;
static Widget            reset_settings = 0;
static Widget            reset_infos    = 0;
static WidgetArray       settings_entries;
static EntryTypeArray    settings_entry_types;
static WidgetStringAssoc settings_values;
static WidgetStringAssoc settings_initial_values;
static WidgetArray       infos_entries;


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
	    confirm_value = settings_values[confirm_w];

	if (confirm_value == "on")
	    gdb_command("set confirm off");
	gdb_command(set_command);
	if (confirm_value == "on")
	    gdb_command("set confirm on");
    }

    if (value != "")
    {
	if (set_command.contains("set $", 0) && !set_command.contains(" = "))
	    gdb_command(set_command + " = " + value);
	else
	    gdb_command(set_command + " " + value);
    }
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
static void SetOnOffCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "on");
    else
	gdb_set_command((String)client_data, "off");
}

// ToggleButton reply
static void SetTrueFalseCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "true");
    else
	gdb_set_command((String)client_data, "false");
}

// ToggleButton reply
static void SetSensitiveCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "sensitive");
    else
	gdb_set_command((String)client_data, "insensitive");
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

// ToggleButton reply
static void SetNoNumCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	gdb_set_command((String)client_data, "0");
    else
	gdb_set_command((String)client_data, "1");
}

// ToggleButton reply
static void SetDisplayCB(Widget, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	data_disp->new_user_display((String)client_data);
    else
	data_disp->delete_user_display((String)client_data);
}

// Update state of `reset' button
static void update_reset_settings()
{
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	if (settings_initial_values[entry] != settings_values[entry])
	{
	    XtSetSensitive(reset_settings, True);
	    return;
	}
    }

    XtSetSensitive(reset_settings, False);
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
	XtVaSetValues(button, XmNset, set, NULL);
    }

    if (reset_infos != 0)
	XtSetSensitive(reset_infos, have_info);
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

    update_reset_settings();
}
    

// Process output of `show' command
void process_show(string command, string value, bool init)
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
	    cerr << "Warning: cannot set " << quote(base)
		 << " to " << quote(value) << "\n";
#endif
	    return;
	}

	value = value.from(start_value);
	value = value.after(base);
	if (value.contains('\n'))
	    value = value.before('\n');
	read_leading_blanks(value);
	strip_final_blanks(value);
    }

    if (value.contains('\n'))
	value = value.before('\n');

    if (!init)
	set_status(value);

    if (gdb->type() == GDB)
    {
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
    }

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
	settings_values[button] = value;
	if (init)
	    settings_initial_values[button] = value;
    }

    if (!init)
	update_reset_settings();

    if (button != 0 && XmIsRowColumn(button))
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

	XtVaSetValues(button, XmNset, set, NULL);
	return;
    }
    else if (button != 0 && XmIsTextField(button))
    {
	XtVaSetValues(button, 
		      XmNvalue,                 value.chars(),
		      XmNcursorPosition,        0,
		      XmNcursorPositionVisible, True,
		      NULL);
	return;
    }

#if 0
    cerr << "Warning: cannot set " << quote(set_command)
	 << " to " << quote(value) << "\n";
#endif
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
    if (ing < first_space || ing < second_space || second_space < 0)
	return true;

    return false;
}

// Determine entry type
static EntryType entry_type(DebuggerType type,
			    const string& base, 
			    const string& doc,
			    const string& value)
{
    static regex rxnonzero1("non-?(0|zero|null)");
    static regex rxnonzero2("!= *(0|zero|null)");

    switch (type)
    {
    case GDB:
	if (base.contains("check", 0))
	    return CheckOptionMenuEntry;
	else if (base.contains("language", 0) || base.contains("demangle", 0))
	    return OtherOptionMenuEntry;
	else if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
	    return OnOffToggleButtonEntry;
	else if ((value.contains("0.\n", -1) || value.contains("1.\n", -1))
		 && (is_verb(doc)))
	    return NumToggleButtonEntry;
	break;

    case DBX:
	if (doc.contains(rxnonzero1) || doc.contains(rxnonzero2))
	    return NumToggleButtonEntry;
	else if (value.contains("on", 0) || value.contains("off", 0))
	    return OnOffToggleButtonEntry;
	else if (value.contains("true", 0) || value.contains("false", 0))
	    return TrueFalseToggleButtonEntry;
	else if (value.contains("sensitive", 0) 
		 || value.contains("insensitive", 0))
	    return SensitiveToggleButtonEntry;
	else if (has_nr(value))
	    return TextFieldEntry;
	else if (base.contains("version")
	    || base.contains("run_io")
	    || base.contains("follow_fork_mode"))
	    return OtherOptionMenuEntry;
	break;

    case XDB:
	break;			// FIXME
    }

    // When all else fails, display a text
    return TextFieldEntry;
}


// The GDB question cache
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

static void strip_leading(string& doc, const string& key)
{
    if (doc.contains(key, 0))
	doc = doc.after(key);
}

static void strip_from(string& doc, const string& key)
{
    if (doc.contains(key))
	doc = doc.before(key);
}

static void munch_doc(string& doc)
{
    read_leading_blanks(doc);

    // Sun DBX 3.0
    strip_leading(doc, "# ");
    strip_leading(doc, "If on, ");
    strip_leading(doc, "If true, ");
    strip_leading(doc, "When on, ");
    strip_leading(doc, "When `on', ");
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
    
    // DEC DBX
    strip_leading(doc, "$");
    strip_leading(doc, "non-0 => ");
    strip_leading(doc, "non-0 implies ");
    strip_leading(doc, "if non-0 ");
    strip_leading(doc, "if set, ");
    strip_leading(doc, "this ");
    strip_leading(doc, "is ");
    strip_leading(doc, "contains ");
    strip_leading(doc, "the ");
    strip_leading(doc, "name of ");
    strip_leading(doc, "specify ");
    strip_leading(doc, "which ");

    // SGI DBX
    strip_leading(doc, "if != 0, ");
    strip_leading(doc, "if != 0 ");
    strip_leading(doc, "if 1 ");
    strip_leading(doc, "then ");

    // GDB
    strip_leading(doc, "whether to ");
    strip_leading(doc, "the ");

    // More DEC DBX
    strip_from(doc, "we're looking at");
    strip_from(doc, "we're debugging");
    strip_from(doc, "if this is non-0");
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

    if (doc.contains("0 =>"))
	doc = "Don't" + doc.after("0 =>");

    if (doc.length() > 0)
	doc[0] = toupper(doc[0]);
}


// Get DBX documentation
static string _get_dbx_help(string dbxenv, string base)
{
    string dbx_help;
    if (dbxenv == "dbxenv")
	dbx_help = cached_gdb_question("help " + dbxenv);
    if (dbx_help.freq('\n') <= 2)
	dbx_help = cached_gdb_question("help variable");
    if (dbx_help.freq('\n') <= 2)
	dbx_help = cached_gdb_question("help $variables");

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
static string get_dbx_help(string dbxenv, string base, int width)
{
    string dbx_help = _get_dbx_help(dbxenv, base);
    dbx_help = dbx_help.after(base);
    read_leading_blanks(dbx_help);

    if (dbx_help.contains("  - "))
    {
	dbx_help = dbx_help.after("  - ");
	read_leading_blanks(dbx_help);
    }
    else if (dbx_help.contains(" # "))
    {
	dbx_help = dbx_help.after(" # ");
	read_leading_blanks(dbx_help);
    }

    // Remove remaining `# ' prefixes (Solaris DBX)
    dbx_help.gsub("  # ", "");
    dbx_help.gsub(" # ", " number ");

    // Reduce spaces (Solaris DBX & DEC DBX)
    compress_spaces(dbx_help, width);

    return dbx_help;
}

string get_dbx_help(string dbxenv, string base)
{
    return get_dbx_help(dbxenv, base, 60);
}

static string get_dbx_doc(string dbxenv, string base)
{
    // Some specials
    if (base == "rtc_biu_at_exit")
	return "Produce memory report at exit";
    if (base == "run_autostart")
	return "Let `step' and `next' implicitly start the program";
    if (base == "follow_fork_inherit")
	return "When following child, inherit events";
    if (base == "follow_fork_mode")
	return "When process forks";
    if (base == "run_io")
	return "Redirect I/O to";
    if (base == "output_list_size")
	return "Lines to print in the `list' command";
    if (base == "suppress_startup_message")
	return "Suppress startup message below release";
    if (base == "disassembler_version")
	return "Disassembler version";
    if (base == "$byteaccess")
	return "Access memory items less than 4 bytes";
    if (base == "$dispix")
	return "Display pixie instructions";
    
    // Generic help
    string dbx_doc = get_dbx_help(dbxenv, base, -1);
    read_leading_blanks(dbx_doc);

    dbx_doc.gsub("etc. ", "etc ");
    if (dbx_doc.contains(". "))
	dbx_doc = dbx_doc.before(". ");
    if (dbx_doc.contains("; "))
	dbx_doc = dbx_doc.before("; ");
    dbx_doc.gsub("etc ", "etc. ");

#if 0
    if (dbx_doc == "")
    {
	dbx_doc = base;
	dbx_doc.gsub('_', ' ');
    }
#endif

    munch_doc(dbx_doc);

    return dbx_doc;
}

static void add_settings(Widget form, int& row, DebuggerType type, 
			 EntryType entry_filter, string gdb_class = "set");

// Add single button
static void add_button(Widget form, int& row, DebuggerType type,
		       EntryType entry_filter, string line)
{
    if (line == "")
	return;

    string set_command;		// Command to create the setting
    string show_command;	// Command to display the value
    string doc;			// Documentation string
    string base;		// Common base of set_command and show_command
    string value;		// Current variable value
    EntryType e_type;		// Type of the entry

    bool is_set = true;		// Command sets a value
    bool is_add = false;	// Command adds a value

    switch (type)
    {
    case GDB:
	{
	    if (!line.contains(" -- "))
		return;			// No help line

	    set_command  = line.before(" -- ");
	    doc          = line.after(" -- ");
	    base         = set_command.after(' ');
	    if (base == "")
		base = set_command;
	    show_command = "show " + base;

	    if (entry_filter != DisplayToggleButtonEntry)
	    {
		if (base == "args")
		    return; // Already handled in `Run...' editor

		if (base == "radix")
		    return; // Already handled in input- and output-radix

		is_set = doc.contains("Set ", 0);
		is_add = doc.contains("Add ", 0);

		if (!is_set && !is_add)
		{
		    // Generic command or `set variable' - list `set'
		    // subcommands
		    add_settings(form, row, type, entry_filter, set_command);
		    return;
		}

		value = cached_gdb_question(show_command);
		if (is_set && value.freq('\n') > 1)
		{
		    // Generic command - list `set' subcommands
		    add_settings(form, row, type, entry_filter, set_command);
		    return;
		}

		if (is_set && !value.contains(".\n", -1))
		    return;

		e_type = entry_type(type, base, doc, value);

		if (is_set)
		    doc = doc.after("Set ");
		else if (is_add)
		    doc = doc.after("Add ");

		if (is_add && doc.contains("of "))
		    doc = doc.after("of ");
	    }
	    else
	    {
		if (base == "handle")
		    return;	// We already have `info signals'.
		if (base == "watchpoints")
		    return;	// We already have `info breakpoints'.
		if (base == "target")
		    return;	// We already have `info files'.
		if (base == "address")
		    return;	// `info address' requires an argument.
		if (base == "line")
		    return;	// `info line' requires an argument.

		// These infos produce too much output for our data
		// window to be of any use.  Even more, `info
		// functions' causes my X terminal to crash - AZ.
		if (base == "types")
		    return;
		if (base == "functions")
		    return;
		if (base == "variables")
		    return;

		e_type = DisplayToggleButtonEntry;
		strip_leading(doc, "Show ");
		strip_leading(doc, "Print ");
		strip_leading(doc, "out ");
		strip_leading(doc, "the ");
	    }
	    munch_doc(doc);
	}
        break;

    case DBX:
	{
	    if (!line.contains("$", 0) 
		&& !(line.contains(rxidentifier, 0) && islower(line[0])))
		return;			// No help line

	    base = line.before(rxwhite);
	    if (base == "")
		return;

	    value = line.after(rxwhite);

	    string dbxenv;
	    if (base[0] == '$')
		dbxenv = "set";
	    else
		dbxenv = "dbxenv";

	    set_command = dbxenv + " " + base;
	    if (base == "run_savetty")
		return; // Makes no sense under a GUI

	    show_command = set_command + " " + value;
	    doc = get_dbx_doc(dbxenv, base);
	    e_type = entry_type(type, base, get_dbx_help(dbxenv, base), value);

	    static regex rxdont("Do ?n['o]t");
	    if (doc.contains(rxdont, 0) && e_type == NumToggleButtonEntry)
	    {
		doc = doc.after(rxdont);
		munch_doc(doc);
		e_type = NoNumToggleButtonEntry;
	    }
	}
        break;

    case XDB:
	return;			// FIXME
    }

    if (e_type != entry_filter)
	return;

    if (doc == "")
	return;			// No need to support undocumented stuff

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

    default:
	callback = 0;
	break;
    }

    if (callback == 0)
    {
	if (is_set)
	    label = verify(XmCreateLabel(form, base, args, arg));
	else
	    label = verify(XmCreateLabel(form, "the" + base, args, arg));

	XtManageChild(label);
    }
    else
    {
	entry = label = 
	    verify(XmCreateToggleButton(form, set_command, args, arg));
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
    Widget help = verify(XmCreatePushButton(form, "help", args, arg));
    XtManageChild(help);

    // Add entry
    switch (e_type)
    {
    case OnOffToggleButtonEntry:
    case TrueFalseToggleButtonEntry:
    case SensitiveToggleButtonEntry:
    case NumToggleButtonEntry:
    case NoNumToggleButtonEntry:
    case DisplayToggleButtonEntry:
	// All is done
	break;

    case CheckOptionMenuEntry:
	{
	    // `set check'
	    arg = 0;
	    Widget menu = 
		verify(XmCreatePulldownMenu(form, "menu", args, arg));

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
	    entry = verify(XmCreateOptionMenu(form, set_command, args, arg));
	    XtManageChild(entry);
	}
	break;

    case OtherOptionMenuEntry:
	{
	    // `set language / set demangle'
	    arg = 0;
	    Widget menu = 
		verify(XmCreatePulldownMenu(form, "menu", args, arg));

	    // Possible options are listed upon `set language'
	    // without value
	    string options;
	    char separator = '\n';

	    switch (gdb->type())
	    {
	    case GDB:
		options = cached_gdb_question("set " + base);
		separator = '\n';
		break;

	    case DBX:
		options = _get_dbx_help("dbxenv", base);
		options = options.after('<');
		options = options.before('>');

		if (options == "")
		{
		    if (base == "follow_fork_mode")
			options = "parent|child|both|ask";
		}
		separator = '|';
		break;

	    case XDB:
		return;		// FIXME
	    }

	    while (options != "")
	    {
		string option = options;
		if (option.contains(separator))
		    option = option.before(separator);
		options = options.after(separator);

		read_leading_blanks(option);
		strip_final_blanks(option);

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

		if (option == "" || option.contains(':', -1))
		    continue;

		MString xmlabel(label);
		arg = 0;
		if (gdb->type() == GDB)
		{
		    XtSetArg(args[arg], XmNlabelString, 
			     xmlabel.xmstring()); arg++;
		}
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
	    entry = 
		verify(XmCreateOptionMenu(form, 
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
	entry = verify(XmCreateTextField(form, set_command, args, arg));
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
	verify(XmCreateSeparator(form, "leader", args, arg));
    XtManageChild(leader);

    // Add help callback
    XtAddCallback(help, XmNactivateCallback, 
		  HelpOnThisCB, XtPointer(entry));

    if (e_type != DisplayToggleButtonEntry)
    {
	// Make entry insensitive if part of initialization commands.
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

	// Register entry
	settings_entries     += entry;
	settings_entry_types += e_type;
    }
    else
    {
	// Register entry
	infos_entries        += entry;
    }

    row++;
}


// Add buttons
static void add_settings(Widget form, int& row, DebuggerType type,
			 EntryType entry_filter, string gdb_class)
{
    string commands;

    switch (type)
    {
    case GDB:
	commands = cached_gdb_question("help " + gdb_class);
	break;

    case XDB:
	commands = cached_gdb_question("I");
	break;

    case DBX:
	commands = cached_gdb_question("dbxenv");
	if (commands.freq('\n') <= 2)
	    commands = cached_gdb_question("set");
	break;
    }

    while (commands != "")
    {
	string line = commands.before('\n');
	commands    = commands.after('\n');
	add_button(form, row, type, entry_filter, line);
    }
}

// Add separator
static void add_separator(Widget form, int& row)
{
    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNleftAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,  XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_POSITION); arg++;
    XtSetArg(args[arg], XmNbottomPosition,   row + 1);           arg++;
    Widget sep = verify(XmCreateSeparator(form, "sep", args, arg));
    XtManageChild(sep);
    row++;
}

// Reset settings
static void ResetSettingsCB (Widget, XtPointer, XtPointer)
{
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	if (settings_initial_values[entry] != settings_values[entry])
	    gdb_set_command(XtName(entry), settings_initial_values[entry]);
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
static string get_help_line(string command, DebuggerType type)
{
    (void) type;

    string reply = cached_gdb_question("help " + command);
    reply = reply.before('\n');
    if (reply.contains('.'))
	reply = reply.before('.');
    reply = command + " -- " + reply;
    return reply;
}

// Create settings or infos editor
static Widget create_panel(DebuggerType type, bool create_settings)
{
    string title_msg;
    if (create_settings)
	title_msg = gdb->title() + " Settings";
    else
	title_msg = gdb->title() + " Status Displays";
    StatusDelay delay("Retrieving " + title_msg);

    Arg args[10];
    int arg;

    arg = 0;
    Widget panel = verify(XmCreatePromptDialog(find_shell(), 
					       create_settings ? 
					       "settings" : "infos", 
					       args, arg));
    Delay::register_shell(panel);

    if (lesstif_hacks_enabled)
	XtUnmanageChild(XmSelectionBoxGetChild(panel, XmDIALOG_APPLY_BUTTON));

    // Remove old prompt
    XtUnmanageChild(XmSelectionBoxGetChild(panel, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(panel, XmDIALOG_SELECTION_LABEL));

    XtAddCallback(panel, XmNhelpCallback, ImmediateHelpCB, 0);
    XtAddCallback(panel, XmNokCallback, UnmanageThisCB, 
    	      XtPointer(panel));

    Widget cancel = XmSelectionBoxGetChild(panel, XmDIALOG_CANCEL_BUTTON);
    if (create_settings)
    {
	XtRemoveAllCallbacks(cancel, XmNactivateCallback);
	XtAddCallback(cancel, XmNactivateCallback, ResetSettingsCB, 0);
	XtVaSetValues(panel, XmNdefaultButton, Widget(0), NULL);
    }
    else
    {
	XtRemoveAllCallbacks(cancel, XmNactivateCallback);
	XtAddCallback(cancel, XmNactivateCallback, DeleteAllInfosCB, 0);
    }

    // Add a rowcolumn widget
    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNspacing,      0); arg++;
    Widget column =
        verify(XmCreateRowColumn(panel, "column", args, arg));
    XtManageChild(column);

    // Add a label
    arg = 0;
    MString xmtitle(title_msg);
    XtSetArg(args[arg], XmNlabelString, xmtitle.xmstring()); arg++;
    Widget title = verify(XmCreateLabel(column, "title", args, arg));
    XtManageChild(title);

    // Add a scrolled window...
    arg = 0;
    Widget scroll = 
        verify(XmCreateScrolledWindow(column, "scroll", args, arg));

    // ...and a form.
    arg = 0;
    Widget form = verify(XmCreateForm(scroll, "form", args, arg));

    if (create_settings)
    {
	settings_form  = form;
    }

    // Add setting buttons to the button box.
    int row = 0;
    int last_row = row;

    if (create_settings)
    {
	add_settings(form, row, type, OnOffToggleButtonEntry);
	add_settings(form, row, type, TrueFalseToggleButtonEntry);
	add_settings(form, row, type, SensitiveToggleButtonEntry);
	add_settings(form, row, type, NumToggleButtonEntry);
	add_settings(form, row, type, NoNumToggleButtonEntry);

	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, type, OtherOptionMenuEntry);

	if (row != last_row)
	    add_separator(form, row);

	last_row = row;
	add_settings(form, row, type, CheckOptionMenuEntry);

	if (row != last_row)
	    add_separator(form, row);

	if (type == GDB)
	{
	    last_row = row;
	    add_button(form, row, type, TextFieldEntry, 
		       get_help_line("dir", type));
	    add_button(form, row, type, TextFieldEntry, 
		       get_help_line("path", type));
	    if (row != last_row)
		add_separator(form, row);
	}

	add_settings(form, row, type, TextFieldEntry);
    }
    else
    {
	add_settings(form, row, type, DisplayToggleButtonEntry, "info");
    }

    // Clean up cached documentation stuff
    clear_gdb_question_cache();

    // Setup values
    if (create_settings)
    {
	reset_settings = cancel;
	update_reset_settings();
    }
    else
    {
	reset_infos = cancel;
	update_infos();
    }

    if (row > 0)
	XtVaSetValues(form, XmNfractionBase, row, NULL);
    XtManageChild(form);
    XtManageChild(scroll);

    InstallButtonTips(panel);

    return panel;
}


// Create settings editor
static Widget create_settings(DebuggerType type)
{
    static Widget settings = 0;
    if (settings == 0 && gdb->isReadyWithPrompt() && gdb->type() == type)
	settings = create_panel(type, true);
    return settings;
}

// Create infos editor
static Widget create_infos(DebuggerType type)
{
    static Widget infos = 0;
    if (infos == 0 && gdb->isReadyWithPrompt() && gdb->type() == type)
	infos = create_panel(type, false);
    return infos;
}


// Popup editor for debugger settings
void dddPopupSettingsCB (Widget, XtPointer, XtPointer)
{
    Widget settings = create_settings(gdb->type());
    if (settings == 0)
	return;

    XtManageChild(settings);
    raise_shell(settings);
}

// Popup editor for debugger infos
void dddPopupInfosCB (Widget, XtPointer, XtPointer)
{
    Widget infos = create_infos(gdb->type());
    if (infos == 0)
	return;

    XtManageChild(infos);
    raise_shell(infos);
}


// Fetch GDB settings string
string get_settings(DebuggerType type)
{
    Widget settings = create_settings(type);
    if (settings == 0)
	return "";

    string command = "";
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	string value = settings_values[entry];
	if (value == "unlimited")
	    value = "0";

	string base = XtName(entry);

	switch (type)
	{
	case GDB:
	case DBX:
	    if (base == "dbxenv disassembler_version" 
		|| base == "dbxenv rtc_error_log_file_name"
		|| base == "dbxenv output_log_file_name")
	    {
		// Do nothing (DBX) - dependent on the current machine etc.
	    }
	    else if (base == "set remotelogfile" && value == "")
	    {
		// This is the default setting - do nothing (GDB)
	    }
	    else if (base == "set remotedevice" && value == "")
	    {
		// This is the default setting - do nothing (GDB)
	    }
	    else if (base.contains("set $cur", 0))
	    {
		// Do nothing - dependent on the current file (DBX)
	    }
	    else if (base.contains("set $", 0))
	    {
		// Add setting (DBX).
		command += base + " = " + value + '\n';
	    }
	    else if (base.contains("set ", 0))
	    {
		// Add setting (GDB).
		command += base + " " + value + '\n';
	    }
	    else if (base.contains("dbxenv ", 0))
	    {
		// Add setting (DBX).
		command += base + " " + value + '\n';
	    }
	    else
	    {
		// `dir' and `path' values are not saved, since they are
		// dependent on the current machine and the current
		// executable (GDB).
	    }
	    break;

	case XDB:
	    // Add setting (FIXME)
	    command += base + ' ' + value + '\n';
	    break;
	}
    }

    return command;
}
