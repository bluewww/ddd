// $Id$ -*- C++ -*-
// Modify debugger settings

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

#include "AppData.h"
#include "Assoc.h"
#include "ComboBox.h"
#include "Command.h"
#include "DataDisp.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "EntryType.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "LessTifH.h"
#include "SmartC.h"
#include "SourceView.h"
#include "StringSA.h"
#include "VarArray.h"
#include "WidgetSA.h"
#include "buttons.h"
#include "cook.h"
#include "comm-manag.h"
#include "ddd.h"
#include "mydialogs.h"
#include "logo.h"
#include "question.h"
#include "regexps.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "wm.h"


//-----------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------

const Dimension EXTRA_SPACE     = 10;   // Minimum space between label / entry
const Dimension EXTRA_WIDTH     =  6;   // Additional space for safety
const Dimension MAX_HEIGHT     = 300;   // Maximum height of window



//-----------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------

static Widget            settings_panel = 0;
static Widget            settings_form  = 0;
static Widget            reset_settings_button = 0;
static WidgetArray       settings_entries;
static EntryTypeArray    settings_entry_types;
static WidgetStringAssoc settings_values;
static WidgetStringAssoc settings_initial_values;


static Widget            infos_panel        = 0;
static Widget            reset_infos_button = 0;
static WidgetArray       infos_entries;



//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

