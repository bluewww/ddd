// $Id$
// GDB communication manager.

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char comm_manager_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// GDB communication manager
// Name conventions:
// ...OA  : an OAProc used in GDBAgent::on_answer
// ...OAC : an OACProc used in GDBAgent::on_answer_completion()
// ...HP  : A handler procedure; see HandlerL.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "comm-manag.h"

#include "bool.h"
#include "cook.h"
#include "Command.h"
#include "ddd.h"
#include "dbx-lookup.h"
#include "exit.h"
#include "disp-read.h"
#include "DispBuffer.h"
#include "PosBuffer.h"
#include "string-fun.h"
#include "post.h"
#include "cmdtty.h"
#include "editing.h"
#include "history.h"
#include "SourceView.h"
#include "DataDisp.h"
#include "DispValue.h"
#include "version.h"
#include "VoidArray.h"
#include "buttons.h"
#include "question.h"
#include "regexps.h"
#include "index.h"
#include "settings.h"
#include "AppData.h"

#include <ctype.h>

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

// Shall we filter data from the answer?
enum Filtering {NoFilter, TryFilter, Filter};

// Additional data given to every single command.
class CmdData {
public:
    string      command;	  // The command issued
    Widget      origin;		  // Origin of this command
    Filtering   filter_disp;      // NoFilter:  do not filter displays.
				  // TryFilter: do filter if present.
                                  // Filter:    do filter.
    DispBuffer* disp_buffer;      // Display filter.
    PosBuffer*  pos_buffer;       // Position filter.
    bool        new_exec_pos;     // CMD results in new exec position.
    bool        new_frame_pos;    // CMD results in new frame position.
    bool        set_frame_pos;    // True if frame is to be changed manually.
    int         set_frame_arg;    // Argument: 0: reset, +/-N: move N frames
    string      set_frame_func;   // Argument: new function
    string      graph_cmd;	  // Graph command

    string      user_answer;	  // Buffer for the complete answer
    OQCProc     user_callback;	  // User callback
    void *      user_data;	  // User data
    bool        user_verbose;	  // Flag as given to send_gdb_command()
    bool        user_prompt;	  // Flag as given to send_gdb_command()
    bool        user_check;	  // Flag as given to send_gdb_command()

private:
    static void clear_origin(Widget w, XtPointer client_data, 
			     XtPointer call_data);

    void add_destroy_callback()
    {
	if (origin != 0)
	    XtAddCallback(origin, XtNdestroyCallback, clear_origin, 
			  (XtPointer)this);
    }

    void remove_destroy_callback()
    {
	if (origin != 0)
	    XtRemoveCallback(origin, XtNdestroyCallback, clear_origin,
			     (XtPointer)this);
    }

public:
    // Constructor
    CmdData (Widget orig = 0, Filtering fd = TryFilter)
	: command(""),
	  origin(orig),
	  filter_disp(fd),
	  disp_buffer(0),
	  pos_buffer(0),
	  new_exec_pos(false),
	  new_frame_pos(false),
	  set_frame_pos(false),
	  set_frame_arg(0),
	  set_frame_func(""),
	  graph_cmd(""),

	  user_answer(""),
	  user_callback(0),
	  user_data(0),
	  user_verbose(true),
	  user_prompt(true),
	  user_check(true)
    {
	add_destroy_callback();
    }

    // Destructor
    ~CmdData ()
    {
	remove_destroy_callback();
	delete disp_buffer;
	delete pos_buffer;
    }

private:
    CmdData(const CmdData&)
	: command(""),
	  origin(0),
	  filter_disp(TryFilter),
	  disp_buffer(0),
	  pos_buffer(0),
	  new_exec_pos(false),
	  new_frame_pos(false),
	  set_frame_pos(false),
	  set_frame_arg(0),
	  set_frame_func(""),
	  graph_cmd(""),

	  user_answer(""),
	  user_callback(0),
	  user_data(0),
	  user_verbose(true),
	  user_prompt(true),
	  user_check(true)
    {
	assert(0);
    }

    CmdData& operator = (const CmdData&)
    {
	assert(0); return *this;
    }
};

void CmdData::clear_origin(Widget w, XtPointer client_data, XtPointer)
{
    (void) w;			// Use it

    // The widget is being destroyed.  Remove all references.
    CmdData *cmd_data = (CmdData *)client_data;
    assert(w == cmd_data->origin);
    cmd_data->origin = 0;
}


// Additional data given to extra commands.
class PlusCmdData {
public:
    string      command;	       // The command issued

    int      n_init;	               // # of initialization commands

    bool     refresh_initial_line;     // send 'info line' / `func'
    bool     refresh_file;             // send 'file'
    bool     refresh_line;             // send 'list'
    bool     refresh_pwd;	       // send 'pwd'
    bool     refresh_class_path;       // send 'use'
    bool     refresh_breakpoints;      // send 'info b'
    bool     refresh_where;            // send 'where'
    bool     refresh_frame;            // send 'frame'
    bool     refresh_pc;               // refresh pc
    bool     refresh_registers;        // send 'info registers'
    bool     refresh_threads;          // send 'info threads'
    bool     refresh_data;             // send 'display'
    bool     refresh_user;             // send user-defined commands
    bool     refresh_addr;             // send commands to get addresses
    bool     refresh_disp_info;        // send 'info display'
    bool     refresh_history_filename; // send 'show history filename'
    bool     refresh_history_size;     // send 'show history size'
    bool     refresh_setting;	       // send 'show SETTING'
    string   set_command;	       // setting to update
    string   break_arg;		       // argument when setting breakpoint
    int      n_refresh_data;	       // # of data displays to refresh
    int      n_refresh_user;	       // # of user displays to refresh

    bool     config_frame;	       // try 'frame'
    bool     config_func;	       // try 'func'
    bool     config_run_io;	       // try 'dbxenv run_io'
    bool     config_print_r;	       // try 'print -r'
    bool     config_where_h;	       // try 'where -h'
    bool     config_display;	       // try 'display'
    bool     config_clear;	       // try 'clear'
    bool     config_handler;	       // try 'help handler'
    bool     config_pwd;	       // try 'pwd'
    bool     config_setenv;	       // try 'help setenv'
    bool     config_edit;	       // try 'help edit'
    bool     config_make;	       // try 'help make'
    bool     config_regs;	       // try 'help regs'
    bool     config_named_values;      // try 'print "ddd"'
    bool     config_when_semicolon;    // try 'help when'
    bool     config_delete_comma;      // try 'delete 4711 4712'
    bool     config_err_redirection;   // try 'help run'
    bool     config_givenfile;         // try 'help givenfile'
    bool     config_cont_sig;          // try 'help cont'
    bool     config_xdb;	       // try XDB settings
    bool     config_output;            // try 'output'
    bool     config_program_language;  // try 'show language'

    PlusCmdData ()
	: command(""),
	  n_init(0),
	  refresh_initial_line(false),
	  refresh_file(false),
	  refresh_line(false),
	  refresh_pwd(false),
	  refresh_class_path(false),
	  refresh_breakpoints(false),
	  refresh_where(false),
	  refresh_frame(false),
	  refresh_pc(false),
	  refresh_registers(false),
	  refresh_threads(false),
	  refresh_data(false),
	  refresh_user(false),
	  refresh_addr(false),
	  refresh_disp_info(false),
	  refresh_history_filename(false),
	  refresh_history_size(false),
	  refresh_setting(false),
	  set_command(""),
	  break_arg(""),
	  n_refresh_data(0),
	  n_refresh_user(0),

