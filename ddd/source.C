// $Id$ -*- C++ -*-
// Source callbacks and actions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char source_rcsid[] = 
    "$Id$";

#include "config.h"

#include "source.h"

#include "AppData.h"
#include "ArgField.h"
#include "GDBAgent.h"
#include "UndoBuffer.h"
#include "SourceView.h"
#include "TimeOut.h"
#include "Command.h"
#include "cook.h"
#include "ctrl.h"
#include "dbx-lookup.h"
#include "events.h"
#include "ddd.h"
#include "deref.h"
#include "post.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"
#include "fortranize.h"

#include <iostream>
#ifdef IF_MOTIF
#include <Xm/Xm.h>
#include <Xm/Text.h>
#endif // IF_MOTIF


//-----------------------------------------------------------------------------
// Arg
//-----------------------------------------------------------------------------

static string current_arg(bool globals_first = false)
{
    return fortranize(source_arg->get_string(), globals_first);
}

//-----------------------------------------------------------------------------
// Moving
//-----------------------------------------------------------------------------

void gdbLookupCB(CB_ARG_LIST_NULL)
{
    source_view->lookup(current_arg(true));
}

void gdbUndoCB(CB_ARG_LIST_NULL)
{
    undo_buffer.undo();
}

void gdbRedoCB(CB_ARG_LIST_NULL)
{
    undo_buffer.redo();
}


//-----------------------------------------------------------------------------
// Printing
//-----------------------------------------------------------------------------

void gdbPrintCB(CB_ALIST_12(Widget w, XtP(bool) client_data))
{
    const bool internal = (bool)(long)client_data;

    const string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command(gdb->print_command(arg, internal), w);
}

void gdbPrintRefCB(CB_ALIST_12(Widget w, XtP(bool) client_data))
{
    const bool internal = (bool)(long)client_data;

    const string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command(gdb->print_command(deref(arg), internal), w);
}

void gdbDisplayCB(CB_ALIST_1(Widget w))
{
    string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command("graph display " + arg, w);
}

void gdbDispRefCB(CB_ALIST_1(Widget w))
{
    string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command("graph display " + deref(arg), w);
}

void gdbWhatisCB(CB_ALIST_1(Widget w))
{
    string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command(gdb->whatis_command(arg), w);
}




//-----------------------------------------------------------------------------
// Breakpoints
//-----------------------------------------------------------------------------

bool have_breakpoint_at_arg()
{
    return source_view->breakpoint_at(current_arg(true)) != 0;
}

bool have_enabled_breakpoint_at_arg()
{
    BreakPoint *bp = source_view->breakpoint_at(current_arg(true));
    return bp != 0 && bp->enabled();
}

#if defined(IF_XM)

void gdbBreakAtCB(Widget w, XtPointer, XtPointer)
{
    source_view->create_bp(current_arg(true), w);
}

void gdbTempBreakAtCB(Widget w, XtPointer, XtPointer)
{
    source_view->create_temp_bp(current_arg(true), w);
}

void gdbRegexBreakAtCB(Widget w, XtPointer, XtPointer)
{
    gdb_command("rbreak " + source_arg->get_string(), w);
}

#else

void gdbBreakAtCB(GUI::Widget *w)
{
    source_view->create_bp(current_arg(true), w);
}

void gdbTempBreakAtCB(GUI::Widget *w)
{
    source_view->create_temp_bp(current_arg(true), w);
}

void gdbRegexBreakAtCB(GUI::Widget *w)
{
    gdb_command1("rbreak " + source_arg->get_string(), w);
}

#endif

#if defined(IF_XM)

void gdbClearAtCB(Widget w, XtPointer, XtPointer)
{
    source_view->clear_bp(current_arg(true), w);
}

void gdbToggleBreakCB(Widget w, XtPointer, XtPointer)
{
    source_view->set_bp(current_arg(true), 
			!have_breakpoint_at_arg(), false, "", w);
}

void gdbContUntilCB(Widget w, XtPointer, XtPointer)
{
    source_view->temp_n_cont(current_arg(true), w);
}

void gdbSetPCCB(Widget w, XtPointer, XtPointer)
{
    source_view->move_pc(current_arg(true), w);
}

