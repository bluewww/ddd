// $Id$ -*- C++ -*-
// Save and edit DDD options

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char options_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "options.h"

#include "config.h"

#include "AppData.h"
#include "DataDisp.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "SourceView.h"
#include "cook.h"
#include "Command.h"
#include "ddd.h"
#include "file.h"
#include "filetype.h"
#include "post.h"
#include "session.h"
#include "settings.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "windows.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Scale.h>
#include <Xm/DialogS.h>
#include <Xm/BulletinB.h>
#include <Xm/MessageB.h>

#include <stdio.h>
#include <fstream.h>
#include <unistd.h>

#if defined(HAVE_LINK) && !defined(HAVE_LINK_DECL)
extern "C" int link (const char *oldname, const char *newname);
#endif

#if defined(HAVE_SYMLINK) && !defined(HAVE_SYMLINK_DECL)
extern "C" int symlink (const char *oldname, const char *newname);
#endif

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

    update_options();

    if (info->set)
	set_status("Caching machine code.");
    else
	set_status("Not caching machine code.  "
		   "Machine code cache has been cleared.");
}

void sourceToggleDisplayLineNumbersCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.display_line_numbers = info->set;

    update_options();

    if (info->set)
	set_status("Displaying line numbers.");
    else
	set_status("Not displaying line numbers.");
}

void sourceSetUseSourcePathCB (Widget, XtPointer client_data, XtPointer)
{
    Boolean state = int(client_data);

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
    Boolean state = int(client_data);

    app_data.display_glyphs = state;

    update_options();

    string displaying_breakpoints_and_positions =
	"Displaying breakpoints and positions ";
    if (state)
	set_status(displaying_breakpoints_and_positions + " as glyphs.");
    else
	set_status(displaying_breakpoints_and_positions + " in the text.");
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
    update_options();

    set_status("Tab width set to " + itostring(app_data.tab_width) + ".");
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
    Boolean state = int(client_data);

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
    Boolean state = int(client_data);

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
    Boolean state = int(client_data);

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
    Boolean state = int(client_data);

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

    if (policy == XmPOINTER)
    {
	// Leave old focus on the default button
	Widget default_button = 0;

	Widget bulletin_board = w;
	while (bulletin_board != 0 && 
	       !XtIsSubclass(bulletin_board, xmBulletinBoardWidgetClass))
	    bulletin_board = XtParent(bulletin_board);

	if (bulletin_board != 0)
	{
	    XtVaGetValues(bulletin_board, 
			  XmNdefaultButton, &default_button, NULL);
	}

	if (default_button == 0)
	    default_button = w;

	XmProcessTraversal(default_button, XmTRAVERSE_CURRENT);
    }

    // Apply to existing shells
    const WidgetArray& shells = Delay::shells();
    for (int i = 0; i < shells.size(); i++)
    {
	Widget shell = shells[i];
	while (shell != 0 && !XtIsSubclass(shell, vendorShellWidgetClass))
	    shell = XtParent(shell);
	if (shell != 0)
	    XtVaSetValues(shell, XmNkeyboardFocusPolicy, policy, NULL);
    }

    // Apply to future shells
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

    if (policy == XmEXPLICIT)
    {
	// Place new focus on this button
	XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    }

    update_options();
}

void dddSetPannerCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = int(client_data);
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
    DebuggerType type = DebuggerType(int(client_data));

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

void dddSetStartupLogoCB (Widget w, XtPointer client_data, XtPointer)
{
    app_data.show_startup_logo = String(client_data);

    update_options();
    post_startup_warning(w);
}




// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void dddSetEditCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.edit_command = command;
    set_status("Edit Sources command is " + quote(command));
    update_options();
}


void dddSetGetCoreCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.get_core_command = command;
    set_status("Get Core command is " + quote(command));
    update_options();
}


void dddSetPSCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.ps_command = command;
    set_status("List Processes command is " + quote(command));
    update_options();
}


void dddSetTermCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.term_command = command;
    set_status("Execution Window command is " + quote(command));
    update_options();
}


void dddSetUncompressCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.uncompress_command = command;
    set_status("Uncompress command is " + quote(command));
    update_options();
}


void dddSetWWWCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.www_command = command;
    set_status("Web Browser is " + quote(command));
    update_options();
}





