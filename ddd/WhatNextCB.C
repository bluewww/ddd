// $Id$ -*- C++ -*-
// DDD `What next?' help

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char WhatNextCB_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "WhatNextCB.h"

#include "commandQ.h"
#include "converters.h"
#include "ddd.h"
#include "editing.h"
#include "exit.h"
#include "file.h"
#include "question.h"
#include "status.h"
#include "verify.h"
#include "version.h"
#include "wm.h"
#include "AppData.h"
#include "DataDisp.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "SourceView.h"

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>

// Show a suggestion named NAME
static void hint_on(String name)
{
    // Create some `dummy' widget and create a help text for it
    Widget suggestion = 
	verify(XmCreateInformationDialog(find_shell(), name, 0, 0));

    ImmediateHelpCB(suggestion, XtPointer(0), XtPointer(0));

    DestroyWhenIdle(suggestion);
}

static bool no_program()
{
    string current_file;
    int current_pid;
    bool attached;
    get_current_file(current_file, current_pid, attached);
    return current_file == "";
}

static bool no_source_and_no_code()
{
    return !source_view->have_source()
	&& (gdb->type() != GDB || !app_data.disassemble);
}

static bool no_gdb()
{
    return !gdb_initialized;
}

static bool gdb_has_crashed()
{
    return !no_gdb() && (gdb == 0 || gdb->pid() <= 0 || !gdb->running());
}

static bool code_but_no_source()
{
    return source_view->have_pc() && !source_view->have_exec_pos()
	&& (gdb->type() != GDB || !app_data.disassemble);
}

// Return 1 if the signal specified in PROGRAM_STATE is passed to the
// program; 0 if not, -1 if undecided.
static int passed_to_program(string program_state)
{
    string signal = program_state.from("SIG");
    signal = signal.through(rxalpha);

    if (signal != "")
    {
	string signal_description = program_state.after(signal);
	signal_description = signal_description.after(rxwhite);

	if (signal_description == "")
	    signal_description = signal;
	
	defineConversionMacro("SIGNAL", signal);
	defineConversionMacro("SIGNAL_DESCRIPTION", signal_description);

	string ans = gdb_question("info handle " + signal);

	// `info handle SIGINT' output has the form
	// "Signal        Stop\tPrint\tPass to program\tDescription\n"
	// "SIGINT        Yes\tYes\tNo\t\tInterrupt\n"

	ans.downcase();
	string header = ans.before("pass ");
	int tabs_before_pass = header.freq('\t');
	string info = ans.after('\n');
	while (tabs_before_pass--)
	    info = info.after('\t');
	if (info.contains('y', 0))
	    return 1;
	if (info.contains('n', 0))
	    return 0;
    }
    return -1;
}

// Return whether the program is still running
bool program_running(string& state)
{
    state = "is not being run";

    switch (gdb->type())
    {
    case GDB:
        {
	    // In case we have a core dump, treat the program as
	    // `running' - remember we still have the possibility to
	    // examine variables, etc.
	    string ans = gdb_question("info files");
	    if (ans.contains("core dump"))
		return true;

	    ans = gdb_question("info program");
	    if (ans.contains("not being run"))
		return false;

	    if (ans.contains("\nIt stopped "))
	    {
		state = ans.from("\nIt stopped ");
		state = "has " + state.after("\nIt ");
		state = state.before('.');
	    }
	}
	break;

    case DBX:
    case XDB:
	if (!source_view->have_exec_pos())
	    return false;

	state = "has stopped";
	break;
    }

    return true;
}

// Give a help dependent on current DDD state
void WhatNextCB(Widget, XtPointer, XtPointer)
{
    // Special DDD states
    if (ddd_has_crashed)
    {
	hint_on("fatal_dialog");
	return;
    }

    // Special GDB states
    if (no_gdb())
    {
	hint_on("no_debugger_dialog");
	return;
    }

    if (gdb_has_crashed())
    {
	hint_on("terminated_dialog");
	return;
    }

    if (gdb_asks_yn)
    {
	hint_on("yn_dialog");
	return;
    }

    if (!gdb->isReadyWithPrompt())
    {
	hint_on("busy_dialog");
	return;
    }

    // Typical start-up situations
    if (no_program())
    {
	hint_on("no_program");
	return;
    }

    if (no_source_and_no_code())
    {
	hint_on("no_source_and_no_code");
	return;
    }

    // Examine state
    if (source_view->have_selection())
    {
	hint_on("item_selected");
	return;
    }

    if (data_disp->have_selection())
    {
	hint_on("display_selected");
	return;
    }

    if (current_line() != "")
    {
	hint_on("command_entered");
	return;
    }

    string program_state;
    if (!program_running(program_state))
    {
	hint_on("program_not_running");
	return;
    }

    // Program has stopped and nothing is selected.
    defineConversionMacro("PROGRAM_STATE", program_state);

    if (code_but_no_source())
    {
	hint_on("code_but_no_source");
	return;
    }

    if (gdb->type() == GDB && program_state.contains("signal"))
    {
	int p = passed_to_program(program_state);
	if (p > 0)
	{
	    hint_on("stopped_at_passed_signal");
	    return;
	}
	else if (p == 0)
	{
	    hint_on("stopped_at_ignored_signal");
	    return;
	}
    }

    hint_on("stopped");
}
