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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
bool is_single_display_cmd (const string& cmd, GDBAgent *gdb)
{
    static regex RXsingle_display_cmd(
        "[ \t]*"
	"(disp|displ|displa|display)[ \t]+[^ ]+");

    switch (gdb->type())
    {
    case GDB:
	return cmd.matches (RXsingle_display_cmd);

    case DBX:
	// DBX has no `display' equivalent
	return false;

    case XDB:
	return false;		// FIXME
    }

    return false;
}

bool is_nop_cmd(const string& cmd)
{
    static regex RXnop_cmd("[ \t]*(echo|print|help|show|info)([ \t]+.*)?");

    return cmd.matches(RXnop_cmd);
}

// ***************************************************************************
// 
bool is_running_cmd (const string& cmd, GDBAgent *gdb)
{
    static regex RXrunning_cmd(
	"[ \t]*"
	"(r|ru|run"
	"|rer|rerun"
	"|c|cont|contin|continu|continue"
	"|u|unt|unti|until"
	"|s|si|step|stepi"
	"|n|ni|next|nexti"
	"|fin|fini|finis|finish"
        "|R|S"
	")([ \t]+.*)?");

    static regex RXdisplay(
	"[ \t]*(disp|displ|displa|display)([ \t]+.*)?");

    switch (gdb->type())
    {
    case GDB:
	return cmd.matches(RXrunning_cmd)
	    || cmd.matches(RXdisplay)
	    || is_display_cmd(cmd);

    case DBX:
    case XDB:
	return cmd.matches (RXrunning_cmd)
	    || is_display_cmd(cmd);
    }

    return false;
}

bool is_run_cmd (const string& cmd)
{
    static regex RXrun_cmd(
	"[ \t]*(r|rer|rerun|ru|run|R)([ \t]+.*)?");

    return cmd.matches (RXrun_cmd);
}

bool is_set_args_cmd (const string& cmd)
{
    static regex RXset_args_cmd(
	"[ \t]*set[ \t]+args([ \t]+.*)?");

    return cmd.matches (RXset_args_cmd);
}

// ***************************************************************************
// true, wenn cmd ein display erzeugt
// 
bool is_display_cmd (const string& cmd)
{
    static regex RXdisplay_cmd_and_args(
	"[ \t]*(disp|displ|displa|display)[ \t]+.*");

    return cmd.matches (RXdisplay_cmd_and_args);
}

// ***************************************************************************
// 
bool is_frame_cmd (const string& cmd)
{
    static regex RXframe_cmd(
	"[ \t]*"
	"(up"
	"|do|down"
	"|f|fra|fram|frame"
	"|top|V"
	")([ \t]+.*)?");

    return cmd.matches (RXframe_cmd);
}

bool is_up_cmd (const string& cmd)
{
    static regex RXup_cmd("[ \t]*up([ \t]+.*)?");

    return cmd.matches (RXup_cmd);
}

bool is_down_cmd (const string& cmd)
{
    static regex RXdown_cmd("[ \t]*(do|down)([ \t]+.*)?");

    return cmd.matches (RXdown_cmd);
}

// ***************************************************************************
// 
bool is_set_cmd (const string& cmd)
{
    static regex RXset_cmd("[ \t]*(set|assign|pq)([ \t]+.*)?");

    return cmd.matches (RXset_cmd);
}

// ***************************************************************************
// 
bool is_setting_cmd (const string& cmd)
{
    static regex RXsetting_cmd("[ \t]*(set|dbxenv)[ \t]+.*");

    return !cmd.contains('=') && cmd.matches (RXsetting_cmd);
}

// ***************************************************************************
// 
bool is_file_cmd (const string& cmd, GDBAgent *gdb)
{
    static regex RXfile_cmd("[ \t]*file([ \t]+.*)?");

    static regex RXdebug_cmd("[ \t]*debug([ \t]+.*)?");

    switch (gdb->type())
    {
    case GDB:
	return cmd.matches (RXfile_cmd);
    case DBX:
	return cmd.matches (RXdebug_cmd);
    case XDB:
	return false;		// FIXME
    }

    assert(0);
    return false;
}

// ***************************************************************************
// 
bool is_graph_cmd (const string& cmd)
{
    static regex RXgraph_cmd("[ \t]*graph[ \t]+.*"); 

    return cmd.matches(RXgraph_cmd);
}

