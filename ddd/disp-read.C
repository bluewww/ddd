// $Id$
// Helper functions to process GDB display output

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

char disp_read_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Verschiedene Routinen, die das Erkennen bzw. Lesen
// (und Erzeugen) von display-Ausgaben erleichtern.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "assert.h"
#include "disp-read.h"
#include "value-read.h"
#include "string-fun.h"
#include "cook.h"

#include <stdlib.h> // atoi()
#include <stdio.h>  // sprintf()


//----------------------------------------------------------------------------
// fuer die Erkennung bestimmter Befehle
//----------------------------------------------------------------------------

// ***************************************************************************
// false, wenn cmd ein einzelnes display erzeugt
// 
bool is_single_display_cmd (const string& cmd, DebuggerType type)
{
    static regex RXsingle_display_cmd(
	"[ \t]*disp\\(lay?\\|l\\)?[ \t]+[^ ]+",
	true);

    switch (type)
    {
    case GDB:
	return cmd.matches (RXsingle_display_cmd);

    case DBX:
	// DBX has no `display' equivalent
	return false;
    }

    return false;
}

bool is_nop_cmd(const string& cmd)
{
    static regex RXnop_cmd(
	"[ \t]*"
	"\\("
	"echo"
	"\\|print"
	"\\|help"
	"\\|show"
	"\\|info"
	"\\)\\([ \t].*\\)?",
	true);

    return cmd.matches(RXnop_cmd);
}

// ***************************************************************************
// 
bool is_running_cmd (const string& cmd, DebuggerType type)
{
    static regex RXrunning_cmd(
	"[ \t]*"
	"\\(r\\|run?"
	"\\|rerun"
	"\\|c\\|cont\\(inue?\\|in?\\)?"
	"\\|u\\|unt\\(il?\\)?"
	"\\|si?\\|stepi?"
	"\\|ni?\\|nexti?"
	"\\|fin\\(ish?\\|i\\)?"
	"\\)\\([ \t].*\\)?",
	true);


    static regex RXdisplay(
	"[ \t]*disp\\(lay?\\|l\\)?",
	true);

    switch (type)
    {
    case GDB:
	return cmd.matches(RXrunning_cmd)
	    || cmd.matches(RXdisplay)
	    || is_display_cmd(cmd);

    case DBX:
	return cmd.matches (RXrunning_cmd)
	    || is_display_cmd(cmd);
    }

    return false;
}

bool is_run_cmd (const string& cmd)
{
    static regex RXrun_cmd(
	"[ \t]*r\\(er\\)?\\(un?\\)?\\([ \t].*\\)?",
	true);

    return cmd.matches (RXrun_cmd);
}

bool is_set_args_cmd (const string& cmd)
{
    static regex RXset_args_cmd(
	"[ \t]*set[ \t][ \t]*args\\([ \t].*\\)?",
	true);

    return cmd.matches (RXset_args_cmd);
}

// ***************************************************************************
// true, wenn cmd ein display erzeugt
// 
bool is_display_cmd (const string& cmd)
{
    static regex RXdisplay_cmd_and_args(
	"[ \t]*disp\\(lay?\\|l\\)?[ \t]+.*",
	true);

    return cmd.matches (RXdisplay_cmd_and_args);
}

// ***************************************************************************
// 
bool is_frame_cmd (const string& cmd)
{
    static regex RXframe_cmd(
	"[ \t]*"
	"\\(up"
	"\\|do\\(wn?\\)?"
	"\\|f\\(rame?\\|ra?\\)?"
	"\\)\\([ \t].*\\)?",
	true);

    return cmd.matches (RXframe_cmd);
}

bool is_up_cmd (const string& cmd)
{
    static regex RXup_cmd(
	"[ \t]*up\\([ \t].*\\)?",
	true);

    return cmd.matches (RXup_cmd);
}

bool is_down_cmd (const string& cmd)
{
    static regex RXdown_cmd(
	"[ \t]*do\\(wn?\\)?\\([ \t].*\\)?",
	true);

    return cmd.matches (RXdown_cmd);
}

// ***************************************************************************
// 
bool is_set_cmd (const string& cmd)
{
    static regex RXset_cmd(
	"[ \t]*set\\([ \t].*\\)?"
	"[ \t]*assign\\([ \t].*\\)?",
	true);

    return cmd.matches (RXset_cmd);
}

// ***************************************************************************
// 
bool is_file_cmd (const string& cmd, DebuggerType type)
{
    static regex RXfile_cmd(
	"[ \t]*file\\([ \t].*\\)?",
	true);

    static regex RXdebug_cmd(
	"[ \t]*debug\\([ \t].*\\)?",
	true);

    switch (type)
    {
    case GDB:
	return cmd.matches (RXfile_cmd);
    case DBX:
	return cmd.matches (RXdebug_cmd);
    }

    assert(0);
    return false;
}

