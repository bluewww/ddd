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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char options_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "options.h"

#include "AppData.h"
#include "DataDisp.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "cook.h"
#include "ddd.h"
#include "post.h"
#include "status.h"
#include "string-fun.h"
#include "windows.h"

#include <Xm/Xm.h>
#include <Xm/ToggleB.h>

#include <stdio.h>
#include <fstream.h>


//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// True if options were changed
bool options_changed         = false;
bool startup_options_changed = false;


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
    options_changed = true;
}

void sourceToggleCacheSourceFilesCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_source_files = info->set;

    if (info->set)
	set_status("Caching source texts.");
    else
	set_status("Not caching source texts.");

    update_options();
    options_changed = true;
}

void sourceToggleCacheMachineCodeCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cache_machine_code = info->set;

    if (info->set)
	set_status("Caching machine code.");
    else
	set_status("Not caching machine code.");

    update_options();
    options_changed = true;
}

void sourceToggleDisplayGlyphsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.display_glyphs = info->set;

    if (info->set)
	set_status("Displaying breakpoints and positions as glyphs.");
    else
	set_status("Displaying breakpoints and positions in the text.");

    update_options();
    options_changed = true;
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
    options_changed = true;
}

//-----------------------------------------------------------------------------
// Graph Options
//-----------------------------------------------------------------------------

void graphToggleShowGridCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XtNshowGrid, info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Grid on.");
    else
	set_status("Grid off.");

    update_options();
    options_changed = true;
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
    options_changed = true;
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
    options_changed = true;
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
    options_changed = true;
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
	set_status("Automatic Layout on.");
    else
	set_status("Automatic Layout off.");

    update_options();
    options_changed = true;
}

//-----------------------------------------------------------------------------
// General Options
//-----------------------------------------------------------------------------

void dddToggleGroupIconifyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.group_iconify = info->set;

    if (info->set)
	set_status(DDD_NAME " windows are iconified as a group.");
    else
	set_status(DDD_NAME " windows are iconified separately.");

    update_options();
    options_changed = true;
}

void dddToggleGlobalTabCompletionCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.global_tab_completion = info->set;

    if (info->set)
	set_status("TAB key completes in all " DDD_NAME " windows.");
    else
	set_status("TAB key completes in " DDD_NAME " command window only.");

    update_options();
    options_changed = true;
}

void dddToggleSeparateExecWindowCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.separate_exec_window = info->set;

    if (info->set)
	set_status("Debugged program will be executed in a "
		   "separate execution window.");
    else
	set_status("Debugged program will be executed in the " 
		   DDD_NAME " command window.");

    update_options();
    options_changed = true;
}

void dddToggleSaveOptionsOnExitCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.save_options_on_exit = info->set;

    if (info->set)
	set_status("Options will be saved when " DDD_NAME " exits.");
    else
	set_status("Options must be saved manually.");

    update_options();
    options_changed = true;
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
    options_changed = true;
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
    options_changed = true;
}


//-----------------------------------------------------------------------------
// Startup Options
//-----------------------------------------------------------------------------
static void post_startup_warning(Widget /* w */)
{
#if 0
    post_warning(
	"This change will be effective only after\n"
	"options are saved and " DDD_NAME " is restarted.", 
	"startup_warning", w);
#endif
}

void dddSetSeparateWindowsCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);

    app_data.separate_data_window   = state;
    app_data.separate_source_window = state;

    if (state)
	set_status("Next " DDD_NAME
		   " invocation will start-up with separate windows.");
    else
	set_status("Next " DDD_NAME 
		   " invocation will start-up with one single window.");

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
}

