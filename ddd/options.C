// $Id$ -*- C++ -*-
// Save and edit DDD options

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003, 2004, 2005 Free Software Foundation, Inc.
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

char options_rcsid[] = 
    "$Id$";

#include "options.h"

#include "config.h"

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

#include "AppData.h"
#include "DataDisp.h"
#include "DestroyCB.h"
#include "DispBox.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "SourceView.h"
#include "TimeOut.h"
#include "UndoBuffer.h"
#include "cook.h"
#include "Command.h"
#include "comm-manag.h"
#include "ddd.h"
#include "file.h"
#include "filetype.h"
#include "frame.h"
#include "gdbinit.h"
#include "plotter.h"
#include "post.h"
#include "resources.h"
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
#include <Xm/PanedW.h>

#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <string.h>		// strerror
#include <time.h>		// ctime

#include <limits.h>
#ifndef ARG_MAX
#define ARG_MAX 4096
#endif

#include <signal.h>

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
// Helper Decls
//-----------------------------------------------------------------------------

// If the current state file has changed, return true.
// If MODE is ACKNOWLEDGE, refer to last acknowledgement time.  
// If MODE is ACCESS, refer to last access time (read or write).
// If RESET is given, reset modification time.
enum ChangeMode { ACKNOWLEDGE, ACCESS };
static bool options_file_has_changed(ChangeMode mode, bool reset = false);


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
    Boolean state = (int)(long)client_data;

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
    Boolean state = (int)(long)client_data;

    app_data.display_glyphs = state;

    update_options();

    string displaying =	"Displaying breakpoints and positions ";
    if (state)
	set_status(displaying + "as glyphs.");
    else
	set_status(displaying + "as text characters.");
}

void sourceSetAllRegistersCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
    {
	set_status("Showing all registers.");
	app_data.all_registers = true;
    }

    update_options();
}

void sourceSetIntRegistersCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
    {
	set_status("Showing integer registers only.");
	app_data.all_registers = false;
    }

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

    app_data.indent_source = info->value;
    update_options();

    set_status("Source indentation set to " + 
	       itostring(app_data.indent_source) + ".");
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
    const string alias_detection = "Alias detection ";

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
    XtSetArg(args[arg], ARGSTR(XtNshowHints), info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Hints on.");
    else
	set_status("Hints off.");

    update_options();
}

void graphToggleShowAnnotationsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], ARGSTR(XtNshowAnnotations), info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Annotations on.");
    else
	set_status("Annotations off.");

    data_disp->refresh_titles();

    update_options();
}

void graphToggleShowDependentTitlesCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.show_dependent_display_titles = info->set;
    if (info->set)
	set_status("Dependent titles on.");
    else
	set_status("Dependent titles off.");

    data_disp->refresh_titles();

    update_options();
}

void graphToggleClusterDisplaysCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.cluster_displays = info->set;

    if (info->set)
	set_status("Display clustering enabled.");
    else
	set_status("Display clustering disabled.");

    update_options();
}

void graphToggleSnapToGridCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], ARGSTR(XtNsnapToGrid), info->set); arg++;
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
    XtSetArg(args[arg], ARGSTR(XtNlayoutMode), mode); arg++;
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
    XtSetArg(args[arg], ARGSTR(XtNautoLayout), info->set); arg++;
    XtSetValues(data_disp->graph_edit, args, arg);

    if (info->set)
	set_status("Automatic layout on.");
    else
	set_status("Automatic layout off.");

    update_options();
}

void graphToggleAutoCloseCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.auto_close_data_window = info->set;

    if (info->set)
	set_status("Automatic closing on.");
    else
	set_status("Automatic closing off.");

    update_options();
}

void graphSetGridSizeCB (Widget, XtPointer, XtPointer call_data)
{
    XmScaleCallbackStruct *info = (XmScaleCallbackStruct *)call_data;

    Arg args[10];
    Cardinal arg = 0;

    if (info->value >= 2)
    {
	XtSetArg(args[arg], ARGSTR(XtNgridWidth),  info->value); arg++;
	XtSetArg(args[arg], ARGSTR(XtNgridHeight), info->value); arg++;
	XtSetArg(args[arg], ARGSTR(XtNshowGrid),   True); arg++;
	XtSetValues(data_disp->graph_edit, args, arg);
	set_status("Grid size set to " + itostring(info->value) + ".");
    }
    else
    {
	XtSetArg(args[arg], ARGSTR(XtNshowGrid), False); arg++;
	XtSetValues(data_disp->graph_edit, args, arg);
	set_status("Grid off.");
    }

    update_options();
}

void graphSetDisplayPlacementCB(Widget, XtPointer client_data,
				XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
    {
	unsigned char orientation = (unsigned char)(int)(long)client_data;
	app_data.display_placement = orientation;

	switch (app_data.display_placement)
	{
	case XmVERTICAL:
	    set_status("New displays will be placed "
		       "below the downmost display.");
	    break;
	    
	case XmHORIZONTAL:
	    set_status("New displays will be placed on the "
		       "right of the rightmost display.");
	    break;
	}

	update_options();
    }
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
    Boolean state = (int)(long)client_data;

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

void dddToggleWarnIfLockedCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.warn_if_locked = info->set;

    if (info->set)
	set_status(DDD_NAME " will warn you "
		   "when multiple " DDD_NAME " instances are running.");
    else
	set_status(DDD_NAME " will not warn you "
		   "when multiple " DDD_NAME " instances are running.");

    update_options();
}