	  config_frame(false),
	  config_func(false),
	  config_run_io(false),
	  config_print_r(false),
	  config_where_h(false),
	  config_display(false),
	  config_clear(false),
	  config_handler(false),
	  config_pwd(false),
	  config_setenv(false),
	  config_edit(false),
	  config_make(false),
	  config_regs(false),
	  config_named_values(false),
	  config_when_semicolon(false),
	  config_delete_comma(false),
	  config_err_redirection(false),
	  config_givenfile(false),
	  config_cont_sig(false),
	  config_xdb(false),
	  config_output(false),
	  config_program_language(false)
    {}
};

static void user_cmdOA  (const string&, void *);
static void user_cmdOAC (void *);
static void plusOQAC (const StringArray&, const VoidArray&, void *);

// Handle graph command in CMD, with WHERE_ANSWER being the GDB reply
// to a `where 1' command; return true iff recognized
static bool handle_graph_cmd(string& cmd, const string& where_answer,
			     Widget origin, bool verbose, bool prompt);

// Handle output of initialization commands
static void process_init(const string& answer, void *data = 0);

// Handle output of batch commands
static void process_batch(const string& answer, void *data = 0);


// Process asynchronous GDB answers
static void AsyncAnswerHP(Agent *, void *, void *);

static string print_cookie = "4711";

//-----------------------------------------------------------------------------

// Replace all occurrences of `@N@' by N + the current breakpoint base;
// Replace all occurrences of `@AUTO@' by the current command prefix.
static void fix_symbols(string& cmd)
{
#if RUNTIME_REGEX
    static regex rxnum("@[0-9]+@");
#endif
    int i;
    while ((i = index(cmd, rxnum, "@")) >= 0)
    {
	int j = cmd.index('@', i + 1);
	int base = SourceView::next_breakpoint_number() - 1;
	cmd.at(i, j - i + 1) = itostring(atoi(cmd.chars() + i + 1) + base);
    }

    cmd.gsub("@AUTO@", app_data.auto_command_prefix);
}

inline String str(String s)
{
    return s != 0 ? s : "";
}

void start_gdb()
{
    // Register asynchronous answer handler
    gdb->addHandler(AsyncAnswer, AsyncAnswerHP);

    // Setup command data
    CmdData* cmd_data     = new CmdData;
    cmd_data->command     = "<init>";
    cmd_data->filter_disp = NoFilter;      // No `display' output
    cmd_data->pos_buffer  = new PosBuffer; // Find initial pos
    cmd_data->user_prompt = true;

    PlusCmdData* plus_cmd_data = new PlusCmdData;
    plus_cmd_data->command = "<init>";
    StringArray cmds;
    VoidArray dummy;

    // Fetch initialization commands
    string init;
    string settings;
    switch (gdb->type())
    {
    case GDB:
	init     = str(app_data.gdb_init_commands);
	settings = str(app_data.gdb_settings);
	break;

    case DBX:
	init     = str(app_data.dbx_init_commands);
	settings = str(app_data.dbx_settings);
	break;

    case XDB:
	init     = str(app_data.xdb_init_commands);
	settings = str(app_data.xdb_settings);
	break;

    case JDB:
	init     = str(app_data.jdb_init_commands);
	settings = str(app_data.jdb_settings);
	break;
    }
    string restart = str(app_data.restart_commands);

    // Place init commands in CMDS array
    while (init != "")
    {
	string command = init.before('\n');
	if (is_graph_cmd(command))
	{
	    // To be handled later by DDD - enqueue in command queue
	    Command c(command, 0, process_batch);
	    c.priority = COMMAND_PRIORITY_INIT;
	    gdb_command(c);
	}
	else
	{
	    // Process right now
	    cmds += command;
	}
	init = init.after('\n');
    }
    plus_cmd_data->n_init = cmds.size();

    // Add some additional init commands with reply handling
    switch (gdb->type())
    {
    case GDB:
	cmds += "info line";	// Fails if no symbol table is loaded.
	cmds += "list";		// But works just fine after a `list'.
	cmds += "info line";
	plus_cmd_data->refresh_initial_line = true;
	cmds += "output " + print_cookie;
	plus_cmd_data->config_output = true;
	cmds += "show language";
	plus_cmd_data->config_program_language = true;
	cmds += "pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "info breakpoints";
	plus_cmd_data->refresh_breakpoints = true;
	cmds += "show history filename";
	plus_cmd_data->refresh_history_filename = true;
	cmds += "show history size";
	plus_cmd_data->refresh_history_size = true;
	break;

    case DBX:
	plus_cmd_data->refresh_initial_line = true;

	cmds += "frame";
	plus_cmd_data->config_frame = true;
	cmds += "func";
	plus_cmd_data->config_func = true;
	cmds += "dbxenv run_io";
	plus_cmd_data->config_run_io = true;
	cmds += "print -r " + print_cookie;
	plus_cmd_data->config_print_r = true;
	cmds += "where -h";
	plus_cmd_data->config_where_h = true;
	cmds += "display";
	plus_cmd_data->config_display = true;
	cmds += "clear";
	plus_cmd_data->config_clear = true;
	cmds += "help handler";
	plus_cmd_data->config_handler = true;
	cmds += "pwd";
	plus_cmd_data->config_pwd = true;
	cmds += "help setenv";
	plus_cmd_data->config_setenv = true;
	cmds += "help edit";
	plus_cmd_data->config_edit = true;
	cmds += "help make";
	plus_cmd_data->config_make = true;
	cmds += "help regs";
	plus_cmd_data->config_regs = true;
	cmds += "print \"" DDD_NAME "\"";
	plus_cmd_data->config_named_values = true;
	cmds += "help when";
	plus_cmd_data->config_when_semicolon = true;
	cmds += "delete " + print_cookie + " " + print_cookie;
	plus_cmd_data->config_delete_comma = true;
	cmds += "help run";
	plus_cmd_data->config_err_redirection = true;
	cmds += "help givenfile";
	plus_cmd_data->config_givenfile = true;
	cmds += "help cont";
	plus_cmd_data->config_cont_sig = true;

	cmds += "sh pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "file";
	plus_cmd_data->refresh_file = true;
	cmds += "list";
	plus_cmd_data->refresh_line = true;
	cmds += "status";
	plus_cmd_data->refresh_breakpoints = true;
	break;

    case XDB:
	cmds += "L";
	plus_cmd_data->refresh_initial_line = true;
	cmds += "tm";
	plus_cmd_data->config_xdb = true;
	cmds += "!pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "lb";
	plus_cmd_data->refresh_breakpoints = true;
	break;

    case JDB:
	cmds += "use";
	plus_cmd_data->refresh_class_path = true;
	break;
    }

    while (dummy.size() < cmds.size())
	dummy += (void *)0;

    gdb->start_plus (user_cmdOA,
		     user_cmdOAC,
		     cmd_data,
		     cmds,
		     dummy,
		     cmds.size(),
		     plusOQAC,
		     (void*)plus_cmd_data);

    // Enqueue restart and settings commands
    init_session(restart, settings);

    // One last command to clear the delay, set up breakpoints and
    // issue prompt
    Command c("# reset");
    c.priority = COMMAND_PRIORITY_INIT;
    c.verbose  = false;
    c.prompt   = false;
    c.check    = true;
    gdb_command(c);
}

