// $Id$ -*- C++ -*-
//

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

char options_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "options.h"

#include "AppData.h"
#include "DataDisp.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "SourceView.h"
#include "cook.h"
#include "ddd.h"
#include "post.h"
#include "status.h"
#include "string-fun.h"
#include "windows.h"
#include "settings.h"

#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <Xm/Scale.h>

#include <stdio.h>
#include <fstream.h>



//-----------------------------------------------------------------------------
// Source Options
//-----------------------------------------------------------------------------

void sourceToggleFindWordsOnlyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.find_words_only = info->set;

    if (info->set)
	set_status("Finding only complete words.");
    else
	set_status("Finding arbitrary occurrences.");

    update_options();
}

void sourceToggleCacheSourceFilesCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_source_files = info->set;

    if (info->set)
	set_status("Caching source texts.");
    else
	set_status("Not caching source texts.  "
		   "Source text cache has been cleared.");

    update_options();
}

void sourceToggleCacheMachineCodeCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_machine_code = info->set;

    if (info->set)
	set_status("Caching machine code.");
    else
	set_status("Not caching machine code.  "
		   "Machine code cache has been cleared.");

    update_options();
}

void sourceSetUseSourcePathCB (Widget, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.use_source_path = state;
    string referring_to_sources_using =
	"Referring to sources using ";

    if (state)
	set_status(referring_to_sources_using + "full source file paths.");
    else
	set_status(referring_to_sources_using + "source file base names.");

    source_arg->set_string(source_view->line_of_cursor());
    update_options();
}

void sourceSetDisplayGlyphsCB (Widget, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.display_glyphs = state;
    string displaying_breakpoints_and_positions =
	"Displaying breakpoints and positions ";

    if (state)
	set_status(displaying_breakpoints_and_positions + " as glyphs.");
    else
	set_status(displaying_breakpoints_and_positions + " in the text.");

    update_options();
}

void sourceToggleDisassembleCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.disassemble = info->set;

    if (info->set)
	set_status("Showing machine code.");
    else
	set_status("Not showing machine code.");

    update_options();
}

void sourceToggleAllRegistersCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.all_registers = info->set;

    if (info->set)
	set_status("Showing all registers.");
    else
	set_status("Showing integer registers only.");

    update_options();
}

void sourceSetTabWidthCB (Widget, XtPointer, XtPointer call_data)
{
    XmScaleCallbackStruct *info = (XmScaleCallbackStruct *)call_data;

    app_data.tab_width = info->value;

    set_status("Tab width set to " + itostring(app_data.tab_width) + ".");

    update_options();
}

//-----------------------------------------------------------------------------
// Graph Options
//-----------------------------------------------------------------------------

void graphToggleDetectAliasesCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.detect_aliases = info->set;

    if (info->set)
	set_status("Alias detection enabled.");
    else
	set_status("Alias detection disabled.");

    update_options();
}

void graphToggleShowHintsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNshowHints, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Hints on.");
    else
	set_status("Hints off.");

    update_options();
}


void graphToggleSnapToGridCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNsnapToGrid, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Snap to grid on.");
    else
	set_status("Snap to grid off.");

    update_options();
}


void graphToggleCompactLayoutCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    LayoutMode mode = RegularLayoutMode;
    if (info->set)
	mode = CompactLayoutMode;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNlayoutMode, mode); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Compact layout enabled.");
    else
	set_status("Regular layout enabled.");

    update_options();
}

void graphToggleAutoLayoutCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNautoLayout, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Automatic layout on.");
    else
	set_status("Automatic layout off.");

    update_options();
}

void graphSetGridSizeCB (Widget, XtPointer, XtPointer call_data)
{
    XmScaleCallbackStruct *info = (XmScaleCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;

    if (info->value >= 2)
    {
	XtSetArg(args[arg], XtNgridWidth,  info->value); arg++;
	XtSetArg(args[arg], XtNgridHeight, info->value); arg++;
	XtSetArg(args[arg], XtNshowGrid,   True); arg++;
	XtSetValues(data_disp->graph_edit, args, arg);
	set_status("Grid size set to " + itostring(info->value) + ".");
    }
    else
    {
	XtSetArg(args[arg], XtNshowGrid, False); arg++;
	XtSetValues(data_disp->graph_edit, args, arg);
	set_status("Grid off.");
    }

    update_options();
}

//-----------------------------------------------------------------------------
// General Options
//-----------------------------------------------------------------------------

void dddToggleGroupIconifyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.group_iconify = info->set;
    string ddd_windows_are_iconified = 
	DDD_NAME " windows are iconified ";

    if (info->set)
	set_status(ddd_windows_are_iconified + "as a group.");
    else
	set_status(ddd_windows_are_iconified + "separately.");

    update_options();
}