// ---------------------------------------------------------------------------
// Get core
// ---------------------------------------------------------------------------

static bool copy(const string& src, const string& dest)
{
    FILE *from = fopen(src, "r");
    if (from == NULL)
	return false;

    FILE *to = fopen(dest, "w");
    if (to == NULL)
	return false;

    int c;
    while ((c = getc(from)) != EOF)
	putc(c, to);

    fclose(from);
    if (fclose(to) == EOF)
    {
	unlink(dest);
	return false;
    }

    return true;
}

static bool move(const string& from, const string& to)
{
    if (rename(from, to) == 0)
	return true;

    if (copy(from, to) && unlink(from) == 0)
	return true;

    return false;
}


// Get core from running program
static bool _get_core(const string& session, unsigned long flags)
{
    const bool may_kill      = (flags & MAY_KILL);
    const bool dont_save     = (flags & DONT_SAVE);

    create_session_dir(session);
    string target = session_core_file(session);

    ProgramInfo info;
    if (!info.running)
    {
	// The program is not running.
	if (info.core != "" && info.core != NO_GDB_ANSWER)
	{
	    // We already have some core file.
	    if (dont_save)
		return true;	// Fine

	    if (info.core == target)
	    {
		// It is our target.
		return true;
	    }

	    // Remove old target, if any
	    unlink(target);

#ifdef HAVE_LINK
	    // Try a hard link from current core file to target
	    if (link(info.core, target) == 0)
		return true;
#endif
	    
#ifdef HAVE_SYMLINK
	    // Try a symlink link from target to current core file
	    if (symlink(info.core, target) == 0)
		return true;
#endif

	    // Looks as if we have to copy some large core file.  Blechhh.
	    StatusDelay delay("Copying core file");
	    return copy(info.core, target);
	}

	// Program is not running, and we have no core.  Quit.
	return false;
    }

    if (info.pid <= 0)
    {
	// Without process id, no chance to get a core file.
	return false;
    }

    // Try `gcore' command
    string gcore = app_data.get_core_command;
    if (gcore != "" && !dont_save)
    {
	// Get new core file from running process
	StatusDelay delay("Getting core file");

	string gcore_target = target + "." + itostring(info.pid);
	gcore.gsub("@FILE@", target);
	gcore.gsub("@PID@",  itostring(info.pid));
	sh_command(gcore);

	if (is_core_file(gcore_target) && move(gcore_target, target) == 0)
	    return true;
    }

    // Try direct kill.
    if (may_kill)
    {
	if (dont_save)
	    return true;	// Will probably work

	// Get new core file from running process
	StatusDelay delay("Killing process");

	int tries = 10;
	while (tries-- > 0 && kill(info.pid, SIGABRT) == 0)
	{
	    Command c("stepi");
	    c.verbose  = false;
	    c.check    = true;
	    c.priority = COMMAND_PRIORITY_AGAIN;
	    gdb_command(c);
	    syncCommandQueue();
	}

	string core = SourceView::full_path("core");
	if (is_core_file(core))
	{
	    // It worked.  Fine!
	    if (!move(core, target))
	    {
		// Move failed -- sorry
		unlink(core);
		return false;
	    }

	    if (gdb->type() == GDB)
	    {
		// Load the core file just saved, such that we can
		// keep on examining data in this session.
		Command c("core " + target);
		c.verbose  = false;
		c.check    = true;
		c.priority = COMMAND_PRIORITY_AGAIN;
		gdb_command(c);
		c.command  = "graph refresh";
		gdb_command(c);
		c.command  = "# reset";
		gdb_command(c);
		syncCommandQueue();
	    }

	    return true;
	}
    }

    return false;
}

static bool get_core(const string& session, unsigned long flags)
{
    const bool interact      = (flags & MAY_INTERACT);

    bool ok = _get_core(session, flags);
    if (!ok && interact)
	post_warning("Could not save core file.", "incomplete_save_warning");

    return ok;
}

static bool must_kill_to_get_core()
{
    return !_get_core(app_data.session, DONT_SAVE);
}


//-----------------------------------------------------------------------------
// Write state
//-----------------------------------------------------------------------------

