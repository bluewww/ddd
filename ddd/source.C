// $Id$ -*- C++ -*-
// Source callbacks and actions

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

const char source_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "source.h"

#include "AppData.h"
#include "ArgField.h"
#include "GDBAgent.h"
#include "SourceView.h"
#include "commandQ.h"
#include "cook.h"
#include "ctrl.h"
#include "dbx-lookup.h"
#include "events.h"
#include "ddd.h"
#include "shell.h"
#include "status.h"

#include <iostream.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>


void gdbBreakArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();
    string pos;

    switch (gdb->type())
    {
    case GDB:
	if (arg != "" && arg[0] == '0')
	    arg = "*" + arg; // Address given
	gdb_command("break " + arg, w);
	break;

    case DBX:
	if (arg.matches(rxint))
	{
	    // Line number given
	    gdb_command("stop at " + arg, w);
	}
	else if (arg.contains(":") && !arg.contains("::"))
	{
	    // Function:Line given
	    pos = arg;
	}
	else
	{
	    // Function name given
	    pos = dbx_lookup(arg);
	}

	if (pos != "")
	{
	    gdb_command("file " + pos.before(":"), w);
	    gdb_command("stop at " + pos.after(":"), w);
	}
	break;

    case XDB:
	gdb_command("b " + arg, w);
	break;
    }
}

void gdbClearArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string pos;
    string arg = source_arg->get_string();
    switch (gdb->type())
    {
    case GDB:
	if (arg != "" && arg[0] == '0')
	    arg = "*" + arg; // Address given
	gdb_command(SourceView::clear_command(arg));
	break;

    case DBX:
	if (arg.matches(rxint))
	{
	    // Line number given
	    gdb_command(SourceView::clear_command(arg));
	}
	else if (arg.contains(":") && !arg.contains("::"))
	{
	    // Function:Line given
	    pos = arg;
	}
	else
	{
	    // Function name given
	    pos = dbx_lookup(arg);
	}

	if (pos != "")
	{
	    gdb_command("file " + pos.before(":"), w);
	    gdb_command(SourceView::clear_command(pos.after(":")), w);
	}
	break;

    case XDB:
	gdb_command(SourceView::clear_command(arg), w);
	break;
    }
}

void gdbLineArgCmdCB(Widget w, XtPointer client_data, XtPointer)
{
    string cmd = (String)client_data;
    string arg = source_arg->get_string();

    gdb_command(cmd + arg, w);
}

void gdbPrintArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(gdb->print_command(arg), w);
}

void gdbDisplayArgCmdCB(Widget w, XtPointer, XtPointer)
{
    string cmd = "graph display";
    string arg = source_arg->get_string();

    if (arg != "" && !arg.matches(rxwhite))
	gdb_command(cmd + " " + arg, w);
}

void gdbLookupCB(Widget, XtPointer, XtPointer)
{
    string arg = source_arg->get_string();
    source_view->lookup(arg);
}

void gdbFindForwardCB(Widget, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = 
	(XmPushButtonCallbackStruct *)call_data;

    string s = source_arg->get_string();
    source_view->find(s, SourceView::forward, 
		      app_data.find_words_only, time(cbs->event));
}

void gdbFindBackwardCB(Widget, XtPointer, XtPointer call_data)
{
    XmPushButtonCallbackStruct *cbs = 
	(XmPushButtonCallbackStruct *)call_data;

    string s = source_arg->get_string();
    source_view->find(s, SourceView::backward, 
		      app_data.find_words_only, time(cbs->event));
}

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
    string pos = source_view->line_of_cursor();
    string file = pos.before(':');
    string line = pos.after(':');

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

void gdbReloadSourceCB  (Widget, XtPointer, XtPointer)
{
    source_view->reload();
}

void gdbGoBackCB  (Widget, XtPointer, XtPointer)
{
    source_view->go_back();
}

void gdbGoForwardCB  (Widget, XtPointer, XtPointer)
{
    source_view->go_forward();
}