void gdbToggleEnableBreakpointCB(Widget w, XtPointer, XtPointer)
{
    BreakPoint *bp = source_view->breakpoint_at(current_arg(true));
    if (bp != 0)
    {
	if (bp->enabled())
	    source_view->disable_bp(bp->number(), w);
	else
	    source_view->enable_bp(bp->number(), w);
    }
}

void gdbToggleEnableWatchpointCB(Widget w, XtPointer, XtPointer)
{
    BreakPoint *bp = source_view->watchpoint_at(current_arg(true));
    if (bp != 0)
    {
	if (bp->enabled())
	    source_view->disable_bp(bp->number(), w);
	else
	    source_view->enable_bp(bp->number(), w);
    }
}

#else

void gdbClearAtCB(GUI::Widget *w)
{
    source_view->clear_bp(current_arg(true), w);
}

void gdbToggleBreakCB(GUI::Widget *w)
{
    source_view->set_bp(current_arg(true), 
			!have_breakpoint_at_arg(), false, "", w);
}

void gdbContUntilCB(GUI::Widget *w)
{
    source_view->temp_n_cont(current_arg(true), w);
}

void gdbSetPCCB(GUI::Widget *w)
{
    source_view->move_pc(current_arg(true), w);
}

void gdbToggleEnableBreakpointCB(GUI::Widget *w)
{
    BreakPoint *bp = source_view->breakpoint_at(current_arg(true));
    if (bp != 0)
    {
	if (bp->enabled())
	    source_view->disable_bp(bp->number(), w);
	else
	    source_view->enable_bp(bp->number(), w);
    }
}

void gdbToggleEnableWatchpointCB(GUI::Widget *w)
{
    BreakPoint *bp = source_view->watchpoint_at(current_arg(true));
    if (bp != 0)
    {
	if (bp->enabled())
	    source_view->disable_bp(bp->number(), w);
	else
	    source_view->enable_bp(bp->number(), w);
    }
}

#endif

void gdbEditBreakpointPropertiesCB(CB_ALIST_NULL)
{
    BreakPoint *bp = source_view->breakpoint_at(current_arg(true));
    if (bp != 0)
	source_view->edit_breakpoint_properties(bp->number());
}

void gdbEditWatchpointPropertiesCB(CB_ALIST_NULL)
{
    BreakPoint *bp = source_view->watchpoint_at(current_arg(true));
    if (bp != 0)
	source_view->edit_breakpoint_properties(bp->number());
}



//-----------------------------------------------------------------------------
// Watchpoints
//-----------------------------------------------------------------------------

// Like in `print', we don't use GLOBALS_FIRST for fortranizing the
// current arg
bool have_watchpoint_at_arg()
{
    return source_view->watchpoint_at(current_arg()) != 0;
}
bool have_watchpoint_at_ref_arg()
{
    return source_view->watchpoint_at(deref(current_arg())) != 0;
}

bool have_enabled_watchpoint_at_arg()
{
    BreakPoint *bp = source_view->watchpoint_at(current_arg());
    return bp != 0 && bp->enabled();
}

void gdbWatchCB(CB_ALIST_12(Widget w, XtP(long) client_data))
{
    const string arg = current_arg();

#if 0 // We may have different kinds of watchpoints!
    if (have_watchpoint_at_arg())
    {
	// Don't place multiple watchpoints on one expression
	gdbUnwatchCB(w, client_data, call_data);
    }
#endif

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command(gdb->watch_command(arg, 
				       WatchMode((int)(long)client_data)), w);
}

void gdbWatchRefCB(Widget w, XtPointer, XtPointer)
{
    string arg = current_arg();

    if (!arg.empty() && !arg.matches(rxwhite))
	gdb_command(gdb->watch_command(deref(arg)), w);
}

void gdbUnwatchCB(CB_ALIST_NULL)
{
    if (gdb->type() == JDB)
    {
	// JDB 1.2 has an `unwatch' command
	string arg = current_arg();
	if (!arg.empty() && !arg.matches(rxwhite))
	{
	    gdb_command("unwatch all " + arg);
	    gdb_command("unwatch access " + arg);
	}
    }
    else
    {
	// All other debuggers handle watchpoints like breakpoints
	BreakPoint *wp = source_view->watchpoint_at(current_arg());
	if (wp == 0)
	    return;
	source_view->delete_bp(wp->number());
    }
}

