// $Id$
// Helper functions to process GDB display output

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char disp_read_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Misc routines to process `display' output
//-----------------------------------------------------------------------------

#include "assert.h"
#include "disp-read.h"
#include "value-read.h"
#include "string-fun.h"
#include "cook.h"
#include "regexps.h"

#include <stdlib.h>		// atoi()
#include <stdio.h>		// sprintf()
#include <ctype.h>		// isspace()


//----------------------------------------------------------------------------
// Recognize specific debugger commands
//----------------------------------------------------------------------------

// True if CMD creates one signle display
bool is_single_display_cmd (const string& cmd, GDBAgent *gdb)
{
#if RUNTIME_REGEX
    static regex rxsingle_display_cmd(
        "[ \t]*"
	"(disp|displ|displa|display)[ \t]+[^ ]+");
#endif

    switch (gdb->type())
    {
    case GDB:
	return cmd.matches (rxsingle_display_cmd);

    case DBX:
    case XDB:
    case JDB:
	return false;
    }

    return false;
}

// True if CMD has no effect on DDD state
bool is_nop_cmd(const string& cmd)
{
#if RUNTIME_REGEX
    // All these command have no effect on DDD state...
    static regex rxnop_cmd("[ \t]*(echo|help|show|info|where|shell|sh)([ \t]+.*)?");

    // ... except for this one.
    static regex rxop_cmd("[ \t]*(info[ \t]+line)([ \t]+.*)?");
#endif

    return cmd.matches(rxnop_cmd) && !cmd.matches(rxop_cmd);
}

// True if CMD changes program state
bool is_running_cmd (const string& cmd, GDBAgent *gdb)
{
#if RUNTIME_REGEX
    static regex rxrunning_cmd(
	"[ \t]*"
	"(r|ru|run"
	"|rer|rerun"
	"|c|cont|contin|continu|continue"
	"|u|unt|unti|until"
	"|s|si|step|stepi"
	"|n|ni|next|nexti"
	"|j|ju|jump"
	"|k|ki|kill"
	"|fin|fini|finis|finish"
        "|R|S"
	")([ \t]+.*)?");

    static regex rxdisplay("[ \t]*(disp|displ|displa|display)([ \t]+.*)?");
#endif

    switch (gdb->type())
    {
    case GDB:
	return cmd.matches(rxrunning_cmd)
	    || cmd.matches(rxdisplay)
	    || is_display_cmd(cmd);

    case DBX:
    case XDB:
    case JDB:
	return cmd.matches (rxrunning_cmd)
	    || is_display_cmd(cmd);
    }

    return false;
}

// True if CMD starts program
bool is_run_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxrun_cmd("[ \t]*(r|rer|rerun|ru|run|R)([ \t]+.*)?");
#endif

    return cmd.matches (rxrun_cmd);
}

// True if CMD sets program args
bool is_set_args_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxset_args_cmd("[ \t]*set[ \t]+args([ \t]+.*)?");
#endif

    return cmd.matches (rxset_args_cmd);
}

// True if CMD affects the program counter
bool is_pc_cmd(const string& cmd)
{
    return cmd.contains("$pc");
}

// True if CMD creates at least one display
bool is_display_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxdisplay_cmd_and_args(
	"[ \t]*(disp|displ|displa|display)[ \t]+.*");
#endif

    return cmd.matches (rxdisplay_cmd_and_args);
}

// True if CMD changes the list of displays
bool is_data_cmd (const string& cmd)
{
    // enable display, disable display, delete display, undisplay
#if RUNTIME_REGEX
    static regex rxdata("[ \t]*[a-z ]*display([ \t]+-?[0-9][0-9]*.*)?");
#endif

    return cmd.matches(rxdata);
}

// True if CMD deletes a display
bool is_delete_display_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxundisplay("[ \t]*(delete[ \t]+|un)display([ \t]+.*)?");
#endif

    return cmd.matches(rxundisplay);
}

// True if CMD enables a display
bool is_enable_display_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxenable("[ \t]*enable[ \t]+display([ \t]+.*)?");
#endif

    return cmd.matches(rxenable);
}

// True if CMD disables a display
bool is_disable_display_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxdisable("[ \t]*disable[ \t]+display([ \t]+.*)?");
#endif

    return cmd.matches(rxdisable);
}

// True if CMD changes current frame
bool is_frame_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxframe_cmd(
	"[ \t]*"
	"(up"
	"|do|down"
	"|f|fra|fram|frame"
	"|suspend"
	"|resume"
	"|top|V"
	")([ \t]+.*)?");
#endif

    return cmd.matches (rxframe_cmd);
}

// True if CMD moves up in stack
bool is_up_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxup_cmd("[ \t]*up([ \t]+.*)?");
#endif

    return cmd.matches (rxup_cmd);
}

// True if CMD moves down in stack
bool is_down_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxdown_cmd("[ \t]*(do|down)([ \t]+.*)?");
#endif

    return cmd.matches (rxdown_cmd);
}

// True if CMD loads a new state (core file or process)
bool is_core_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxcore_cmd("[ \t]*(attach|core|core-file)([ \t]+.*)?");
#endif

    return cmd.matches (rxcore_cmd);
}

// True if CMD changes the current thread
bool is_thread_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxthread_cmd("[ \t]*thread([ \t]+.*)?");
#endif

    return cmd.matches (rxthread_cmd);
}

// True if CMD changes variables
bool is_set_cmd (const string& cmd, GDBAgent *gdb)
{
#if RUNTIME_REGEX
    static regex rxset1_cmd("[ \t]*(set[ \t]+var[a-z]*|assign|pq)([ \t]+.*)?");
    static regex rxset2_cmd("[ \t]*(set|p|print|output)[ \t]+[^=]+=[^=].*");
#endif

    return cmd.matches(rxset1_cmd) || 
	(gdb->type() == GDB && cmd.matches(rxset2_cmd));
}

// True if CMD changes debugger settings
bool is_setting_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxsetting_cmd("[ \t]*(set|dbxenv)[ \t]+.*");
    static regex rxpath_cmd("[ \t]*(dir|directory|path)([ \t]+.*)?");
#endif

    if (is_set_args_cmd(cmd))
	return false;		// `set args' command

    return cmd.matches (rxsetting_cmd) || cmd.matches(rxpath_cmd);
}