inline String bool_value(bool value)
{
    return value ? "true" : "false";
}

string app_value(string resource, const string& value)
{
    String app_name;
    String app_class;
    XtGetApplicationNameAndClass(XtDisplay(find_shell()), 
				 &app_name, &app_class);

    if (resource.contains(string(app_name) + ".", 0))
	return string(app_class) + resource.from(".") + ": " + value;
    else
	return string(app_class) + "*" + resource + ": " + value;
}

inline string bool_app_value(const string& name, bool value)
{
    return app_value(name, bool_value(value));
}

inline string int_app_value(const string& name, int value)
{
    return app_value(name, itostring(value));
}

string string_app_value(const string& name, string value)
{
    value = cook(value);

    // Xt cannot read `\t', so leave it unchanged.
    value.gsub("\\t", '\t');

    if (value.contains("\\n"))
    {
	value.gsub("\\n", "\\n\\\n");
	if (value.contains("\\\n", -1))
	    value = value.before(int(value.length()) - 2);
	value = "\\\n" + value;
    }

    return app_value(name, value);
}

static string widget_value(Widget w, String name)
{
    String value = 0;
    XtVaGetValues(w, 
		  XtVaTypedArg, name, XtRString, &value, sizeof(value),
		  NULL);

    if (value == 0)
	value = "";

    return string_app_value(string(XtName(w)) + "." + name, value);
}

static string widget_size(Widget w)
{
    Dimension width, height;
    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);

    string s;
    s += int_app_value(string(XtName(w)) + "." + XmNwidth, width);
    s += '\n';
    s += int_app_value(string(XtName(w)) + "." + XmNheight, height);

    if (XmIsText(w) || XmIsTextField(w))
    {
	short columns;
	XtVaGetValues(w, XmNcolumns, &columns, NULL);
	if (columns > 0)
	{
	    s += '\n';
	    s += int_app_value(string(XtName(w)) + "." + XmNcolumns, columns);
	}
    }
    if (XmIsText(w))
    {
	short rows;
	XtVaGetValues(w, XmNrows, &rows, NULL);
	if (rows > 0)
	{
	    s += '\n';
	    s += int_app_value(string(XtName(w)) + "." + XmNrows, rows);
	}
    }

    return s;
}

static string widget_geometry(Widget w)
{
    Dimension width, height;
    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);

    XWindowAttributes attr;
    XGetWindowAttributes(XtDisplay(w), frame(w), &attr);

    ostrstream geometry;
    geometry << width << "x" << height << "+" << attr.x << "+" << attr.y;

    return string_app_value(string(XtName(w)) + ".geometry", geometry);
}

bool saving_options_kills_program(unsigned long flags)
{
    ProgramInfo info;

    const bool save_session  = (flags & SAVE_SESSION);
    const bool may_kill      = (flags & MAY_KILL);
    const bool save_core     = (flags & SAVE_CORE);

    return info.running
	&& save_session
	&& save_core
	&& must_kill_to_get_core()
	&& !may_kill;
}

bool saving_options_excludes_data(unsigned long flags)
{
    const bool save_session  = (flags & SAVE_SESSION);
    const bool save_core     = (flags & SAVE_CORE);

    return save_session
	&& !save_core
	&& data_disp->need_core_to_restore();
}

