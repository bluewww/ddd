// $Id$ -*- C++ -*-
// DDD command-line actions

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

char editing_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "editing.h"

#include "ArgField.h"
#include "DataDisp.h"
#include "status.h"
#include "commandQ.h"
#include "complete.h"
#include "ctrl.h"
#include "ddd.h"
#include "cmdtty.h"
#include "post.h"
#include "history.h"
#include "string-fun.h"
#include "cook.h"
#include "misc.h"

#include <iostream.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

// True if last input was at gdb prompt
bool gdb_input_at_prompt = true;


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

static void move_to_pos(XtPointer client_data, XtIntervalId *)
{
    XmTextPosition pos = XmTextPosition(client_data);

    XmTextSetInsertionPosition(gdb_w, pos);
    XmTextShowPosition(gdb_w, pos);
}

static void move_to_end_of_line(XtPointer, XtIntervalId *id)
{
    move_to_pos(XtPointer(XmTextGetLastPosition(gdb_w)), id);
}

static XmTextPosition start_of_line()
{
    String str = XmTextGetString(gdb_w);
    string s = str;
    XtFree(str);

    int start = s.index("\n(", -1);
    if (start < 0)
	start = s.index("\n>", -1);
    if (start < 0 && !s.contains('(', 0) && !s.contains('>', 0))
	return XmTextPosition(-1);

    return start + 1;
}


//-----------------------------------------------------------------------------
// Incremental search
//-----------------------------------------------------------------------------

enum ISearchState { ISEARCH_NONE = 0, ISEARCH_NEXT = 1, ISEARCH_PREV = -1 };
static ISearchState isearch_state = ISEARCH_NONE;
static string isearch_string = "";
static string isearch_line = "";
static bool have_isearch_line = false;
static bool isearch_motion_ok = false;

// Return current line
string current_line()
{
    if (have_isearch_line)
	return isearch_line;

    String str = XmTextGetString(gdb_w);
    string input(str + promptPosition, 
		 XmTextGetLastPosition(gdb_w) - promptPosition);
    XtFree(str);
    return input;
}

// Show prompt according to current mode
static void show_isearch()
{
    XmTextPosition start = start_of_line();
    if (start == XmTextPosition(-1))
	return;

    string prompt;
    switch (isearch_state)
    {
    case ISEARCH_NONE:
	prompt = gdb->default_prompt();
	break;

    case ISEARCH_NEXT:
	prompt = "(i-search)";
	break;

    case ISEARCH_PREV:
	prompt = "(reverse-i-search)";
	break;
    }

    if (isearch_state != ISEARCH_NONE)
	prompt += "`" + cook(isearch_string) + "': ";
    string input = current_line();
    string line  = prompt + input;

    bool old_private_gdb_output = private_gdb_output;
    private_gdb_output = true;
    XmTextReplace(gdb_w, start, XmTextGetLastPosition(gdb_w), (String)line);
    promptPosition = start + prompt.length();

    XmTextPosition pos = promptPosition;
    int index = input.index(isearch_string);
    if (isearch_state == ISEARCH_NONE || index < 0)
    {
	XmTextSetHighlight(gdb_w, 0, XmTextGetLastPosition(gdb_w),
			   XmHIGHLIGHT_NORMAL);
    }
    else
    {
	XmTextSetHighlight(gdb_w,
			   0,
			   pos + index,
			   XmHIGHLIGHT_NORMAL);
	XmTextSetHighlight(gdb_w,
			   pos + index, 
			   pos + index + isearch_string.length(),
			   XmHIGHLIGHT_SECONDARY_SELECTED);
	XmTextSetHighlight(gdb_w, 
			   pos + index + isearch_string.length(),
			   XmTextGetLastPosition(gdb_w),
			   XmHIGHLIGHT_NORMAL);
    }

    if (index >= 0)
	pos += index;

    XmTextSetInsertionPosition(gdb_w, pos);
    XmTextShowPosition(gdb_w, pos);
    have_isearch_line = false;
    private_gdb_output = old_private_gdb_output;
}

