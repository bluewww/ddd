// $Id$ -*- C++ -*-
// DDD command history

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

char history_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "history.h"

#include "AppData.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "MString.h"
#include "StringA.h"
#include "args.h"
#include "cook.h"
#include "commandQ.h"
#include "ddd.h"
#include "editing.h"
#include "mydialogs.h"
#include "post.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>


//-----------------------------------------------------------------------------
// Command history
//-----------------------------------------------------------------------------

// History viewer
static Widget gdb_history_w  = 0;
static Widget gdb_commands_w = 0;

// History storage
static StringArray gdb_history;

// Index to current history entry
static int gdb_current_history;

// File name to save the history
static string gdb_history_file;

// Size of saved history
static int    gdb_history_size = 100;

// True if the history was just loaded
static bool   gdb_new_history = true;

// True if history command was issued
static bool private_gdb_history = false;

void init_history_file()
{
    // Setup history
    char *home = getenv("HOME");
    if (home == 0)
	gdb_history_file = ".ddd_history";
    else
	gdb_history_file = string(home) + "/.ddd_history";
}

void set_line_from_history()
{
    private_gdb_history = true;

    const string& input = gdb_history[gdb_current_history];
    XmTextReplace(gdb_w, promptPosition,
		  XmTextGetLastPosition(gdb_w), (String)input);
    XmTextSetInsertionPosition(gdb_w, XmTextGetLastPosition(gdb_w));

    if (gdb_history_w)
	ListSetAndSelectPos(gdb_commands_w, gdb_current_history + 1);

    private_gdb_history = false;
}

void set_history_from_line(const string& line,
			   bool ignore_history_commands)
{
    if (ignore_history_commands && private_gdb_history)
	return;

    while (gdb_history.size() < 1)
	gdb_history += "";
    gdb_history[gdb_history.size() - 1] = line;

    if (gdb_history_w)
    {
	int pos = gdb_history.size();

	// XmListReplaceItemsPos() disturbs the current selection, so
	// save it here
	int *selected;
	int selected_count;
	if (!XmListGetSelectedPos(gdb_commands_w, &selected, &selected_count))
	    selected = 0;

	MString xm_line(line, LIST_CHARSET);
	XmString xms = xm_line.xmstring();
	XmListReplaceItemsPos(gdb_commands_w, &xms, 1, pos);

	if (selected)
	{
	    // Restore old selection
	    for (int i = 0; i < selected_count; i++)
		XmListSelectPos(gdb_commands_w, selected[i], False);
	    XtFree((char *)selected);
	}
    }
}

// Enter LINE in history
void add_to_history(const string& line)
{
    if (!gdb->isReadyWithPrompt())
	return;

    set_history_from_line(line);

    if (gdb_history.size() < 2 || line != gdb_history[gdb_history.size() - 2])
    {
	gdb_history += "";

	if (gdb_history_w)
	{
	    MString xm_line(line, LIST_CHARSET);
	    int pos = gdb_history.size();
	    XmListAddItem(gdb_commands_w, xm_line.xmstring(), pos - 1);
	    XmListSelectPos(gdb_commands_w, 0, False);
	    XmListSetBottomPos(gdb_commands_w, 0);
	}
    }

    gdb_current_history = gdb_history.size();
    set_history_from_line("");

    if (gdb_history_w)
    {
	XmListSelectPos(gdb_commands_w, 0, False);
	XmListSetBottomPos(gdb_commands_w, 0);
    }

    gdb_new_history = false;

    add_to_arguments(line);
    update_arguments();
}

// Load history from history file
void load_history()
{
    if (gdb_history_file != "")
    {
	Delay d;

	ifstream is(gdb_history_file);
	if (is.bad())
	    return;

	static StringArray empty;
	gdb_history = empty;

	assert(gdb_history.size() == 0);

	while (is)
	{
	    char line[BUFSIZ];
	    line[0] = '\0';

	    is.getline(line, sizeof(line));
	    if (line[0] != '\0')
	    {
		gdb_history += line;
		add_to_arguments(line);
	    }
	}

	gdb_history += "";
	gdb_current_history = gdb_history.size() - 1;
	gdb_new_history = true;

	update_arguments();
    }
}

// Save history into history file
void save_history(Widget origin)
{
    if (gdb_history_file != "")
    {
	StatusDelay delay("Saving history in " + quote(gdb_history_file));
	ofstream os(gdb_history_file);
	if (os.bad())
	{
	    post_error("Cannot save history in " + quote(gdb_history_file),
		       "history_save_error", origin);
	    return;
	}

	int start = gdb_history.size() - gdb_history_size;
	if (start < 0)
	    start = 0;

	for (int i = start; i < gdb_history.size(); i++)
	    os << gdb_history[i] << "\n";
    }
}