// True if CMD changes debuggee
bool is_file_cmd (const string& cmd, GDBAgent *gdb)
{
    if (cmd == "# reset")
	return true;

    switch (gdb->type())
    {
    case GDB:
    {
#if RUNTIME_REGEX
	static regex rxfile_cmd("[ \t]*file([ \t]+.*)?");
#endif
	return cmd.matches (rxfile_cmd);
    }

    case DBX:
    {
#if RUNTIME_REGEX
	static regex rxdebug_cmd("[ \t]*(debug|givenfile)([ \t]+.*)?");
#endif
	return cmd.matches (rxdebug_cmd);
    }

    case XDB:
    case JDB:
	return false;		// FIXME
    }

    assert(0);
    return false;
}

// True if CMD is a DDD graph command
bool is_graph_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxgraph_cmd("[ \t]*graph[ \t]+.*"); 
#endif

    return cmd.matches(rxgraph_cmd);
}

// True if CMD is a refresh command
bool is_refresh_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxrefresh_cmd("[ \t]*refresh([ \t]+.*)?");
#endif

    return cmd.matches(rxrefresh_cmd);
}

// True if CMD affects breakpoints
bool is_break_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxbreak_cmd(
	"[ \t]*"
	"("
	"[th]*(b|br|bre|brea|break|b[a-z])"
	"|cl|cle|clea|clear|d[a-z]"
	"|info[ \t]+(li|lin|line)"
	"|stop"
	")([ \t]+.*)?");
#endif

    return cmd.matches(rxbreak_cmd);
}

// True if CMD changes current cursor position
bool is_lookup_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxlookup_cmd("[ \t]*(func|v)[ \t]+.*");
#endif

    return cmd.matches(rxlookup_cmd);
}

// True if CMD changes directory
bool is_cd_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxcd_cmd("[ \t]*cd([ \t]+.*)?");
#endif

    return cmd.matches(rxcd_cmd);
}

// True if CMD changes class path
bool is_use_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxuse_cmd("[ \t]*use[ \t]+.*");
#endif

    return cmd.matches(rxuse_cmd);
}

// True if CMD invokes `make'
bool is_make_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxmake_cmd("[ \t]*(sh[ \t]+|!)?make([ \t]+.*)?");
#endif

    return cmd.matches(rxmake_cmd);
}

// True if CMD quits GDB
bool is_quit_cmd (const string& cmd)
{
    return cmd.contains('q', 0); // `quit', `q', or whatever
}