// When i-search is done, show history position given in client_data
static void isearch_done(XtPointer client_data, XtIntervalId *)
{
    int history = int(client_data);

    if (history >= 0)
    {
	bool old_private_gdb_output = private_gdb_output;
	private_gdb_output = true;
	goto_history(history);
	have_isearch_line = false;
	private_gdb_output = old_private_gdb_output;
    }

    show_isearch();
}

void isearch_again(ISearchState new_isearch_state, XEvent *event)
{
    if (isearch_state == ISEARCH_NONE)
    	isearch_string = "";

    if (isearch_state == new_isearch_state)
    {
	// Same state - search again
	int history = search_history(isearch_string, int(isearch_state), true);
	if (history < 0)
	    XtCallActionProc(gdb_w, "beep", event, 0, 0);
	else
	    isearch_done(XtPointer(history), 0);
    }
    else
    {
	isearch_state = new_isearch_state;
	show_isearch();
    }
}

// Action: enter reverse i-search
void isearch_prevAct(Widget, XEvent *event, String *, Cardinal *)
{
    isearch_again(ISEARCH_PREV, event);
}

// Action: enter forward i-search
void isearch_nextAct(Widget, XEvent *event, String *, Cardinal *)
{
    isearch_again(ISEARCH_NEXT, event);
}

// Action: exit i-search
void isearch_exitAct(Widget, XEvent *, String *, Cardinal *)
{
    clear_isearch();
}

// Exit i-search mode and return to normal mode
void clear_isearch(bool reset, bool show)
{
    if (isearch_state != ISEARCH_NONE)
    {
	isearch_state = ISEARCH_NONE;
	if (show)
	    show_isearch();

	if (reset)
	{
	    set_history_from_line(current_line());
	    goto_history();
	}
    }

    isearch_motion_ok = false;
}

// Handle incremental searches; return true if processed
static bool do_isearch(Widget, XmTextVerifyCallbackStruct *change)
{
    if (isearch_state == ISEARCH_NONE)
	return false;

    string saved_isearch_string = isearch_string;
    bool processed = false;

    if (change->startPos == change->endPos)
    {
	// Character insertion
	string input = string(change->text->ptr, change->text->length);
	if (!input.contains('\n', -1))
	{
	    // Add to current search string
	    isearch_string += input;
	    processed = true;
	}
    }
    else if (change->endPos - change->startPos == 1)
    {
	// Backspace - remove last character from search string
	if (isearch_string != "")
	    isearch_string.after(int(isearch_string.length()) - 2) = "";
	else
	    clear_isearch(true, false);

	processed = true;
    }

    if (processed)
    {
	int history = -1;
	if (isearch_string == "" || current_line().contains(isearch_string))
	{
	    // Search string found in current line
	    history = -1;
	}
	else
	{
	    history = search_history(isearch_string, int(isearch_state));
	    if (history < 0)
	    {
		// Search string not found in history
		XtCallActionProc(gdb_w, "beep", change->event, 0, 0);
		isearch_string = saved_isearch_string;
	    }
	}

	// Make this a no-op
	if (!have_isearch_line)
	{
	    isearch_line = current_line();
	    have_isearch_line = true;
	}

	// Redraw current line with appropriate prompt
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			isearch_done, XtPointer(history));

	// Don't quit upon the following call to gdbMotionCB()
	isearch_motion_ok = (change->currInsert != change->newInsert);
    }

    return processed;
}

static void clear_isearch_after_motion(XtPointer, XtIntervalId *)
{
    clear_isearch(false);
}


//-----------------------------------------------------------------------------
// Misc actions
//-----------------------------------------------------------------------------

void controlAct(Widget w, XEvent*, String *params, Cardinal *num_params)
{
    clear_isearch();

    if (*num_params != 1)
    {
	cerr << "gdb-control: usage: gdb-control(CONTROL-CHARACTER)\n";
	return;
    }

    gdb_keyboard_command = true;
    _gdb_command(ctrl(params[0]), w);
}

void insert_source_argAct   (Widget w, XEvent*, String*, Cardinal*)
{
    clear_isearch();

    string arg = source_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable(w)) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable(w)) {
	    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
	    XmTextFieldReplace(w, pos, pos, (String)arg);
	}
    }
}