// Enqueue init commands
void init_session(const string& restart, const string& settings)
{
    string init_commands = restart + settings;

    // Process all start-up commands.  These should load the file, etc.
    while (init_commands != "")
    {
	Command c(init_commands.before('\n'), Widget(0), OQCProc(0));
	c.priority = COMMAND_PRIORITY_INIT;
	if (is_file_cmd(c.command, gdb) || is_core_cmd(c.command))
	{
	    // Give feedback on the files used and their state
	    c.verbose = true;
	    c.echo    = true;
	    c.prompt  = true;
	    c.check   = true;
	}
	else if (gdb->type() == JDB && is_use_cmd(c.command))
	{
	    c.check = true;
	}

	// Translate breakpoint numbers to the current base.
	fix_symbols(c.command);
	gdb_command(c);

	init_commands = init_commands.after('\n');
    }
}



//-----------------------------------------------------------------------------
// Send the GDB command CMD to GDB.
// No special processing whatsoever.
//-----------------------------------------------------------------------------

void send_gdb_ctrl(string cmd, Widget origin)
{
    CmdData* cmd_data      = new CmdData(origin, NoFilter);
    cmd_data->command      = cmd;
    cmd_data->disp_buffer  = new DispBuffer;
    cmd_data->pos_buffer   = new PosBuffer;
    cmd_data->new_exec_pos = true;
    cmd_data->origin       = origin;

    if (cmd == '\004')
	gdb_is_exiting = true;

    bool send_ok = gdb->send_user_ctrl_cmd(cmd, cmd_data);
    if (!send_ok)
	post_gdb_busy(origin);
}



//-----------------------------------------------------------------------------
// Send user command to GDB
//-----------------------------------------------------------------------------

// True iff last command was cancelled
static bool command_was_cancelled = false;

