// $Id$ -*- C++ -*-
// Save and edit DDD options

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
#include "frame.h"
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
#include <errno.h>
#include <string.h>		// strerror

#include <signal.h>

#if HAVE_PTRACE
extern "C" {
#if HAVE_SYS_PTRACE_H
#include <sys/ptrace.h>
#endif
#if !HAVE_PTRACE_DECL
extern int ptrace(int request, int pid, int addr, int data);
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
}
#endif

#if HAVE_LINK && !HAVE_LINK_DECL
extern "C" int link (const char *oldname, const char *newname);
#endif

#if HAVE_SYMLINK && !HAVE_SYMLINK_DECL
extern "C" int symlink (const char *oldname, const char *newname);
#endif

#if !HAVE_POPEN_DECL
extern "C" FILE *popen(const char *command, const char *mode);
#endif
#if !HAVE_PCLOSE_DECL
extern "C" int pclose(FILE *stream);
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

void sourceToggleFindCaseSensitiveCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.find_case_sensitive = info->set;

    if (info->set)
	set_status("Case-sensitive search enabled.");
    else
	set_status("Case-sensitive search disabled.");

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

#if 0
    if (info->set)
	set_status("Displaying line numbers.");
    else
	set_status("Not displaying line numbers.");
#endif
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

    string displaying =	"Displaying breakpoints and positions ";
    if (state)
	set_status(displaying + "as glyphs.");
    else
	set_status(displaying + "as text characters.");
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

void sourceSetSourceIndentCB (Widget, XtPointer, XtPointer call_data)
{
    XmScaleCallbackStruct *info = (XmScaleCallbackStruct *)call_data;

    if (info->value > int(app_data.line_number_width))
    {
	app_data.indent_source = info->value - app_data.line_number_width;
	app_data.display_line_numbers = true;
    }
    else
    {
	app_data.indent_source = info->value;
	app_data.display_line_numbers = false;
    }
    update_options();

    string msg = "Source indentation set to " + 
	itostring(app_data.indent_source) + "; line numbers ";
    if (app_data.display_line_numbers)
	msg += "enabled";
    else
	msg += "disabled";
    msg += ".";

    set_status(msg);
}

void sourceSetCodeIndentCB (Widget, XtPointer, XtPointer call_data)
{
    XmScaleCallbackStruct *info = (XmScaleCallbackStruct *)call_data;

    app_data.indent_code = info->value;
    update_options();

    set_status("Code indentation set to " + 
	       itostring(app_data.indent_code) + ".");
}

//-----------------------------------------------------------------------------
// Graph Options
//-----------------------------------------------------------------------------

void graphToggleDetectAliasesCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.detect_aliases = info->set;
    string alias_detection = "Alias detection ";

    if (info->set)
	set_status(alias_detection + "enabled.");
    else
	set_status(alias_detection + "disabled.");

    update_options();
}

void graphToggleAlign2dArraysCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.align_2d_arrays = info->set;
    string displaying_arrays_as = 
	"Two-dimensional arrays will be displayed as ";

    if (info->set)
	set_status(displaying_arrays_as + "tables.");
    else
	set_status(displaying_arrays_as + "nested one-dimensional arrays.");

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

void dddToggleUniconifyWhenReadyCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.uniconify_when_ready = info->set;

    if (info->set)
	set_status(DDD_NAME " windows will be uniconified automatically "
		   "whenever " DDD_NAME " becomes ready.");
    else
	set_status(DDD_NAME " windows always remain iconified.");

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

void dddToggleCheckGrabsCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.check_grabs = info->set;

    if (info->set)
	set_status("Checking for grabs.");
    else
	set_status("Not checking for grabs.");

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
#if 0
    static bool posted = false;

    if (!posted)
	post_warning("This change will only be effective\n"
		     "after saving options and restarting " DDD_NAME ".",
		     "startup_warning", w);

    posted = true;
#endif
    (void) w;			// Use it
}