// ***************************************************************************
// 
bool is_graph_cmd (const string& cmd)
{
    static regex RXgraph_cmd(
	"[ \t]*graph[ \t]+.*",
	true); 

    return cmd.matches(RXgraph_cmd);
}

// ***************************************************************************
// 
bool is_refresh_cmd (const string& cmd)
{

static regex RXrefresh_cmd(
    "[ \t]*refresh",
    true);
    return (cmd.index (RXrefresh_cmd) == 0);
}

// ***************************************************************************
// 
bool is_break_cmd (const string& cmd)
{
    static regex RXbreak_cmd(
	"[ \t]*"
	"\\("
	"[th]*\\(b\\|br\\|bre\\|brea\\|break\\)"
	"\\|cl\\|cle\\|clea\\|clear"
	"\\|info[ \t]+\\(li\\|lin\\|line\\)"
	"\\|stop"
	"\\)",
	true);

    return (cmd.index (RXbreak_cmd) == 0);
}

// ***************************************************************************
// 
bool is_lookup_cmd (const string& cmd)
{
    static regex RXlookup_cmd(
	"[ \t]*func[ \t]+",
	true);

    return (cmd.index (RXlookup_cmd) == 0);
}

// ***************************************************************************
// 
string get_display_expression (const string& display_cmd)
{
    static regex RXdisplay_cmd(
	"[ \t]*disp\\(lay?\\|l\\)?[ \t]+",
	true);

    string d(display_cmd);
    return d.after(RXdisplay_cmd);
}


//----------------------------------------------------------------------------
// fuer das Erkennen von Displayausdruecken
//----------------------------------------------------------------------------

static regex RXgdb_begin_of_display(
    "^[1-9][0-9]*:  *[^ ]",
    true);

static regex RXdbx_begin_of_display(
    "\\(^\\|\n\\)[^ \t\n)}][^=\n]* = ",
    true);

// ***************************************************************************
// -1, wenn gdb_answer kein display enthaelt, 
// sonst den index des ersten displays.
// 
static int _display_index (const string& gdb_answer, DebuggerType type)
{
    switch (type)
    {
    case GDB:
	return gdb_answer.index (RXgdb_begin_of_display);

    case DBX:
	return gdb_answer.index (RXdbx_begin_of_display);
    }
    return -1;
}

int display_index(const string& gdb_answer, DebuggerType type)
{
    int index = _display_index(gdb_answer, type);
    if (index >= 0 && gdb_answer[index] == '\n')
	index++;

    // clog << "display_index(" << quote(gdb_answer) << ") = " 
    //      << index << "\n";
    return index;
}

// ***************************************************************************
// 
int contains_display (const string& gdb_answer, DebuggerType type)
{
    int ret = -1;
    switch (type)
    {
    case GDB:
	ret = gdb_answer.index (RXgdb_begin_of_display) != -1;
	break;

    case DBX:
	ret = gdb_answer.index (RXdbx_begin_of_display) != -1;
	break;
    }

    // clog << "contains_display(" << quote(gdb_answer) << ") = " 
    //      << ret << "\n";
    return ret;
}

// ***************************************************************************
// gibt index zurueck, an dem ein Display anfangen koennte (d.h. index eines
// moeglichen Display-Anfangs
// 
static int _possible_begin_of_display (string gdb_answer, DebuggerType type)
{
    const regex& RXbegin_of_display =
	type == GDB ? RXgdb_begin_of_display : RXdbx_begin_of_display;

    if (contains_display (gdb_answer, type))
	return gdb_answer.index (RXbegin_of_display);
    gdb_answer += "a";
    if (contains_display (gdb_answer, type))
	return gdb_answer.index (RXbegin_of_display);
    gdb_answer.at ("a", -1) = " a";
    if (contains_display (gdb_answer, type))
	return gdb_answer.index (RXbegin_of_display);
    gdb_answer.at (" a", -1) = ": a";
    if (contains_display (gdb_answer, type))
	return gdb_answer.index (RXbegin_of_display);

    return -1;
}

int possible_begin_of_display(string gdb_answer, DebuggerType type)
{
    int index = _possible_begin_of_display(gdb_answer, type);
    if (index >= 0 && gdb_answer[index] == '\n')
	index++;

    // clog << "possible_begin_of_display(" << quote(gdb_answer) 
    //      << ") = " << index << "\n";

    return index;
}

