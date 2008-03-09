// $Id$ -*- C++ -*-
// TTY command interface

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char cmdtty_rcsid[] = 
    "$Id$";

#include "cmdtty.h"

#include "AppData.h"
#include "GDBAgent.h"
#include "ReadLineA.h"
#include "SourceView.h"
#include "ddd.h"
#include "editing.h"
#include "exit.h"

#include <Xm/Xm.h>
#include <Xm/Text.h>

//-----------------------------------------------------------------------------
// Command TTY
//-----------------------------------------------------------------------------

// All communication with the command TTY passes through this variable
static ReadLineAgent *command_tty = 0;

// true if input comes from command tty
static bool tty_gdb_input;

// TTY input received
static void tty_command(Agent *, void *, void *call_data)
{
    annotate("post-prompt");

    DataLength *d = (DataLength *)call_data;

    // Simply insert text, invoking all necessary callbacks
    tty_gdb_input = true;
    XmTextInsert(gdb_w, XmTextGetLastPosition(gdb_w), (String)d->data);
    tty_gdb_input = false;
}

// TTY EOF received
static void tty_eof(Agent *, void *, void *)
{
    // Forward EOF to GDB (or whatever GDB is just running)
    gdb->send_user_ctrl_cmd("\004");
    if (gdb_input_at_prompt)
	gdb_is_exiting = true;
}

// Echo on TTY
void tty_out(const string& text)
{
    if (tty_gdb_input)
	return;

    _tty_out(text);
}

// Output TEXT on controlling TTY (unconditionally)
void _tty_out(const string& text)
{
    if (command_tty == 0)
	return;

    command_tty->write(text.chars(), text.length());
}

// Output TEXT on controlling TTY if we're in annotation mode
void tty_full_name(const string& pos)
{
    if (command_tty == 0)
	return;

    if (app_data.annotate >= 2)
    {
	const string s = "source " + pos;
	annotate(s.chars());
    }
    else if (app_data.annotate == 1)
    {
	_tty_out("\032\032" + pos + "\n");
    }
    else
    {
	const string line = source_view->get_line(pos);
	if (!line.empty())
	    _tty_out(line + "\n");
    }
}

// Issue an artificial prompt
void prompt()
{
    annotate("pre-prompt");

    bool saved_tty_gdb_input = tty_gdb_input;
    tty_gdb_input = true;

    _gdb_out(gdb->prompt());

    tty_gdb_input = saved_tty_gdb_input;

    if (!tty_gdb_input && command_tty != 0)
	command_tty->prompt(gdb->prompt());

    annotate("prompt");
}

// Initialize command TTY
void init_command_tty()
{
    assert(command_tty == 0);

    command_tty = new ReadLineAgent(XtWidgetToApplicationContext(gdb_w));
    command_tty->addHandler(Input, tty_command);
    command_tty->addHandler(InputEOF, tty_eof);
    command_tty->start();
}

// Close command TTY
void kill_command_tty()
{
    if (command_tty == 0)
	return;

    delete command_tty;
    command_tty = 0;
}

// Check if command tty is still running
bool tty_running()
{
    return command_tty != 0 && command_tty->running();
}

// Annotations
void annotate(const char *text)
{
    if (app_data.annotate < 2)
	return;

    _tty_out("\n\032\032");
    _tty_out(text);
    _tty_out("\n");
}