void dddSetGlobalTabCompletionCB(Widget, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.global_tab_completion = state;

    if (state)
	set_status("TAB key completes in all " DDD_NAME " windows.");
    else
	set_status("TAB key completes in " DDD_NAME " debugger console only.");

    update_options();
}

void dddToggleSeparateExecWindowCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.separate_exec_window = info->set;
    string debugged_program_will_be_executed_in =
	"Debugged program will be executed in ";

    if (info->set)
	set_status(debugged_program_will_be_executed_in 
		   + "a separate execution window.");
    else
	set_status(debugged_program_will_be_executed_in 
		   + "the " DDD_NAME " debugger console.");

    update_options();
}

void dddToggleUngrabMousePointerCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.ungrab_mouse_pointer = info->set;
    string automatic_ungrabbing_of_mouse_pointer =
	"Automatic ungrabbing of mouse pointer";

    if (info->set)
	set_status(automatic_ungrabbing_of_mouse_pointer + "enabled.");
    else
	set_status(automatic_ungrabbing_of_mouse_pointer + "disabled.");

    update_options();
}

void dddToggleSaveHistoryOnExitCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.save_history_on_exit = info->set;

    if (info->set)
	set_status("History will be saved when " DDD_NAME " exits.");
    else
	set_status("History will not be saved.");

    update_options();
}

void dddToggleSuppressWarningsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.suppress_warnings = info->set;

    if (info->set)
	set_status("X Warnings are suppressed.");
    else
	set_status("X Warnings are not suppressed.");

    update_options();
}

void dddToggleButtonTipsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.button_tips = info->set;

    if (info->set)
	set_status("Button tips enabled.");
    else
	set_status("Button tips disabled.");

    update_options();
}

void dddToggleValueTipsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.value_tips = info->set;

    if (info->set)
	set_status("Value tips enabled.");
    else
	set_status("Value tips disabled.");

    update_options();
}

void dddToggleButtonDocsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.button_docs = info->set;

    if (info->set)
	set_status("Button docs enabled.");
    else
	set_status("Button docs disabled.");

    update_options();
}

void dddToggleValueDocsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.value_docs = info->set;

    if (info->set)
	set_status("Value docs enabled.");
    else
	set_status("Value docs disabled.");

    update_options();
}


//-----------------------------------------------------------------------------
// Startup Options
//-----------------------------------------------------------------------------
static void post_startup_warning(Widget w)
{
    (void) w;
#if 0
    post_warning(
	"This change will be effective only after\n"
	"options are saved and " DDD_NAME " is restarted.", 
	"startup_warning", w);
#endif
}

static string next_ddd_will_start_with = 
    "Next " DDD_NAME " invocation will start-up with ";

void dddSetSeparateWindowsCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.separate_data_window   = state;
    app_data.separate_source_window = state;

    if (state)
	set_status(next_ddd_will_start_with + "separate windows.");
    else
	set_status(next_ddd_will_start_with + "one single window.");

    update_options();
    post_startup_warning(w);
}

void dddSetStatusAtBottomCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.status_at_bottom = state;

    if (state)
	set_status(next_ddd_will_start_with + "status at bottom.");
    else
	set_status(next_ddd_will_start_with + "status at top.");

    update_options();
    post_startup_warning(w);
}

void dddSetToolBarCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.tool_bar = state;
    string tool_buttons_are_located_in = "Tool buttons are located in ";

    if (state)
	set_status(tool_buttons_are_located_in + "source window.");
    else
	set_status(tool_buttons_are_located_in + "command tool.");

    update_options();
    post_startup_warning(w);
}

void dddSetKeyboardFocusPolicyCB (Widget w, XtPointer client_data, XtPointer)
{
    unsigned char policy = (unsigned char)int(client_data);

    if (policy != XmEXPLICIT && policy != XmPOINTER)
	return;

    StatusDelay delay(policy == XmEXPLICIT ?
		      "Setting click-to-type keyboard focus policy" :
		      "Setting pointer-driven keyboard focus policy");

    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell && !XmIsVendorShell(shell))
	    shell = XtParent(shell);
	if (shell)
	{
	    XtVaSetValues(shell,
			  XmNkeyboardFocusPolicy, policy,
			  NULL);
	}
    }

    string keyboardFocusPolicy = "*" + string(XmNkeyboardFocusPolicy);
    XrmDatabase target = XtDatabase(XtDisplay(w));
    switch (policy)
    {
    case XmEXPLICIT:
	XrmPutStringResource(&target, keyboardFocusPolicy, "EXPLICIT");
	break;

    case XmPOINTER:
	XrmPutStringResource(&target, keyboardFocusPolicy, "POINTER");
	break;
    }

    update_options();
}

void dddSetPannerCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);
    app_data.panned_graph_editor = state;

    if (state)
	set_status(next_ddd_will_start_with + "a panned graph editor.");
    else
	set_status(next_ddd_will_start_with + "a scrolled graph editor.");

    update_options();
    post_startup_warning(w);
}

void dddSetDebuggerCB (Widget w, XtPointer client_data, XtPointer)
{
    DebuggerType type = DebuggerType(client_data);

    switch (type)
    {
    case DBX:
	app_data.debugger = "dbx";
	break;

    case GDB:
	app_data.debugger = "gdb";
	break;

    case XDB:
	app_data.debugger = "xdb";
	break;
    }

    set_status(next_ddd_will_start_with + "a " 
	       + upcase(app_data.debugger) + " debugger.");

    update_options();
    post_startup_warning(w);
}

//-----------------------------------------------------------------------------
// Option management
//-----------------------------------------------------------------------------

inline String bool_value(bool value)
{
    return value ? "true" : "false";
}

inline string bool_app_value(const string& name, bool value)
{
    return DDD_CLASS_NAME "*" + name + ": " + bool_value(value);
}

inline string int_app_value(const string& name, int value)
{
    return DDD_CLASS_NAME "*" + name + ": " + itostring(value);
}

string string_app_value(const string& name, string value)
{
    value = cook(value);
    if (value.contains("\\n"))
    {
	value.gsub("\\n", "\\n\\\n");
	if (value.contains("\\\n", -1))
	    value = value.before(int(value.length()) - 2);
	value = "\\\n" + value;
    }

    return DDD_CLASS_NAME "*" + name + ": " + value;
}

string widget_value(Widget w, String name)
{
    String value = 0;
    XtVaGetValues(w, 
		  XtVaTypedArg, name, XtRString, &value, sizeof(value),
		  NULL);

    if (value == 0)
	value = (String)XtNewString("");

    return string_app_value(string(XtName(w)) + "." + name, value);
}

string options_file()
{
    char *home = getenv("HOME");
    if (home == 0)
    {
	static int warned = 0;
	if (warned++ == 0)
	    cerr << "Warning: environment variable HOME undefined\n";
	home = ".";
    }

    return string(home) + "/.dddinit";
}

