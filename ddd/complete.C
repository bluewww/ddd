// $Id$ -*- C++ -*-
// Command and arg completion

// Copyright (C) 1996-1997 Technische Universitaet Braunschweig, Germany.
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

char complete_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "complete.h"

#include "AppData.h"
#include "Delay.h"
#include "SmartC.h"
#include "ddd.h"
#include "disp-read.h"
#include "editing.h"
#include "post.h"
#include "regexps.h"
#include "string-fun.h"

#include <ctype.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>


// Completion delay flag
static Delay *completion_delay = 0;

//-----------------------------------------------------------------------------
// Line Completion
//-----------------------------------------------------------------------------

// Remove adjacent duplicates in A
static void uniq(string *a, int& size)
{
    int j = 1;
    for (int i = 1; i < size; i++)
	if (a[i] != a[j - 1])
	    a[j++] = a[i];
    
    size = j;
}

// Info passed to reply functions
struct CompletionInfo {
    Widget widget;		// Widget
    XEvent *event;		// Event 
    string input;		// Current input
    string cmd;			// Current command
    string prefix;		// Current prefix

    CompletionInfo()
	: widget(0), event(0), input(), cmd(), prefix()
    {}
private:
    CompletionInfo(const CompletionInfo&)
	: widget(0), event(0), input(), cmd(), prefix()
    {
	assert(0);
    }

    CompletionInfo& operator = (const CompletionInfo&)
    {
	assert(0); return *this;
    }
};

static void complete_reply(const string& complete_answer, void *qu_data);

// Set completion
static void set_completion(const CompletionInfo& info, string completion)
{
    // Set input to common prefix
    if (info.widget == gdb_w)
    {
	private_gdb_output = true;

	XmTextReplace(gdb_w, promptPosition,
		      XmTextGetLastPosition(gdb_w), 
		      (String)completion);

	private_gdb_output = false;
    }
    else
    {
	if (XmIsTextField(info.widget))
	{
	    XmTextFieldSetString(info.widget, (String)completion);
	}
	else if (XmIsText(info.widget))
	{
	    XmTextSetString(info.widget, (String)completion);
	}
    }
}

// Same, but for single possible completion
static string complete_single_completion(string completion)
{
    // Only one possible expansion: Add final single quote if
    // necessary and add final space as well.

    if (completion.freq('\'') % 2 != 0)
	completion += '\'';
    completion += ' ';

    return completion;
}


// All completions are done
static void completion_done(const CompletionInfo& info)
{
    if (XmIsTextField(info.widget))
    {
	XmTextPosition last_pos = 
	    XmTextFieldGetLastPosition(info.widget);
	XmTextFieldSetInsertionPosition(info.widget, last_pos);
	XmTextFieldShowPosition(info.widget, last_pos);
	XmTextFieldSetEditable(info.widget, true);
    }
    else if (XmIsText(info.widget))
    {
	XmTextPosition last_pos = 
	    XmTextGetLastPosition(info.widget);
	XmTextSetInsertionPosition(info.widget, last_pos);
	XmTextShowPosition(info.widget, last_pos);
	XmTextSetEditable(info.widget, true);
    }

    XmTextSetEditable(gdb_w, true);
}

void clear_completion_delay()
{
    if (completion_delay)
	delete completion_delay;
    completion_delay = 0;
}

static string *completions  = 0;
static int completions_size = 0;

// Send completion question
static void complete(Widget w, XEvent *e, string input, string cmd)
{
    // Issue diagnostic if completion doesn't work right now
    if (!gdb->isReadyWithPrompt())
    {
	post_gdb_busy(w);
	return;
    }

    static CompletionInfo info;
    info.widget = w;
    info.event  = e;
    info.input  = input;
    info.cmd    = cmd;
    info.prefix = "";

    // Compare with last completions
    static Widget last_completion_w     = 0;
    static int    last_completion_index = -1;
    static string last_completion       = string(char(-1));

    if (completions_size > 0 
	&& completions
	&& completions[0] != ""
	&& w == last_completion_w)
    {
	// Check if this is to be the next completion
	string next_completion = "";

	if (input == last_completion)
	{
	    // We have already shown possible completions:
	    // Expand to first completion
	    last_completion_index = 0;
	    next_completion = complete_single_completion(completions[0]);
	}
	else if (completions_size > 1
		 && last_completion_index >= 0 
		 && input == complete_single_completion(
		     completions[last_completion_index]))
	{
	    if (last_completion_index < completions_size - 1)
	    {
		// Show next completion
		next_completion = complete_single_completion(
		    completions[++last_completion_index]);
	    }
	    else
	    {
		// All completions shown; re-start with initial input
		last_completion_index = -1;
		next_completion = last_completion;
	    }
	}

	if (next_completion != "")
	{
	    set_completion(info, next_completion);
	    completion_done(info);
	    return;
	}
    }

    // Start a new completion session

    // Clear old completions
    last_completion_w     = w;
    last_completion       = input;
    last_completion_index = -1;

    if (completions)
    {
	delete[] completions;
	completions = 0;
    }
    completions_size = 0;

    // Go and ask GDB for completions.

    if (is_graph_cmd(cmd))
    {
	// Allow completion of `graph' commands
	info.prefix = cmd.through(rxwhite);
	cmd = cmd.from(int(info.prefix.length()));
    }
    string complete_cmd = "complete " + cmd;

    if (XmIsTextField(w))
	XmTextFieldSetEditable(w, false);
    else if (XmIsText(w))
	XmTextSetEditable(w, false);
    XmTextSetEditable(gdb_w, false);
    
    gdb->send_question(complete_cmd, complete_reply, (void *)&info);

    completion_delay = new Delay;
}