// Send user command CMD to GDB.  Invoke CALLBACK with DATA upon
// completion.  If ECHO and VERBOSE are set, issue command in GDB
// console.  If VERBOSE is set, issue answer in GDB console.  If
// PROMPT is set, issue prompt.  If CHECK is set, add appropriate GDB
// commands to get GDB state.
void send_gdb_command(string cmd, Widget origin,
		      OQCProc callback, void *data,
		      bool echo, bool verbose, bool prompt, bool check)
{
    // Pass control commands unprocessed to GDB.
    if (cmd.length() == 1 && iscntrl(cmd[0]))
    {
	// Don't issue control characters at the GDB prompt
	if (verbose && !gdb->isReadyWithPrompt())
	{
	    char c = cmd[0];

	    if (c < ' ')
		gdb_out(string("^") + char('@' + c));
	    else
		gdb_out("^?");	// DEL
	}

	if (cmd == "\004")
	    command_was_cancelled = true;

	send_gdb_ctrl(cmd, origin);
	return;
    }

    // Pass process I/O unprocessed to GDB.
    if (!gdb->isReadyWithPrompt())
    {
	// GDB isn't ready; maybe this is input to the debugged
	// process or an answer to a GDB confirmation question.

	bool send_ok = true;
	if (gdb->isBusyOnQuestion())
	{
	    // We have a question pending.  Don't interfere.
	    send_ok = false;
	}
	else
	{
	    if (cmd == "no")
		command_was_cancelled = true;

	    // We do not wait for GDB output.  Pass CMD unprocessed to
	    // GDB, leaving current user_data unharmed.
	    cmd += '\n';
	    send_ok = gdb->send_user_ctrl_cmd(cmd);
	}

	if (!send_ok)
	    post_gdb_busy(origin);
	return;
    }

    command_was_cancelled = false;

    // Setup extra command information
    CmdData* cmd_data       = new CmdData(origin);
    cmd_data->command       = cmd;
    cmd_data->disp_buffer   = new DispBuffer;
    cmd_data->pos_buffer    = new PosBuffer;
    cmd_data->user_callback = callback;
    cmd_data->user_data     = data;
    cmd_data->user_verbose  = verbose;
    cmd_data->user_prompt   = prompt;
    cmd_data->user_check    = check;

    PlusCmdData* plus_cmd_data = new PlusCmdData;
    plus_cmd_data->command = cmd;

    // Breakpoints may change any time
    if (gdb->has_volatile_breakpoints())
	plus_cmd_data->refresh_breakpoints = true;

    // User command output may change any time
    plus_cmd_data->refresh_user    = true;

    // Addresses may change any time
    plus_cmd_data->refresh_addr    = true;

    if (source_view->where_required())
    {
	plus_cmd_data->refresh_where = true;
	plus_cmd_data->refresh_frame = true;
    }

    if (source_view->register_required())
    {
	plus_cmd_data->refresh_registers = true;
    }

    if (source_view->thread_required())
    {
	plus_cmd_data->refresh_threads = true;
    }

    if (data_disp->count_data_displays() == 0 || 
	!gdb->has_display_command())
    {
	// No displays
	cmd_data->filter_disp = NoFilter;
    }
    
    if (!check || 
	gdb->recording() ||
	is_nop_cmd(cmd) || 
	is_graph_cmd(cmd) || 
	(gdb->type() == GDB && starts_recording(cmd)))
    {
	cmd_data->filter_disp = NoFilter;

	if (check)
	{
	    delete cmd_data->pos_buffer;
	    cmd_data->pos_buffer = 0;
	}

	if (is_define_cmd(cmd))
	    set_need_defines(true);

	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_addr        = false;
	plus_cmd_data->refresh_user        = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;

	if (is_graph_cmd(cmd))
	{
	    cmd_data->graph_cmd = cmd;
	}
	else if (gdb->type() == GDB && starts_recording(cmd))
	{
	    gdb->recording(true);
	}
    }
    else if (is_file_cmd(cmd, gdb))
    {
	// File may change: display main() function and update displays
	if (cmd != "# reset")
	{
	    plus_cmd_data->refresh_initial_line = true;
	}

	plus_cmd_data->refresh_data = true;

	if (gdb->has_display_command())
	    plus_cmd_data->refresh_disp_info = true;
	
	switch (gdb->type())
	{
	case DBX:
	    plus_cmd_data->refresh_file = true;
	    plus_cmd_data->refresh_line = true;
	    break;

	case GDB:
	case XDB:
	case JDB:
	    break;		// FIXME
	}
    }
    else if (is_single_display_cmd(cmd, gdb))
    {
	// No new displays
	cmd_data->filter_disp = NoFilter;

	// Breakpoints, Frames, Code and Registers won't change
	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_addr        = false;
    }
    else if (is_data_cmd(cmd))
    {
	plus_cmd_data->refresh_data      = true;

	// Breakpoints, Frames, Code and Registers won't change
	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_addr        = false;
    }
    else if (is_running_cmd(cmd, gdb) || is_pc_cmd(cmd))
    {
	// New displays and new exec position
	if (gdb->has_display_command())
	    cmd_data->filter_disp = Filter;
	cmd_data->new_exec_pos = true;
	if (gdb->type() == DBX)
	{
	    plus_cmd_data->refresh_file  = true;
	    // plus_cmd_data->refresh_line  = true;
	    if (gdb->has_frame_command())
		plus_cmd_data->refresh_frame = true;
	}
	if (is_pc_cmd(cmd))
	{
	    plus_cmd_data->refresh_frame = true;
	    plus_cmd_data->refresh_pc    = true;
	}
	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_thread_cmd(cmd) || is_core_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp   = NoFilter;
	cmd_data->new_frame_pos = true;
	cmd_data->new_exec_pos  = true;

	plus_cmd_data->refresh_breakpoints = is_thread_cmd(cmd);
	plus_cmd_data->refresh_where       = true;
	plus_cmd_data->refresh_frame       = true;
	plus_cmd_data->refresh_data        = true;
	plus_cmd_data->refresh_threads     = true;
    }
    else if (is_frame_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp   = NoFilter;
	cmd_data->new_frame_pos = true;

	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = true;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_data        = true;

	if (gdb->type() == DBX)
	{
	    // We need to get the current file as well...
	    plus_cmd_data->refresh_file  = true;
	}
	if (gdb->type() == JDB)
	{
	    // Get the current frame via `where'
	    plus_cmd_data->refresh_where = true;
	}
    }
    else if (is_set_cmd(cmd, gdb))
    {
	// Update displays
	plus_cmd_data->refresh_data = true;

	// Addresses won't change
	plus_cmd_data->refresh_addr = false;
    }
    else if (is_lookup_cmd(cmd))
    {
	if (gdb->type() == DBX)
	{
	    // In DBX, `func' changes the stack frame
	    cmd_data->new_frame_pos      = true;
	    plus_cmd_data->refresh_frame = true;
	    plus_cmd_data->refresh_file  = true;
	    plus_cmd_data->refresh_line  = true;
	}
	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_addr        = false;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_cd_cmd(cmd))
    {
	plus_cmd_data->refresh_pwd         = true;
	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_addr        = false;
    }
    else if (gdb->type() == JDB && is_use_cmd(cmd))
    {
	plus_cmd_data->refresh_class_path  = true;
	plus_cmd_data->set_command         = cmd;
	plus_cmd_data->refresh_breakpoints = false;
	plus_cmd_data->refresh_where       = false;
	plus_cmd_data->refresh_frame       = false;
	plus_cmd_data->refresh_registers   = false;
	plus_cmd_data->refresh_threads     = false;
	plus_cmd_data->refresh_addr        = false;
    }
    else if (is_setting_cmd(cmd))
    {
	get_settings(gdb->type());
	plus_cmd_data->refresh_setting = true;
	plus_cmd_data->set_command     = cmd;
	plus_cmd_data->refresh_data    = false;
	plus_cmd_data->refresh_addr    = false;

	if (gdb->type() == GDB && cmd.contains("history"))
	{
	    // Refresh history settings, too
	    plus_cmd_data->refresh_history_filename = true;
	    plus_cmd_data->refresh_history_size     = true;
	}
    }
    else if (is_quit_cmd(cmd))
    {
	gdb_is_exiting = true;
    }
    else if (is_break_cmd(cmd))
    {
	plus_cmd_data->break_arg = get_break_expression(cmd);
	plus_cmd_data->refresh_breakpoints = true;
    }
    else
    {
	// Unknown command -- refresh everything
	if (gdb->has_display_command())
	    cmd_data->filter_disp = Filter;

	cmd_data->new_frame_pos = true;
	cmd_data->new_exec_pos  = true;

	plus_cmd_data->refresh_breakpoints = true;
	plus_cmd_data->refresh_where       = true;
	plus_cmd_data->refresh_frame       = true;
	plus_cmd_data->refresh_data        = true;
	plus_cmd_data->refresh_threads     = true;
    }

    if (cmd_data->new_exec_pos
	|| plus_cmd_data->refresh_frame 
	|| plus_cmd_data->refresh_data)
    {
	// New program state: clear value cache
	clear_value_cache();
	DispValue::clear_type_cache();
    }

    if (plus_cmd_data->refresh_frame && 
	!gdb->has_frame_command() && 
	gdb->type() != JDB)
    {
	// We have a backtrace window open, but DBX has no ``frame''
	// command to set the selected frame.  Use this hack instead.
	string arg_s = cmd.after(rxblanks_or_tabs);

	if (is_up_cmd(cmd) || is_down_cmd(cmd))
	{
	    // Set the new selected frame from the `up'/`down'
	    // argument.

	    int arg;
	    if (arg_s == "")
		arg = 1;
	    else
		arg = get_positive_nr(arg_s);
	    if (arg > 0)
	    {
		cmd_data->set_frame_pos = true;

		int direction = 1;
		if (is_up_cmd(cmd))
		    direction = -direction;
		if (gdb->type() == XDB)
		    direction = -direction;

		cmd_data->set_frame_arg = direction * arg;
	    }
	}
	else if (is_lookup_cmd(cmd))
	{
	    // Switch to new function
	    cmd_data->set_frame_pos = true;
	    cmd_data->set_frame_func = arg_s;
	}
	else
	{
	    // Make bottom frame the selected frame
	    cmd_data->set_frame_pos = true;
	    cmd_data->set_frame_arg = 0;
	}

	plus_cmd_data->refresh_frame = false;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }

    if (gdb->type() != GDB)
	plus_cmd_data->refresh_registers = false;

    if (gdb->type() != GDB && gdb->type() != JDB)
	plus_cmd_data->refresh_threads = false;

    if (gdb->type() == GDB && cmd_data->pos_buffer != 0)
    {
	// Filtering GDB output for current function and PC is rather
	// expensive.  Hence, we scan GDB output only if actually
	// required.
	cmd_data->pos_buffer->check_pc   = source_view->need_pc();
	cmd_data->pos_buffer->check_func = data_disp->need_scope();
    }

    if (echo && verbose)
    {
	string c = cmd;
	strip_auto_command_prefix(c);
	gdb_out(c + "\n");
	gdb_input_at_prompt = true;
    }

    StringArray cmds;
    VoidArray dummy;

    assert(plus_cmd_data->n_init == 0);
    assert(!plus_cmd_data->config_frame);
    assert(!plus_cmd_data->config_func);
    assert(!plus_cmd_data->config_run_io);
    assert(!plus_cmd_data->config_print_r);
    assert(!plus_cmd_data->config_where_h);
    assert(!plus_cmd_data->config_display);
    assert(!plus_cmd_data->config_clear);
    assert(!plus_cmd_data->config_handler);
    assert(!plus_cmd_data->config_pwd);
    assert(!plus_cmd_data->config_setenv);
    assert(!plus_cmd_data->config_edit);
    assert(!plus_cmd_data->config_make);
    assert(!plus_cmd_data->config_regs);
    assert(!plus_cmd_data->config_named_values);
    assert(!plus_cmd_data->config_when_semicolon);
    assert(!plus_cmd_data->config_delete_comma);
    assert(!plus_cmd_data->config_err_redirection);
    assert(!plus_cmd_data->config_givenfile);
    assert(!plus_cmd_data->config_cont_sig);
    assert(!plus_cmd_data->config_xdb);
    assert(!plus_cmd_data->config_output);
    assert(!plus_cmd_data->config_program_language);
    
    // Setup additional trailing commands
    switch (gdb->type())
    {
    case GDB:
	if (plus_cmd_data->refresh_initial_line)
	{
	    cmds += "info line";	// Fails if no symbol table is loaded.
	    cmds += "list";		// But works just fine after a `list'.
	    cmds += "info line";
	}
	if (plus_cmd_data->refresh_pwd)
	    cmds += "pwd";
	assert(!plus_cmd_data->refresh_class_path);
	assert(!plus_cmd_data->refresh_file);
	assert(!plus_cmd_data->refresh_line);
	if (plus_cmd_data->refresh_breakpoints)
	    cmds += "info breakpoints";
	if (plus_cmd_data->refresh_where)
	    cmds += "where";
	if (plus_cmd_data->refresh_frame)
	    cmds += gdb->frame_command();
	if (plus_cmd_data->refresh_registers)
	    cmds += source_view->refresh_registers_command();
	if (plus_cmd_data->refresh_threads)
	    cmds += "info threads";
	if (plus_cmd_data->refresh_data)
	    plus_cmd_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (plus_cmd_data->refresh_user)
	    plus_cmd_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->info_display_command();
	if (plus_cmd_data->refresh_history_filename)
	    cmds += "show history filename";
	if (plus_cmd_data->refresh_history_size)
	    cmds += "show history size";
	if (plus_cmd_data->refresh_setting)
	{
	    string show_command = "show ";
	    if (cmd.contains("set ", 0))
		show_command += cmd.after("set ");
	    else if (cmd.contains("directory ", 0))
		show_command += "directories";
	    else if (cmd.contains("path ", 0))
		show_command += "paths";
	    else
		show_command += cmd;

	    if (show_command.freq(' ') >= 2)
	    {
		// Strip last argument from `show' command
		int index = show_command.index(' ', -1);
		show_command = show_command.before(index);
	    }
	    cmds += show_command;
	}
	break;

    case DBX:
	if (plus_cmd_data->refresh_pwd)
	    cmds += "pwd";
	assert(!plus_cmd_data->refresh_class_path);
	if (plus_cmd_data->refresh_file)
	    cmds += "file";
	if (plus_cmd_data->refresh_line)
	    cmds += "list";
	if (plus_cmd_data->refresh_breakpoints)
	    cmds += "status";
	if (plus_cmd_data->refresh_where)
	    cmds += "where";
	if (plus_cmd_data->refresh_frame)
	    cmds += gdb->frame_command();
	assert (!plus_cmd_data->refresh_registers);
	assert (!plus_cmd_data->refresh_threads);
	if (plus_cmd_data->refresh_data)
	    plus_cmd_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (plus_cmd_data->refresh_user)
	    plus_cmd_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->info_display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	if (plus_cmd_data->refresh_setting)
	    cmds += cmd.before(rxwhite);
	break;

    case XDB:
	if (plus_cmd_data->refresh_initial_line)
	    cmds += "L";
	if (plus_cmd_data->refresh_pwd)
	    cmds += "!pwd";
	assert(!plus_cmd_data->refresh_class_path);
	assert(!plus_cmd_data->refresh_file);
	assert(!plus_cmd_data->refresh_line);
	if (plus_cmd_data->refresh_breakpoints)
	    cmds += "lb";
	if (plus_cmd_data->refresh_where)
	    cmds += "t";
	if (plus_cmd_data->refresh_frame)
	    cmds += gdb->frame_command();
	assert (!plus_cmd_data->refresh_registers);
	assert (!plus_cmd_data->refresh_threads);
	if (plus_cmd_data->refresh_data)
	    plus_cmd_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (plus_cmd_data->refresh_user)
	    plus_cmd_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_setting);
	break;

    case JDB:
	assert (!plus_cmd_data->refresh_initial_line);
	assert (!plus_cmd_data->refresh_pwd);
	if (plus_cmd_data->refresh_class_path)
	    cmds += "use";
	assert(!plus_cmd_data->refresh_file);
	assert(!plus_cmd_data->refresh_line);
	if (plus_cmd_data->refresh_breakpoints)
	    cmds += "clear";
	if (plus_cmd_data->refresh_where)
	    cmds += "where";
	assert (!plus_cmd_data->refresh_registers);
	if (plus_cmd_data->refresh_threads)
	    cmds += "threads";
	if (plus_cmd_data->refresh_data)
	    plus_cmd_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (plus_cmd_data->refresh_user)
	    plus_cmd_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_setting);
	break;
    }

    while (dummy.size() < cmds.size())
	dummy += (void *)0;

    if (cmd_data->graph_cmd != "")
    {
	// Instead of DDD `graph' commands, we send a `func', `frame'
	// or `where' command to get the current scope.
	if (gdb->has_func_command())
	    cmd = gdb->func_command();
	else if (gdb->has_frame_command())
	    cmd = gdb->frame_command();
	else
	    cmd = gdb->where_command();
    }

    // Send commands
    bool send_ok;
    if (cmds.size() > 0)
    {
	send_ok = gdb->send_user_cmd_plus(cmds, dummy, cmds.size(),
					  plusOQAC, (void*)plus_cmd_data,
					  cmd, (void *)cmd_data);
    }
    else
    {
	send_ok = gdb->send_user_cmd(cmd, (void *)cmd_data);
	delete plus_cmd_data;
    }

    if (!send_ok)
	post_gdb_busy(origin);
}