// Fetch display expression from DISPLAY_CMD
string get_display_expression (const string& display_cmd)
{
#if RUNTIME_REGEX
    static regex rxdisplay_cmd("[ \t]*(disp|displ|displa|display)[ \t]+");
#endif

    string d(display_cmd);
    return d.after(rxdisplay_cmd);
}


// Fetch breakpoint expression from CMD
string get_break_expression (const string& cmd)
{
    string arg = cmd;

    if (arg.contains("stop", 0))
	arg = arg.after(' ');	    // Skip `in' or `at'

    // Fetch first word after command
    arg = arg.after(' ');
    arg += ' ';

    if (arg.contains('\'', 0))
	arg = arg.through('\'', 1); // Handle GDB quoting
    else
	arg = arg.before(' ');

    return arg;
}


//----------------------------------------------------------------------------
// Handle `display' output
//----------------------------------------------------------------------------

// Return index of first display in GDB_ANSWER; -1 if none
int display_index (const string& gdb_answer, GDBAgent *gdb)
{
#if RUNTIME_REGEX
    static regex rxgdb_begin_of_display("[1-9][0-9]*:  *[^ ]");
    static regex rxdbx_begin_of_display("[^ \t\n)}][^=\n]* = ");
#endif

    const regex *prx = 0;

    switch (gdb->type())
    {
    case GDB: 
	prx = &rxgdb_begin_of_display;
	break;

    case DBX:
    case XDB:
    case JDB:
	prx = &rxdbx_begin_of_display;
	break;

    default:
	assert(0);
	abort();
    }

    const regex& rx = *prx;

    for (unsigned i = 0; i < gdb_answer.length(); i++)
	if (i == 0 || gdb_answer[i - 1] == '\n')
	    if (gdb_answer.contains(rx, i))
		return i;

    return -1;
}

// True if GDB_ANSWER contains some display
bool contains_display (const string& gdb_answer, GDBAgent *gdb)
{
    return display_index(gdb_answer, gdb) >= 0;
}

// Return index of possible display start; -1 if none
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


// True if next non-space character in DISPLAYS is a newline
static bool next_is_nl(const string& displays)
{
    unsigned int i = 0;
    while (i < displays.length() && displays[i] == ' ')
	i++;
    return i < displays.length() && displays[i] == '\n';
}

// Return next display from DISPLAYS ("" if none); remove it from
// DISPLAYS
string read_next_display (string& displays, GDBAgent *gdb)
{
    string next_display;

    // string old_displays = displays;
    // clog << "read_next_display(" << quote(old_displays) << ")...\n";

    if (is_disabling(displays, gdb))
    {
	// After a `disabling' message, `display' output finishes
	next_display = displays;
	displays = "";
    }
    else
    {
	do {
	    next_display += read_token(displays);
	} while (displays != "" && !next_is_nl(displays));
    }
    displays = displays.after('\n');

    // clog << "read_next_display(" << quote(old_displays) << ") = "
    //      << quote(next_display) << "\n";
    return next_display;
}

// Return "NR: NAME = " from DISPLAY
string get_disp_value_str (const string& display, GDBAgent *)
{
    string d(display);

    // For some types, XDB issues `NAME = VALUE', for others, `VALUE'.
    // DDD compensates for this by having XDB prepend `NAME = '.
    // In case we have `NAME = NAME = VALUE', strip first `NAME = '.
#if RUNTIME_REGEX
    static regex rxeqeq("[^{};,\n= ]+ = [^{}();,\n= ]+ = .*");
#endif
    if (d.matches(rxeqeq))
	d = d.after(" = ");

    int nl_index = d.index('\n');
    int eq_index = d.index(" = ");
    if (eq_index >= 0 && (eq_index < nl_index || nl_index < 0))
	return d.after(" = ");

    return d;
}



//----------------------------------------------------------------------------
// Handle `info display' output
//----------------------------------------------------------------------------

#if RUNTIME_REGEX
static regex rxgdb_begin_of_display_info("[1-9][0-9]*:   ");
static regex rxdbx_begin_of_display_info("[(][1-9][0-9]*[)] ");
#endif