bool save_options(unsigned long flags)
{
    const bool create        = (flags & CREATE_OPTIONS);
    const bool save_session  = (flags & SAVE_SESSION);
    const bool save_geometry = (flags & SAVE_GEOMETRY);
    const bool save_core     = (flags & SAVE_CORE);
    const bool interact      = (flags & MAY_INTERACT);

    string session = 
	(save_session ? app_data.session : (char *)DEFAULT_SESSION);

    create_session_dir(session);
    const string file = session_state_file(session);

    string options = (save_session ? "session" : "options");
    string status = (create ? "Creating " : "Saving ") + options + " in ";

    StatusDelay delay(status + quote(file));

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
    string workfile = file + "#";
    ofstream os(workfile);
    if (os.bad())
    {
	workfile = file;
	os.open(workfile);
    }
    if (os.bad())
    {
	if (interact)
	    post_error("Cannot save " + options + " in " + quote(workfile),
		       "options_save_error");
	return false;
    }

    os << dddinit << delimiter << " -- " DDD_NAME " WILL OVERWRITE IT\n";
    os << string_app_value(XtNdddinitVersion, DDD_VERSION) << "\n";

    if (create)
    {
	app_data.dddinit_version = DDD_VERSION;
	os.close();
	rename(workfile, file);
	return true;
    }

    os << "\n! Debugger settings\n";
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


    os << "\n! Preferences\n";
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
    os << bool_app_value(XtNdisplayLineNumbers,
			 app_data.display_line_numbers) << "\n";
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
    os << string_app_value(XtNshowStartupLogo,
			   app_data.show_startup_logo) << "\n";
    os << bool_app_value(XtNungrabMousePointer,
			 app_data.ungrab_mouse_pointer) << "\n";
    os << bool_app_value(XtNsaveHistoryOnExit,
			 app_data.save_history_on_exit) << "\n";
    os << string_app_value(XtNdebugger,
			   app_data.debugger) << "\n";
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

    get_tool_offset();

    os << int_app_value(XtNtoolRightOffset,
			app_data.tool_right_offset) << "\n";
    os << int_app_value(XtNtoolTopOffset,
			app_data.tool_top_offset) << "\n";

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
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtCGridSize, grid_width) << "\n";
    }
    else
    {
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtNgridWidth,  grid_width) << "\n";
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtNgridHeight, grid_height) << "\n";
    }

    // Helpers
    os << "\n! Helpers\n";
    os << string_app_value(XtNeditCommand,       app_data.edit_command)
       << '\n';
    os << string_app_value(XtNgetCoreCommand,    app_data.get_core_command)
       << '\n';
    os << string_app_value(XtNpsCommand,         app_data.ps_command)
       << '\n';
    os << string_app_value(XtNtermCommand,       app_data.term_command)
       << '\n';
    os << string_app_value(XtNuncompressCommand, app_data.uncompress_command)
       << '\n';
    os << string_app_value(XtNwwwCommand,        app_data.www_command)
       << '\n';
    os << string_app_value(XtNprintCommand,      app_data.print_command) 
       << "\n";

    // Buttons
    os << "\n! Buttons\n";
    os << string_app_value(XtNconsoleButtons, app_data.console_buttons) 
       << '\n';
    os << string_app_value(XtNsourceButtons,  app_data.source_buttons)
       << '\n';
    os << string_app_value(XtNdataButtons,    app_data.data_buttons)
       << '\n';
    os << bool_app_value(XtNverifyButtons,    app_data.verify_buttons)
       << '\n';

    // Shortcut expressions
    os << "\n! Display shortcuts\n";
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
		expr += string('\t') + app_data.label_delimiter + ' ' 
		    + labels[i];
	}
	os << string_app_value(XtNdisplayShortcuts, expr) << "\n";
    }

    // Widget sizes.
    os << "\n! Window sizes\n";

    // We must enable all PanedWindow children in order to get the
    // correct sizes.  Ugly hack.
    popups_disabled  = true;
    bool had_data    = have_data_window();
    bool had_source  = have_source_window();
    bool had_code    = XtIsManaged(source_view->code_form());
    bool had_command = have_command_window();

    gdbOpenDataWindowCB(gdb_w, 0, 0);
    gdbOpenSourceWindowCB(gdb_w, 0, 0);
    XtManageChild(source_view->code_form());
    gdbOpenCommandWindowCB(gdb_w, 0, 0);

    os << widget_size(data_disp->graph_edit)           << "\n";
    Widget porthole_w = XtParent(data_disp->graph_edit);
    if (porthole_w != data_disp->graph_form())
	os << widget_size(porthole_w) << "\n";
    os << widget_size(data_disp->graph_form())         << "\n";
    os << widget_size(source_view->source())           << "\n";
    os << widget_size(source_view->source_form())      << "\n";
    os << widget_size(source_view->code())             << "\n";
    os << widget_size(source_view->code_form())        << "\n";
    os << widget_size(gdb_w)                           << "\n";
    os << widget_size(XtParent(gdb_w))                 << "\n";

    if (!had_data)
	gdbCloseDataWindowCB(gdb_w, 0, 0);
    if (!had_source)
	gdbCloseSourceWindowCB(gdb_w, 0, 0);
    if (!had_code)
	XtUnmanageChild(source_view->code_form());
    if (!had_command)
	gdbCloseCommandWindowCB(gdb_w, 0, 0);
    popups_disabled = false;

    if (save_geometry)
    {
	// Widget geometry
	os << "\n! Last " << DDD_NAME << " geometry\n";

	if (command_shell)
	    os << widget_geometry(command_shell)     << "\n";
	if (source_view_shell)
	    os << widget_geometry(source_view_shell) << "\n";
	if (data_disp_shell)
	    os << widget_geometry(data_disp_shell)   << "\n";
    }

    bool ok = true;
    if (save_session)
    {
	// Restart commands
	os << "\n! Last " << DDD_NAME << " session\n";

	ostrstream rs;

	// Get exec and core file
	ProgramInfo info;
	string core = session_core_file(session);
	if (info.file == NO_GDB_ANSWER)
	{
	    if (interact)
		post_warning("Could not save program name.",
			     "incomplete_save_warning");
	    ok = false;
	}
	else
	{
	    switch (gdb->type())
	    {
	    case GDB:
		rs << "set confirm off\n";
		if (info.file != "")
		    rs << "file " << info.file << "\n";
		rs << "core " << core << "\n";
		break;

	    case DBX:
		if (info.file != "")
		    rs << "debug " << info.file << " " << core << "\n";
		break;

	    case XDB:
		// FIXME
		break;
	    }
	}

	// Get breakpoints and cursor position
	bool breakpoints_ok = source_view->get_state(rs);
	if (!breakpoints_ok)
	{
	    if (interact)
		post_warning("Could not save all breakpoints", 
			     "incomplete_save_warning");
	    ok = false;
	}

	// Get displays
	StringArray scopes;
	bool displays_ok = true;

	if (displays_ok)
	    displays_ok = data_disp->get_scopes(scopes);

	if (save_core)
	    get_core(session, flags);

	if (displays_ok)
	    displays_ok = data_disp->get_state(rs, scopes);

	if (!displays_ok)
	{
	    if (interact)
		post_warning("Could not save all data displays.",
			     "incomplete_save_warning");
	    ok = false;
	}

	os << string_app_value(XtNrestartCommands, string(rs)) << "\n";
    }

    save_option_state();
    save_settings_state();

    os.close();
    if (os.bad())
    {
	if (interact)
	    post_error("Cannot save " + options + " in " + quote(workfile),
		       "options_save_error");
	ok = false;
	unlink(workfile);
    }

    if (workfile != file && rename(workfile, file) != 0)
    {
	if (interact)
	    post_error("Cannot rename " + quote(workfile)
		       + " to " + quote(file),
		       "options_save_error");
	ok = false;
	unlink(workfile);
    }

    return ok;
}

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

