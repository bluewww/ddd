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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char editing_rcsid[] = 
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

#include <iostream.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>


// True if last input was at gdb prompt
bool gdb_input_at_prompt = true;

void controlAct(Widget w, XEvent*, String *params, Cardinal *num_params)
{
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
    string arg = source_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable) {
	    XmTextPosition pos = XmTextFieldGetInsertionPosition(w);
	    XmTextFieldReplace(w, pos, pos, (String)arg);
	}
    }
}

void insert_graph_argAct (Widget w, XEvent*, String*, Cardinal*)
{
    string arg = DataDisp::graph_arg->get_string();
    if (XmIsText(w)) {
	if (XmTextGetEditable) {
	    XmTextPosition pos = XmTextGetInsertionPosition(w);
	    XmTextReplace(w, pos, pos, (String)arg);
	}
    }
    else if (XmIsTextField(w)) {
	if (XmTextFieldGetEditable) {
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

static void move_to_end_of_line(XtPointer, XtIntervalId *)
{
    XmTextPosition lastPos = XmTextGetLastPosition(gdb_w);
    XmTextSetInsertionPosition(gdb_w, lastPos);
    XmTextShowPosition(gdb_w, lastPos);
}

// Veto changes before the current input line
void gdbModifyCB(Widget gdb_w, XtPointer, XtPointer call_data)
{
    if (private_gdb_output)
	return;

    XmTextVerifyCallbackStruct *change = 
	(XmTextVerifyCallbackStruct *)call_data;

    if (change->startPos < promptPosition && change->text->length == 0)
    {
	// Attempt to delete text before prompt
#ifdef LESSTIF_VERSION
	// This works with LessTif only;
	// using Motif, it causes a core dump on Solaris.  - AZ
	change->doit = false;
#else
	// Make it a no-op
	change->startPos = change->endPos = change->newInsert = 
	    change->currInsert;
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

    if (change->newInsert >= promptPosition)
	return;

    // We are before the current prompt: don't change the cursor
    // position if a key was pressed.
    if (change->event != NULL && 
	(change->event->type == KeyPress || change->event->type == KeyRelease))
    {
#ifdef LESSTIF_VERSION
	// This works with LessTif only;
	// using Motif, it causes a core dump on Solaris.  - AZ
	change->doit = false;
#else
	// Make it a no-op.
	change->newInsert = change->currInsert;
#endif
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
	set_history_from_line(input, true);
    }
    else
    {
	// Process entered lines
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
    String str = XmTextGetString(gdb_w);
    string s = str;
    XtFree(str);

    int index = s.index("\n(", -1);
    if (index < 0)
	return;
    index++;

    private_gdb_output = true;

    XmTextReplace(gdb_w, 0, index, "");

    promptPosition  -= index;
    messagePosition -= index;
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

    XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct *)call_data;

    Cardinal zero = 0;
    end_of_lineAct(gdb_w, cbs->event, 0, &zero);
    XtCallActionProc(gdb_w, "process-return", cbs->event, 0, zero);
}