void dddSetBuiltinPlotWindowCB (Widget, XtPointer client_data, XtPointer)
{
    if ((int)(long)client_data)
	app_data.plot_term_type = "xlib";
    else
	app_data.plot_term_type = "x11";

    string plot_term_type = downcase(app_data.plot_term_type);

    if (plot_term_type.contains("xlib", 0))
    {
	set_status("Next plot will be done in builtin " DDD_NAME " window.");
    }
    else if (plot_term_type.contains("x11", 0))
    {
	set_status("Next plot will be done in external " + 
		   cook(app_data.plot_window_class) + " window.");
    }
    else
    {
	set_status("Next plot will be done in an unknown place.");
    }

    clear_plot_window_cache();
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

void dddToggleSaveOptionsOnExitCB (Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.save_options_on_exit = info->set;

    if (info->set)
	set_status("Current options will be saved when exiting DDD.");
    else
	set_status("Current options will not be saved when exiting DDD.");

    update_options();
}


//-----------------------------------------------------------------------------
// Maintenance
//-----------------------------------------------------------------------------

void dddSetCrashCB(Widget, XtPointer client_data, XtPointer)
{
    int state = (int)(long)client_data;
    string msg = "When " DDD_NAME " crashes, ";

    switch (state)
    {
    case 0:
	app_data.dump_core        = False;
	app_data.debug_core_dumps = False;
	msg += "do nothing.";
	break;

    case 1:
	app_data.dump_core        = True;
	app_data.debug_core_dumps = False;
	msg += "dump core.";
	break;

    case 2:
	app_data.dump_core        = True;
	app_data.debug_core_dumps = True;
	msg += "dump core and invoke a debugger.";
	break;
    }

    set_status(msg);
    update_options();
}

void dddClearMaintenanceCB(Widget, XtPointer, XtPointer)
{
    app_data.maintenance = False;
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
    int state = (int)(long)client_data;
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
    Boolean state = (int)(long)client_data;

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
    Boolean state = (int)(long)client_data;

    app_data.command_toolbar = state;
    const string tool_buttons_are_located_in = "Tool buttons are located in ";

    if (state)
	set_status(tool_buttons_are_located_in + "command toolbar.");
    else
	set_status(tool_buttons_are_located_in + "command tool.");

    update_options();
    post_startup_warning(w);
}

void dddSetKeyboardFocusPolicyCB (Widget w, XtPointer client_data, XtPointer)
{
    unsigned char policy = (unsigned char)(int)(long)client_data;

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
			  XmNdefaultButton, &default_button, XtPointer(0));
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
	    XtVaSetValues(shell, XmNkeyboardFocusPolicy, policy, XtPointer(0));
    }

    // Apply to future shells
    const string keyboardFocusPolicy = "*" + string(XmNkeyboardFocusPolicy);
    XrmDatabase target = XtDatabase(XtDisplay(w));
    switch (policy)
    {
    case XmEXPLICIT:
	XrmPutStringResource(&target, keyboardFocusPolicy.chars(), "EXPLICIT");
	break;

    case XmPOINTER:
	XrmPutStringResource(&target, keyboardFocusPolicy.chars(), "POINTER");
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
    Boolean state = (int)(long)client_data;
    app_data.panned_graph_editor = state;

    if (state)
	set_status(next_ddd_will_start_with + "a panned graph editor.");
    else
	set_status(next_ddd_will_start_with + "a scrolled graph editor.");

    update_options();
    post_startup_warning(w);
}

static void report_debugger_type()
{
    DebuggerType type;
    bool type_ok = get_debugger_type(app_data.debugger, type);

    if (!type_ok || app_data.auto_debugger)
    {
	set_status("Next " DDD_NAME 
		   " invocation will determine the debugger automatically.");
    }
    else
    {
	string title;
	if (type == PERL)
	    title = "Perl";
	else
	    title = upcase(app_data.debugger);
	set_status(next_ddd_will_start_with + "a " + title + " debugger.");
    }
}

void dddSetDebuggerCB (Widget w, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (!info->set)
	return;

    DebuggerType type = DebuggerType((int)(long)client_data);
    app_data.debugger = default_debugger(type);
    app_data.auto_debugger = false;

    report_debugger_type();

    update_options();
    post_startup_warning(w);
}

void dddToggleAutoDebuggerCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.auto_debugger = info->set;

    report_debugger_type();

    update_options();
    post_startup_warning(w);
}

void dddSetCutCopyPasteBindingsCB (Widget, XtPointer client_data, 
				   XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (!info->set)
	return;

    BindingStyle style = BindingStyle((int)(long)client_data);
    app_data.cut_copy_paste_bindings = style;

    switch (style)
    {
    case KDEBindings:
	set_status(next_ddd_will_start_with + 
		   "KDE-style Cut/Copy/Paste bindings.");
	break;
    case MotifBindings:
	set_status(next_ddd_will_start_with + 
		   "Motif-style Cut/Copy/Paste bindings.");
	break;
    }

    update_options();
}

void dddSetSelectAllBindingsCB (Widget, XtPointer client_data, 
				XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (!info->set)
	return;

    BindingStyle style = BindingStyle((int)(long)client_data);
    app_data.select_all_bindings = style;

    switch (style)
    {
    case KDEBindings:
	set_status(next_ddd_will_start_with + 
		   "KDE-style Select All bindings.");
	break;
    case MotifBindings:
	set_status(next_ddd_will_start_with + 
		   "Motif-style Select All bindings.");
	break;
    }

    update_options();
}

void dddSetUndoBufferSizeCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    string value = s;
    XtFree(s); s = 0;

    s = CONST_CAST(char*,value.chars());
    long val = strtol(value.chars(), &s, 0);
    if (s != value)
    {
	app_data.max_undo_size = int(val * 1000);
	undo_buffer.max_history_size = app_data.max_undo_size;
    }

    update_reset_preferences();
}

void dddClearUndoBufferCB(Widget, XtPointer, XtPointer)
{
    StatusDelay delay("Clearing undo buffer");
    undo_buffer.clear();
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

#if XmVersion >= 2000
    switch (info->set)
    {
    case XmSET:
	app_data.button_color_key        = "c";
	app_data.active_button_color_key = "c";
	break;

    case XmUNSET:
	app_data.button_color_key        = "g";
	app_data.active_button_color_key = "g";
	break;

    case XmINDETERMINATE:
	app_data.button_color_key        = "g";
	app_data.active_button_color_key = "c";
	break;
    }
#else
    if (info->set)
	app_data.button_color_key = "c";
    else
	app_data.button_color_key = "g";
#endif

    string button_color_key        = app_data.button_color_key;
    string active_button_color_key = app_data.active_button_color_key;

    if (button_color_key == 'c' && active_button_color_key == 'c')
	set_status(next_ddd_will_start_with + "color buttons.");
    else if (button_color_key == active_button_color_key)
	set_status(next_ddd_will_start_with + "grey buttons.");
    else			// indeterminate
	set_status(next_ddd_will_start_with + "grey/color buttons.");

    update_options();
    post_startup_warning(w);
}

void dddToggleToolbarsAtBottomCB(Widget w, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.toolbars_at_bottom = info->set;

    if (info->set)
	set_status(next_ddd_will_start_with + "toolbars at bottom.");
    else
	set_status(next_ddd_will_start_with + "toolbars at top.");

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

    app_data.edit_command = command.chars();
    // set_status("Edit Sources command is " + quote(command));
    update_reset_preferences();
}

void dddSetPlotCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.plot_command = command.chars();
    // set_status("Edit Sources command is " + quote(command));
    update_reset_preferences();
}


void dddSetGetCoreCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.get_core_command = command.chars();
    // set_status("Get Core command is " + quote(command));
    update_reset_preferences();
}


void dddSetPSCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.ps_command = command.chars();
    // set_status("List Processes command is " + quote(command));
    update_reset_preferences();
}


void dddSetTermCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.term_command = command.chars();
    // set_status("Execution Window command is " + quote(command));
    update_reset_preferences();
}


void dddSetUncompressCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.uncompress_command = command.chars();
    // set_status("Uncompress command is " + quote(command));
    update_reset_preferences();
}


void dddSetWWWCommandCB(Widget w, XtPointer, XtPointer)
{
    String s = XmTextFieldGetString(w);
    static string command;
    command = s;
    XtFree(s);

    app_data.www_command = command.chars();
    // set_status("Web Browser is " + quote(command));
    update_reset_preferences();
}





// ---------------------------------------------------------------------------
// Get core
// ---------------------------------------------------------------------------

static bool copy(const string& src, const string& dest)
{
    FILE *from = fopen(src.chars(), "r");
    if (from == 0)
	return false;

    FILE *to = fopen(dest.chars(), "w");
    if (to == 0)
	return false;

    int c;
    while ((c = getc(from)) != EOF)
	putc(c, to);

    fclose(from);
    if (fclose(to) == EOF)
    {
	unlink(dest.chars());
	return false;
    }

    return true;
}

static bool move(const string& from, const string& to)
{
    if (rename(from.chars(), to.chars()) == 0)
	return true;

    if (copy(from, to) && unlink(from.chars()) == 0)
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
    const bool may_kill    = (flags & MAY_KILL);
    const bool may_ptrace  = (flags & MAY_PTRACE);
    const bool may_gcore   = (flags & MAY_GCORE);
    const bool dont_save   = (flags & DONT_SAVE);
    const bool reload_core = !(flags & DONT_RELOAD_CORE);

    if (!gdb->has_core_files())
	return true;		// No need to get core files

    create_session_dir(session);
    target = session_core_file(session);

    ProgramInfo info;
    if (!info.running)
    {
	// The program is not running.
	if (!info.core.empty() && info.core != NO_GDB_ANSWER)
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
	    unlink(target.chars());

#if HAVE_LINK
	    // Try a hard link from current core file to target
	    if (link(info.core.chars(), target.chars()) == 0)
		return true;
#endif

#if HAVE_SYMLINK
	    // Try a symlink link from target to current core file
	    if (symlink((char *)info.core.chars(), (char *)target.chars()) == 0)
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
	      std::cerr << ddd_NAME ": PTRACE_ATTACH: "
		     << strerror(errno) << '\n';
	    }
	    else
	    {
		// 4. Get a core file from the running process
		ok = ptrace(PTRACE_DUMPCORE, info.pid, 
			    int(gcore_target.chars()), 0);

		if (ok < 0)
		{
		  std::cerr << ddd_NAME ": PTRACE_DUMPCORE: "
			 << strerror(errno) << '\n';
		}
	    
		// 5. Detach from the debuggee, leaving it stopped
		kill(info.pid, SIGSTOP);
		ok = ptrace(PTRACE_DETACH, info.pid, 0x1, SIGSTOP);

		if (ok < 0)
		{
		  std::cerr << ddd_NAME ": PTRACE_DETACH: "
			 << strerror(errno) << '\n';
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
	if (!gcore.empty() && gdb->type() == GDB && info.pid > 0)
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
	    std::ostringstream errs;
	    FILE *fp = popen(cmd.chars(), "r");
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
		std::cerr << string(errs);

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
	    std::ostringstream os;
	    os << "signal(" << SIGABRT << ", " 
	       << (unsigned long)SIG_DFL << ")";
	    gdb_question(gdb->print_command(string(os)));

	    // Send signal
	    gdb_question(gdb->signal_command(SIGABRT));
	}

	if (is_core_file(core))
	{
	    // It worked.  Fine!  Move generated core file to target.
	    bool ok = move(core, target);
	    
	    if (!ok)
	    {
		// Move failed.  Sorry.
		unlink(core.chars());
	    }

	    // Restore the old core file, if any.
	    if (had_a_core_file)
		move(core_backup, core);

	    if (ok && gdb->type() == GDB && reload_core)
	    {
		// Load the core file just saved, such that we can
		// keep on examining data in this session.
		Command c("core " + gdb->quote_file(target));
		c.verbose  = false;
		c.prompt   = false;
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
// Reload state
//-----------------------------------------------------------------------------

static bool options_file_has_changed(ChangeMode mode, bool reset)
{
    static string last_options_file = "";
    static time_t last_access      = 0;
    static time_t last_acknowledge = 0;

    string options_file = session_state_file(app_data.session);

    if (options_file != last_options_file)
    {
	// Name of state file has changed
	last_options_file = options_file;
	mode  = ACCESS;
	reset = true;
    }

    time_t modification_time = last_modification_time(options_file);

#if 0
    std::clog << quote(options_file) << " last modified " 
	      << ctime(&modification_time);
#endif

    if (reset || last_acknowledge == 0)
	last_acknowledge = modification_time;
    if ((reset && mode == ACCESS) || last_access == 0)
	last_access = modification_time;

    time_t& last_time = (mode == ACKNOWLEDGE ? last_acknowledge : last_access);
    if (modification_time > last_time)
    {
	// File has been written since last check
	return true;
    }

    // File is unchanged
    return false;
}

inline const _XtString str(const _XtString s)
{
    return s != 0 ? s : "";
}

static Boolean done_if_idle(XtPointer data)
{
    if (emptyCommandQueue() && can_do_gdb_command())
    {
	update_settings();	// Refresh settings and signals
	update_signals();

	delete (Delay *)data;
	return True;		// Remove from the list of work procs
    }

    return False;		// Get called again
}

static void done(const string&, void *data)
{
    XtAppAddWorkProc(XtWidgetToApplicationContext(command_shell),
		     done_if_idle, data);
}

static void reload_options()
{
    static string session;
    session = app_data.session;

    string file = session_state_file(session);

    StatusDelay *delay_ptr = 
	new StatusDelay("Loading options from " + quote(file));

    XrmDatabase session_db = XrmGetFileDatabase(file.chars());

    Widget toplevel = find_shell();
    while (XtParent(toplevel) != 0)
	toplevel = XtParent(toplevel);

    if (session_db == 0)
    {
	delay_ptr->outcome = "failed";
	delete delay_ptr;
	return;
    }

    XrmDatabase target = XtDatabase(XtDisplay(toplevel));

    static XrmDatabase default_db = app_defaults(XtDisplay(toplevel));
    XrmMergeDatabases(default_db, &target);

    XrmMergeDatabases(session_db, &target);

    XtVaGetApplicationResources(toplevel, (XtPointer)&app_data,
				ddd_resources, ddd_resources_size, 
				XtPointer(0));

    // Keep session ID across reloads
    app_data.session = session.chars();

    save_option_state();
    options_file_has_changed(ACCESS, true);

    // Set options and buttons
    update_options();
    update_user_buttons();

    // Pop down settings and signals panel (such that GDB settings
    // and signals will be updated from scratch)
    reset_settings();
    reset_signals();

    // Load GDB settings.  Don't care about init or restart commands here.
    string restart = "";
    string settings;
    switch (gdb->type())
    {
    case BASH:
	settings = str(app_data.bash_settings);
	break;

    case DBG:
	settings = str(app_data.dbg_settings);
	break;

    case DBX:
	settings = str(app_data.dbx_settings);
	break;

    case GDB:
	settings = str(app_data.gdb_settings);
	break;

    case MAKE:
	settings = str(app_data.make_settings);
	break;

    case JDB:
	settings = str(app_data.jdb_settings);
	break;

    case PERL:
	settings = str(app_data.perl_settings);
	break;

    case PYDB:
	settings = str(app_data.pydb_settings);
	break;

    case XDB:
	settings = str(app_data.xdb_settings);
	break;

    }

    init_session(restart, settings, app_data.source_init_commands);

    // One last command to reload the new settings
    Command c("# reset");
    c.callback = done;
    c.data     = (void *)(Delay *)delay_ptr;
    c.priority = COMMAND_PRIORITY_BATCH;
    c.verbose  = false;
    c.prompt   = false;
    c.check    = true;
    gdb_command(c);
}

static void ReloadOptionsCB(Widget, XtPointer, XtPointer)
{
    reload_options();
}

static void DontReloadOptionsCB(Widget, XtPointer, XtPointer)
{
    // Acknowledge change
    options_file_has_changed(ACKNOWLEDGE, true);
}

// Pending timer
static XtIntervalId check_options_timer = 0;

static void CheckOptionsFileCB(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    assert(*id == check_options_timer);
    check_options_timer = 0;

    if (options_file_has_changed(ACKNOWLEDGE))
    {
	// Options file has changed since last acknowledgement -- offer reload
	static Widget dialog = 0;

	if (dialog == 0)
	{
	    dialog = verify(XmCreateQuestionDialog(
				find_shell(),
				XMST("reload_options_dialog"),
				0, 0));
	    Delay::register_shell(dialog);
	    XtAddCallback(dialog, XmNokCallback,     ReloadOptionsCB, 0);
	    XtAddCallback(dialog, XmNcancelCallback, DontReloadOptionsCB, 0);
	    XtAddCallback(dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
	}

	if (!XtIsManaged(dialog))
	    manage_and_raise(dialog);
    }
    
    if (app_data.check_options > 0)
    {
	// Try again later
	check_options_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(find_shell()),
			    app_data.check_options * 1000,
			    CheckOptionsFileCB, client_data);
    }
}

void check_options_file()
{
    if (check_options_timer != 0)
	XtRemoveTimeOut(check_options_timer);

    check_options_timer = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(find_shell()), 0,
			CheckOptionsFileCB, XtPointer(0));
}


//-----------------------------------------------------------------------------
// Write state
//-----------------------------------------------------------------------------

static bool is_fallback_value(const string& resource, string val)
{
    static XrmDatabase default_db = app_defaults(XtDisplay(find_shell()));

    static String app_name  = 0;
    static String app_class = 0;

    if (app_name == 0)
	XtGetApplicationNameAndClass(XtDisplay(find_shell()), 
				     &app_name, &app_class);

    string str_name  = string(app_name)  + "*" + resource;
    string str_class = string(app_class) + "*" + resource;

    char *type;
    XrmValue xrmvalue;
    Bool success = XrmGetResource(default_db, str_name.chars(), str_class.chars(), 
				  &type, &xrmvalue);
    string default_val = NO_GDB_ANSWER;

    if (success)
    {
	const char *str = (const char *)xrmvalue.addr;
	int len   = xrmvalue.size - 1; // includes the final `\0'
	default_val = string(str, len);
    }

    val = uncook(val);

#if 0
    if (val != default_val)
    {
	std::clog << resource
		  << ": val " << quote(val)
		  << " != default " << quote(default_val)
		  << '\n';
    }
#endif

    return val == default_val;
}

static string app_value(const string& resource, const string& value, 
			Boolean check_default)
{
    static String app_name  = 0;
    static String app_class = 0;

    if (app_name == 0)
	XtGetApplicationNameAndClass(XtDisplay(find_shell()), 
				     &app_name, &app_class);

    string prefix = "";
    if (check_default && is_fallback_value(resource, value))
	prefix = "! ";

    string s = prefix + app_class;

    if (resource.contains(string(app_name) + ".", 0))
	s += resource.from(".") + ": " + value;
    else
	s += "*" + resource + ": " + value;

    if (!prefix.empty())
	s.gsub('\n', "\n" + prefix);

    return s;
}

inline const _XtString bool_value(Boolean value)
{
    // Since GDB uses `on' and `off' for its settings, we do so, too. 
    return value ? "on" : "off";
}

inline const _XtString binding_value(BindingStyle value)
{
    switch (value)
    {
    case KDEBindings:
	return "KDE";

    case MotifBindings:
	return "Motif";
    }

    return "";			// Never reached
}

static string bool_app_value(const string& name, Boolean value, 
			     Boolean check_default = False)
{
    return app_value(name, bool_value(value), check_default);
}

static string int_app_value(const string& name, int value,
			    Boolean check_default = False)
{
    return app_value(name, itostring(value), check_default);
}

static string binding_app_value(const string& name, BindingStyle value,
				Boolean check_default = False)
{
    return app_value(name, binding_value(value), check_default);
}


static string string_app_value(const string& name, const _XtString v,
			       Boolean check_default = False)
{
    if (v == 0)
	return "";

    string value = cook(v);

    // Xt cannot read `\t', so leave it unchanged.
    value.gsub("\\t", '\t');

    if (value.contains("\\n"))
    {
	value.gsub("\\n", "\\n\\\n");
	value.gsub("\\\\n\\\n\\n", "\\\\n\\n");

	if (value.contains("\\\n", -1))
	    value = value.before(int(value.length()) - 2);
	value = "\\\n" + value;
    }

    return app_value(name, value, check_default);
}

static string widget_value(Widget w, const _XtString name,
			   Boolean check_default = False)
{
    String value = 0;
    XtVaGetValues(w, 
		  XtVaTypedArg, name, XtRString, &value, sizeof(value),
		  XtPointer(0));

    return string_app_value(string(XtName(w)) + "." + name, value, 
			    check_default);
}

static string orientation_app_value(const string& name, unsigned char v,
				    Boolean check_default = False)
{
    string value = "XmBAD_ORIENTATION";

    switch (v)
    {
    case XmVERTICAL:
	value = "XmVERTICAL";
	break;

    case XmHORIZONTAL:
	value = "XmHORIZONTAL";
	break;

    case XmNO_ORIENTATION:
	value = "XmNO_ORIENTATION";
	break;
    }

    return app_value(name, value, check_default);
}

static string paned_widget_size(Widget w, bool height_only = false)
{
    string s;
    const Boolean check_default = False;

    if (XmIsText(w) || XmIsTextField(w))
    {
	// Store rows and columns
	short columns = 0;
	XtVaGetValues(w, XmNcolumns, &columns, XtPointer(0));
	if (!height_only && columns > 0)
	{
	    if (!s.empty())
		s += '\n';
	    s += int_app_value(string(XtName(w)) + "." + XmNcolumns, columns,
			       check_default);
	}

	if (XmIsText(w))
	{
	    short rows = 0;
	    XtVaGetValues(w, XmNrows, &rows, XtPointer(0));
	    if (rows > 0)
	    {
		if (!s.empty())
		    s += '\n';
		s += int_app_value(string(XtName(w)) + "." + XmNrows, rows,
				   check_default);
	    }
	}
    }
    else
    {
	// We store the size of the paned child, in order to account
	// for scrolled windows etc.
	Widget ref = w;
	while (XtParent(ref) != 0 && !XmIsPanedWindow(XtParent(ref)))
	    ref = XtParent(ref);
	if (XtParent(ref) == 0)
	    ref = w;

	// Store absolute sizes
	Dimension width  = 0;
	Dimension height = 0;
	XtVaGetValues(ref, XmNwidth, &width, XmNheight, &height, XtPointer(0));

	if (!height_only)
	    s += int_app_value(string(XtName(w)) + "." + XmNwidth, width,
			       check_default);

	if (!s.empty())
	    s += '\n';
	s += int_app_value(string(XtName(w)) + "." + XmNheight, height,
			   check_default);
    }

    return s;
}

inline string paned_widget_height(Widget w)
{
    return paned_widget_size(w, true);
}

static string widget_geometry(Widget w, bool include_size = false)
{
    const Boolean check_default = False;

    Dimension width, height;
    XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, XtPointer(0));

    XWindowAttributes attr;
    XGetWindowAttributes(XtDisplay(w), frame(w), &attr);

    std::ostringstream geometry;
    if (include_size)
	geometry << width << "x" << height;
    geometry << "+" << attr.x << "+" << attr.y;
    string geo(geometry);

    return string_app_value(string(XtName(w)) + ".geometry", geo.chars(), 
			    check_default);
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

bool get_restart_commands(string& restart, unsigned long flags)
{
    bool ok = true;
    const bool interact     = (flags & MAY_INTERACT);
    const bool save_core    = (flags & SAVE_CORE);
    const bool save_session = (flags & SAVE_SESSION);
    const bool reload_file  = !(flags & DONT_RELOAD_FILE);

    string session = 
	(save_session ? app_data.session : DEFAULT_SESSION.chars());

    ProgramInfo info;
    if (info.file == NO_GDB_ANSWER)
    {
	if (interact)
	    post_warning("Could not save program name.",
			 "program_name_missing_warning");
	ok = false;
    }

    // Stream to hold data and breakpoints
    std::ostringstream rs;

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
	info.running || (!info.core.empty() && info.core != NO_GDB_ANSWER);

    bool have_data_displays = (data_disp->count_data_displays() > 0);

    IntArray display_numbers;
    data_disp->get_all_display_numbers(display_numbers);
    bool have_displays = display_numbers.size() > 0;

    if (have_data || have_displays)
    {
	// Get displays
	StringArray scopes;
	bool displays_ok = true;

	if (have_data && have_data_displays && displays_ok)
	    displays_ok = data_disp->get_scopes(scopes);

	if (have_data && save_core)
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
    std::ostringstream es;

    if (reload_file)
    {
	// Get exec and core file
	switch (gdb->type())
	{
	case GDB:
	    es << "set confirm off\n";
	    if (!info.file.empty() && info.file != NO_GDB_ANSWER)
		es << "file " << gdb->quote_file(info.file) << '\n';
	    if (core_ok)
		es << "core " << gdb->quote_file(core) << '\n';
	    break;

	case DBX:
	    if (!info.file.empty() && info.file != NO_GDB_ANSWER)
	    {
		string cmd = gdb->debug_command(info.file);
		if (!cmd.empty())
		{
		    es << cmd;
		    if (core_ok)
			es << " " << core;
		    es << '\n';
		}
	    }
	    break;

	case PERL:
	case BASH:
	case MAKE:
	case JDB:
	    if (!info.file.empty() && info.file != NO_GDB_ANSWER)
	    {
		string cmd = gdb->debug_command(info.file);
		if (!cmd.empty())
		    es << cmd << '\n';
	    }
	    break;

	case XDB:
	case PYDB:
	case DBG:
	    // FIXME
	    break;
	}
    }

    restart = string(es) + string(rs) + get_signals(gdb->type());
    restart.gsub(app_data.auto_command_prefix, "@AUTO@");

    return ok;
}

bool save_options(unsigned long flags)
{
    const bool create        = (flags & CREATE_OPTIONS);
    const bool save_session  = (flags & SAVE_SESSION);
    const bool save_geometry = (flags & SAVE_GEOMETRY);
    const bool interact      = (flags & MAY_INTERACT);

    if (find_shell() == 0) {
	// We cannot use *_app_value() because we have no shell
	// available.  Presumably we have been called from an error
	// handler very early in program startup.
	if (interact)
	    post_error("Cannot save options", "options_save_error");
	return false;
    }

    string session = 
	(save_session ? app_data.session : DEFAULT_SESSION.chars());

    create_session_dir(session);
    const string file = session_state_file(session);

    string options = (save_session ? "session" : "options");
    string status = (create ? "Creating " : "Saving ") + options + " in ";

    StatusDelay delay(status + quote(file));

    const char *delimiter = "! DO NOT ADD ANYTHING BELOW THIS LINE";

    // Read the file contents into memory ...
    string dddinit;
    {
	std::ifstream is(file.chars());
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
	    char line[ARG_MAX + BUFSIZ];
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
    }

    // ... and write them back again
    bool ok = true;
    string workfile = file + "#";
    std::ofstream os(workfile.chars());
    if (os.bad())
    {
	workfile = file;
	os.open(workfile.chars());
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
    os << string_app_value(XtNdddinitVersion, DDD_VERSION) << '\n';

    if (create)
    {
	app_data.dddinit_version = DDD_VERSION;
	os.close();

	if (workfile != file && rename(workfile.chars(), file.chars()) != 0)
	{
	    if (interact)
		post_error("Cannot rename " + quote(workfile)
			   + " to " + quote(file) + ": " + strerror(errno),
			   "options_save_error");
	    ok = false;
	    unlink(workfile.chars());
	}

	return ok;
    }

    if (app_data.initial_session != 0)
    {
	os << "\n! Session.\n";
	os << string_app_value(XtNinitialSession, app_data.initial_session) 
	   << '\n';
    }

    os << "\n! Debugger settings.\n";
    os << bool_app_value(XtNautoDebugger, app_data.auto_debugger) << '\n';
    os << string_app_value(XtNdebugger, app_data.debugger) << '\n';
    os << bool_app_value(XtNuseSourcePath, app_data.use_source_path) << '\n';

    string bash_settings = app_data.bash_settings;
    string dbg_settings  = app_data.dbg_settings;
    string dbx_settings  = app_data.dbx_settings;
    string gdb_settings  = app_data.gdb_settings;
    string xdb_settings  = app_data.xdb_settings;
    string jdb_settings  = app_data.jdb_settings;
    string make_settings = app_data.make_settings;
    string perl_settings = app_data.perl_settings;
    string pydb_settings = app_data.pydb_settings;

    if (need_settings() || need_save_defines())
    {
	string settings;

	settings += get_settings(gdb->type(), flags);
	settings += get_defines(gdb->type(), flags);

	settings.gsub(app_data.auto_command_prefix, "@AUTO@");

	switch (gdb->type())
	{
	case BASH:
	    bash_settings = settings;
	    break;

	case DBG:
	    dbg_settings = settings;
	    break;

	case DBX:
	    dbx_settings = settings;
	    break;

	case GDB:
	    gdb_settings = settings;
	    break;

	case MAKE:
	    make_settings = settings;
	    break;

	case JDB:
	    jdb_settings = settings;
	    break;

	case PERL:
	    perl_settings = settings;
	    break;

	case PYDB:
	    pydb_settings = settings;
	    break;

	case XDB:
	    xdb_settings = settings;
	    break;

	}
    }

    os << string_app_value(XtNbashSettings, bash_settings.chars(), True) << '\n';
    os << string_app_value(XtNdbgSettings,  dbg_settings.chars(), True)  << '\n';
    os << string_app_value(XtNdbxSettings,  dbx_settings.chars(), True)  << '\n';
    os << string_app_value(XtNgdbSettings,  gdb_settings.chars(), True)  << '\n';
    os << string_app_value(XtNjdbSettings,  jdb_settings.chars(), True)  << '\n';
    os << string_app_value(XtNperlSettings, perl_settings.chars(), True) << '\n';
    os << string_app_value(XtNpydbSettings, pydb_settings.chars(), True) << '\n';
    os << string_app_value(XtNxdbSettings,  xdb_settings.chars(), True)  << '\n';

    os << "\n! Source.\n";
    os << bool_app_value(XtNfindWordsOnly,
			 app_data.find_words_only) << '\n';
    os << bool_app_value(XtNfindCaseSensitive,
			 app_data.find_case_sensitive) << '\n';
    os << int_app_value(XtNtabWidth,
			 app_data.tab_width, True) << '\n';
    os << int_app_value(XtNindentSource,
			 app_data.indent_source, True) << '\n';
    os << int_app_value(XtNindentCode,
			 app_data.indent_code, True) << '\n';
    os << bool_app_value(XtNcacheSourceFiles,
			 app_data.cache_source_files) << '\n';
    os << bool_app_value(XtNcacheMachineCode,
			 app_data.cache_machine_code) << '\n';
    os << bool_app_value(XtNdisplayGlyphs,
			 app_data.display_glyphs) << '\n';
    os << bool_app_value(XtNdisplayLineNumbers,
			 app_data.display_line_numbers) << '\n';
    os << bool_app_value(XtNdisassemble,
			 app_data.disassemble) << '\n';
    os << bool_app_value(XtNallRegisters,
			 app_data.all_registers) << '\n';

    os << "\n! Undo Buffer.\n";
    os << int_app_value(XtNmaxUndoDepth,
			 app_data.max_undo_depth, True) << '\n';
    os << int_app_value(XtNmaxUndoSize,
			 app_data.max_undo_size, True) << '\n';

    // Misc stuff
    os << "\n! Misc preferences.\n";
    unsigned char policy = '\0';
    XtVaGetValues(command_shell, 
		  XmNkeyboardFocusPolicy, &policy,
		  XtPointer(0));
    switch (policy)
    {
    case XmPOINTER:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "POINTER") 
	   << '\n';
	break;
    case XmEXPLICIT:
	os << string_app_value(string(XmNkeyboardFocusPolicy), "EXPLICIT")
	   << '\n';
	break;
    }

    os << bool_app_value(XtNstatusAtBottom,
			 app_data.status_at_bottom) << '\n';
    os << bool_app_value(XtNsuppressWarnings,
			 app_data.suppress_warnings) << '\n';
    os << bool_app_value(XtNwarnIfLocked,
			 app_data.warn_if_locked) << '\n';
    os << bool_app_value(XtNcheckGrabs,
			 app_data.check_grabs) << '\n';
    os << bool_app_value(XtNsaveHistoryOnExit,
			 app_data.save_history_on_exit) << '\n';
    os << string_app_value(XtNpaperSize,
			   app_data.paper_size) << '\n';
    os << bool_app_value(XtNblinkWhileBusy,
			 app_data.blink_while_busy) << '\n';
    os << bool_app_value(XtNsplashScreen,
			 app_data.splash_screen) << '\n';
    os << bool_app_value(XtNstartupTips, 
			 app_data.startup_tips) << '\n';

    // Keys
    os << "\n! Keys.\n";
    os << bool_app_value(XtNglobalTabCompletion, 
			 app_data.global_tab_completion) << '\n';
    os << binding_app_value(XtNcutCopyPasteBindings,
			    app_data.cut_copy_paste_bindings) << '\n';
    os << binding_app_value(XtNselectAllBindings,
			    app_data.select_all_bindings) << '\n';

    // Graph editor
    os << "\n! Data.\n";
    os << bool_app_value(XtNpannedGraphEditor, 
			 app_data.panned_graph_editor) << '\n';
    os << widget_value(data_disp->graph_edit, XtNshowGrid)   << '\n';
    os << widget_value(data_disp->graph_edit, XtNsnapToGrid) << '\n';
    os << widget_value(data_disp->graph_edit, XtNshowHints)  << '\n';
    os << widget_value(data_disp->graph_edit, XtNshowAnnotations) << '\n';
    os << widget_value(data_disp->graph_edit, XtNlayoutMode) << '\n';
    os << widget_value(data_disp->graph_edit, XtNautoLayout) << '\n';
    os << bool_app_value(XtNshowBaseDisplayTitles, 
			 app_data.show_base_display_titles) << '\n';
    os << bool_app_value(XtNshowDependentDisplayTitles,
			 app_data.show_dependent_display_titles) << '\n';
    os << bool_app_value(XtNautoCloseDataWindow,
			 app_data.auto_close_data_window) << '\n';

    Dimension grid_width, grid_height;
    XtVaGetValues(data_disp->graph_edit,
		  XtNgridWidth, &grid_width,
		  XtNgridHeight, &grid_height,
		  XtPointer(0));
    if (!save_session && grid_width == grid_height)
    {
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtCGridSize, grid_width, True) << '\n';
    }
    else
    {
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtNgridWidth,  grid_width, True) << '\n';
	os << int_app_value(string(XtName(data_disp->graph_edit)) + "." 
			    + XtNgridHeight, grid_height, True) << '\n';
    }
    os << bool_app_value(XtNdetectAliases,  app_data.detect_aliases)   << '\n';
    os << bool_app_value(XtNclusterDisplays,app_data.cluster_displays) << '\n';

    os << orientation_app_value(XtNdisplayPlacement,
				app_data.display_placement) << '\n';

    os << bool_app_value(XtNalign2dArrays, app_data.align_2d_arrays)  << '\n';

    os << orientation_app_value(XtNarrayOrientation, 
				app_data.array_orientation) << '\n';
    os << orientation_app_value(XtNstructOrientation, 
				app_data.struct_orientation) << '\n';
    os << bool_app_value(XtNshowMemberNames,
			 app_data.show_member_names) << '\n';

    // Themes
    os << "\n! Themes.\n";
    std::ostringstream themes;
    themes << DispBox::theme_manager;
    static string themes_s;
    themes_s = themes;
    app_data.themes = themes_s.chars();
    os << string_app_value(XtNthemes, themes_s.chars()) << '\n';

    // Tips
    os << "\n! Tips.\n";
    os << bool_app_value(XtNbuttonTips,
			 app_data.button_tips) << '\n';
    os << bool_app_value(XtNvalueTips,
			 app_data.value_tips) << '\n';
    os << bool_app_value(XtNbuttonDocs,
			 app_data.button_docs) << '\n';
    os << bool_app_value(XtNvalueDocs,
			 app_data.value_docs) << '\n';

    // Helpers
    os << "\n! Helpers.\n";
    os << string_app_value(XtNeditCommand,    app_data.edit_command, True)
       << '\n';
    os << string_app_value(XtNgetCoreCommand, app_data.get_core_command, True)
       << '\n';
    os << string_app_value(XtNpsCommand,      app_data.ps_command, True)
       << '\n';
    os << string_app_value(XtNtermCommand,    app_data.term_command, True)
       << '\n';
    os << string_app_value(XtNuncompressCommand, app_data.uncompress_command,
			   True) << '\n';
    os << string_app_value(XtNwwwCommand,     app_data.www_command, True) 
       << '\n';
    os << string_app_value(XtNplotCommand,    app_data.plot_command, True)
       << '\n';
    os << string_app_value(XtNplotTermType,   app_data.plot_term_type)
       << '\n';
    os << string_app_value(XtNprintCommand,   app_data.print_command, True) 
       << '\n';

    // Toolbar
    os << "\n! Tool Bars.\n";

#if 0  // We cannot change this interactively.  Don't save.
    os << bool_app_value(XtNcommonToolBar,
			 app_data.common_toolbar)  << '\n';
#endif

    os << bool_app_value(XtNtoolbarsAtBottom, 
			 app_data.toolbars_at_bottom) << '\n';
    os << bool_app_value(XtNbuttonImages,
			 app_data.button_images)   << '\n';
    os << bool_app_value(XtNbuttonCaptions,
			 app_data.button_captions) << '\n';

    if (!save_session &&
	app_data.flat_toolbar_buttons == app_data.flat_dialog_buttons)
    {
	os << bool_app_value(XtCFlatButtons,
			     app_data.flat_toolbar_buttons) << '\n';
    }
    else
    {
	os << bool_app_value(XtNflatToolbarButtons,
			     app_data.flat_toolbar_buttons) << '\n';
	os << bool_app_value(XtNflatDialogButtons,
			     app_data.flat_dialog_buttons) << '\n';
    }
    os << string_app_value(XtNbuttonColorKey,
			   app_data.button_color_key) << '\n';
    os << string_app_value(XtNactiveButtonColorKey,
			   app_data.active_button_color_key) << '\n';

    // Command tool
    os << "\n! Command Tool.\n";
    get_tool_offset();
    os << bool_app_value(XtNcommandToolBar,
			 app_data.command_toolbar) << '\n';
    os << int_app_value(XtNtoolRightOffset,
			app_data.tool_right_offset, True) << '\n';
    os << int_app_value(XtNtoolTopOffset,
			app_data.tool_top_offset, True) << '\n';

    // Buttons
    os << "\n! Buttons.\n";
    os << string_app_value(XtNconsoleButtons, app_data.console_buttons)
       << '\n';
    os << string_app_value(XtNsourceButtons,  app_data.source_buttons)
       << '\n';
    os << string_app_value(XtNdataButtons,    app_data.data_buttons)
       << '\n';
    os << bool_app_value(XtNverifyButtons,    app_data.verify_buttons)
       << '\n';

    // Shortcut expressions
    os << "\n! Display shortcuts.\n";
    {
	StringArray exprs;
	StringArray labels;
	data_disp->get_shortcut_menu(exprs, labels);
	string expr = "";

	for (int i = 0; i < exprs.size(); i++)
	{
	    if (i > 0)
		expr += '\n';

	    expr += exprs[i];

	    if (!labels[i].empty())
	    {
		expr += string('\t') + app_data.label_delimiter + ' ' + 
		    labels[i];
	    }
	}

	string bash_display_shortcuts = app_data.bash_display_shortcuts;
 	string dbg_display_shortcuts  = app_data.dbg_display_shortcuts;
	string dbx_display_shortcuts  = app_data.dbx_display_shortcuts;
	string gdb_display_shortcuts  = app_data.gdb_display_shortcuts;
	string jdb_display_shortcuts  = app_data.jdb_display_shortcuts;
	string make_display_shortcuts = app_data.make_display_shortcuts;
	string pydb_display_shortcuts = app_data.pydb_display_shortcuts;
	string perl_display_shortcuts = app_data.perl_display_shortcuts;
	string xdb_display_shortcuts  = app_data.xdb_display_shortcuts;

	switch (gdb->type())
	{
	case BASH: bash_display_shortcuts = expr; break;
 	case DBG:  dbg_display_shortcuts  = expr; break;
	case DBX:  dbx_display_shortcuts  = expr; break;
	case GDB:  gdb_display_shortcuts  = expr; break;
	case JDB:  jdb_display_shortcuts  = expr; break;
	case MAKE: make_display_shortcuts = expr; break;
	case PERL: perl_display_shortcuts = expr; break;
	case PYDB: pydb_display_shortcuts = expr; break;
	case XDB:  xdb_display_shortcuts  = expr; break;
	}

	os << string_app_value(XtNbashDisplayShortcuts,
			       bash_display_shortcuts.chars(), True) << '\n';
 	os << string_app_value(XtNdbgDisplayShortcuts,
 			       dbg_display_shortcuts.chars(), True)  << '\n';
	os << string_app_value(XtNdbxDisplayShortcuts,
			       dbx_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNgdbDisplayShortcuts, 
			       gdb_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNmakeDisplayShortcuts,
			       make_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNjdbDisplayShortcuts,
			       jdb_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNperlDisplayShortcuts,
			       perl_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNpydbDisplayShortcuts,
			       pydb_display_shortcuts.chars(), True) << '\n';
	os << string_app_value(XtNxdbDisplayShortcuts,
			       xdb_display_shortcuts.chars(), True) << '\n';
    }

    // Fonts
    os << "\n! Fonts.\n";
    os << string_app_value(XtNdefaultFont,
			   app_data.default_font, True) << '\n';
    os << string_app_value(XtNvariableWidthFont, 
			   app_data.variable_width_font, True) << '\n';
    os << string_app_value(XtNfixedWidthFont,
			   app_data.fixed_width_font, True) << '\n';
    os << string_app_value(XtNdataFont,
			   app_data.data_font, True) << '\n';
    if (!save_session &&
	app_data.default_font_size == app_data.variable_width_font_size &&
	app_data.default_font_size == app_data.fixed_width_font_size &&
	app_data.default_font_size == app_data.data_font_size)
    {
	os << int_app_value(XtCFontSize, app_data.default_font_size)
	   << '\n';
    }
    else
    {
	os << int_app_value(XtNdefaultFontSize,
			    app_data.default_font_size) << '\n';
	os << int_app_value(XtNvariableWidthFontSize, 
			    app_data.variable_width_font_size) << '\n';
	os << int_app_value(XtNfixedWidthFontSize, 
			    app_data.fixed_width_font_size) << '\n';
	os << int_app_value(XtNdataFontSize, 
			    app_data.data_font_size) << '\n';
    }

    // Windows.
    os << "\n! Windows.\n";
    os << bool_app_value(XtNopenDataWindow,      
			 app_data.data_window)      << '\n';
    os << bool_app_value(XtNopenSourceWindow,    
			 app_data.source_window)    << '\n';
    os << bool_app_value(XtNopenDebuggerConsole, 
			 app_data.debugger_console) << '\n';

    if (!save_session && 
	!app_data.separate_source_window && !app_data.separate_data_window)
    {
	os << bool_app_value(XtCSeparate, False) << '\n';
    }
    else if (!save_session &&
	     app_data.separate_source_window && app_data.separate_data_window)
    {
	os << bool_app_value(XtCSeparate, True) << '\n';
    }
    else
    {
	os << bool_app_value(XtNseparateSourceWindow, 
			     app_data.separate_source_window) << '\n';
	os << bool_app_value(XtNseparateDataWindow, 
			     app_data.separate_data_window) << '\n';
    }
    os << bool_app_value(XtNseparateExecWindow,
			 app_data.separate_exec_window) << '\n';
    os << bool_app_value(XtNgroupIconify,
			 app_data.group_iconify) << '\n';
    os << bool_app_value(XtNuniconifyWhenReady,
			 app_data.uniconify_when_ready) << '\n';

    // Maintenance
    os << "\n! Maintenance.\n";
    os << bool_app_value(XtNdumpCore,       app_data.dump_core) << '\n';
    os << bool_app_value(XtNdebugCoreDumps, app_data.debug_core_dumps) << '\n';

    // Window sizes.
    os << "\n! Window sizes.\n";

    os << paned_widget_height(data_disp->graph_edit) << '\n';
    os << paned_widget_size(source_view->source())   << '\n';
    os << paned_widget_size(source_view->code())     << '\n';
    os << paned_widget_size(gdb_w)                   << '\n';

    if (save_geometry)
    {
	// Widget geometry
	os << "\n! Last " DDD_NAME " geometry.\n";

	if (command_shell)
	    os << widget_geometry(command_shell)     << '\n';
	if (source_view_shell)
	    os << widget_geometry(source_view_shell) << '\n';
	if (data_disp_shell)
	    os << widget_geometry(data_disp_shell)   << '\n';
    }

    if (save_session)
    {
	// Restart commands
	os << "\n! Last " DDD_NAME " session.\n";

	string restart;
	bool restart_ok = get_restart_commands(restart, flags);
	if (!restart_ok)
	    ok = false;

	os << string_app_value(XtNrestartCommands, restart.chars()) << '\n';
    }

    bool saved = true;

    os.close();
    if (os.bad())
    {
	if (interact)
	    post_error("Cannot save " + options + " in " + quote(workfile),
		       "options_save_error");
	ok = saved = false;
	unlink(workfile.chars());
    }

    if (workfile != file && rename(workfile.chars(), file.chars()) != 0)
    {
	if (interact)
	    post_error("Cannot rename " + quote(workfile)
		       + " to " + quote(file) + ": " + strerror(errno),
		       "options_save_error");
	ok = saved = false;
	unlink(workfile.chars());
    }

    if (saved)
    {
	save_option_state();
	save_settings_state();
	options_file_has_changed(ACCESS, true);
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
    else if (options_file_has_changed(ACCESS))
    {
	// Options file has changed since last access; request confirmation
	static Widget dialog = 0;
	if (dialog)
	    DestroyWhenIdle(dialog);

	dialog = verify(XmCreateQuestionDialog(
			    find_shell(w), 
			    XMST("overwrite_options_dialog"),
			    0, 0));
	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, DoSaveOptionsCB, 
		      XtPointer(flags));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, 0);

	manage_and_raise(dialog);
    }
    else if (saving_options_kills_program(flags))
    {
	// Saving session would kill program; request confirmation
	static Widget dialog = 0;
	if (dialog)
	    DestroyWhenIdle(dialog);

	dialog = verify(XmCreateQuestionDialog(
			    find_shell(w), 
			    XMST("kill_to_save_dialog"),
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

	dialog = verify(XmCreateQuestionDialog(
			    find_shell(w), 
			    XMST("data_not_saved_dialog"),
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