//-----------------------------------------------------------------------------
// Part of the answer has been received
//-----------------------------------------------------------------------------

void user_cmdOA (const string& answer, void* data) 
{
    string ans = answer;
    CmdData *cmd_data = (CmdData *) data;
    if (cmd_data->pos_buffer)
	cmd_data->pos_buffer->filter(ans);

    if (cmd_data->filter_disp != NoFilter)
    {
	// Filter displays
	cmd_data->disp_buffer->filter(ans);
    }

    cmd_data->user_answer += ans;

    // Output remaining answer
    if (cmd_data->user_verbose && cmd_data->graph_cmd == "")
	gdb_out(ans);
}


//-----------------------------------------------------------------------------
// Answer is complete
//-----------------------------------------------------------------------------

// These two are required for the DBX `file' command.
// DBX does not issue file names when stopping, so use these instead.
static string last_pos_found;	// Last position found
static bool last_new_exec_pos;	// True if last command was new exec position
static bool last_new_frame_pos;	// True if last command was new frame position

// Return current JDB frame; 0 if none
inline int jdb_frame()
{
    return get_positive_nr(gdb->prompt().from("["));
}

// Command completed
void user_cmdOAC(void *data)
{
    gdb_is_exiting = false;

    CmdData *cmd_data = (CmdData *) data;
    PosBuffer *pos_buffer = cmd_data->pos_buffer;
    bool check     = cmd_data->user_check;
    bool verbose   = cmd_data->user_verbose;
    bool do_prompt = cmd_data->user_prompt;

    string answer = "";
    if (pos_buffer)
    {
	answer = pos_buffer->answer_ended();
	cmd_data->user_answer += answer;
    }

    if (pos_buffer && pos_buffer->started_found())
    {
	// Program has been restarted - clear position history
	source_view->clear_history();
    }

    if (pos_buffer && pos_buffer->terminated_found())
    {
	// Program has been terminated - clear execution position
	source_view->clear_execution_position();
    }

    if (pos_buffer && pos_buffer->recompiled_found())
    {
	// Program has been recompiled - clear code and source cache,
	// clear execution position, and reload current source.
	source_view->clear_code_cache();
	source_view->clear_file_cache();
	source_view->clear_execution_position();
	source_view->reload();

	// Refresh current displays -- they'll probably be lost
	Command c(data_disp->refresh_display_cmd());
	c.verbose  = false;
	c.prompt   = false;
	c.priority = COMMAND_PRIORITY_SYSTEM;
	gdb_command(c);
    }

    if (pos_buffer && pos_buffer->auto_cmd_found())
    {
	// Program (or GDB) issued auto command(s) to be executed by DDD
	string auto_commands = pos_buffer->get_auto_cmd();

	while (auto_commands != "")
	{
	    string command = auto_commands.before('\n');
	    auto_commands = auto_commands.after('\n');

	    Command c(command, cmd_data->origin);
	    c.priority = COMMAND_PRIORITY_BATCH;
	    c.echo    = false;
	    c.verbose = true;
	    c.prompt  = (auto_commands == "");
	    gdb_command(c);
	}

	// Don't issue prompt now; let the auto command do this
	do_prompt = false;
    }

    if (cmd_data->graph_cmd != "")
    {
	// Process graph command
	string cmd = cmd_data->graph_cmd;
	bool ok = handle_graph_cmd(cmd, cmd_data->user_answer, 
				   cmd_data->origin,
				   cmd_data->user_verbose,
				   cmd_data->user_prompt);
	if (!ok)
	{
	    // Unknown command -- try again with base command
	    cmd_data->graph_cmd   = "";
	    cmd_data->user_answer = "";
	    gdb->send_user_cmd(cmd, (void *)cmd_data);
	    return;
	}

	// No need for further checks
	check        = false;

	// Ignore the answer
	verbose      = false;

	// Don't issue any further prompt
	do_prompt = false;
    }

    // Set execution position
    if (check && pos_buffer && pos_buffer->pos_found())
    {
	string pos  = pos_buffer->get_position();
	string func = pos_buffer->get_function();

	if (func != "")
	{
	    // clog << "Current function is " << quote(func) << "\n";
	    data_disp->process_scope(func);
	}

	last_pos_found = pos;
	tty_full_name(pos);

	if (!pos.contains(':') && func != "")
	{
	    string file = "";

	    // No file found, but a function name
	    switch (gdb->type())
	    {
	    case DBX:
	    case XDB:
		file = dbx_lookup(func);
		file = file.before(':');
		break;

	    case GDB:
		// GDB always issues file names on positions...
		break;

	    case JDB:
		// FIXME
		break;
	    }

	    if (file != "")
		pos = file + ':' + pos;
	}

	last_new_exec_pos =  cmd_data->new_exec_pos;
	last_new_frame_pos = cmd_data->new_frame_pos;

	if (gdb->type() == JDB && jdb_frame() > 1)
	{
	    // A breakpoint was reached at some lower frame.  Don't
	    // change the current position now.
	}
	else if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	{
	    source_view->show_execution_position(pos, cmd_data->new_exec_pos, 
						 pos_buffer->signaled_found());
	}
	else
	{
	    // Lookup command: do not change exec position
	    source_view->show_position(pos);
	}
    }
    else if (check && pos_buffer)
    {
	// Command should have issued new position, or only PC was
	// found, and command was not cancelled: Clear old exec position
	if ((cmd_data->new_exec_pos || pos_buffer->pc_found())
	    && !command_was_cancelled)
	    source_view->show_execution_position();
    }

    // Up/Down is done: set frame position in backtrace window
    if (cmd_data->set_frame_pos)
    {
	if (cmd_data->set_frame_func != "")
	    source_view->set_frame_func(cmd_data->set_frame_func);
	else
	    source_view->set_frame_pos(cmd_data->set_frame_arg);
    }

    // Set PC position
    if (check && pos_buffer && pos_buffer->pc_found())
    {
	string pc = pos_buffer->get_pc();
	if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	    source_view->show_pc(pc, XmHIGHLIGHT_SELECTED,
				 cmd_data->new_exec_pos,
				 pos_buffer->signaled_found());
	else
	    source_view->show_pc(pc, XmHIGHLIGHT_NORMAL);
    }

    if (verbose)
    {
	// Show answer
	gdb_out(answer);
    }

    // Process displays
    if (check && cmd_data->filter_disp != NoFilter)
    {
	assert(gdb->has_display_command());

	if (verbose)
	    gdb_out(cmd_data->disp_buffer->answer_ended());

	if (cmd_data->filter_disp == Filter
	    || cmd_data->disp_buffer->displays_found())
	{
	    bool disabling_occurred;
	    string displays = cmd_data->disp_buffer->get_displays();
	    string not_my_displays = 
		data_disp->process_displays(displays, disabling_occurred);

	    if (verbose)
		gdb_out(not_my_displays);
	    
	    cmd_data->disp_buffer->clear();

	    // If GDB disabled any display, try once more
	    if (disabling_occurred)
	    {
		cmd_data->filter_disp = Filter;
		cmd_data->user_prompt = false;	// No more prompts
		gdb->send_user_cmd(gdb->display_command());
		return;
	    }
	}
    }

    if (cmd_data->user_callback != 0)
    {
	// Invoke user-defined callback
	cmd_data->user_callback(cmd_data->user_answer, cmd_data->user_data);
    }

    delete cmd_data;

    if (do_prompt)
	prompt();
}