// Reset it all
void reset_settings()
{
    if (settings_panel != 0)
	DestroyWhenIdle (settings_panel);

    settings_panel          = 0;
    settings_form           = 0;
    reset_settings_button   = 0;
    
    settings_entries        = WidgetArray();
    settings_entry_types    = EntryTypeArray();
    settings_values         = WidgetStringAssoc();
    settings_initial_values = WidgetStringAssoc();
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
static void update_reset_settings_button()
{
    for (int i = 0; i < settings_entries.size(); i++)
    {
	Widget entry = settings_entries[i];
	if (settings_initial_values[entry] != settings_values[entry])
	{
	    XtSetSensitive(reset_settings_button, True);
	    return;
	}
    }

    XtSetSensitive(reset_settings_button, False);
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

    if (reset_infos_button != 0)
	XtSetSensitive(reset_infos_button, have_info);
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
    set_need_defines(false);
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
	strip_space(value);
    }

    if (value.contains('\n'))
	value = value.before('\n');

    if (!init)
	set_status(value);

    if (gdb->type() == GDB)
    {
	if (value.contains(" is "))
	    value = value.after(" is ", -1);
	if (value.contains(".", -1))
	    value = value.before(int(value.length()) - 1);

	if (value.contains("assumed to be "))
	    value = value.after("assumed to be ");
	if (value.contains("\"auto;", 0) || 
	    value.contains("set automatically", 0))
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
	update_reset_settings_button();

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
#if RUNTIME_REGEX
    static regex rxnonzero1("no[nt][ -]?(0|zero|null)");
    static regex rxnonzero2("!= *(0|zero|null)");
#endif

    switch (type)
    {
    case GDB:
	if (base.contains("check", 0))
	    return CheckOptionMenuEntry;
	if (base.contains("endian", 0) ||
	    base.contains("architecture", 0))
	    return TargetOptionMenuEntry;
	if (base.contains("language", 0) || 
	    base.contains("demangle", 0))
	    return OtherOptionMenuEntry;
	if (value.contains("on.\n", -1) || value.contains("off.\n", -1))
	    return OnOffToggleButtonEntry;
	if ((value.contains("0.\n", -1) || value.contains("1.\n", -1))
	    && (is_verb(doc)))
	    return NumToggleButtonEntry;
	break;

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

    case XDB:
    case JDB:
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
    if (answer == "")
	answer = gdb_question(question, timeout);
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
    strip_leading_space(doc);
}

static void strip_from(string& doc, const string& key)
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
static string _get_dbx_help(string dbxenv, string base)
{
    string dbx_help;
    if (dbxenv == "dbxenv")
	dbx_help = cached_gdb_question("help dbxenv", -1);
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
static string get_dbx_help(string dbxenv, string base, int width)
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

string get_dbx_help(string dbxenv, string base)
{
    return get_dbx_help(dbxenv, base, 60);
}

struct DBXTranslation {
    String base;
    String doc;
};

static DBXTranslation dbx_translations[] = 
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

static string get_dbx_doc(string dbxenv, string base)
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
    if (dbx_doc == "")
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
    XtWidgetGeometry size;
    size.request_mode = CWWidth;
    XtQueryGeometry(w, NULL, &size);
    return size.width;
}

static Dimension preferred_height(Widget w)
{
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(w, NULL, &size);
    return size.height;
}

static void add_settings(Widget form, int& row, Dimension& max_width,
			 DebuggerType type, EntryType entry_filter, 
			 string gdb_class = "set");

// Add single button
static void add_button(Widget form, int& row, Dimension& max_width,
		       DebuggerType type, EntryType entry_filter,
		       string line)
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
		    add_settings(form, row, max_width, 
				 type, entry_filter, set_command);
		    return;
		}

		value = cached_gdb_question(show_command);
		if (is_set && value.freq('\n') > 1)
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
		if (e_type != entry_filter)
		    return;

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
	    if (base == "run_savetty")
		return; // Makes no sense under a GUI

	    value = line.after(rxwhite);

	    string dbxenv;
	    if (base[0] == '$')
		dbxenv = "set";
	    else
		dbxenv = "dbxenv";

	    e_type = entry_type(type, base, get_dbx_help(dbxenv, base), value);
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
	}
        break;

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

	    Widget option_label = XmOptionLabelGadget(entry);
	    XtUnmanageChild(option_label);
	}
	break;

    case OtherOptionMenuEntry:
    case TargetOptionMenuEntry:
	{
	    // set language / set demangle / set architecture / set endian
	    arg = 0;
	    Widget menu = 
		verify(XmCreatePulldownMenu(form, "menu", args, arg));

	    string options;
	    char separator = '\n';

	    switch (gdb->type())
	    {
	    case GDB:
		if (base == "architecture")
		{
		    // Possible options are listed upon `info architecture'
		    // and separated by ` '.
		    options = cached_gdb_question("info " + base);
		    options = "auto" + options.from('\n');
		    separator = ' ';
		}
		else if (base == "endian")
		{
		    // Hardwired options
		    options = "auto\nbig endian\nlittle endian\n";
		}
		else
		{
		    // Possible options are listed upon `set BASE'
		    options = cached_gdb_question("set " + base);
		}
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
	    case JDB:
		return;		// FIXME
	    }

	    while (options != "")
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
	    entry = verify(XmCreateOptionMenu(form, set_command, args, arg));
	    XtManageChild(entry);

	    Widget option_label = XmOptionLabelGadget(entry);
	    XtUnmanageChild(option_label);
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

    Dimension width = preferred_width(label);
    if (entry != label)
	width += preferred_width(entry);
    width += preferred_width(help);

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
	string init = app_data.gdb_init_commands;
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

    max_width = max(width, max_width);

    row++;
}


// Add buttons
static void add_settings(Widget form, int& row, Dimension& max_width,
			 DebuggerType type, EntryType entry_filter,
			 string gdb_class)
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
    {
	// Some DBXes know `dbxenv'; others know `set'.  We cannot use
	// is_invalid() or likewise here, since the output may contain
	// `error' strings that would cause is_invalid() to fail.
	// Just try both and use the longer reply (may take time).

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
    }

    while (commands != "")
    {
	string line = commands.before('\n');
	commands    = commands.after('\n');
	add_button(form, row, max_width, type, entry_filter, line);
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
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    Widget panel = verify(XmCreatePromptDialog(find_shell(), 
					       create_settings ? 
					       "settings" : "infos", 
					       args, arg));
    Delay::register_shell(panel);

    if (lesstif_version <= 79)
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
    Dimension max_width = 0;
    int row = 0;
    int last_row = row;

    if (create_settings)
    {
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
    }
    else
    {
	add_settings(form, row, max_width, type, 
		     DisplayToggleButtonEntry, "info");
    }

    // Clean up cached documentation stuff
    clear_gdb_question_cache();

    // Setup values
    if (create_settings)
    {
	reset_settings_button = cancel;
	update_reset_settings_button();
    }
    else
    {
	reset_infos_button = cancel;
	update_infos();
    }

    // Set number of rows
    if (row > 0)
	XtVaSetValues(form, XmNfractionBase, row, NULL);
    XtManageChild(form);

    // Set sizes
    max_width += EXTRA_SPACE;
    XtVaSetValues(form, 
		  XmNwidth, max_width, NULL);

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
		  NULL);

    if (height + spacing > MAX_HEIGHT)
    {
	// Form must be scrolled
	Dimension scrollbar_width = 15;   // Additional space for scrollbar

	if (vertical_scroll_bar != 0)
	{
	    XtWidgetGeometry size;
	    size.request_mode = CWWidth;
	    XtQueryGeometry(vertical_scroll_bar, NULL, &size);
	    scrollbar_width = size.width;
	}
	
	XtVaSetValues(scroll,
		      XmNheight, MAX_HEIGHT,
		      XmNwidth, max_width + 
		                spacing + scrollbar_width + EXTRA_WIDTH,
		      NULL);
    }
    else
    {
	// Form need not be scrolled
	XtVaSetValues(scroll,
		      XmNheight, height + spacing,
		      XmNwidth, max_width + spacing,
		      NULL);
    }

    XtManageChild(scroll);

    InstallButtonTips(panel);

    return panel;
}