// ***************************************************************************
// 
bool is_refresh_cmd (const string& cmd)
{
    static regex RXrefresh_cmd("[ \t]*refresh([ \t]+.*)?");

    return cmd.matches(RXrefresh_cmd);
}

// ***************************************************************************
// 
bool is_break_cmd (const string& cmd)
{
    static regex RXbreak_cmd(
	"[ \t]*"
	"("
	"[th]*(b|br|bre|brea|break|b[a-z])"
	"|cl|cle|clea|clear|d[a-z]"
	"|info[ \t]+(li|lin|line)"
	"|stop"
	")([ \t]+.*)?");

    return cmd.matches(RXbreak_cmd);
}

// ***************************************************************************
// 
bool is_lookup_cmd (const string& cmd)
{
    static regex RXlookup_cmd("[ \t]*(func|v)[ \t]+.*");

    return cmd.matches(RXlookup_cmd);
}

// ***************************************************************************
// 
bool is_cd_cmd (const string& cmd)
{
    static regex RXcd_cmd("[ \t]*cd([ \t]+.*)?");

    return cmd.matches(RXcd_cmd);
}

// ***************************************************************************
// 
string get_display_expression (const string& display_cmd)
{
    static regex RXdisplay_cmd("[ \t]*(disp|displ|displa|display)[ \t]+");

    string d(display_cmd);
    return d.after(RXdisplay_cmd);
}


//----------------------------------------------------------------------------
// fuer das Erkennen von Displayausdruecken
//----------------------------------------------------------------------------

// ***************************************************************************
// -1, wenn gdb_answer kein display enthaelt, 
// sonst den index des ersten displays.
// 
int display_index (const string& gdb_answer, GDBAgent *gdb)
{
    static regex RXgdb_begin_of_display("[1-9][0-9]*:  *[^ ]");
    static regex RXdbx_begin_of_display("[^ \t\n)}][^=\n]* = ");

    regex *prx = 0;

    switch (gdb->type())
    {
    case GDB: 
	prx = &RXgdb_begin_of_display;
	break;
    case DBX:
	prx = &RXdbx_begin_of_display;
	break;
    case XDB:
	prx = &RXdbx_begin_of_display;
	break;

    default:
	assert(0);
	abort();
    }

    regex& rx = *prx;

    for (unsigned i = 0; i < gdb_answer.length(); i++)
	if (i == 0 || gdb_answer[i - 1] == '\n')
	    if (gdb_answer.contains(rx, i))
		return i;

    return -1;
}

// ***************************************************************************
// 
bool contains_display (const string& gdb_answer, GDBAgent *gdb)
{
    return display_index(gdb_answer, gdb) >= 0;
}

// ***************************************************************************
// gibt index zurueck, an dem ein Display anfangen koennte (d.h. index eines
// moeglichen Display-Anfangs
// 
int possible_begin_of_display (string gdb_answer, GDBAgent *gdb)
{
    int index = -1;

    if (index == -1)
	index = display_index(gdb_answer, gdb);

    if (index == -1)
    {
	gdb_answer += "a";
	index = display_index(gdb_answer, gdb);
    }

    if (index == -1)
    {
	gdb_answer.at ("a", -1) = " a";
	index = display_index(gdb_answer, gdb);
    }

    if (index == -1)
    {
	gdb_answer.at (" a", -1) = ": a";
	index = display_index(gdb_answer, gdb);
    }

    return index;
}


static bool next_is_nl(string& displays)
{
    unsigned int i = 0;
    while (i < displays.length() && displays[i] == ' ')
	i++;
    return i < displays.length() && displays[i] == '\n';
}

// ***************************************************************************
// gibt den naechsten Display zurueck falls vorhanden, und
// schneidet diesen von displays vorne ab.
// 
string read_next_display (string& displays, GDBAgent *)
{
    string next_display;

    // string old_displays = displays;
    // clog << "read_next_display(" << quote(old_displays) << ")...\n";

    do {
	next_display += read_token(displays);
    } while (displays != "" && !next_is_nl(displays));

    displays = displays.after('\n');

    // clog << "read_next_display(" << quote(old_displays) << ") = "
    //      << quote(next_display) << "\n";
    return next_display;
}