//-----------------------------------------------------------------------------
// Process DDD `graph' commands (graph display, graph refresh).
//-----------------------------------------------------------------------------

// Fetch display numbers from COMMAND into NUMBERS
static bool read_displays(string command, IntArray& numbers, bool verbose)
{
    while (has_nr(command))
	numbers += atoi(read_nr_str(command));

    strip_space(command);
    if (command != "")
    {
	int nr = data_disp->display_number(command, verbose);
	if (nr == 0)
	    return false;	// No such display
	numbers += nr;
    }

    return true;		// Ok
}

// Handle graph command in CMD, with WHERE_ANSWER being the GDB reply
// to a `where 1' command; return true iff recognized
static bool handle_graph_cmd(string& cmd, const string& where_answer, 
			     Widget origin, bool verbose, bool prompt)
{
    string scope;
    if (gdb->has_func_command())
	scope = ((string&)where_answer).before('\n'); // `func' output
    else
	scope = get_scope(where_answer); // `where' or `frame' output

    cmd = cmd.after("graph ");
    if (is_display_cmd(cmd))
    {
	string rcmd = reverse(cmd);

	string depends_on = "";
	string when_in    = "";
	DeferMode deferred = DeferNever;
	BoxPoint *pos = 0;

	for (;;)
	{
	    strip_leading_space(rcmd);

#if RUNTIME_REGEX
	    static regex rxdep("[ \t]+no[ \t]+tnedneped[ \t]+");
	    static regex rxwhen("[ \t]+ni[ \t]+nehw[ \t]+(ro[ \t]won[ \t])?");
	    static regex rxat(
		"[)]?[0-9]*[1-9]-?[ \t]*,[ \t]*[0-9]*[1-9]-?[(]?"
		"[ \t]+ta[ \t]+.*");
#endif

	    int dep_index  = rcmd.index(rxdep);
	    int when_index = rcmd.index(rxwhen);

	    if (dep_index >= 0 && (when_index < 0 || dep_index < when_index))
	    {
		// Check for `dependent on DISPLAY'
		depends_on = reverse(rcmd.before(dep_index));
		strip_space(depends_on);

		rcmd = rcmd.after(dep_index);
		rcmd = rcmd.after("tnedneped");
		continue;
	    }

	    if (when_index >= 0 && (dep_index < 0 || when_index < dep_index))
	    {
		// Check for `[now or] when in FUNC'
		when_in = reverse(rcmd.before(when_index));
		strip_space(when_in);
		rcmd = rcmd.from(when_index);

		int matchlen = rxwhen.match(rcmd.chars(), rcmd.length());
		string clause = rcmd.before(matchlen);
		rcmd = rcmd.from(matchlen);

		if (clause.contains("won"))
		    deferred = DeferIfNeeded;
		else
		    deferred = DeferAlways;
		continue;
	    }

	    if (rcmd.matches(rxat))
	    {
		// Check for `at X, Y' or `at (X, Y)'
		if (pos == 0)
		    pos = new BoxPoint;

		string y = reverse(rcmd.before(','));
		(*pos)[Y] = get_nr(y);
		string x = rcmd.after(',');
		x = reverse(x.before(rxwhite));
		(*pos)[X] = get_nr(x);
		rcmd = rcmd.after("ta");
		continue;
	    }

	    break;
	}

	cmd = reverse(rcmd);
	string display_expression = get_display_expression(cmd);

	if (when_in != "" && when_in != scope)
	{
	    data_disp->new_displaySQ(display_expression, when_in, pos,
				     depends_on, deferred, origin, 
				     verbose, prompt);
	}
	else
	{
	    data_disp->new_displaySQ(display_expression, scope, pos,
				     depends_on, deferred, origin,
				     verbose, prompt);
	}
    }
    else if (is_refresh_cmd(cmd))
    {
	data_disp->refresh_displaySQ(origin, verbose, prompt);
    }
    else if (is_data_cmd(cmd))
    {
	IntArray numbers;
	bool ok = read_displays(cmd.after("display"), numbers, verbose);
	if (ok)
	{
	    if (is_delete_display_cmd(cmd))
	    {
		data_disp->delete_displaySQ(numbers, verbose, prompt);
	    }
	    else if (is_disable_display_cmd(cmd))
	    {
		data_disp->disable_displaySQ(numbers, verbose, prompt);
	    }
	    else if (is_enable_display_cmd(cmd))
	    {
		data_disp->enable_displaySQ(numbers, verbose, prompt);
	    }
	}
    }
    else
    {
	// Unknown command
	return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Process output of configuration commands
//-----------------------------------------------------------------------------

bool is_known_command(const string& answer)
{
    string ans = downcase(answer);

    strip_space(ans);

    // In longer messages (help texts and such), only check the first
    // and last line.
    if (ans.freq('\n') > 1)
    {
	int last_nl = ans.index('\n', -1);
	ans = ans.before('\n') + ans.from(last_nl + 1);
    }

    return ans.contains("program is not active")     // DBX
	|| (!ans.contains("syntax")                  // DEC DBX
	    && !ans.contains("invalid keyword")      // DEC DBX
	    && !ans.contains("undefined command")    // GDB
	    && !ans.contains("ambiguous command")    // GDB
	    && !ans.contains("not found")            // SUN DBX 3.0
	    && !ans.contains("is unknown")           // SUN DBX 3.0
	    && !ans.contains("not a known")          // AIX DBX 3.1
	    && !ans.contains("unrecognized")         // AIX DBX & SUN DBX 1.0
	    && !ans.contains("no help available")    // AIX DBX
	    && !ans.contains("expected")             // SGI DBX
	    && !ans.contains("invoked in line mode") // SCO DBX
	    && !ans.contains("huh?")                 // JDB
	    && !ans.contains("unknown", 0));         // XDB
}

static void process_init(const string& answer, void *)
{
    if (answer == NO_GDB_ANSWER)
	return;			// Command was canceled

    // cerr << answer;
}

static void process_batch(const string& answer, void *)
{
    if (answer == NO_GDB_ANSWER)
	return;			// Command was canceled

    // cerr << answer;
}

static void process_config_frame(string& answer)
{
    gdb->has_frame_command(is_known_command(answer));
}

static void process_config_func(string& answer)
{
    gdb->has_func_command(is_known_command(answer));
}

static void process_config_run_io(string& answer)
{
    gdb->has_run_io_command(is_known_command(answer));
}

static void process_config_print_r(string& answer)
{
    gdb->has_print_r_option(is_known_command(answer) 
			    && answer.contains(print_cookie));
}

static void process_config_output(string& answer)
{
    gdb->has_output_command(is_known_command(answer) 
			    && answer.contains(print_cookie));
}

static void process_config_where_h(string& answer)
{
    gdb->has_where_h_option(is_known_command(answer));
}

static void process_config_display(string& answer)
{
    gdb->has_display_command(is_known_command(answer));
}

static void process_config_clear(string& answer)
{
    gdb->has_clear_command(is_known_command(answer));
}

static void process_config_handler(string& answer)
{
    gdb->has_handler_command(is_known_command(answer));
}

static void process_config_pwd(string& answer)
{
    gdb->has_pwd_command(is_known_command(answer));
}

static void process_config_setenv(string& answer)
{
    gdb->has_setenv_command(is_known_command(answer));
}

static void process_config_edit(string& answer)
{
    gdb->has_edit_command(is_known_command(answer));
}

static void process_config_make(string& answer)
{
    gdb->has_make_command(is_known_command(answer));
}

static void process_config_regs(string& answer)
{
    gdb->has_regs_command(is_known_command(answer));
}

static void process_config_named_values(string& answer)
{
    // SUN DBX 4.0 issues "DDD" on `print -r "DDD"', but has named
    // values anyway.  Work around this.
    gdb->has_named_values(gdb->has_print_r_option() 
			  || answer.contains(" = "));
}

static void process_config_when_semicolon(string& answer)
{
    gdb->has_when_command(is_known_command(answer));
#if RUNTIME_REGEX
    static regex rxsemicolon_and_brace("; *[}]");
#endif
    gdb->has_when_semicolon(answer.contains(rxsemicolon_and_brace));
}

static void process_config_delete_comma(string& answer)
{
    gdb->wants_delete_comma(!is_known_command(answer));
}

static void process_config_err_redirection(string& answer)
{
    gdb->has_err_redirection(answer.contains(">&"));
}

static void process_config_givenfile(string& answer)
{
    gdb->has_givenfile_command(is_known_command(answer));
}

static void process_config_cont_sig(string& answer)
{
    gdb->has_cont_sig_command(answer.contains("[sig "));
}

static void process_config_tm(string& answer)
{
    // If the `tm' command we just sent SUSPENDED macros instead of
    // ACTIVATING them, we sent another `tm' in order to re-activate
    // them.
    if (answer.contains("SUSPENDED"))
	gdb_question("tm", 0);
}

static void process_config_program_language(string& lang)
{
    gdb->program_language(lang);
}


//-----------------------------------------------------------------------------
// Handle GDB answers to DDD questions sent after GDB command
//-----------------------------------------------------------------------------

void plusOQAC (const StringArray& answers,
	       const VoidArray& /* qu_datas */,
	       void*  data)
{
    int count = answers.size();

    PlusCmdData* plus_cmd_data = (PlusCmdData *)data;
    int qu_count = 0;
    string file;

    while (plus_cmd_data->n_init > 0)
    {
	// Handle output of initialization commands
	process_init(answers[qu_count++]);
	plus_cmd_data->n_init--;
    }

    if (plus_cmd_data->refresh_initial_line)
    {
	switch (gdb->type())
	{
	case GDB:
	{
	    // Handle `info line' output
	    string info_line1 = answers[qu_count++];
	    string list       = answers[qu_count++];
	    string info_line2 = answers[qu_count++];

	    // Skip initial message lines like `Reading symbols...'
	    while (list != "" && !has_nr(list))
		list = list.after('\n');

	    if (atoi(list) == 0)
	    {
		// No listing => no source => ignore `info line' output
	    }
	    else
	    {
		// Handle `info line' output
		string info_line = info_line1;
		if (!info_line.contains("Line ", 0))
		    info_line = info_line2;

		source_view->process_info_line_main(info_line);
	    }
	    break;
	}

	case XDB:
	    source_view->process_info_line_main(answers[qu_count++]);
	    break;

	case DBX:
	case JDB:
	    break;
	}
    }

    // Make sure XDB understands macros
    if (plus_cmd_data->config_xdb)
	process_config_tm(answers[qu_count++]);

    if (plus_cmd_data->config_frame)
	process_config_frame(answers[qu_count++]);

    if (plus_cmd_data->config_func)
	process_config_func(answers[qu_count++]);

    if (plus_cmd_data->config_run_io)
	process_config_run_io(answers[qu_count++]);

    if (plus_cmd_data->config_print_r)
	process_config_print_r(answers[qu_count++]);

    if (plus_cmd_data->config_where_h)
	process_config_where_h(answers[qu_count++]);

    if (plus_cmd_data->config_display)
	process_config_display(answers[qu_count++]);

    if (plus_cmd_data->config_clear)
	process_config_clear(answers[qu_count++]);

    if (plus_cmd_data->config_handler)
	process_config_handler(answers[qu_count++]);

    if (plus_cmd_data->config_pwd)
	process_config_pwd(answers[qu_count++]);

    if (plus_cmd_data->config_setenv)
	process_config_setenv(answers[qu_count++]);

    if (plus_cmd_data->config_edit)
	process_config_edit(answers[qu_count++]);

    if (plus_cmd_data->config_make)
	process_config_make(answers[qu_count++]);

    if (plus_cmd_data->config_regs)
	process_config_regs(answers[qu_count++]);

    if (plus_cmd_data->config_named_values)
	process_config_named_values(answers[qu_count++]);

    if (plus_cmd_data->config_when_semicolon)
	process_config_when_semicolon(answers[qu_count++]);

    if (plus_cmd_data->config_delete_comma)
	process_config_delete_comma(answers[qu_count++]);

    if (plus_cmd_data->config_err_redirection)
	process_config_err_redirection(answers[qu_count++]);

    if (plus_cmd_data->config_givenfile)
	process_config_givenfile(answers[qu_count++]);

    if (plus_cmd_data->config_cont_sig)
	process_config_cont_sig(answers[qu_count++]);

    if (plus_cmd_data->config_output)
	process_config_output(answers[qu_count++]);

    if (plus_cmd_data->config_program_language)
	process_config_program_language(answers[qu_count++]);

    if (plus_cmd_data->refresh_pwd)
	source_view->process_pwd(answers[qu_count++]);

    if (plus_cmd_data->refresh_class_path)
    {
	string ans = answers[qu_count++];
	source_view->process_use(ans);
	process_show(plus_cmd_data->set_command, ans);
    }

    if (plus_cmd_data->refresh_file)
    {
	assert (gdb->type() == DBX);

	file = answers[qu_count++];

	// Simple sanity check
	strip_trailing_space(file);
	if (file.contains('\n'))
	    file = file.before('\n');
	if (file.contains(' '))
	    file = "";

	if (file != "" && !plus_cmd_data->refresh_line)
	{
	    string current_file = source_view->file_of_cursor().before(':');
	    if (current_file != file)
	    {
		// File has changed and we already have the current line
		// - load new current file
		if (last_pos_found.contains(':'))
		    last_pos_found = file + ":" + last_pos_found.after(':');
		else
		    last_pos_found = file + ":" + last_pos_found;

		if (last_new_exec_pos || last_new_frame_pos)
		{
		    source_view->show_execution_position(last_pos_found,
							 last_new_exec_pos,
							 false, true);
		}
		else
		{
		    source_view->lookup(last_pos_found, true);
		}
	    }
	}
    }

    if (plus_cmd_data->refresh_line)
    {
	string listing = answers[qu_count++];

	if (file != "")
	{
	    int line;
	    if (plus_cmd_data->refresh_initial_line && atoi(listing) > 0)
		line = atoi(listing);
	    else
		line = line_of_listing(listing);
	    last_pos_found = file + ":" + itostring(line);

	    if (last_new_exec_pos || last_new_frame_pos)
	    {
		source_view->show_execution_position(last_pos_found,
						     last_new_exec_pos);
	    }
	    else
	    {
		source_view->lookup(last_pos_found, true);
	    }
	}
    }

    if (plus_cmd_data->refresh_breakpoints)
    {
	source_view->process_info_bp(answers[qu_count++], 
				     plus_cmd_data->break_arg);
	update_arg_buttons();
    }

    if (plus_cmd_data->refresh_where)
    {
	string& where_output = answers[qu_count++];

	if (gdb->type() == JDB)
	{
	    // In JDB, the first line issued by `where' is also the
	    // current exec position in the current frame.
	    PosBuffer pb;
	    string w(where_output);
	    pb.filter(w);
	    pb.answer_ended();

	    if (pb.pos_found())
		source_view->show_execution_position(pb.get_position());
	}

	source_view->process_where(where_output);
    }

    if (plus_cmd_data->refresh_frame)
    {
	if (gdb->type() == JDB)
	{
	    // In JDB, the current frame is part of the prompt.
	    string prompt = gdb->prompt();
	    source_view->process_frame(prompt);
	}
	else
	{
	    string& answer = answers[qu_count++];

	    if (plus_cmd_data->refresh_pc)
	    {
		PosBuffer pb;
		pb.filter(answer);
		if (pb.pos_found())
		    source_view->show_execution_position(pb.get_position(), 
							 true);
		if (pb.pc_found())
		    source_view->show_pc(pb.get_pc(), 
					 XmHIGHLIGHT_SELECTED, true);
	    }
	    source_view->process_frame(answer);
	}
    }

    if (plus_cmd_data->refresh_registers)
	source_view->process_registers(answers[qu_count++]);

    if (plus_cmd_data->refresh_threads)
	source_view->process_threads(answers[qu_count++]);

    if (plus_cmd_data->refresh_data)
    {
	string ans = "";
	for (int i = 0; i < plus_cmd_data->n_refresh_data; i++)
	    ans += answers[qu_count++];

	if (plus_cmd_data->n_refresh_data > 0)
	{
	    bool disabling_occurred = false;
	    data_disp->process_displays(ans, disabling_occurred);
	    if (disabling_occurred)
		data_disp->refresh_displaySQ();
	}
    }

    if (plus_cmd_data->refresh_user)
    {
	StringArray answers(((string *)answers) + qu_count,
			    plus_cmd_data->n_refresh_user);
	data_disp->process_user(answers);
	qu_count += plus_cmd_data->n_refresh_user;
    }

    if (plus_cmd_data->refresh_addr)
	data_disp->refresh_addr();

    if (plus_cmd_data->refresh_disp_info)
	data_disp->process_info_display(answers[qu_count++]);

    if (plus_cmd_data->refresh_history_filename)
	process_history_filename(answers[qu_count++]);

    if (plus_cmd_data->refresh_history_size)
	process_history_size(answers[qu_count++]);

    if (plus_cmd_data->refresh_setting)
    {
	string ans = answers[qu_count++];
	process_show(plus_cmd_data->set_command, ans);

	// Just in case we've changed the source language
	PosBuffer pb;
	pb.filter(ans);
    }

    assert (qu_count == count);
    if (qu_count != count)
	abort();

    delete plus_cmd_data;
}


//-----------------------------------------------------------------------------
// Process asynchronous GDB answers
//-----------------------------------------------------------------------------

static void AsyncAnswerHP(Agent *source, void *, void *call_data)
{
    string& answer = *((string *)call_data);
    GDBAgent *gdb = ptr_cast(GDBAgent, source);

    if (gdb->type() == JDB)
    {
	// In JDB, any thread may hit a breakpoint asynchronously.
	// Fetch its position.
	static string answer_buffer;
	answer_buffer += answer;
	if (gdb->ends_with_prompt(answer_buffer))
	{
	    PosBuffer pb;
	    pb.filter(answer_buffer);
	    pb.answer_ended();
	    if (pb.pos_found())
		source_view->show_execution_position(pb.get_position());

	    answer_buffer = "";
	}
    }

    _gdb_out(answer);
}