// ***************************************************************************
// gibt den naechsten Display zurueck falls vorhanden, und
// schneidet diesen von displays vorne ab.
// 
string read_next_display (string& displays, DebuggerType)
{
    string next_display;

    // string old_displays = displays;
    // clog << "read_next_display(" << quote(old_displays) << ")...\n";

    do {
	next_display += read_token(displays);
    } while (displays != "" && displays[0] != '\n');
    displays = displays.after(0);

    // clog << "read_next_display(" << quote(old_displays) << ") = "
    //      << quote(next_display) << "\n";
    return next_display;
}

// ***************************************************************************
// schneidet vom display "'nr': 'name' = " vorne ab.
// 
string get_disp_value_str (/*const*/ string& display, DebuggerType)
{
    return display.after (" = ");
}

//----------------------------------------------------------------------------
// fuer das Erkennen der Ausdruecke bei info display
//----------------------------------------------------------------------------

static regex RXgdb_begin_of_display_info(
    "^[1-9][0-9]*:   ",
    true);

static regex RXdbx_begin_of_display_info(
    "^([1-9][0-9]*) ",
    true);

// ***************************************************************************
// gibt den ersten Display-Info aus 
// Ist kein weiteres display-info vorhanden, sind return-Wert und gdb_answer
// gleich "".
// 
string read_first_disp_info (string& gdb_answer, DebuggerType type)
{
    int i = 0;

    switch(type)
    {
    case GDB:
	i = gdb_answer.index (RXgdb_begin_of_display_info);
	break;

    case DBX:
	i = gdb_answer.index (RXdbx_begin_of_display_info);
	break;
    }

    if (i > 0) {
	gdb_answer = gdb_answer.from (i);
	return read_next_disp_info (gdb_answer, type);
    }
    gdb_answer = "";
    return "";
}


// ***************************************************************************
// gibt den naechsten Display-Info aus 
// Ist kein weiteres display-info vorhanden, sind return-Wert und gdb_answer
// gleich "".
// 
string read_next_disp_info (string& gdb_answer, DebuggerType type)
{
    switch (type)
    {
    case GDB:
    {
	string next_disp_info;
	int startpos = gdb_answer.index (": ");
	int i = gdb_answer.index (RXgdb_begin_of_display_info, startpos + 2);
	if (i > 0) {
	    next_disp_info = gdb_answer.before (i);
	    gdb_answer = gdb_answer.from (i);
	}
	else {
	    next_disp_info = gdb_answer;
	    gdb_answer = "";
	}
	return next_disp_info;
    }
    break;

    case DBX:
    {
	string next_disp_info;
	int i = gdb_answer.index('\n');
	if (i > 0)
	{
	    next_disp_info = gdb_answer.before(i);
	    gdb_answer = gdb_answer.after(i);
	}
	else
	{
	    next_disp_info = gdb_answer;
	    gdb_answer = "";
	}
	return next_disp_info;
    }
    break;
    }

    return "";
}

// ***************************************************************************
// schneidet "'nr': " vorne ab
//
string get_info_disp_str (string& display_info, DebuggerType type)
{
    switch (type)
    {
    case GDB:
	return display_info.after (RXgdb_begin_of_display_info);

    case DBX:
	return display_info.after (") ");
    }

    return "";
}

// ***************************************************************************
//
int disp_is_disabled (const string& info_disp_str, DebuggerType type)
{
    static regex RXy_or_n("[yn]", true);

    switch (type)
    {
    case GDB:
	assert ((info_disp_str.index (RXy_or_n) == 0));
	return (info_disp_str.index ("n") == 0);

    case DBX:
	return false;		// no display disabling in dbx
    }

    return false;
}



//----------------------------------------------------------------------------
// fuer Knotenerzeugung (Lesen des Display-Anfangs)
//----------------------------------------------------------------------------

// ***************************************************************************
//
string  read_disp_nr_str (string& display, DebuggerType type)
{
    switch (type)
    {
    case GDB:
    {
	static regex RXgdb_disp_nr(
	    "[1-9][0-9]*",
	    true);

	string disp_nr = display.through (RXgdb_disp_nr);
	display = display.after (": ");
	return disp_nr;
    }
    case DBX:
	return read_disp_name(display, type);
    }

    return "";
}

// ***************************************************************************
//
string  read_disp_name   (string& display, DebuggerType)
{
    string name = display.before (" = ");
    display = display.after (" = ");
    return name;
}

// ***************************************************************************
//
bool is_disabling(const string& value, DebuggerType type)
{
    return type == GDB && value.contains("\nDisabling display ");
}

bool is_not_active(const string& value, DebuggerType type)
{
    return type == DBX 
	&& (value.contains(" is not active\n", -1)
	    || value.contains(" is not active", -1)
	    || value.contains("<not active>\n", -1)
	    || value.contains("<not active>", -1));
}