// Create settings editor
static Widget create_settings(DebuggerType type)
{
    if (settings_panel == 0 && gdb->isReadyWithPrompt() && gdb->type() == type)
    {
	settings_panel = create_panel(type, true);
	get_defines(type);
    }
    return settings_panel;
}

// Create infos editor
static Widget create_infos(DebuggerType type)
{
    if (infos_panel == 0 && gdb->isReadyWithPrompt() && gdb->type() == type)
	infos_panel = create_panel(type, false);
    return infos_panel;
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

// True iff settings might have changed
bool need_settings()
{
    return settings_panel != 0;
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
	    if (base == "dbxenv disassembler_version" ||
		base == "dbxenv rtc_error_log_file_name" ||
		base == "dbxenv output_log_file_name")
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
	    else if (base.contains("set $cur", 0) ||
		     base.contains("set $new", 0) ||
		     base.contains("set $pid", 0))
	    {
		// Do nothing - dependent on the current file (DEC DBX)
	    }
	    else if (base == "set $defaultin" ||
		     base == "set $defaultout" ||
		     base == "set $historyevent")
	    {
		// Do nothing - dependent on the current state (SGI DBX)
	    }
	    else if (base.contains("set $", 0))
	    {
		// Add setting (DBX).
		command += base + " = " + value + '\n';
	    }
	    else if (base.contains("set ", 0))
	    {
		// Add setting (GDB).

		// Jonathan Edwards <edwards@intranet.com> states:
		// DDD gets all confused with gdb radix settings. When
		// it configures gdb it assumes a decimal radix. But
		// if you specify a non-decimal radix, then all
		// settings made after that are incorrect.  I would
		// suggest prefixing "0d" to all decimal numbers.
		if (value.matches(rxint) && atoi(value) > 1)
		    value.prepend("0d");

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
	case JDB:
	    // Add setting
	    command += base + ' ' + value + '\n';
	    break;
	}
    }

    return command;
}



//-----------------------------------------------------------------------
// Trace Command Definitions
//-----------------------------------------------------------------------

static bool update_define_needed = false;

static StringStringAssoc defs;

// Call this function if command definitions have changed
void set_need_defines(bool val)
{
    update_define_needed = val;
}

bool need_defines()
{
    return update_define_needed;
}

static bool update_define(const string& command)
{
    // We have a new command - update user buttons.
    clear_help_cache(command);
    update_user_buttons();

    string text = gdb_question("show user " + command);
    if (text == NO_GDB_ANSWER)
	return false;

    // `Undefined' means command is undefined; `Not a user command'
    // means this is a built-in command.
    if (text.contains("Undefined", 0) || text.contains("Not a", 0))
    {
	defs.remove(command);
	return true;
    }

    string def  = "";

    while (text != "")
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
    if (!need_defines())
	return;

    StatusDelay delay("Retrieving Command Definitions");

    string commands = gdb_question("help user-defined");
    if (commands == NO_GDB_ANSWER)
	return;

    while (commands != "")
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

    set_need_defines(false);
}