void insert_graph_argAct (Widget w, XEvent*, String*, Cardinal*)
{
    clear_isearch();

    string arg = DataDisp::graph_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable(w)) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable(w)) {
	    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
	    XmTextFieldReplace(w, pos, pos, (String)arg);
	}
    }
}

void next_tab_groupAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_NEXT_TAB_GROUP);
}

void prev_tab_groupAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_PREV_TAB_GROUP);
}

void get_focusAct (Widget w, XEvent*, String*, Cardinal*)
{
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
}


//-----------------------------------------------------------------------------
// Editing actions
//-----------------------------------------------------------------------------

void beginning_of_lineAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextSetInsertionPosition(gdb_w, promptPosition);
}

void end_of_lineAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextSetInsertionPosition(gdb_w, XmTextGetLastPosition(gdb_w));
}

void forward_characterAct(Widget w, XEvent *e, 
			  String *args, Cardinal *num_args)
{
    XtCallActionProc(w, "forward-character", e, args, *num_args);
}

void backward_characterAct(Widget, XEvent*, String*, Cardinal*)
{
    XmTextPosition pos = XmTextGetInsertionPosition(gdb_w);
    if (pos > promptPosition)
	XmTextSetInsertionPosition(gdb_w, pos - 1);
}

void set_lineAct(Widget, XEvent*, String* params, Cardinal* num_params)
{
    string input = "";
    if (num_params && *num_params > 0)
	input = params[0];
    XmTextReplace(gdb_w, promptPosition, 
		  XmTextGetLastPosition(gdb_w), (String)input);
}

void delete_or_controlAct(Widget w, XEvent *e, 
			  String *args, Cardinal *num_args)
{
    string input = current_line();
    strip_final_newlines(input);
    if (input == "")
	XtCallActionProc(w, "gdb-control", e, args, *num_args);
    else
	XtCallActionProc(w, "delete-next-character", e, args, *num_args);
}



//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

// Veto changes before the current input line
void gdbModifyCB(Widget gdb_w, XtPointer, XtPointer call_data)
{
    if (private_gdb_output)
	return;

    XmTextVerifyCallbackStruct *change = 
	(XmTextVerifyCallbackStruct *)call_data;

    if (do_isearch(gdb_w, change))
	return;

    clear_isearch();

    if (change->startPos < promptPosition && change->text->length == 0)
    {
	// Attempt to change text before prompt
#if 0
	// With Motif, this causes a core dump on Solaris.  - AZ
	change->doit = false;
#else
	// Make it a no-op
	change->startPos = change->endPos = change->newInsert
	    = change->currInsert = promptPosition;
	XtCallActionProc(gdb_w, "beep", change->event, 0, 0);
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			move_to_pos, XtPointer(promptPosition));
#endif
	return;
    }

    // Make sure newlines are always inserted at the end of the line
    if (change->startPos == change->endPos &&
	(change->startPos < promptPosition || 
	 (change->text->length == 1 && change->text->ptr[0] == '\n')))
    {
	// Add any text at end of text window
	XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
	change->newInsert = change->startPos = change->endPos = lastPos;
	
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			move_to_end_of_line, XtPointer(0));
    }
}

// Veto key-based cursor movements before current line
void gdbMotionCB(Widget, XtPointer, XtPointer call_data)
{
    if (private_gdb_output)
	return;

    XmTextVerifyCallbackStruct *change = 
	(XmTextVerifyCallbackStruct *)call_data;

    if (isearch_state != ISEARCH_NONE)
    {
	if (!isearch_motion_ok)
	{
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			    clear_isearch_after_motion, XtPointer(0));
	}
	isearch_motion_ok = false;
    }

    if (change->event != NULL
	&& (change->event->type == KeyPress 
	    || change->event->type == KeyRelease))
    {
	if (change->newInsert < promptPosition)
	{
	    // We are before the current prompt: don't change the cursor
	    // position if a key was pressed.
#if 0
	    // With Motif, this causes a core dump on Solaris.  - AZ
	    change->doit = false;
#else
	    // Make it a no-op.
	    XtCallActionProc(gdb_w, "beep", change->event, 0, 0);
	    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
			    move_to_pos, XtPointer(promptPosition));
#endif
	}
    }
}