// ***************************************************************************
// schneidet vom display "'nr': 'name' = " vorne ab.
// 
string get_disp_value_str (/*const*/ string& display, GDBAgent *)
{
    string d(display);

    // For some types, XDB issues `NAME = VALUE', for others, `VALUE'.
    // DDD compensates for this by having XDB prepend `NAME = '.
    // In case we have `NAME = NAME = VALUE', strip first `NAME = '.
    static regex RXeqeq("[^{};,\n= ]+ = [^{}();,\n= ]+ = .*");
    if (d.matches(RXeqeq))
	d = d.after(" = ");
    else if (d.contains(" = "))
	return d.after (" = ");

    return d;
}

//----------------------------------------------------------------------------
// fuer das Erkennen der Ausdruecke bei info display
//----------------------------------------------------------------------------

// ***************************************************************************
// -1, wenn gdb_answer kein display enthaelt, 
// sonst den index des ersten displays.
// 
int display_info_index (const string& gdb_answer, GDBAgent *gdb)
{
    static regex RXgdb_begin_of_display_info("[1-9][0-9]*:   ");
    static regex RXdbx_begin_of_display_info("[(][1-9][0-9]*[)] ");

    regex *prx = 0;

    switch (gdb->type())
    {
    case GDB: 
	prx = &RXgdb_begin_of_display_info;
	break;
    case DBX:
	prx = &RXdbx_begin_of_display_info;
	break;

    default:
	assert(0);
	abort();
    }

    regex& rx = *prx;

    for (unsigned i = 0; i < gdb_answer.length(); i++)
	if (i == 0 || gdb_answer[i - 1] == '\n')
	    if (gdb_answer.contains(rx, i))
		return i;

    return -1;
}

// ***************************************************************************
// gibt den ersten Display-Info aus 
// Ist kein weiteres display-info vorhanden, sind return-Wert und gdb_answer
// gleich "".
// 
string read_first_disp_info (string& gdb_answer, GDBAgent *gdb)
{
    int i = display_info_index(gdb_answer, gdb);
    if (i > 0) {
	gdb_answer = gdb_answer.from (i);
	return read_next_disp_info (gdb_answer, gdb);
    }
    gdb_answer = "";
    return "";
}


// ***************************************************************************
// gibt den naechsten Display-Info aus 
// Ist kein weiteres display-info vorhanden, sind return-Wert und gdb_answer
// gleich "".
// 
string read_next_disp_info (string& gdb_answer, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
    {
	static regex RXgdb_next_display_info("\n[1-9][0-9]*:   ");

	string next_disp_info;
	int startpos = gdb_answer.index (": ");
	int i = gdb_answer.index (RXgdb_next_display_info, startpos + 2);
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

    case XDB:
	break;			// FIXME
    }

    return "";
}

// ***************************************************************************
// schneidet "'nr': " vorne ab
//
string get_info_disp_str (string& display_info, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
	return display_info.after (":   ");

    case DBX:
	return display_info.after (") ");

    case XDB:
	return "";		// FIXME
    }

    return "";
}

// ***************************************************************************
//
bool disp_is_disabled (const string& info_disp_str, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
	return info_disp_str.length() > 0 && info_disp_str[0] == 'n';

    case DBX:
	return false;		// no display disabling in dbx

    case XDB:
	return false;		// FIXME
    }

    return false;
}



//----------------------------------------------------------------------------
// fuer Knotenerzeugung (Lesen des Display-Anfangs)
//----------------------------------------------------------------------------

// ***************************************************************************
//
string  read_disp_nr_str (string& display, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
    {
	static regex RXgdb_disp_nr("[1-9][0-9]*");

	string disp_nr = display.through (RXgdb_disp_nr);
	display = display.after (": ");
	return disp_nr;
    }
    case DBX:
	return read_disp_name(display, gdb);

    case XDB:
	return "";		// FIXME
    }

    return "";
}

// ***************************************************************************
// Return display name from DISPLAY; leave display value in DISPLAY
string read_disp_name (string& display, GDBAgent *gdb)
{
    string name = display.before (" = ");
    display = get_disp_value_str(display, gdb);
    return name;
}

// ***************************************************************************
//
bool is_disabling(const string& value, GDBAgent *gdb)
{
    return gdb->type() == GDB && value.contains("\nDisabling display ");
}

bool is_not_active(const string& value, GDBAgent *gdb)
{
    return gdb->type() == DBX 
	&& (value.contains(" is not active\n", -1)
	    || value.contains(" is not active", -1)
	    || value.contains("<not active>\n", -1)
	    || value.contains("<not active>", -1));
}
