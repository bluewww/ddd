// $Id$ -*- C++ -*-
// DDD command history

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

char history_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "history.h"

#include "Assoc.h"
#include "ComboBox.h"
#include "Command.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "MString.h"
#include "MakeMenu.h"
#include "SmartC.h"
#include "SourceView.h"
#include "StringA.h"
#include "VoidArray.h"
#include "args.h"
#include "cook.h"
#include "cwd.h"
#include "ddd.h"
#include "disp-read.h"
#include "editing.h"
#include "filetype.h"
#include "home.h"
#include "logo.h"
#include "misc.h"
#include "mydialogs.h"
#include "post.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "uniquify.h"
#include "verify.h"
#include "wm.h"

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
static string _gdb_history_file;

// Size of saved history
static int gdb_history_size = 100;

// True if the history was just loaded
static bool gdb_new_history = true;

// True if history command was issued
static bool private_gdb_history = false;

// Update all combo boxes
static void update_combo_boxes();

// Update combo boxes listening to NEW_ENTRY
static void update_combo_boxes(const string& new_entry);




void set_gdb_history_file(const string& file)
{
    _gdb_history_file = file;
}

string gdb_history_file()
{
    return _gdb_history_file;
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
    update_combo_boxes(line);
}

// Load history from history file
void load_history(const string& file)
{
    if (file == "")
	return;

    // Delay d;

    ifstream is(file);
    if (is.bad())
	return;

    static StringArray empty;
    gdb_history = empty;

    assert(gdb_history.size() == 0);

    // If the first command in history is a `file' command,
    // insert them all and disregard later ones.
    bool first_command = true;
    bool first_is_file = false;
    bool get_files     = true;

    while (is)
    {
	char _line[BUFSIZ];
	_line[0] = '\0';

	is.getline(_line, sizeof(_line));
	if (_line[0] != '\0')
	{
	    string line(_line);

	    bool added = false;
	    if (is_file_cmd(line, gdb) && line != "# reset")
	    {
		if (first_command)
		    first_is_file = true;

		if (get_files)
		{
		    string arg = line.after(rxwhite);
		    add_to_recent(arg);
		    added = first_is_file;
		}
	    }
	    else
	    {
		if (first_is_file)
		    get_files = false;
		if (first_command)
		    first_is_file = false;
	    }

	    if (!added && line[0] != '#')
	    {
		gdb_history += line;
		add_to_arguments(line);
	    }

	    first_command = false;
	}
    }

    gdb_history += "";
    gdb_current_history = gdb_history.size() - 1;
    gdb_new_history = true;

    update_arguments();
    update_combo_boxes();
}

// Save history into history file
void save_history(const string& file, Widget origin)
{
    if (file != "")
    {
	StatusDelay delay("Saving history in " + quote(file));
	ofstream os(file);
	if (os.bad())
	{
	    post_error("Cannot save history in " + quote(file),
		       "history_save_error", origin);
	    delay.outcome = "failed";
	    return;
	}

	// Save the 10 most recently opened files
	int i;
	StringArray recent;
	get_recent(recent);
	for (i = recent.size() - 1; i >= 0 && i >= recent.size() - 10; i--)
	    os << gdb->debug_command(recent[i]) << "\n";

	// Now save the command history itself
	int start = gdb_history.size() - gdb_history_size;
	if (start < 0)
	    start = 0;

	for (i = start; i < gdb_history.size(); i++)
	    os << gdb_history[i] << "\n";
    }
}

// Set history file name
void process_history_filename(string answer)
{
    answer = answer.after('"');
    answer = answer.before('"');
    set_gdb_history_file(answer);
}