// Handle possible completions
static void complete_reply(const string& complete_answer, void *qu_data)
{
    const CompletionInfo& info = *((CompletionInfo *)qu_data);

    bool from_gdb_w = (info.widget == gdb_w);

    string input = info.input;
    strip_space(input);

    assert(completions == 0);

    int lines = complete_answer.freq('\n') + 1;
    completions      = new string[lines];
    completions_size = split(complete_answer, completions, lines, '\n');
    smart_sort(completions, completions_size);
    uniq(completions, completions_size);

    if (completions_size == 0 || completions[0] == "")
    {
	// No completion (sigh)
	XtCallActionProc(gdb_w, "beep", info.event, 0, 0);
    }
    else if (completions[0].index("Undefined command:") == 0)
    {
	// GDB versions earlier than 4.13 do not support
	// the `complete' command.

	string message;
	for (int i = 0; i < completions_size; i++)
	    message += completions[i] + '\n';
	post_error(message, "no_completion_error", info.widget);
    }
    else
    {
	if (info.cmd != info.input)
	{
	    // Strip initial base command
	    for (int i = 0; i < completions_size; i++)
		completions[i] = completions[i].after(' ');
	}

	if (info.prefix != "")
	{
	    // Add `graph' prefix again
	    for (int i = 0; i < completions_size; i++)
		completions[i].prepend(info.prefix);
	}

	// Find common prefix
	string common_pfx = completions[0];
	int i;
	for (i = 1; i < completions_size; i++)
	    common_pfx = common_prefix(common_pfx, completions[i]);

	if (completions_size > 1 && input == common_pfx)
	{
	    // We're already as far as we can get
	    if (from_gdb_w)
	    {
		// Show possible expansions in command window.
		int skip = common_pfx.index(rxwhite, -1) + 1;
	    
		string insertion;
		if (from_gdb_w)
		    insertion += input;
		insertion += "\n";
		for (i = 0; i < completions_size; i++)
		{
		    insertion += completions[i].from(skip);
		    insertion += "\n";
		}
		gdb_out(insertion);
		gdb_out(gdb->prompt());

		XmTextSetInsertionPosition(gdb_w, 
					   XmTextGetLastPosition(gdb_w));
	    }
	}
	else
	{
	    string completion = common_pfx;
	    if (completions_size == 1)
		completion = complete_single_completion(completion);
	    set_completion(info, completion);
	}
    }

    completion_done(info);
}

static void tabAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    if (XmIsText(w))
	XtCallActionProc(w, "process-tab", e, args, *num_args);
    else if (XmIsPrimitive(w))
	XtCallActionProc(w, "PrimitiveNextTabGroup", e, args, *num_args);
}

// Complete current GDB command
void complete_commandAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    if (gdb->type() != GDB
	|| w != gdb_w
	|| !gdb->isReadyWithPrompt()
	|| XmTextGetInsertionPosition(w) != XmTextGetLastPosition(w))
    {
	tabAct(w, e, args, num_args);
	return;
    }

    // Insert single quote if necessary
    string input = current_line();
    if (is_break_cmd(input))
    {
	int last_space = input.index(rxwhite, -1);
	if (last_space >= 0)
	{
	    string last_word = input.after(last_space);
	    if (last_word.length() > 0 && last_word[0] != '\'')
	    {
		for (int i = 0; i < int(last_word.length()); i++)
		{
		    char c = last_word[i];
		    if (!isalnum(c) && c != '_')
		    {
			input(last_space + 1, 0) = '\'';
			break;
		    }
		}
	    }
	}
    }

    complete(w, e, input, input);
}


// Complete GDB argument
static void _complete_argAct(Widget w, 
			     XEvent *e, 
			     String* args, 
			     Cardinal* num_args,
			     bool tab)
{
    if ((tab && !app_data.global_tab_completion) 
	|| gdb->type() != GDB 
	|| !gdb->isReadyWithPrompt())
    {
	tabAct(w, e, args, num_args);
	return;
    }

    // The command to use as prefix for completions
    string base = gdb->print_command("", false);
    if (*num_args >= 1)
	base = args[0];
    strip_space(base);

    // Insert single quote if necessary
    String _input = 0;
    if (XmIsTextField(w))
	_input = XmTextFieldGetString(w);
    else if (XmIsText(w))
	_input = XmTextGetString(w);

    if (_input == 0)
	return;

    string input(_input);
    XtFree(_input);

    if (is_break_cmd(base))
    {
	if (input.length() > 0 && input[0] != '\'')
	{
	    for (int i = 0; i < int(input.length()); i++)
	    {
		char c = input[i];
		if (!isalnum(c) && c != '_')
		{
		    input(0, 0) = '\'';
		    break;
		}
	    }
	}
    }

    if (base != "")
	complete(w, e, input, base + " " + input);
    else
	complete(w, e, input, input);
}

void complete_argAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    _complete_argAct(w, e, args, num_args, false);
}

void complete_tabAct(Widget w, XEvent *e, String* args, Cardinal* num_args)
{
    _complete_argAct(w, e, args, num_args, true);
}