static string next_ddd_will_start_with = 
    "Next " DDD_NAME " invocation will start-up with ";

void dddSetSeparateWindowsCB (Widget w, XtPointer client_data, XtPointer)
{
    int state = int(client_data);
    switch (state)
    {
    case 0:
	app_data.separate_data_window   = True;
	app_data.separate_source_window = True;
	app_data.common_toolbar         = False;
	break;

    case 1:
	app_data.separate_data_window   = False;
	app_data.separate_source_window = False;
	app_data.common_toolbar         = True;
	break;

    case 2:
	app_data.separate_data_window   = False;
	app_data.separate_source_window = False;
	app_data.common_toolbar         = False;
	break;
    }

    if (app_data.separate_data_window || app_data.separate_source_window)
	set_status(next_ddd_will_start_with + "separate windows.");
    else if (app_data.common_toolbar)
	set_status(next_ddd_will_start_with + "one window, one toolbar.");
    else
	set_status(next_ddd_will_start_with + "one window, two toolbars.");

    update_options();
    post_startup_warning(w);
}

void dddSetStatusAtBottomCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = int(client_data);

    app_data.status_at_bottom = state;

    if (state)
	set_status(next_ddd_will_start_with + "status line at bottom.");
    else
	set_status(next_ddd_will_start_with + "status line at top.");

    update_options();
    post_startup_warning(w);
}

void dddSetToolBarCB (Widget w, XtPointer client_data, XtPointer)
{
    Boolean state = int(client_data);

    app_data.command_toolbar = state;
    string tool_buttons_are_located_in = "Tool buttons are located in ";

    if (state)
	set_status(tool_buttons_are_located_in + "command toolbar.");
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

    case JDB:
	app_data.debugger = "jdb";
	break;
    }

    set_status(next_ddd_will_start_with + "a " 
	       + upcase(app_data.debugger) + " debugger.");

    update_options();
    post_startup_warning(w);
}

void dddSetStartupLogoCB (Widget w, XtPointer client_data, XtPointer)
{
    app_data.show_startup_logo = (String)client_data;

    update_options();
    post_startup_warning(w);
}

static void toggle_button_appearance(Widget w, Boolean& data, 
				     XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    data = info->set;
    
    string msg = next_ddd_will_start_with;
    if (app_data.button_images && app_data.button_captions)
    {
	msg += " captioned images";
    }
    else if (app_data.button_images && !app_data.button_captions)
    {
	msg += " images only";
    }
    else if (!app_data.button_images && app_data.button_captions)
    {
	msg += " captions only";
    }
    else if (!app_data.button_images && !app_data.button_captions)
    {
	msg += " ordinary labels";
    }

    update_options();
    post_startup_warning(w);
}

void dddToggleButtonCaptionsCB(Widget w, XtPointer, XtPointer call_data)
{
    toggle_button_appearance(w, app_data.button_captions, call_data);
}

void dddToggleButtonImagesCB(Widget w, XtPointer, XtPointer call_data)
{
    toggle_button_appearance(w, app_data.button_images, call_data);
}

void dddToggleFlatButtonsCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.flat_toolbar_buttons = info->set;
    app_data.flat_dialog_buttons  = info->set;

    if (info->set)
	set_status(next_ddd_will_start_with + "flat buttons.");
    else
	set_status(next_ddd_will_start_with + "raised buttons.");

    update_options();
    post_startup_warning(w);
}

void dddToggleColorButtonsCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	app_data.button_color_key = "c";
    else
	app_data.button_color_key = "g";

    if (info->set)
	set_status(next_ddd_will_start_with + "color buttons.");
    else
	set_status(next_ddd_will_start_with + "grey buttons.");

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
    // set_status("Edit Sources command is " + quote(command));
    update_reset_preferences();
}


void dddSetGetCoreCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.get_core_command = command;
    // set_status("Get Core command is " + quote(command));
    update_reset_preferences();
}


void dddSetPSCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.ps_command = command;
    // set_status("List Processes command is " + quote(command));
    update_reset_preferences();
}


void dddSetTermCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.term_command = command;
    // set_status("Execution Window command is " + quote(command));
    update_reset_preferences();
}


void dddSetUncompressCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.uncompress_command = command;
    // set_status("Uncompress command is " + quote(command));
    update_reset_preferences();
}


void dddSetWWWCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.www_command = command;
    // set_status("Web Browser is " + quote(command));
    update_reset_preferences();
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

// Some GDB implementations want a confirmation when detaching:
// `Was stopped when attached, make it runnable again? (y or n) '
// Handle this by turning confirmations off.
static void get_confirm(const string& complete_answer, void *qu_data)
{
    bool *flag = (bool *)qu_data;
    *flag = complete_answer.contains("is on");
}

static void detach()
{
    bool confirm = true;
    gdb_command("show confirm", 0, get_confirm, &confirm);
    syncCommandQueue();

    if (confirm)
	gdb_question("set confirm off");

    gdb_question("detach");

    if (confirm)
	gdb_question("set confirm on");
}

// Get core from running program
static bool _get_core(const string& session, unsigned long flags, 
		      string& target)
{
    const bool may_kill         = (flags & MAY_KILL);
    const bool may_ptrace       = (flags & MAY_PTRACE);
    const bool may_gcore        = (flags & MAY_GCORE);
    const bool dont_save        = (flags & DONT_SAVE);
    const bool dont_reload_core = (flags & DONT_RELOAD_CORE);

    if (!gdb->has_core_files())
	return true;		// No need to get core files

    create_session_dir(session);
    target = session_core_file(session);

    ProgramInfo info;
    if (!info.running)
    {
	// The program is not running.
	if (info.core != "" && info.core != NO_GDB_ANSWER)
	{
	    // We already have some core file.
	    if (dont_save)
		return true;	// Fine

	    StatusDelay delay("Getting core dump from " + quote(info.core));
	    if (info.core == target)
	    {
		// It is our target.
		return true;
	    }

	    if (flags & DONT_COPY_CORE)
	    {
		target = info.core;
		return true;	// Don't copy existing core file
	    }

	    // Remove old target, if any
	    unlink(target);

#if HAVE_LINK
	    // Try a hard link from current core file to target
	    if (link(info.core, target) == 0)
		return true;
#endif

#if HAVE_SYMLINK
	    // Try a symlink link from target to current core file
	    if (symlink(info.core, target) == 0)
		return true;
#endif

	    // Looks as if we have to copy some large core file.  Blechhh.
	    return copy(info.core, target);
	}

	// Program is not running, and we have no core.  Quit.
	return false;
    }

    if (may_ptrace)
    {
#if HAVE_PTRACE_DUMPCORE
	if (gdb->type() == GDB && info.pid > 0)
	{
	    // Try getting core via ptrace(2) call
	    if (dont_save)
		return true;		// Will probably work

	    // Get new core file from running process
	    StatusDelay delay("Getting core dump via ptrace()");
	    
	    // 1. Stop the program being debugged, using a STOP signal.
	    kill(info.pid, SIGSTOP);

	    // 2. Detach GDB from the debuggee.  The debuggee is still stopped.
	    detach();

	    // 3. Attach to the process, using the ptrace() call.
	    string gcore_target = target + "." + itostring(info.pid);
	    int ok = ptrace(PTRACE_ATTACH, info.pid, 0, 0);
	    if (ok < 0)
	    {
		cerr << ddd_NAME ": PTRACE_ATTACH: "
		     << strerror(errno) << "\n";
	    }
	    else
	    {
		// 4. Get a core file from the running process
		ok = ptrace(PTRACE_DUMPCORE, info.pid, 
			    int(gcore_target.chars()), 0);

		if (ok < 0)
		{
		    cerr << ddd_NAME ": PTRACE_DUMPCORE: "
			 << strerror(errno) << "\n";
		}
	    
		// 5. Detach from the debuggee, leaving it stopped
		kill(info.pid, SIGSTOP);
		ok = ptrace(PTRACE_DETACH, info.pid, 0x1, SIGSTOP);

		if (ok < 0)
		{
		    cerr << ddd_NAME ": PTRACE_DETACH: "
			 << strerror(errno) << "\n";
		}
	    }

	    // 6. Attach GDB to the debuggee again.
	    sleep(1);
	    gdb_command("attach " + itostring(info.pid));

	    if (is_core_file(gcore_target) && move(gcore_target, target))
		return true;

	    delay.outcome = "failed";
	}
	else
#endif
	    if (dont_save)
		return false;	// unsupported
    }
    

    if (may_gcore)
    {
	// Try `gcore' command
	string gcore = app_data.get_core_command;
	if (gcore != "" && gdb->type() == GDB && info.pid > 0)
	{
	    if (dont_save)
		return true;	// Will probably work

	    // Get new core file from running process
	    StatusDelay delay("Getting core dump via `gcore'");

	    // 1. Stop the program being debugged, using a STOP signal.
	    // (Other signals may be blocked, caught or ignored)
	    kill(info.pid, SIGSTOP);

	    // 2. Detach GDB from the debuggee.  The debuggee is still stopped.
	    detach();

	    // 3. Invoke `gcore' command.
	    string gcore_target = target + "." + itostring(info.pid);
	    gcore.gsub("@FILE@", target);
	    gcore.gsub("@PID@",  itostring(info.pid));
	    string cmd = sh_command(gcore, true) + " 2>&1";
	    ostrstream errs;
	    FILE *fp = popen(cmd, "r");
	    if (fp != 0)
	    {
		kill(info.pid, SIGSTOP);
		int c;
		while ((c = getc(fp)) != EOF)
		{
		    kill(info.pid, SIGSTOP);
		    errs << c;
		}
	    }
	    int gcore_status = pclose(fp);

	    // 4. Since `gcore' restarts the debuggee, stop it again.
	    kill(info.pid, SIGSTOP);
	    if (gcore_status != 0)
		cerr << string(errs);

	    // 5. Attach GDB again.
	    sleep(1);
	    gdb_command("attach " + itostring(info.pid));

	    if (is_core_file(gcore_target) && move(gcore_target, target))
		return true;

	    delay.outcome = "failed";
	}
	else
	{
	    if (dont_save)
	    {
		// No `gcore' support
		return false;
	    }
	}
    }


    // Try direct kill.
    if (may_kill)
    {
	if (dont_save)
	    return true;	// Will probably work

	// Get new core file from running process
	StatusDelay delay("Getting core dump via killing debuggee");

	string core = SourceView::full_path("core");
	string core_backup = core + "~";

	bool had_a_core_file = false;
	if (is_regular_file(core) || is_directory(core))
	{
	    // There is already a file named `core'.  Preserve it.
	    had_a_core_file = true;

	    // Try `core~', `core1', `core2', etc., until we find a
	    // file name that is not being used yet.
	    int suffix = 0;
	    for (;;)
	    {
		if (!is_regular_file(core_backup)
		    && !is_directory(core_backup))
		{
		    move(core, core_backup);
		    break;
		}

		core_backup = "core" + itostring(++suffix);
	    }
	}

	if (gdb->has_system_calls())
	{
	    // Kill the process, hopefully leaving a core file.
	    // Since g77 catches SIGABRT, we disable its handler first.
	    string enable_signal_cmd = 
		"signal(" + itostring(SIGABRT) + ", " + 
		itostring(int(SIG_DFL)) + ")";
	    gdb_question(gdb->print_command(enable_signal_cmd));
	    gdb_question(gdb->signal_command(SIGABRT));
	}

	if (is_core_file(core))
	{
	    // It worked.  Fine!  Move generated core file to target.
	    bool ok = move(core, target);
	    
	    if (!ok)
	    {
		// Move failed.  Sorry.
		unlink(core);
	    }

	    // Restore the old core file, if any.
	    if (had_a_core_file)
		move(core_backup, core);

	    if (ok && gdb->type() == GDB && !dont_reload_core)
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

	    return ok;
	}

	// No core file.  Sorry.
	delay.outcome = "failed";

	// Restore the old core file, if any.
	if (had_a_core_file)
	    move(core_backup, core);
    }

    return false;
}

