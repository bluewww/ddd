// $Id$ -*- C++ -*-
// Argument Dialog

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

char args_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "args.h"

#include "Delay.h"
#include "HelpCB.h"
#include "StringA.h"
#include "Command.h"
#include "SourceView.h"
#include "ddd.h"
#include "disp-read.h"
#include "mydialogs.h"
#include "regexps.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>

#include <ctype.h>

//-----------------------------------------------------------------------------
// Run, Make, and CD Dialogs
//-----------------------------------------------------------------------------

// Argument storage
static Widget run_dialog;
static Widget run_arguments_w;
static StringArray run_arguments;
static string last_run_argument;
static bool run_arguments_updated = false;

static Widget make_dialog;
static StringArray make_arguments;
static Widget make_arguments_w;
static bool make_arguments_updated = false;
static string last_make_argument;

static Widget cd_dialog;
static StringArray cd_arguments;
static Widget cd_arguments_w;
static bool cd_arguments_updated = false;
static string last_cd_argument;


// Add ARG to the list of arguments
static void add_argument(string arg, StringArray& arguments, 
			 string& last, bool& updated)
{
    strip_final_blanks(arg);
    while (arg.length() > 0 && isspace(arg[0]))
	arg = arg.after(0);

    last = arg;

    // Insertion sort
    int i;
    for (i = 0; i < arguments.size(); i++)
    {
	int cmp = compare(arguments[i], arg);
	if (cmp == 0)
	    return;		// Already present
	if (cmp > 0)
	    break;
    }

    arguments += "<dummy>";

    for (int j = arguments.size() - 1; j > i; j--)
	arguments[j] = arguments[j - 1];
    arguments[i] = arg;

    updated = false;
}

// If LINE is an argument-setting command, add it to the list of arguments
void add_to_arguments(string line)
{
    if (is_set_args_cmd(line))
    {
	string args = line.after("args");
	args = args.after(rxwhite);
	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
    else if (is_run_cmd(line))
    {
	string args = line.after(rxwhite);
	if (gdb->type() == JDB)
	{
	    // Skip class name
	    args = args.after(rxwhite);
	}
	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
    else if (is_make_cmd(line))
    {
	string args = line.after("make");
	args = args.after(rxwhite);
	add_argument(args, make_arguments, last_make_argument, 
		     make_arguments_updated);
    }
    else if (is_cd_cmd(line))
    {
	string dir = line.after("cd");
	dir = dir.after(rxwhite);
	dir = source_view->full_path(dir);
	if (dir.contains('/', 0))
	    add_argument(dir, cd_arguments, last_cd_argument, 
			 cd_arguments_updated);
    }
}


// Update list of arguments
static void update_arguments(Widget dialog, Widget arguments_w,
			     StringArray& arguments, string& last,
			     bool& updated)
{
    if (updated || dialog == 0)
	return;

    bool *selected = new bool[arguments.size()];
    int pos = -1;
    for (int i = 0; i < arguments.size(); i++)
    {
	if (arguments[i] == last)
	    pos = i;
	selected[i] = false;
    }
    if (pos >= 0)
	selected[pos] = true;

    setLabelList(arguments_w, arguments.values(),
		 selected, arguments.size(), false, false);

    if (pos >= 0)
	XmListSelectPos(arguments_w, pos + 1, False);

    delete[] selected;

    Widget text_w = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (char *)last.chars());

    updated = true;
}

void update_run_arguments()
{
    update_arguments(run_dialog, run_arguments_w, run_arguments,
		     last_run_argument, run_arguments_updated);
}

void update_make_arguments()
{
    update_arguments(make_dialog, make_arguments_w, make_arguments,
		     last_make_argument, make_arguments_updated);
}

void update_cd_arguments()
{
    update_arguments(cd_dialog, cd_arguments_w, cd_arguments,
		     last_cd_argument, cd_arguments_updated);
}

void update_arguments()
{
    update_run_arguments();
    update_make_arguments();
    update_cd_arguments();
}

//-----------------------------------------------------------------------------
// Run Dialog
//-----------------------------------------------------------------------------

// Run program with given arguments
static void gdbRunDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    if (args != "")
	args = " " + args;

    switch (gdb->type())
    {
    case GDB:
	if (args == "")
	    gdb_command("set args", run_dialog);
	gdb_command("run" + args, run_dialog);
	break;

    case DBX:
	gdb_command("rerun" + args, run_dialog);
	break;

    case JDB:
	gdb_command("run " + args, run_dialog);
	break;

    case XDB:
	if (args == "")
	    gdb_command("R", run_dialog);
	else
	    gdb_command("r" + args, run_dialog);
	break;
    }
}