// Set history file name
void process_history_filename(string answer)
{
    answer = answer.after('"');
    answer = answer.before('"');
    gdb_history_file = answer;

    static bool history_initialized = false;
    if (!history_initialized)
    {
	history_initialized = true;
	load_history();
    }
}

// Set history size
void process_history_size(string answer)
{
    answer = answer.from(rxint);
    int ret = get_positive_nr(answer);
    if (ret >= 0)
	gdb_history_size = ret;
}

// Set history save
void process_history_save(string answer)
{
    // A `set save history on' in ~/.gdbinit causes the DDD history to
    // be saved (since otherwise, it would be overwritten by the GDB
    // history, which is cluttered with internal DDD commands).

    if (answer.contains("is on"))
    {
	app_data.save_history_on_exit = true;
	update_options();
    }
}

// History viewer
void SelectHistoryCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    gdb_current_history = cbs->item_position - 1;

    clear_isearch();
    set_line_from_history();
}

void HistoryDestroyedCB(Widget, XtPointer client_data, XtPointer)
{
    Widget old_gdb_history_w = Widget(client_data);
    if (gdb_history_w == old_gdb_history_w)
    {
	gdb_history_w = 0;
	gdb_commands_w = 0;
    }
}

void gdbHistoryCB(Widget w, XtPointer, XtPointer)
{
    if (gdb_history_w)
    {
	manage_and_raise(gdb_history_w);
	return;
    }

    Arg args[10];
    int arg;
	
    // Create history viewer
    arg = 0;
    gdb_history_w =
	verify(XmCreateSelectionDialog(find_shell(w), "history_dialog", 
				       args, arg));
    Delay::register_shell(gdb_history_w);

    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_CANCEL_BUTTON));

    gdb_commands_w = XmSelectionBoxGetChild(gdb_history_w, XmDIALOG_LIST);
    XtVaSetValues(gdb_commands_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    Widget apply_w = XmSelectionBoxGetChild(gdb_history_w, 
					    XmDIALOG_APPLY_BUTTON);
    XtVaSetValues(gdb_history_w,
		  XmNdefaultButton, apply_w,
		  NULL);

    XtAddCallback(gdb_commands_w,
		  XmNsingleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNmultipleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNextendedSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNbrowseSelectionCallback, SelectHistoryCB, 0);

    XtAddCallback(gdb_history_w, XmNokCallback, DestroyThisCB, gdb_history_w);
    XtAddCallback(gdb_history_w, XmNapplyCallback, gdbApplySelectionCB, 0);
    XtAddCallback(gdb_history_w, XmNhelpCallback,  ImmediateHelpCB, 0);
    XtAddCallback(gdb_history_w, XtNdestroyCallback, 
		  HistoryDestroyedCB, XtPointer(gdb_history_w));

    bool *selected = new bool[gdb_history.size() + 1];
    for (int i = 0; i < gdb_history.size() + 1; i++)
	selected[i] = false;
    selected[gdb_current_history] = true;

    setLabelList(gdb_commands_w, gdb_history.values(), 
		 selected, gdb_history.size(), false, false);

    delete[] selected;

    set_history_from_line(current_line());
    XmListSelectPos(gdb_commands_w, 0, False);
    XmListSetBottomPos(gdb_commands_w, 0);

    manage_and_raise(gdb_history_w);
}


// Return last command
string last_command_from_history()
{
    if (gdb_history.size() >= 2 && !gdb_new_history)
    {
	return gdb_history[gdb_history.size() - 2];
    }
    else
    {
	// No history yet -- perform a no-op command
	return gdb->echo_command("");
    }
}

// History actions
void prev_historyAct(Widget, XEvent*, String*, Cardinal*)
{
    if (gdb_current_history == 0)
	return;

    while (gdb_current_history >= gdb_history.size())
	gdb_current_history--;
    gdb_current_history--;

    clear_isearch();
    set_line_from_history();
}

void next_historyAct(Widget, XEvent*, String*, Cardinal*)
{
    if (gdb_current_history >= gdb_history.size() - 1)
	return;

    gdb_current_history++;

    clear_isearch();
    set_line_from_history();
}

// Search TEXT in history; return POS iff found, -1 if none
// DIRECTION = -1 means search backward, DIRECTION = +1 means search forward
// RESEARCH = true means skip current position
int search_history(const string& text, int direction, bool research)
{
    int i = min(gdb_current_history, gdb_history.size() - 1);
    if (research)
	i += direction;

    while (i < gdb_history.size() && i >= 0)
    {
	if (gdb_history[i].contains(text))
	    return i;

	i += direction;
    }

    return -1;			// Not found
}

// Set history position to POS; -1 means last position.
void goto_history(int pos)
{
    if (pos == -1)
	pos = gdb_history.size() - 1;

    assert (pos >= 0 && pos < gdb_history.size());
    gdb_current_history = pos;
    set_line_from_history();
}