void dddSetKeyboardFocusPolicyCB (Widget w, XtPointer client_data, XtPointer)
{
    unsigned char policy = (unsigned char)int(client_data);

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

    string keyboardFocusPolicy = string("*") + string(XmNkeyboardFocusPolicy);
    XrmDatabase target = XtDatabase(XtDisplay(w));
    switch (policy)
    {
    case XmEXPLICIT:
	XrmPutStringResource(&target, keyboardFocusPolicy,  "EXPLICIT");
	set_status("Setting click-to-type keyboard focus policy.");
	break;

    case XmPOINTER:
	XrmPutStringResource(&target, keyboardFocusPolicy,  "POINTER");
	set_status("Setting pointer-driven keyboard focus policy.");
	break;
    }

    update_options();
    startup_options_changed = options_changed = true;
}

void dddSetPannerCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = Boolean(client_data != 0);
    app_data.panned_graph_editor = state;

    if (state)
	set_status("Next " DDD_NAME
		   " invocation will start-up with a panned graph editor.");
    else
	set_status("Next " DDD_NAME 
		   " invocation will start-up with a scrolled graph editor.");

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
}

void dddSetDebuggerCB (Widget w, XtPointer client_data, XtPointer)
{
    DebuggerType type = DebuggerType(client_data);

    switch (type)
    {
    case DBX:
	app_data.debugger = "dbx";
	set_status("Next " DDD_NAME
		   " invocation will start-up with a DBX debugger.");
	break;

    case GDB:
	app_data.debugger = "gdb";
	set_status("Next " DDD_NAME
		   " invocation will start-up with a GDB debugger.");
	break;
    }

    update_options();
    post_startup_warning(w);
    startup_options_changed = options_changed = true;
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
    return string(DDD_CLASS_NAME) + "*" + name + ": " + bool_value(value);
}

inline string string_app_value(const string& name, const string& value)
{
    return string(DDD_CLASS_NAME) + "*" + name + ": " + value;
}

inline string int_app_value(const string& name, int value)
{
    return string(DDD_CLASS_NAME) + "*" + name + ": " + itostring(value);
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
	return ".dddinit";
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

    os << dddinit
       << delimiter
       << " -- " DDD_NAME " WILL OVERWRITE IT\n";

    // The version
    os << string_app_value(XtNdddinitVersion,
			   DDD_VERSION) << "\n";

    // Some settable top-level defaults
    os << bool_app_value(XtNfindWordsOnly,
			 app_data.find_words_only) << "\n";
    os << bool_app_value(XtNcacheSourceFiles,
			 app_data.cache_source_files) << "\n";
    os << bool_app_value(XtNcacheMachineCode,
			 app_data.cache_machine_code) << "\n";
    os << bool_app_value(XtNdisplayGlyphs,
			 app_data.display_glyphs) << "\n";
    os << bool_app_value(XtNdisassemble,
			 app_data.disassemble) << "\n";
    os << bool_app_value(XtNgroupIconify,
			 app_data.group_iconify)   << "\n";
    os << bool_app_value(XtNseparateExecWindow,
			 app_data.separate_exec_window) << "\n";
    os << bool_app_value(XtNseparateSourceWindow,
			 app_data.separate_source_window) << "\n";
    os << bool_app_value(XtNseparateDataWindow,
			 app_data.separate_data_window) << "\n";
    os << bool_app_value(XtNpannedGraphEditor,
			 app_data.panned_graph_editor) << "\n";
    os << bool_app_value(XtNsuppressWarnings,
			 app_data.suppress_warnings) << "\n";
    os << bool_app_value(XtNsaveOptionsOnExit,
			 app_data.save_options_on_exit) << "\n";
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

    // Some settable graph editor defaults
    os << widget_value(data_disp->graph_edit, XtNshowGrid)   << "\n";
    os << widget_value(data_disp->graph_edit, XtNsnapToGrid) << "\n";
    os << widget_value(data_disp->graph_edit, XtNshowHints)  << "\n";
    os << widget_value(data_disp->graph_edit, XtNlayoutMode) << "\n";
    os << widget_value(data_disp->graph_edit, XtNautoLayout) << "\n";

    startup_options_changed = options_changed = false;
}

void DDDSaveOptionsCB (Widget w, XtPointer, XtPointer)
{
    save_options(w);
}