// Set program arguments from list
static void SelectRunArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = run_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (char *)args.chars());
}

// Create `Run' dialog
void gdbRunCB(Widget w, XtPointer, XtPointer)
{
    if (run_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	run_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), "run_dialog", 
					   args, arg));

	Delay::register_shell(run_dialog);
	XtAddCallback(run_dialog, XmNokCallback,     gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNapplyCallback,  gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	run_arguments_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_LIST);
	XtAddCallback(run_arguments_w, XmNsingleSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNmultipleSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNextendedSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNbrowseSelectionCallback,
		      SelectRunArgsCB, 0);
    }

    update_run_arguments();
    manage_and_raise(run_dialog);
}


//-----------------------------------------------------------------------------
// Make Dialog
//-----------------------------------------------------------------------------

// Set program arguments from list
static void SelectMakeArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = make_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(make_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (char *)args.chars());
}

// Make program with given arguments
static void gdbMakeDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(make_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    gdb_command(gdb->make_command(args));
}

void gdbMakeAgainCB(Widget, XtPointer, XtPointer)
{
    gdb_command(gdb->make_command(last_make_argument));
}
    

// Create `Make' dialog
void gdbMakeCB(Widget w, XtPointer, XtPointer)
{
    if (!gdb->has_make_command())
	return;

    if (make_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	make_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   "make_dialog", args, arg));

	Delay::register_shell(make_dialog);
	XtAddCallback(make_dialog, XmNokCallback,     gdbMakeDCB, 0);
	XtAddCallback(make_dialog, XmNapplyCallback,  gdbMakeDCB, 0);
	XtAddCallback(make_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	make_arguments_w = XmSelectionBoxGetChild(make_dialog, XmDIALOG_LIST);
	XtAddCallback(make_arguments_w, XmNsingleSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNmultipleSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNextendedSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNbrowseSelectionCallback,
		      SelectMakeArgsCB, 0);
    }

    update_make_arguments();
    manage_and_raise(make_dialog);
}



//-----------------------------------------------------------------------------
// CD Dialog
//-----------------------------------------------------------------------------

// Set program arguments from list
static void SelectChangeDirectoryArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    string args = source_view->full_path(cd_arguments[pos]);
    
    Widget text_w = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (char *)args.chars());
}

// ChangeDirectory program with given arguments
static void gdbChangeDirectoryDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    gdb_command("cd " + source_view->full_path(args));
}

// Create `ChangeDirectory' dialog
void gdbChangeDirectoryCB(Widget w, XtPointer, XtPointer)
{
    if (cd_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	cd_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   "cd_dialog", args, arg));

	Delay::register_shell(cd_dialog);
	XtAddCallback(cd_dialog, XmNokCallback,     gdbChangeDirectoryDCB, 0);
	XtAddCallback(cd_dialog, XmNapplyCallback,  gdbChangeDirectoryDCB, 0);
	XtAddCallback(cd_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	cd_arguments_w = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_LIST);
	XtAddCallback(cd_arguments_w, XmNsingleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNmultipleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNextendedSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNbrowseSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);

	add_argument("..", cd_arguments, last_cd_argument, 
		     cd_arguments_updated);
    }

    update_cd_arguments();
    manage_and_raise(cd_dialog);
}

//-----------------------------------------------------------------------------
// `run' arguments
//-----------------------------------------------------------------------------

void add_running_arguments(string& cmd)
{
    if (gdb->type() != JDB || !is_run_cmd(cmd))
	return;

    read_leading_blanks(cmd);
    string args = cmd.after(rxwhite);
    if (args == "")
    {
	// JDB requires at least a class name after the `run' command.
	string class_name = source_view->line_of_cursor();
	class_name = class_name.before(":");
	cmd += " " + class_name;
    }
}