static void DoSaveOptionsCB(Widget, XtPointer client_data, XtPointer)
{
    unsigned long flags = (unsigned long)client_data;
    save_options(flags);
}

// Save options
void DDDSaveOptionsCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    unsigned long flags = (unsigned long)client_data;
    if ((flags & SAVE_SESSION) && app_data.session == DEFAULT_SESSION)
    {
	// No current session; cannot save
	return;
    }
    else if (saving_options_kills_program(flags))
    {
	// Saving session would kill program; request confirmation
	static Widget dialog = 0;
	if (dialog)
	    DestroyWhenIdle(dialog);

	dialog = verify(XmCreateQuestionDialog(find_shell(w), 
					       "kill_to_save_dialog",
					       0, 0));
	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, DoSaveOptionsCB, 
		      XtPointer(flags | MAY_KILL));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, 0);

	manage_and_raise(dialog);
    }
    else if (saving_options_excludes_data(flags))
    {
	// Saving session results in data loss; request confirmation
	static Widget dialog = 0;
	if (dialog)
	    DestroyWhenIdle(dialog);

	dialog = verify(XmCreateQuestionDialog(find_shell(w), 
					       "data_not_saved_dialog",
					       0, 0));
	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, DoSaveOptionsCB, 
		      XtPointer(flags));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, 0);

	manage_and_raise(dialog);
    }
    else
    {
	DoSaveOptionsCB(w, client_data, call_data);
    }
}
