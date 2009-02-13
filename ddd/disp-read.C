// $Id$
// Helper functions to process GDB display output

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
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

char disp_read_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
// Misc routines to process `display' output
//-----------------------------------------------------------------------------

#include "assert.h"
#include "disp-read.h"
#include "value-read.h"
#include "string-fun.h"
#include "cook.h"
#include "regexps.h"
#include "AppData.h"

#include <stdlib.h>		// atoi()
#include <stdio.h>		// sprintf()
#include <ctype.h>		// isspace()


//----------------------------------------------------------------------------
// Recognize specific debugger commands
//----------------------------------------------------------------------------

// True if CMD creates one single display
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

    case BASH:
    case DBG:
    case DBX:
    case JDB:
    case MAKE:
    case PERL:
    case PYDB:
    case XDB:
	return false;
    }

    return false;
}

// True if CMD has no effect on DDD state
bool is_nop_cmd(const string& cmd)
{
    if (app_data.play_log != 0)
    {
	if (cmd.contains('/', 0) ||
	    cmd.contains('?', 0) ||
	    cmd.contains('.', 0) ||
	    cmd.contains('!', 0))
	    return true;
    }

#if RUNTIME_REGEX
    // All these command have no effect on DDD state
    static regex rxnop_cmd("[ \t]*(echo|help|show|info|where|shell|sh|x)([ \t]+.*)?");
#endif

    return cmd.matches(rxnop_cmd);
}

// True if CMD is `info line' cmd
bool is_info_line_cmd(const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxop_cmd("[ \t]*(info[ \t]+line)([ \t]+.*)?");
#endif

    return cmd.matches(rxop_cmd);
}

// True if CMD executes debuggee
bool is_running_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxrunning_cmd(
	"[ \t]*"
	"(r|ru|run"
	"|rer|rerun"
	"|c|cont|contin|continu|continue"
	"|sig|sign|signa|signal"
	"|u|unt|unti|until"
	"|s|si|step|stepi"
	"|n|ni|next|nexti"
	"|j|ju|jump"
	"|k|ki|kill"
	"|fin|fini|finis|finish"
        "|R|S|exec"
	")([ \t]+.*)?");
#endif

    return cmd.matches(rxrunning_cmd);
}

// True if CMD starts program
bool is_run_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxrun_cmd("[ \t]*(r|rer|rerun|ru|run|R|exec)([ \t]+.*)?");
#endif

    return cmd.matches (rxrun_cmd);
}

// True if CMD runs debuggee for an indefinite period of time
bool is_cont_cmd(const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxcont_cmd("[ \t]*(r|ru|run|rer|rerun|c|cont|contin|"
			    "continu|continue|R|exec)([ \t]+.*)?");
#endif

    return cmd.matches (rxcont_cmd);
}

// True if CMD kills program
bool is_kill_cmd(const string& cmd)
{
    return cmd == "kill" || cmd == "k";
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
    static regex rxdata("[ \t]*[a-z ]*display([ \t]+.*)?");
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

// True if CMD applies a theme
bool is_apply_theme_cmd (const string& cmd)
{
    return cmd.contains("apply theme", 0);
}

// True if CMD unapplies a theme
bool is_unapply_theme_cmd (const string& cmd)
{
    return cmd.contains("unapply theme", 0);
}

// True if CMD toggles a theme
bool is_toggle_theme_cmd (const string& cmd)
{
    return cmd.contains("toggle theme", 0);
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
    static regex rxthread_cmd("[ \t]*(thread"
			      "|threadgroup"
			      "|suspend"
			      "|resume"
	")([ \t]+.*)?");
#endif

    return cmd.matches (rxthread_cmd);
}

// True if CMD changes variables
bool is_assign_cmd(const string& cmd, GDBAgent *gdb)
{
#if RUNTIME_REGEX
    static regex rxset1_cmd("[ \t]*(set[ \t]+var[a-z]*|assign|pq)([ \t]+.*)?");
    static regex rxset2_cmd("[ \t]*(set|p|print|output)[ \t]+[^=]+=[^=].*");
    static regex rxset3_cmd("[ \t]*[^=]+=[^=].*");
#endif

    DebuggerType type = gdb->type();

    if (type == PERL && cmd.contains("O ", 0))
	return false;		// Setting command

    return cmd.matches(rxset1_cmd) || 
	((type == GDB || type == JDB) && cmd.matches(rxset2_cmd)) ||
	((type == PYDB || type == PERL) && cmd.matches(rxset3_cmd));
}

// Get assigned variable
string get_assign_variable(const string& _cmd)
{
    string cmd = _cmd;
    strip_space(cmd);

    if (cmd.contains(":="))
	cmd = cmd.before(":=");
    else
	cmd = cmd.before('=');	// Remove assignment and assigned value

    if (cmd.contains("set var", 0))
	cmd = cmd.after(' ');	// Special case `set variable': two-word cmd

    if (cmd.contains(' '))
	cmd = cmd.after(' ');	// Return argument

    strip_space(cmd);
    return cmd;
}

// True if CMD changes debugger settings
bool is_setting_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxsetting_cmd("[ \t]*(set|dbxenv|O)[ \t]+.*");
    static regex rxpath_cmd("[ \t]*(dir|directory|path)([ \t]+.*)?");
#endif

    if (is_set_args_cmd(cmd))
	return false;		// `set args' command

    return cmd.matches (rxsetting_cmd) || cmd.matches(rxpath_cmd);
}