// Return index of first display in GDB_ANSWER; -1 if none
int display_info_index (const string& gdb_answer, GDBAgent *gdb)
{
    const regex *prx = 0;

    switch (gdb->type())
    {
    case GDB: 
	prx = &rxgdb_begin_of_display_info;
	break;
    case DBX:
	prx = &rxdbx_begin_of_display_info;
	break;
    case JDB:
	return -1;
	break;

    default:
	assert(0);
	abort();
    }

    const regex& rx = *prx;

    for (unsigned i = 0; i < gdb_answer.length(); i++)
	if (i == 0 || gdb_answer[i - 1] == '\n')
	    if (gdb_answer.contains(rx, i))
		return i;

    return -1;
}

// Return first display info from GDB_ANSWER; "" if none.
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


// Return next display info from GDB_ANSWER; "" if none.
string read_next_disp_info (string& gdb_answer, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
    {
	int startpos = gdb_answer.index (": ");
	int i = startpos + 2;

	for (;;)
	{
	    while (i < int(gdb_answer.length()) && gdb_answer[i] != '\n')
		i++;
	    if (i >= int(gdb_answer.length()))
	    {
		// Take entire remaining output as display
		string next_disp_info = gdb_answer;
		gdb_answer = "";
		return next_disp_info;
	    }

	    assert(gdb_answer[i] == '\n');
	    if (gdb_answer.contains(rxgdb_begin_of_display_info, i + 1))
	    {
		// Use output up to `\n[0-9]' as display
		string next_disp_info = gdb_answer.before(i);
		gdb_answer = gdb_answer.after(i);
		return next_disp_info;
	    }

	    i++;
	}
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
    case JDB:
	break;			// FIXME
    }

    return "";
}

// Remove and return "NR: " from DISPLAY.
string get_info_disp_str (string& display_info, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
	return display_info.after (":   ");

    case DBX:
	return display_info.after (") ");

    case XDB:
    case JDB:
	return "";		// FIXME
    }

    return "";
}

// Check whether `disabled' entry in INFO_DISP_STR indicates an enabled display
bool disp_is_disabled (const string& info_disp_str, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
	return info_disp_str.length() > 0 && info_disp_str[0] == 'n';

    case DBX:
	return false;		// no display disabling in dbx

    case XDB:
    case JDB:
	return false;		// FIXME
    }

    return false;
}



//----------------------------------------------------------------------------
// Process `display' output
//----------------------------------------------------------------------------

// Remove and return display number from DISPLAY
string  read_disp_nr_str (string& display, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
    {
#if RUNTIME_REGEX
	static regex rxgdb_disp_nr("[1-9][0-9]*");
#endif

	string disp_nr = display.through (rxgdb_disp_nr);
	display = display.after (": ");
	return disp_nr;
    }
    case DBX:
	return read_disp_name(display, gdb);

    case XDB:
    case JDB:
	return "";		// FIXME
    }

    return "";
}

// Remove and return display name from DISPLAY
string read_disp_name (string& display, GDBAgent *gdb)
{
    read_leading_blanks(display);
    string name = display.before (" = ");
    display = get_disp_value_str(display, gdb);
    return name;
}

// True if some display has been disabled
bool is_disabling(const string& value, GDBAgent *gdb)
{
    return gdb->type() == GDB && value.contains("\nDisabling display ");
}

// True if VALUE is an invalid value (i.e., an error message)
bool is_invalid(const string& value)
{
    // If VALUE ends in two words, it is an error message like `not
    // active' or `no symbol in current context.'.  XDB issues
    // `Unknown name "Foo" (UE369)' and `Local is not active (UE421)'.
#if RUNTIME_REGEX
    static regex rxinvalid_value("("
				 "[a-zA-Z]+ [a-zA-Z]+.*"
				 "|.*[a-zA-Z]+ [a-zA-Z]+(\\.|>)?"
				 ")\n?");
#endif

    return value.contains("Unknown name") 
	|| value.contains("not active")
	|| value.matches(rxinvalid_value);
}


//-----------------------------------------------------------------------------
// Handle `where' output
//-----------------------------------------------------------------------------

// Fetch current scope from GDB `where' output (a function name)
string get_scope(const string& where_output)
{
    // The word before the first parenthesis is the current function.
    int index = where_output.index('(');
    if (index < 0)
	return "";		// no current scope

    do {
	index--;
    } while (index >= 0 && isspace(where_output[index]));

    int end_of_name = index + 1;

    do {
	index--;
    } while (index >= 0 && !isspace(where_output[index]));

    int start_of_name = index + 1;

    return ((string &)where_output).at(start_of_name, 
				       end_of_name - start_of_name);
}