// Set history size
void process_history_size(string answer)
{
    answer = answer.from(rxint);
    int ret = get_positive_nr(answer);
    if (ret >= 0)
	gdb_history_size = ret;
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
	verify(createTopLevelSelectionDialog(find_shell(w), "history_dialog", 
					     args, arg));
    Delay::register_shell(gdb_history_w);

    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_OK_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(gdb_history_w, 
					   XmDIALOG_SELECTION_LABEL));

    gdb_commands_w = XmSelectionBoxGetChild(gdb_history_w, XmDIALOG_LIST);
    XtVaSetValues(gdb_commands_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    XtAddCallback(gdb_commands_w,
		  XmNsingleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNmultipleSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNextendedSelectionCallback, SelectHistoryCB, 0);
    XtAddCallback(gdb_commands_w,
		  XmNbrowseSelectionCallback, SelectHistoryCB, 0);

    XtAddCallback(gdb_history_w, XmNokCallback, gdbApplySelectionCB, 0);
    XtAddCallback(gdb_history_w, XmNapplyCallback, gdbApplySelectionCB, 0);
    XtAddCallback(gdb_history_w, XmNcancelCallback, DestroyThisCB, 
		  gdb_history_w);
    XtAddCallback(gdb_history_w, XmNhelpCallback,  ImmediateHelpCB, 0);
    XtAddCallback(gdb_history_w, XmNdestroyCallback,
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


//-----------------------------------------------------------------------------
// Combo Box Histories
//-----------------------------------------------------------------------------

// Update combo box containing TEXT
static void update_combo_box(Widget text, HistoryFilter filter)
{
    StringArray entries;

    for (int i = gdb_history.size() - 1; i >= 0; i--)
    {
	const string& entry = gdb_history[i];
	string arg = filter(entry);
	if (arg != "")
	{
	    if (entries.size() == 0 || entries[entries.size() - 1] != arg)
		entries += arg;
	}
    }

    smart_sort(entries);
    uniq(entries);
    ComboBoxSetList(text, entries);
}

static WidgetHistoryFilterAssoc combo_boxes;

// Update all combo boxes
static void update_combo_boxes()
{
    for (WidgetHistoryFilterAssocIter iter(combo_boxes); iter.ok(); iter++)
	update_combo_box(iter.key(), iter.value());
}

// Update combo boxes listening to NEW_ENTRY
static void update_combo_boxes(const string& new_entry)
{
    for (WidgetHistoryFilterAssocIter iter(combo_boxes); iter.ok(); iter++)
    {
	HistoryFilter filter = iter.value();
	string arg = filter(new_entry);
	if (arg != "")
	    update_combo_box(iter.key(), iter.value());
    }
}

static void RemoveComboBoxCB(Widget text, XtPointer, XtPointer)
{
    combo_boxes.remove(text);
}

// Tie a ComboBox to global history
void tie_combo_box_to_history(Widget text, HistoryFilter filter)
{
    combo_boxes[text] = filter;
    update_combo_box(text, filter);
    XtAddCallback(text, XmNdestroyCallback, RemoveComboBoxCB, XtPointer(0));
}


//-----------------------------------------------------------------------------
// Recent file history
//-----------------------------------------------------------------------------

static void update_recent_menus();

// Recent files storage
static StringArray recent_files;

// Add FILE to recent file history
void add_to_recent(const string& file)
{
    string full_path;
    if (gdb->type() != JDB)
	full_path = SourceView::full_path(file);
    else
	full_path = file;

    if (recent_files.size() > 0 && 
	recent_files[recent_files.size() - 1] == full_path)
	return;			// Already in list

    for (int i = 0; i < recent_files.size(); i++)
	if (recent_files[i] == full_path ||
	    (!remote_gdb() && 
	     gdb->type() != JDB &&
	     same_file(recent_files[i], full_path)))
	    recent_files[i] = ""; // Clear old entry

    recent_files += full_path;
    update_recent_menus();
}

// Get recent file history (most recent first)
void get_recent(StringArray& arr)
{
    for (int i = recent_files.size() - 1; i >= 0; i--)
    {
	string& recent = recent_files[i];
	if (recent == "")
	    continue;		// Removed

	arr += recent_files[i];
    }
}


// Menus to be updated
static VoidArray menus;

static void update_recent_menu(MMDesc *items)
{
    StringArray recent_files;
    {
	StringArray r;
	get_recent(r);
	for (int i = 0; i < r.size() && items[i].widget != 0; i++)
	    recent_files += r[i];
    }

    // Uniquify labels
    char sep = '/';
    if (gdb->type() == JDB)
	sep = '.';

    StringArray labels;
    uniquify(recent_files, labels, sep);

    // Set labels
    int i;
    for (i = 0; i < labels.size(); i++)
    {
	MString label(itostring(i + 1) + " ");
	label += tt(labels[i]);

	Widget w = items[i].widget;
	set_label(w, label);

	if (!remote_gdb() && gdb->type() != JDB && 
	    !is_exec_file(recent_files[i]))
	    set_sensitive(w, false); // File is not loadable now
	else
	    set_sensitive(w, true);

	XtManageChild(w);
    }

    // Unmanage remaining items
    for (; items[i].widget != 0; i++)
	XtUnmanageChild(items[i].widget);
}

static void update_recent_menus()
{
    for (int i = 0; i < menus.size(); i++)
    {
	MMDesc *items = (MMDesc *)menus[i];
	update_recent_menu(items);
    }
}

void tie_menu_to_recent_files(MMDesc *items)
{
    if (items == 0 || items[0].widget == 0)
	return;

    menus += (void *)items;
    update_recent_menu(items);
}