void gdbToggleWatchCB(CB_ALIST_12(Widget w, XtP(long) client_data))
{
    BreakPoint *bp = source_view->watchpoint_at(current_arg());
    if (bp == 0)
	gdbWatchCB(CB_ARGS_12(w, client_data));
    else
	gdbUnwatchCB(CB_ARGS_NULL);
}


//-----------------------------------------------------------------------------
// Searching
//-----------------------------------------------------------------------------

static SourceView::SearchDirection last_find_direction = SourceView::forward;

void gdbFindCB(CB_ALIST_12(Widget w, XtP(SourceView::SearchDirection) client_data))
{
    SourceView::SearchDirection direction = 
	(SourceView::SearchDirection) (long) client_data;

    assert(direction == SourceView::forward || 
	   direction == SourceView::backward);

    if (direction != last_find_direction)
    {
	last_find_direction = direction;
	update_options();
    }

    // Forget the exact timestamp; use the last processed.
#ifdef IF_MOTIF
    Time tm = XtLastTimestampProcessed(XtDisplay(w));
#else // NOT IF_MOTIF
    Time tm = 0;
#endif // IF_MOTIF

    string key = source_arg->get_string();
    source_view->find(key, direction, 
		      app_data.find_words_only,
		      app_data.find_case_sensitive,
		      tm);
    source_arg->set_string(key);
}


void gdbFindAgainCB(CB_ALIST_1(Widget w))
{
    gdbFindCB(CB_ARGS_12(w, current_find_direction()));
}

SourceView::SearchDirection current_find_direction()
{
    return last_find_direction;
}


//-----------------------------------------------------------------------------
// Editor invocation
//-----------------------------------------------------------------------------

static void gdbDeleteEditAgent(TM_ALIST_1(XtP(Agent *) client_data))
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
}

static string output_buffer;

static void gdbEditOutputHP(Agent *, void *, void *call_data)
{
    DataLength *input = (DataLength *)call_data;
    output_buffer += string(input->data, input->length);
    while (output_buffer.contains('\n'))
    {
	set_status(output_buffer.before('\n'));
	output_buffer = output_buffer.after('\n');
    }
    if (!output_buffer.empty())
	set_status(output_buffer);
}

static void gdbEditDoneHP(Agent *edit_agent, void *, void *)
{
    // Editor has terminated: reload current source file

#ifdef IF_MOTIF
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
		    gdbDeleteEditAgent, 
		    XtPointer(edit_agent));
#else // NOT IF_MOTIF
    Glib::signal_idle().connect(sigc::bind_return(sigc::bind(PTR_FUN(gdbDeleteEditAgent),
							     edit_agent),
						  false));
#endif // IF_MOTIF

    gdbReloadSourceCB(CB_ARGS_NULL);

    edit_agent->removeHandler(InputEOF, gdbEditDoneHP);
    edit_agent->removeHandler(Died,     gdbEditDoneHP);
}

#if defined(IF_XM)
void gdbEditSourceCB  (Widget w, XtPointer, XtPointer)
#else
void gdbEditSourceCB  (GUI::Widget *w)
#endif
{
    string pos = source_view->file_of_cursor();
    string file = pos.before(':');
    string line = pos.after(':');

    if (file.empty() || line == "0")
    {
	post_error("No source.", "no_source_edit_error", w);
	return;
    }

    StatusDelay delay("Invoking editor for " + quote(file));

    string cmd = app_data.edit_command;
    cmd.gsub("@FILE@", file);
    cmd.gsub("@LINE@", line);
    cmd = sh_command(cmd);

    // Invoke an editor in the background
#if defined(IF_XM)
    LiterateAgent *edit_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(w), cmd);
#else
    LiterateAgent *edit_agent = 
	new LiterateAgent(w->get_main(), cmd);
#endif

    output_buffer = "";

    edit_agent->removeAllHandlers(Died);
    edit_agent->addHandler(InputEOF, gdbEditDoneHP);
    edit_agent->addHandler(Died,     gdbEditDoneHP);
    edit_agent->addHandler(Input,    gdbEditOutputHP);
    edit_agent->addHandler(Error,    gdbEditOutputHP);
    edit_agent->start();
}

void gdbReloadSourceCB(CB_ARG_LIST_NULL)
{
    source_view->reload();
}