void save_options(Widget origin)
{
    StatusDelay delay("Saving options in " + quote(options_file()));
    string file = options_file();

    const char delimiter[] = "! DO NOT ADD ANYTHING BELOW THIS LINE";

    // Read the file contents into memory ...
    string dddinit;
    ifstream is(file);
    if (is.bad())
    {
	// File not found: create a new one
	dddinit = 
	    "! DDD initialization file\n"
	    "! Enter your personal DDD resources here.\n"
	    "\n";
    }
    else
    {
	char line[BUFSIZ];
	while (is)
	{
	    line[0] = '\0';
	    is.getline(line, sizeof(line));
	    if (string(line).contains(delimiter, 0))
		break;
	    dddinit += line;
	    dddinit += '\n';
	}
    }

    // ... and write them back again
    ofstream os(file);
    if (os.bad())
    {
	post_error("Cannot save options in " + quote(file),
		   "options_save_error", origin);
	return;
    }

    os << dddinit << delimiter << " -- " DDD_NAME " WILL OVERWRITE IT\n";

    // The version
    os << string_app_value(XtNdddinitVersion,
			   DDD_VERSION) << "\n";

    // Debugger settings
    string gdb_settings = app_data.gdb_settings;
    string dbx_settings = app_data.dbx_settings;
    string xdb_settings = app_data.xdb_settings;

    switch (gdb->type())
    {
    case GDB:
	gdb_settings = get_settings(GDB);
	break;

    case DBX:
	dbx_settings = get_settings(DBX);
	break;

    case XDB:
	xdb_settings = get_settings(XDB);
	break;
    }

    os << string_app_value(XtNgdbSettings, gdb_settings) << "\n";
    os << string_app_value(XtNdbxSettings, dbx_settings) << "\n";
    os << string_app_value(XtNxdbSettings, xdb_settings) << "\n";


    // Some settable top-level defaults
    os << bool_app_value(XtNfindWordsOnly,
			 app_data.find_words_only) << "\n";
    os << int_app_value(XtNtabWidth,
			 app_data.tab_width) << "\n";
    os << bool_app_value(XtNcacheSourceFiles,
			 app_data.cache_source_files) << "\n";
    os << bool_app_value(XtNcacheMachineCode,
			 app_data.cache_machine_code) << "\n";
    os << bool_app_value(XtNuseSourcePath,
			 app_data.use_source_path) << "\n";
    os << bool_app_value(XtNdisplayGlyphs,
			 app_data.display_glyphs) << "\n";
    os << bool_app_value(XtNdisassemble,
			 app_data.disassemble) << "\n";
    os << bool_app_value(XtNallRegisters,
			 app_data.all_registers) << "\n";
    os << bool_app_value(XtNgroupIconify,
			 app_data.group_iconify) << "\n";
    os << bool_app_value(XtNbuttonTips,
			 app_data.button_tips) << "\n";
    os << bool_app_value(XtNvalueTips,
			 app_data.value_tips) << "\n";
    os << bool_app_value(XtNbuttonDocs,
			 app_data.button_docs) << "\n";
    os << bool_app_value(XtNvalueDocs,
			 app_data.value_docs) << "\n";
    os << bool_app_value(XtNstatusAtBottom,
			 app_data.status_at_bottom) << "\n";
    os << bool_app_value(XtNtoolBar,
			 app_data.tool_bar) << "\n";
    os << bool_app_value(XtNseparateExecWindow,
			 app_data.separate_exec_window) << "\n";
    if (!app_data.separate_source_window && !app_data.separate_data_window)
    {
	os << bool_app_value(XtCSeparate, false) << "\n";
    }
    else if (app_data.separate_source_window && app_data.separate_data_window)
    {
	os << bool_app_value(XtCSeparate, true) << "\n";
    }
    else
    {
	os << bool_app_value(XtNseparateSourceWindow, 
			     app_data.separate_source_window) << "\n";
	os << bool_app_value(XtNseparateDataWindow, 
			     app_data.separate_data_window) << "\n";
    }
    os << bool_app_value(XtNpannedGraphEditor,
			 app_data.panned_graph_editor) << "\n";
    os << bool_app_value(XtNsuppressWarnings,
			 app_data.suppress_warnings) << "\n";
    os << bool_app_value(XtNungrabMousePointer,
			 app_data.ungrab_mouse_pointer) << "\n";
    os << bool_app_value(XtNsaveHistoryOnExit,
			 app_data.save_history_on_exit) << "\n";
    os << string_app_value(XtNdebugger,
			   app_data.debugger) << "\n";
    os << string_app_value(XtNprintCommand,
			   app_data.print_command) << "\n";
    os << string_app_value(XtNpaperSize,
			   app_data.paper_size) << "\n";

    unsigned char policy = '\0';
    XtVaGetValues(command_shell, XmNkeyboardFocusPolicy, &policy, NULL);
    switch (policy)
    {
    case XmPOINTER:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "POINTER") 
	   << "\n";
	break;
    case XmEXPLICIT:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "EXPLICIT")
	   << "\n";
	break;
    }

    os << bool_app_value(XtNdetectAliases, app_data.detect_aliases) << "\n";
    os << bool_app_value(XtNblinkWhileBusy, app_data.blink_while_busy) << "\n";

    // Some settable graph editor defaults
    os << widget_value(data_disp->graph_edit, XtNshowGrid)   << "\n";
    os << widget_value(data_disp->graph_edit, XtNsnapToGrid) << "\n";
    os << widget_value(data_disp->graph_edit, XtNshowHints)  << "\n";
    os << widget_value(data_disp->graph_edit, XtNlayoutMode) << "\n";
    os << widget_value(data_disp->graph_edit, XtNautoLayout) << "\n";

    Dimension grid_width, grid_height;
    XtVaGetValues(data_disp->graph_edit,
		  XtNgridWidth, &grid_width,
		  XtNgridHeight, &grid_height,
		  NULL);
    if (grid_width == grid_height)
    {
	os << int_app_value(XtCGridSize, grid_width) << "\n";
    }
    else
    {
	os << int_app_value(XtNgridWidth,  grid_width) << "\n";
	os << int_app_value(XtNgridHeight, grid_height) << "\n";
    }

    save_option_state();
    save_settings_state();
}

void DDDSaveOptionsCB (Widget w, XtPointer, XtPointer)
{
    save_options(w);
}
