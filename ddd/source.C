// $Id$ -*- C++ -*-
// Source callbacks and actions

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

char source_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "source.h"

#include "AppData.h"
#include "ArgField.h"
#include "GDBAgent.h"
#include "SourceView.h"
#include "TimeOut.h"
#include "Command.h"
#include "cook.h"
#include "ctrl.h"
#include "dbx-lookup.h"
#include "events.h"
#include "ddd.h"
#include "post.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"

#include <iostream.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>


//-----------------------------------------------------------------------------
// Moving
//-----------------------------------------------------------------------------

void gdbLookupCB(Widget, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();
    source_view->lookup(arg);
}

void gdbGoBackCB(Widget, XtPointer, XtPointer)
{
    source_view->go_back();
}

void gdbGoForwardCB(Widget, XtPointer, XtPointer)
{
    source_view->go_forward();
}


//-----------------------------------------------------------------------------
// Printing
//-----------------------------------------------------------------------------

void gdbPrintCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(gdb->print_command(arg, false), w);
}

void gdbPrintRefCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(gdb->print_command(gdb->dereferenced_expr(arg), false), w);
}

void gdbDisplayCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command("graph display " + arg, w);
}

void gdbDispRefCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command("graph display " + gdb->dereferenced_expr(arg), w);
}

void gdbWhatisCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(gdb->whatis_command(arg), w);
}




//-----------------------------------------------------------------------------
// Breakpoints
//-----------------------------------------------------------------------------

bool have_break_at_arg()
{
    return source_view->bp_at(source_arg->get_string()) != 0;
}

bool break_enabled_at_arg()
{
    BreakPoint *bp = source_view->bp_at(source_arg->get_string());
    return bp != 0 && bp->enabled();
}

void gdbBreakAtCB(Widget w, XtPointer, XtPointer)
{
    SourceView::create_bp(source_arg->get_string(), w);
}

void gdbTempBreakAtCB(Widget w, XtPointer, XtPointer)
{
    SourceView::create_temp_bp(source_arg->get_string(), w);
}

void gdbClearAtCB(Widget w, XtPointer, XtPointer)
{
    SourceView::clear_bp(source_arg->get_string(), w);
}

void gdbToggleBreakCB(Widget w, XtPointer, XtPointer)
{
    SourceView::set_bp(source_arg->get_string(), !have_break_at_arg(), 
		       false, w);
}

void gdbContUntilCB(Widget w, XtPointer, XtPointer)
{
    SourceView::temp_n_cont(source_arg->get_string(), w);
}

void gdbSetPCCB(Widget w, XtPointer, XtPointer)
{
    SourceView::move_pc(source_arg->get_string(), w);
}

void gdbToggleEnableCB(Widget w, XtPointer, XtPointer)
{
    BreakPoint *bp = source_view->bp_at(source_arg->get_string());
    if (bp != 0)
    {
	if (bp->enabled())
	    SourceView::disable_bp(bp->number(), w);
	else
	    SourceView::enable_bp(bp->number(), w);
    }
}




//-----------------------------------------------------------------------------
// Searching
//-----------------------------------------------------------------------------

static void gdbFindCB(Widget w, 
		      XtPointer call_data, 
		      SourceView::SearchDirection direction)
{
    XmPushButtonCallbackStruct *cbs = 
	(XmPushButtonCallbackStruct *)call_data;

    // LessTif 0.79 sometimes returns NULL in cbs->event.  Handle this.
    Time tm;
    if (cbs->event != 0)
	tm = time(cbs->event);
    else
	tm = XtLastTimestampProcessed(XtDisplay(w));

    string s = source_arg->get_string();
    source_view->find(s, direction, app_data.find_words_only, tm);
}

void gdbFindForwardCB(Widget w, XtPointer, XtPointer call_data)
{
    gdbFindCB(w, call_data, SourceView::forward);
}

void gdbFindBackwardCB(Widget w, XtPointer, XtPointer call_data)
{
    gdbFindCB(w, call_data, SourceView::backward);
}


//-----------------------------------------------------------------------------
// Editor invocation
//-----------------------------------------------------------------------------

static void gdbDeleteEditAgent(XtPointer client_data, XtIntervalId *)
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
}

static void gdbEditDoneHP(Agent *edit_agent, void *client_data, void *)
{
    // Editor has terminated: reload current source file
    string *pfile = (string *)client_data;
    // post_gdb_message("Editing of " + quote(*pfile) + " done.");
    delete pfile;

    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
		    gdbDeleteEditAgent, 
		    XtPointer(edit_agent));
    gdbReloadSourceCB(gdb_w, 0, 0);
}

void gdbEditSourceCB  (Widget w, XtPointer, XtPointer)
{
    string pos = source_view->file_of_cursor();
    string file = pos.before(':');
    string line = pos.after(':');

    if (file == "" || line == "0")
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
    LiterateAgent *edit_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(w), cmd);
    edit_agent->removeAllHandlers(Died);
    edit_agent->addHandler(InputEOF, gdbEditDoneHP, (void *)new string(file));
    edit_agent->start();
}

void gdbReloadSourceCB(Widget, XtPointer, XtPointer)
{
    source_view->reload();
}