// Send completed lines to GDB
void gdbChangeCB(Widget w, XtPointer, XtPointer)
{
    if (private_gdb_output)
	return;

    if (gdb->isReadyWithPrompt())
	gdb_input_at_prompt = true;

    string input = current_line();

    int newlines = input.freq('\n');
    string *lines = new string[newlines + 1];
    split(input, lines, newlines, '\n');

    private_gdb_input = true;

    if (newlines == 0)
    {
	// Non newline found - line is still incomplete
	set_history_from_line(input, true);
    }
    else
    {
	// Process entered lines
	clear_isearch();
	promptPosition = XmTextGetLastPosition(w);
	for (int i = 0; i < newlines; i++)
	{
	    string cmd = lines[i];
	    tty_out(cmd + "\n");

	    if (gdb_input_at_prompt)
	    {
		if (cmd.matches(rxwhite) || cmd == "")
		{
		    // Empty line: repeat last command
		    cmd = last_command_from_history();
		}
		else
		{
		    // Add new command to history
		    add_to_history(cmd);
		}
	    }

	    if (gdb_input_at_prompt && !gdb->isReadyWithPrompt())
	    {
		// GDB is busy and the command last typed at the GDB
		// prompt did not cause any output yet (e.g. a new
		// prompt or diagnostic message).  Since sending CMD
		// directly may interfere with internal communication,
		// place CMD in the command queue instead.

		gdb_command(cmd, w);
	    }
	    else
	    {
		// Process anything else right now, clearing the
		// command queue.

		clearCommandQueue();
		_gdb_command(cmd, w);
	    }
	}
    }

    private_gdb_input = false;

    delete[] lines;
}

//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

void gdbCommandCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    clear_isearch();
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    string command = (String)client_data;
    if (command.contains("..."))
    {
	command = command.before("...") + " ";
	String args[1];
	args[0] = command;
	gdbClearCB(w, client_data, call_data);
	XtCallActionProc(gdb_w, "insert-string", cbs->event, args, 1);
    }
    else
    {
	command.gsub("()", source_arg->get_string());
	gdb_command(command, w);
    }
}

void gdbPrevCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    prev_historyAct(w, cbs->event, 0, &zero);
}

void gdbNextCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    next_historyAct(w, cbs->event, 0, &zero);
}

void gdbISearchPrevCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    isearch_prevAct(w, cbs->event, 0, &zero);
}

void gdbISearchNextCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;
    Cardinal zero = 0;
    isearch_nextAct(w, cbs->event, 0, &zero);
}

void gdbClearCB  (Widget w, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    String args[1] = {""};
    Cardinal num_args = 1;
    set_lineAct(w, cbs->event, args, &num_args);
}

// Remove any text up to the last GDB prompt
void gdbClearWindowCB(Widget, XtPointer, XtPointer)
{
    XmTextPosition start = start_of_line();
    if (start == XmTextPosition(-1))
	return;

    private_gdb_output = true;

    XmTextReplace(gdb_w, 0, start, "");

    promptPosition  -= start;
    messagePosition -= start;
    XmTextSetInsertionPosition(gdb_w, XmTextGetLastPosition(gdb_w));

    private_gdb_output = false;
}

void gdbCompleteCB  (Widget w, XtPointer, XtPointer call_data)
{
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    clear_isearch();
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    Cardinal zero = 0;
    end_of_lineAct(gdb_w, cbs->event, 0, &zero);
    complete_commandAct(gdb_w, cbs->event, 0, &zero);
}

void gdbApplyCB(Widget w, XtPointer, XtPointer call_data)
{
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    clear_isearch();
    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    Cardinal zero = 0;
    end_of_lineAct(gdb_w, cbs->event, 0, &zero);
    XtCallActionProc(gdb_w, "process-return", cbs->event, 0, zero);
}