static bool get_core(const string& session, unsigned long flags, 
		     string& target)
{
    const bool interact      = (flags & MAY_INTERACT);

    bool ok = _get_core(session, flags, target);
    if (!ok && interact)
	post_warning("Could not save core file.", "core_missing_warning");

    return ok;
}

static bool must_kill_to_get_core()
{
    string dummy_target;
    return !_get_core(app_data.session, DONT_SAVE, dummy_target);
}


//-----------------------------------------------------------------------------
// Write state
//-----------------------------------------------------------------------------

inline String bool_value(bool value)
{
    return value ? "on" : "off";
}

static string app_value(string resource, const string& value)
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

static string string_app_value(const string& name, String v)
{
    if (v == 0)
	return "";

    string value = cook(v);

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
    string geo(geometry);

    return string_app_value(string(XtName(w)) + ".geometry", geo);
}

bool saving_options_kills_program(unsigned long flags)
{
    ProgramInfo info;

    const bool save_session  = (flags & SAVE_SESSION);
    const bool may_kill      = (flags & MAY_KILL);
    const bool may_gcore     = (flags & MAY_GCORE);
    const bool may_ptrace    = (flags & MAY_PTRACE);
    const bool save_core     = (flags & SAVE_CORE);

    return info.running
	&& save_session
	&& save_core
	&& must_kill_to_get_core()
	&& !may_kill
	&& !may_gcore
	&& !may_ptrace;
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
	    "! " DDD_NAME " initialization file\n"
	    "! Enter your personal " DDD_NAME " resources here.\n"
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
	delay.outcome = "failed";
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

    if (app_data.initial_session != 0)
    {
	os << "\n! Session\n";
	os << string_app_value(XtNinitialSession, app_data.initial_session) 
	   << "\n";
    }

    os << "\n! Debugger settings\n";
    os << string_app_value(XtNdebugger, app_data.debugger) << "\n";
    os << bool_app_value(XtNuseSourcePath, app_data.use_source_path) << "\n";

    string gdb_settings = app_data.gdb_settings;
    string dbx_settings = app_data.dbx_settings;
    string xdb_settings = app_data.xdb_settings;
    string jdb_settings = app_data.jdb_settings;

    if (need_settings())
    {
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

	case JDB:
	    jdb_settings = get_settings(JDB);
	    break;
	}
    }

    os << string_app_value(XtNgdbSettings, gdb_settings) << "\n";
    os << string_app_value(XtNdbxSettings, dbx_settings) << "\n";
    os << string_app_value(XtNxdbSettings, xdb_settings) << "\n";
    os << string_app_value(XtNjdbSettings, jdb_settings) << "\n";

    os << "\n! Source\n";
    os << bool_app_value(XtNfindWordsOnly,
			 app_data.find_words_only) << "\n";
    os << bool_app_value(XtNfindCaseSensitive,
			 app_data.find_case_sensitive) << "\n";
    os << int_app_value(XtNtabWidth,
			 app_data.tab_width) << "\n";
    os << int_app_value(XtNindentSource,
			 app_data.indent_source) << "\n";
    os << int_app_value(XtNindentCode,
			 app_data.indent_code) << "\n";
    os << bool_app_value(XtNcacheSourceFiles,
			 app_data.cache_source_files) << "\n";
    os << bool_app_value(XtNcacheMachineCode,
			 app_data.cache_machine_code) << "\n";
    os << bool_app_value(XtNdisplayGlyphs,
			 app_data.display_glyphs) << "\n";
    os << bool_app_value(XtNdisplayLineNumbers,
			 app_data.display_line_numbers) << "\n";
    os << bool_app_value(XtNdisassemble,
			 app_data.disassemble) << "\n";
    os << bool_app_value(XtNallRegisters,
			 app_data.all_registers) << "\n";

    os << "\n! Misc Preferences\n";
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

    os << bool_app_value(XtNstatusAtBottom,
			 app_data.status_at_bottom) << "\n";
    os << bool_app_value(XtNsuppressWarnings,
			 app_data.suppress_warnings) << "\n";
    os << string_app_value(XtNshowStartupLogo,
			   app_data.show_startup_logo) << "\n";
    os << bool_app_value(XtNcheckGrabs,
			 app_data.check_grabs) << "\n";
    os << bool_app_value(XtNsaveHistoryOnExit,
			 app_data.save_history_on_exit) << "\n";
    os << string_app_value(XtNpaperSize,
			   app_data.paper_size) << "\n";
    os << bool_app_value(XtNblinkWhileBusy,
			 app_data.blink_while_busy) << "\n";
    os << bool_app_value(XtNstartupTips, 
			 app_data.startup_tips) << "\n";


    // Graph editor
    os << "\n! Data\n";
    os << bool_app_value(XtNpannedGraphEditor, 
			 app_data.panned_graph_editor) << "\n";
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
    os << bool_app_value(XtNdetectAliases,  app_data.detect_aliases)   << "\n";
    os << bool_app_value(XtNalign2dArrays,  app_data.align_2d_arrays)  << "\n";

    // Tips
    os << "\n! Tips\n";
    os << bool_app_value(XtNbuttonTips,
			 app_data.button_tips) << "\n";
    os << bool_app_value(XtNvalueTips,
			 app_data.value_tips) << "\n";
    os << bool_app_value(XtNbuttonDocs,
			 app_data.button_docs) << "\n";
    os << bool_app_value(XtNvalueDocs,
			 app_data.value_docs) << "\n";

    // Windows
    os << "\n! Windows\n";
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
    os << bool_app_value(XtNgroupIconify,
			 app_data.group_iconify) << "\n";
    os << bool_app_value(XtNuniconifyWhenReady,
			 app_data.uniconify_when_ready) << "\n";

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

    // Toolbar
    os << "\n! Toolbars\n";