// True if CMD changes debugger signal handling
bool is_handle_cmd (const string& cmd)
{
    return cmd.contains("handle ", 0) && cmd.freq(' ') >= 2;
}

// True if CMD changes debugger settings
bool is_define_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxdefine_cmd("[ \t]*(define|document)[ \t]+.*");
#endif

    return cmd.matches(rxdefine_cmd);
}
bool is_end_cmd (const string& cmd)
{
    return cmd.contains("end");
}

// True if CMD changes debuggee
bool is_file_cmd (const string& cmd, GDBAgent *gdb)
{
    if (cmd == "# reset")
	return true;

    switch (gdb->type())
    {
    case GDB:
    case PYDB:
    case DBG:
    {
#if RUNTIME_REGEX
	static regex rxfile_cmd("[ \t]*(file|load)([ \t]+.*)?");
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
	return cmd.contains("#file ", 0);

    case JDB:
	return cmd.contains("load ", 0);

    case PERL:
	return cmd.contains("exec ", 0);

    case BASH:
	return cmd.contains("exec ", 0);

    case MAKE:
	return cmd.contains("exec ", 0);
    }

    assert(0);
    ::abort();
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
	"|ignore"
	"|cond|condition"
	"|when"
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

// True if CMD changes current cursor position
bool is_list_cmd (const string& cmd)
{
#if RUNTIME_REGEX
    static regex rxlist_cmd("[ \t]*(l|li|lis|list)([ \t]+.*)?");
#endif

    return cmd.matches(rxlist_cmd);
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
    string c = cmd;
    strip_leading_space(c);
    return c.contains('q', 0);	// `quit', `q', or whatever
}

static bool starts_with(const string& cmd, const string& prefix)
{
    if (prefix.empty())
	return false;

    return cmd == prefix || 
	(prefix.contains(' ', -1) && cmd.contains(prefix, 0)) ||
	cmd.contains(prefix + " ", 0);
}

// True if CMD is a printing command
bool is_print_cmd(const string& cmd, GDBAgent *gdb)
{
    return (starts_with(cmd, gdb->print_command("", true)) ||
	    starts_with(cmd, gdb->print_command("", false)));
}

// True if CMD is some other builtin command
bool is_other_builtin_cmd(const string& cmd, GDBAgent *gdb)
{
    return is_print_cmd(cmd, gdb) ||
	starts_with(cmd, gdb->enable_command("")) ||
	starts_with(cmd, gdb->disable_command("")) ||
	starts_with(cmd, gdb->delete_command("")) ||
	starts_with(cmd, "make") ||
	starts_with(cmd, "source");
}


// Fetch display expression from DISPLAY_CMD
string get_display_expression (const string& display_cmd)
{
#if RUNTIME_REGEX
    static regex rxdisplay_cmd("[ \t]*(disp|displ|displa|display|plot)[ \t]+");
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


// True if ARG has the form `FILE:LINE'
bool is_file_pos(const string& arg)
{
#if RUNTIME_REGEX
    static regex rxfilepos("[^:]*:[1-9][0-9]*");
#endif

    string a = arg;
    strip_space(a);
    return a.matches(rxfilepos);
}

// Return true if CMD begins recording commands 
bool starts_recording(const string& cmd)
{
    return cmd.contains("if", 0) ||
	cmd.contains("while", 0) ||
	cmd.contains("def", 0) ||
	cmd.contains("doc", 0) ||
	cmd.contains("comm", 0); 
}

// Return true if CMD ends recording commands 
bool ends_recording(const string& cmd)
{
    return cmd == "end";
}

// True if CMD calls a function
bool calls_function(const string& cmd)
{
    return cmd.contains("call ", 0) ||
	(!is_graph_cmd(cmd) && cmd.contains('('));
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
    case PYDB:
    case DBG:
	prx = &rxgdb_begin_of_display;
	break;

    case BASH:
    case DBX:
    case JDB:
    case MAKE:
    case PERL:
    case XDB:
	prx = &rxdbx_begin_of_display;
	break;
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
    strip_leading_space(displays);

    // string old_displays = displays;
    // std::clog << "read_next_display(" << quote(old_displays) << ")...\n";

    if (is_disabling(displays, gdb))
    {
	// After a `disabling' message, `display' output finishes
	next_display = displays;
	displays = "";
    }
    else
    {
	for (;;)
	{
	    next_display += read_token(displays);

	    if (displays.empty())
		break;		// All done

	    if (next_display.contains(":()", -1))
	    {
		// AIX DBX gives weird `members' like
		// "pcg =     OM_PCGDownCasters:()\n(_root = 0x2004c248 ..."
		// Be sure to continue such displays.
		continue;
	    }

	    if (next_is_nl(displays))
		break;		// At end of display
	}
    }

    displays = displays.after('\n');
    strip_leading_space(displays);

    // std::clog << "read_next_display(" << quote(old_displays) << ") = "
    //           << quote(next_display) << "\n";
    return next_display;
}

// Return "NR: NAME = " from DISPLAY
string get_disp_value_str (const string& display, GDBAgent *gdb)
{
    string d(display);

    if (gdb->type() == XDB)
    {
	// For some types, XDB issues `NAME = VALUE', for others, `VALUE'.
	// DDD compensates for this by prepending `NAME = '.
	// In case we have `NAME = NAME = VALUE', strip first `NAME = '.
#if RUNTIME_REGEX
	static regex rxeqeq("[^{};,\n= ]+ = [^{}();,\n= ]+ = .*");
#endif
	if (d.matches(rxeqeq))
	    d = d.after(" = ");
    }

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
static int display_info_index (const string& gdb_answer, GDBAgent *gdb)
{
    const regex *prx = 0;

    switch (gdb->type())
    {
    case DBG:
    case GDB: 
    case PYDB:
	prx = &rxgdb_begin_of_display_info;
	break;

    case DBX:
	prx = &rxdbx_begin_of_display_info;
	break;

    case BASH:
    case JDB:
    case MAKE:
    case PERL:
    case XDB:
	return -1;		// No displays in these debuggers
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
    if (i >= 0)
    {
	gdb_answer = gdb_answer.from(i);
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
    case PYDB:
    case DBG:
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
    case PERL:
    case BASH:
    case MAKE:
	break;			// FIXME
    }

    return "";
}

// Remove and return "NR: " from DISPLAY.
string get_info_disp_str (const string& display_info, GDBAgent *gdb)
{
    switch (gdb->type())
    {
    case GDB:
    case PYDB:
    case DBG:
	return display_info.after (":   ");

    case DBX:
	return display_info.after (") ");

    case XDB:
    case JDB:
    case PERL:
    case BASH:
    case MAKE:
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
    case PYDB:
	return info_disp_str.length() > 0 && info_disp_str[0] == 'n';

    case DBG:
    case DBX:
	return false;		// no display disabling in dbx

    case BASH:
    case JDB:
    case MAKE:
    case PERL:
    case XDB:
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
    case PYDB:
    case DBG:
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
    case PERL:
    case BASH:
    case MAKE:
	return "";		// FIXME
    }

    return "";
}

// Remove and return display name from DISPLAY
string read_disp_name (string& display, GDBAgent *gdb)
{
    strip_leading_space(display);
    string name = display.before (" = ");
    display = get_disp_value_str(display, gdb);
    return name;
}

// True if some display has been disabled
bool is_disabling(const string& value, GDBAgent *gdb)
{
    return gdb->type() == GDB && value.contains("\nDisabling display ");
}

// True if VALUE is an valid value (i.e., no error message)
bool is_valid(const string& value, GDBAgent *gdb)
{
    if (gdb->program_language() == LANGUAGE_PERL)
	return true;		// Everything is valid in perl

    // If VALUE ends in two words, it is an error message like `not
    // active' or `no symbol in current context.'.  XDB issues
    // `Unknown name "Foo" (UE369)' and `Local is not active (UE421)'.
#if RUNTIME_REGEX
    static regex rxinvalid_value("("
				 "[a-zA-Z]+ [a-zA-Z]+.*"
				 "|.*[a-zA-Z]+ [a-zA-Z]+(\\.|>)?"
				 ")\n?");
#endif

    if (value.contains("Unknown name"))
	return false;

    if (value.contains("Name unknown"))
	return false;

    if (value.contains("not active"))
	return false;

    if (value.contains("not defined"))
	return false;

    if (value.contains("not valid"))
	return false;

    if (value.contains("Was expecting"))
	return false;		// JDB 1.2

    if (value.contains("incorrect type"))
	return false;		// JDB 1.2

    if (value.contains("not a valid"))
	return false;		// JDB 1.2

    if (value.contains("Encountered \"", 0))
	return false;		// JDB 1.2

    // In JDB 1.2, values start with `instance of'
    if (!value.contains("instance of", 0) && value.matches(rxinvalid_value))
	return false;

    return true;
}


//-----------------------------------------------------------------------------
// Handle `where' output
//-----------------------------------------------------------------------------

// Fetch current scope from GDB `where' output (a function name).
// This also works for JDB `where' output.
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

    return where_output.at(start_of_name, 
			   end_of_name - start_of_name);
}
