// $Id$ -*- C++ -*-
// Argument Dialog

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

const char args_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "args.h"

#include "Delay.h"
#include "HelpCB.h"
#include "StringA.h"
#include "commandQ.h"
#include "ddd.h"
#include "disp-read.h"
#include "mydialogs.h"
#include "string-fun.h"
#include "verify.h"

#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>

#include <ctype.h>

//-----------------------------------------------------------------------------
// Run and Argument Dialog
//-----------------------------------------------------------------------------

// Argument storage
static StringArray gdb_arguments;
static Widget gdb_arguments_w;

static bool arguments_updated = false;
static string last_arguments;

// GDB run dialog
static Widget run_dialog;

// Update list of arguments
void update_arguments()
{
    if (arguments_updated || run_dialog == 0)
	return;

    bool *selected = new bool[gdb_arguments.size()];
    int pos = -1;
    for (int i = 0; i < gdb_arguments.size(); i++)
    {
	if (gdb_arguments[i] == last_arguments)
	    pos = i;
	selected[i] = false;
    }
    if (pos >= 0)
	selected[pos] = true;

    setLabelList(gdb_arguments_w, gdb_arguments.values(),
		 selected, gdb_arguments.size());

    if (pos >= 0)
	XmListSelectPos(gdb_arguments_w, pos + 1, False);

    delete[] selected;

    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (String)last_arguments);

    arguments_updated = true;
}

// Add ARG to the list of arguments
static void add_argument(string arg)
{
    strip_final_blanks(arg);
    while (arg.length() > 0 && isspace(arg[0]))
	arg = arg.after(0);

    last_arguments = arg;

    // Insertion sort
    int i;
    for (i = 0; i < gdb_arguments.size(); i++)
    {
	int cmp = compare(gdb_arguments[i], arg);
	if (cmp == 0)
	    return;		// Already present
	if (cmp > 0)
	    break;
    }

    gdb_arguments += "<dummy>";

    for (int j = gdb_arguments.size() - 1; j > i; j--)
	gdb_arguments[j] = gdb_arguments[j - 1];
    gdb_arguments[i] = arg;

    arguments_updated = false;
}

// If LINE is an argument-setting command, add it to the list of arguments
void add_to_arguments(string line)
{
    if (is_set_args_cmd(line))
    {
	string args = line.after("args");
	args = args.after(rxwhite);
	add_argument(args);
    }
    else if (is_run_cmd(line))
    {
	string args = line.after(rxwhite);
	add_argument(args);
    }
}

// Run program with given arguments
static void gdbRunDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    if (args != "")
	args = " " + args;

    if (gdb->type() == GDB)
    {
	if (args == "")
	    gdb_command("set args", run_dialog);
	gdb_command("run" + args, run_dialog);
    }
    else if (gdb->type() == DBX)
    {
	gdb_command("rerun" + args, run_dialog);
    }
}

// Set program arguments from list
static void SelectArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = gdb_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, (String)args);
}

// Create `Run' dialog
void gdbRunCB(Widget w, XtPointer, XtPointer)
{
    if (run_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	run_dialog = 
	    verify(XmCreateSelectionDialog(w, "run_dialog", args, arg));

	Delay::register_shell(run_dialog);
	XtAddCallback(run_dialog, XmNokCallback, gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNapplyCallback, gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNhelpCallback,  ImmediateHelpCB, 0);

#if 0
	Widget apply_w = XmSelectionBoxGetChild(run_dialog, 
						XmDIALOG_APPLY_BUTTON);
	XtVaSetValues(run_dialog,
		      XmNdefaultButton, apply_w,
		      NULL);
#endif

	gdb_arguments_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_LIST);
	XtAddCallback(gdb_arguments_w, XmNsingleSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNmultipleSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNextendedSelectionCallback,
		      SelectArgsCB, 0);
	XtAddCallback(gdb_arguments_w, XmNbrowseSelectionCallback,
		      SelectArgsCB, 0);
    }

    update_arguments();
    XtManageChild(run_dialog);
}