#if 0
    // We cannot change these interactively, so there's no point in
    // saving them.
    os << bool_app_value(XtNcommonToolBar,
			 app_data.common_toolbar)  << "\n";
    os << bool_app_value(XtNtoolbarsAtBottom, 
			 app_data.toolbars_at_bottom) << "\n";
#endif
    os << bool_app_value(XtNbuttonImages,
			 app_data.button_images)   << "\n";
    os << bool_app_value(XtNbuttonCaptions,
			 app_data.button_captions) << "\n";

    if (app_data.flat_toolbar_buttons == app_data.flat_dialog_buttons)
    {
	os << bool_app_value(XtCFlatButtons,
			     app_data.flat_toolbar_buttons) << "\n";
    }
    else
    {
	os << bool_app_value(XtNflatToolbarButtons,
			     app_data.flat_toolbar_buttons) << "\n";
	os << bool_app_value(XtNflatDialogButtons,
			     app_data.flat_dialog_buttons) << "\n";
    }
    os << string_app_value(XtNbuttonColorKey,
			   app_data.button_color_key) << "\n";

    // Command tool
    os << "\n! Command tool\n";
    if (have_visible_tool_window())
	get_tool_offset();
    os << bool_app_value(XtNcommandToolBar,
			 app_data.command_toolbar) << "\n";
    os << int_app_value(XtNtoolRightOffset,
			app_data.tool_right_offset) << "\n";
    os << int_app_value(XtNtoolTopOffset,
			app_data.tool_top_offset) << "\n";

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

    // Fonts
    os << "\n! Fonts\n";
    os << string_app_value(XtNdefaultFont,
			   app_data.default_font) << "\n";
    os << string_app_value(XtNvariableWidthFont, 
			   app_data.variable_width_font) << "\n";
    os << string_app_value(XtNfixedWidthFont,
			   app_data.fixed_width_font) << "\n";
    if (app_data.default_font_size == app_data.variable_width_font_size &&
	app_data.default_font_size == app_data.fixed_width_font_size)
    {
	os << int_app_value(XtCFontSize, app_data.default_font_size) << "\n";
    }
    else
    {
	os << int_app_value(XtNdefaultFontSize, 
			    app_data.default_font_size) << "\n";
	os << int_app_value(XtNvariableWidthFontSize, 
			    app_data.variable_width_font_size) << "\n";
	os << int_app_value(XtNfixedWidthFontSize, 
			    app_data.fixed_width_font_size) << "\n";
    }

    // Widget sizes.
    os << "\n! Window sizes\n";

    // We must manage all PanedWindow children in order to get the
    // correct sizes.  Ugly hack.
    popups_disabled  = true;
    bool had_data    = have_data_window();
    bool had_source  = have_source_window();
    bool had_code    = have_code_window();
    bool had_command = have_command_window();

    gdbOpenDataWindowCB(gdb_w, 0, 0);
    gdbOpenSourceWindowCB(gdb_w, 0, 0);
    gdbOpenCodeWindowCB(gdb_w, 0, 0);
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
	gdbCloseCodeWindowCB(gdb_w, 0, 0);
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

	ProgramInfo info;
	if (info.file == NO_GDB_ANSWER)
	{
	    if (interact)
		post_warning("Could not save program name.",
			     "program_name_missing_warning");
	    ok = false;
	}

	// Stream to hold data and breakpoints
	ostrstream rs;

	// Get breakpoints and cursor position
	bool breakpoints_ok = source_view->get_state(rs);
	if (!breakpoints_ok)
	{
	    if (interact)
		post_warning("Could not save all breakpoints", 
			     "breakpoint_missing_warning");
	    ok = false;
	}

	bool core_ok = false;
	string core;
	bool have_data = 
	    info.running || (info.core != "" && info.core != NO_GDB_ANSWER);
	if (have_data)
	{
	    // Get displays
	    StringArray scopes;
	    bool displays_ok = true;

	    if (displays_ok)
		displays_ok = data_disp->get_scopes(scopes);

	    if (save_core)
		core_ok = get_core(session, flags, core);

	    if (displays_ok)
	    {
		int target_frame = source_view->get_frame();
		if (target_frame < 0)
		    target_frame = 0;
		displays_ok = data_disp->get_state(rs, scopes, target_frame);
	    }

	    if (!displays_ok)
	    {
		if (interact)
		    post_warning("Could not save all data displays.",
				 "displays_missing_warning");
		ok = false;
	    }
	}

	// Stream to hold exec and core file specs
	ostrstream es;

	// Get exec and core file
	switch (gdb->type())
	{
	case GDB:
	    es << "set confirm off\n";
	    if (info.file != "" && info.file != NO_GDB_ANSWER)
		es << "file " << info.file << "\n";
	    if (core_ok)
		es << "core " << core << "\n";
	    break;

	case DBX:
	    if (info.file != "" && info.file != NO_GDB_ANSWER)
	    {
		string cmd = gdb->debug_command(info.file);
		if (cmd != "")
		{
		    es << cmd;
		    if (core_ok)
			es << " " << core;
		    es << "\n";
		}
	    }
	    break;

	case XDB:
	case JDB:
	    // FIXME
	    break;
	}

	string restart = string(es) + string(rs);
	restart.gsub(app_data.auto_command_prefix, "@AUTO@");

	os << string_app_value(XtNrestartCommands, restart) << "\n";
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