// Get current definitions
string get_defines(DebuggerType type)
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

    for (StringStringAssocIter iter(defs); iter.ok(); iter++)
    {
	string def = iter.value();
	if (def == "")
	{
	    // We don't save empty definitions, such that users have a
	    // way to get rid of them.
	}
	else
	{
	    defines += "define " + iter.key() + "\n" + def + "end\n";
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

    string word = command;
    strip_space(word);
    if (word.contains(' '))
	word = word.before(' ');

    return defs.has(word);
}


//-----------------------------------------------------------------------
// Edit Command Definitions
//-----------------------------------------------------------------------

// Data

static Widget name_w;		// Name of defined command
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

static void add_button(string name, String& menu)
{
    string s = menu;
    if (s != "" && !s.contains('\n', -1))
	s += '\n';
    s += name + "\n";
    menu = (String)XtNewString(s.chars());
}

static void remove_button(string name, String& menu)
{
    string s = string("\n") + menu;
    s.gsub("\n" + name + "\n", string("\n"));
    menu = (String)XtNewString(s.chars() + 1);
}

enum ButtonTarget { ConsoleTarget, SourceTarget, DataTarget };

static String &target_string(ButtonTarget t)
{
    switch (t)
    {
    case ConsoleTarget:
	return app_data.console_buttons;
	break;

    case SourceTarget:
	return app_data.source_buttons;
	break;

    case DataTarget:
	return app_data.data_buttons;
	break;
    }

    static String null = 0;
    return null;
}

static void ToggleButtonCB(Widget, XtPointer client_data, XtPointer call_data)
{
    string name = current_name();
    ButtonTarget target = (ButtonTarget) (long) client_data;

    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    String& str = target_string(target);

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

MMDesc button_menu[] =
{
    { "console", MMToggle, { ToggleButtonCB, XtPointer(ConsoleTarget) }},
    { "source",  MMToggle, { ToggleButtonCB, XtPointer(SourceTarget) }},
    { "data",    MMToggle, { ToggleButtonCB, XtPointer(DataTarget) }},
    MMEnd
};

static void refresh_toggle(ButtonTarget t)
{
    Widget& w = button_menu[t].widget;
    string s = string("\n") + target_string(t);
    string name = current_name();

    Boolean old_state;
    XtVaGetValues(w, XmNset, &old_state, NULL);

    Boolean new_state = s.contains("\n" + name + "\n");
    if (old_state != new_state)
	XtVaSetValues(w, XmNset, new_state, NULL);

#if 1
    set_sensitive(w, name != "");
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
    // Refresh combo box
    StringArray commands;
    for (StringStringAssocIter iter(defs); iter.ok(); iter++)
	commands += iter.key();
    smart_sort(commands);
    ComboBoxSetList(name_w, commands);
}

// Editing stuff

// Text field has changed -- update buttons
void UpdateDefinePanelCB(Widget, XtPointer, XtPointer)
{
    if (name_w == 0)
	return;			// Not yet created

    string name = current_name();

    set_sensitive(record_w, !gdb->recording() && name != "");
    set_sensitive(apply_w,  !gdb->recording() && name != "" && defs.has(name));
    set_sensitive(end_w,    gdb->recording());
    set_sensitive(edit_w,   !gdb->recording() && name != "" && defs.has(name));

    set_sensitive(name_w, !gdb->recording());
    set_sensitive(XtParent(name_w), !gdb->recording());
    set_sensitive(editor_w, !gdb->recording());

    refresh_toggles();
}

static void update_defineHP(Agent *, void *client_data, void *call_data)
{
    bool gdb_ready = bool(call_data);
    if (gdb_ready && !gdb->recording())
    {
	char *c = (char *)client_data;
	bool ok = update_define(c);

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
}

static void update_define_later(const string& command)
{
    char *c = new char[command.length() + 1];
    strcpy(c, (char *)command);

    gdb->addHandler(ReadyForQuestion, update_defineHP, (void *)c);
}


// Log recording state
static void RecordingHP(Agent *, void *, void *call_data)
{
    bool recording = bool(call_data);

    if (!recording)
    {
	// Recording is over.  Don't get called again.
	gdb->removeHandler(Recording, RecordingHP);

	// Fetch new command definition
	string name = current_name();
	update_define_later(name);
    }

    UpdateDefinePanelCB();
}

static void RecordCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    gdb->removeHandler(Recording, RecordingHP);
    gdb->addHandler(Recording, RecordingHP);

    string name = current_name();
    gdb_command("define " + name, w);
}

// Activate the button given in CLIENT_DATA
static void ActivateCB(Widget, XtPointer client_data, 
		       XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    
    Widget button = Widget(client_data);
    XtCallActionProc(button, "ArmAndActivate", cbs->event, (String *)0, 0);
}

static void EndCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    gdb_command("end", w);
}

static void EditCommandDefinitionCB(Widget w, XtPointer, XtPointer)
{
    string name = current_name();

    if (XtIsManaged(XtParent(editor_w)))
    {
	XtUnmanageChild(XtParent(editor_w));
	set_sensitive(name_w, True);
	set_sensitive(XtParent(name_w), True);

	MString label = "Edit " + MString(">>", "small");
	set_label(edit_w, label);

	String _commands = XmTextGetString(editor_w);
	string cmd = _commands;
	XtFree(_commands);

	if (!cmd.contains('\n', -1))
	    cmd += '\n';

	if (cmd != defs[name])
	{
	    StringArray commands;
	    while (cmd != "")
	    {
		string c = cmd.before('\n');
		if (c != "")
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
    else
    {
	// update_define(name);
	set_sensitive(name_w, False);
	set_sensitive(XtParent(name_w), False);

	string def = "";
	if (defs.has(name))
	    def = defs[name];

	XmTextSetString(editor_w, (String)def);

	XtManageChild(XtParent(editor_w));
	MString label = "Edit " + MString("<<", "small");
	set_label(edit_w, label);
    }
}

// Apply the given command
static void ApplyCB(Widget, XtPointer, XtPointer)
{
    string name = current_name();
    if (name != "")
 	gdb_command(name);
}

MMDesc commands_menu[] =
{
    { "record", MMPush, \
      { RecordCommandDefinitionCB }, 0, &record_w },
    { "end",    MMPush | MMInsensitive, \
      { EndCommandDefinitionCB }, 0, &end_w },
    { "edit",   MMPush, \
      { EditCommandDefinitionCB }, 0, &edit_w },
    MMEnd
};

static MMDesc panel_menu[] = 
{
    { "name",     MMComboBox, { UpdateDefinePanelCB }, 0, &name_w },
    { "commands", MMButtonPanel, MMNoCB, commands_menu },
    { "button",   MMButtonPanel, MMNoCB, button_menu },
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
					     "define_command",
					     args, arg));
	XtVaSetValues(dialog, XmNdefaultButton, Widget(0), NULL);

	// Remove old prompt
	Widget text = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
	XtUnmanageChild(text);
	Widget old_label = 
	    XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL);
	XtUnmanageChild(old_label);

	Delay::register_shell(dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
						   XmDIALOG_APPLY_BUTTON));
	apply_w = XmSelectionBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
	
	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
	Widget form = XmCreateRowColumn(dialog, "form", args, arg);
	XtManageChild(form);

	Widget panel = MMcreatePanel(form, "panel", panel_menu);

	XtVaSetValues(panel,
		      XmNmarginWidth,    0,
		      XmNmarginHeight,   0,
		      NULL);

	arg = 0;
	XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
        editor_w = XmCreateScrolledText(form, "text", args, arg);
	XtUnmanageChild(XtParent(editor_w));
	XtManageChild(editor_w);

	MMaddCallbacks(panel_menu);
	InstallButtonTips(panel);

	MMadjustPanel(panel_menu);

	XtAddCallback(dialog, XmNokCallback, UnmanageThisCB, 
		      XtPointer(dialog));
	XtAddCallback(dialog, XmNcancelCallback, ApplyCB, NULL);
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
	XtAddCallback(name_w, XmNactivateCallback, ActivateCB, 
		      XtPointer(record_w));

	set_need_defines(true);
	update_defines();
    }

    UpdateDefinePanelCB();
    refresh_combo_box();
    manage_and_raise(dialog);
}
