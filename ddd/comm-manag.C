// $Id$
// GDB communication manager

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

#include "AppData.h"
#include "Command.h"
#include "DataDisp.h"
#include "DispBuffer.h"
#include "DispValue.h"
#include "PosBuffer.h"
#include "UndoBuffer.h"
#include "SourceView.h"
#include "VoidArray.h"
#include "bool.h"
#include "buttons.h"
#include "cmdtty.h"
#include "cook.h"
#include "cmdtty.h"
#include "dbx-lookup.h"
#include "ddd.h"
#include "disp-read.h"
#include "editing.h"
#include "exit.h"
#include "file.h"
#include "history.h"
#include "home.h"
#include "index.h"
#include "post.h"
#include "question.h"
#include "regexps.h"
#include "roulette.h"
#include "settings.h"
#include "shell.h"
#include "string-fun.h"
#include "version.h"
#include "windows.h"

#include <ctype.h>
#include <fstream.h>


//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

// Shall we filter data from the answer?
enum Filtering {NoFilter, TryFilter, Filter};

// Additional data given to every single command.
class CmdData {
public:
    string      command;	  // The command issued
    string      undo_command;	  // Undoing command, if any
    bool        undo_is_exec;	  // True if undoing command is exec command
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
    string      lookup_arg;	  // Argument when looking up sources

    string      user_answer;	  // Buffer for the complete answer
    OQCProc     user_callback;	  // User callback
    void *      user_data;	  // User data
    bool        user_verbose;	  // Flag as given to send_gdb_command()
    bool        user_prompt;	  // Flag as given to send_gdb_command()
    bool        user_check;	  // Flag as given to send_gdb_command()
    bool        recorded;	  // True if command was recorded

    bool        disabling_occurred; // Flag: GDB disabled displays

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
    CmdData (Widget orig = 0, Filtering filter = TryFilter)
	: command(""),
	  undo_command(""),
	  undo_is_exec(true),
	  origin(orig),
	  filter_disp(filter),
	  disp_buffer(0),
	  pos_buffer(0),
	  new_exec_pos(false),
	  new_frame_pos(false),
	  set_frame_pos(false),
	  set_frame_arg(0),
	  set_frame_func(""),
	  graph_cmd(""),
	  lookup_arg(""),

	  user_answer(""),
	  user_callback(0),
	  user_data(0),
	  user_verbose(true),
	  user_prompt(true),
	  user_check(true),
	  recorded(false),

	  disabling_occurred(false)
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
	  undo_command(""),
	  undo_is_exec(true),
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
	  lookup_arg(""),

	  user_answer(""),
	  user_callback(0),
	  user_data(0),
	  user_verbose(true),
	  user_prompt(true),
	  user_check(true),
	  recorded(false)
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


// Data given to extra commands.
class ExtraData {
public:
    string   command;		       // The command issued
    StringArray extra_commands;	       // The additional commands

    int      n_init;	               // # of initialization commands

    bool     refresh_initial_line;     // send 'info line' / `func'
    bool     refresh_file;             // send 'file'
    bool     refresh_line;             // send 'list'
    bool     refresh_recent_files;     // get program info
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
    bool     refresh_handle;	       // send 'info handle SIGNAL'
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
    bool     config_examine;           // try 'help examine'
    bool     config_rerun;             // try 'help rerun'
    bool     config_xdb;	       // try XDB settings
    bool     config_output;            // try 'output'
    bool     config_program_language;  // try 'show language'

    OACProc  user_callback;	       // callback
    void     *user_data;	       // user data

    ExtraData ()
	: command(""),
	  n_init(0),
	  refresh_initial_line(false),
	  refresh_file(false),
	  refresh_line(false),
	  refresh_recent_files(false),
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
	  refresh_handle(false),
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
	  config_examine(false),
	  config_rerun(false),
	  config_xdb(false),
	  config_output(false),
	  config_program_language(false),

	  user_callback(0),
	  user_data(0)
    {}
};

static void partial_answer_received(const string&, void *);
static void command_completed(void *);
static void extra_completed(const StringArray&, const VoidArray&, void *);

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

// Run with `-fr $HOME'
static void RussianRouletteHP(const string&, void *data);

static string print_cookie = "4711";



//-----------------------------------------------------------------------------
// Symbol fixing
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


//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

inline String str(String s)
{
    return s != 0 ? s : "";
}

void start_gdb(bool config)
{
    // Register asynchronous answer handler
    gdb->removeHandler(AsyncAnswer, AsyncAnswerHP);
    gdb->addHandler(AsyncAnswer, AsyncAnswerHP);

    // Setup command data
    CmdData* cmd_data     = new CmdData;
    cmd_data->command     = "<init>";
    cmd_data->filter_disp = NoFilter;      // No `display' output
    cmd_data->pos_buffer  = new PosBuffer; // Find initial pos
    cmd_data->user_prompt = true;

    ExtraData* extra_data = new ExtraData;
    extra_data->command = "<init>";
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

    case PYDB:
	init     = str(app_data.pydb_init_commands);
	settings = str(app_data.pydb_settings);
	break;

    case PERL:
	init     = str(app_data.perl_init_commands);
	settings = str(app_data.perl_settings);
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
    extra_data->n_init = cmds.size();
    extra_data->refresh_recent_files = true;

    // Add some additional init commands with reply handling
    switch (gdb->type())
    {
    case GDB:
	cmds += "info line";	// Fails if no symbol table is loaded.
	cmds += "list";		// But works just fine after a `list'.
	cmds += "info line";
	extra_data->refresh_initial_line = true;
	cmds += "output " + print_cookie;
	extra_data->config_output = true;
	cmds += "show language";
	extra_data->config_program_language = true;
	cmds += "pwd";
	extra_data->refresh_pwd = true;
	cmds += "info breakpoints";
	extra_data->refresh_breakpoints = true;
	cmds += "show history filename";
	extra_data->refresh_history_filename = true;
	cmds += "show history size";
	extra_data->refresh_history_size = true;
	break;

    case DBX:
	extra_data->refresh_initial_line = true;

	if (config)
	{
	    cmds += "frame";
	    extra_data->config_frame = true;
	    cmds += "func";
	    extra_data->config_func = true;
	    cmds += "dbxenv run_io";
	    extra_data->config_run_io = true;
	    cmds += "print -r " + print_cookie;
	    extra_data->config_print_r = true;
	    cmds += "where -h";
	    extra_data->config_where_h = true;
	    cmds += "display";
	    extra_data->config_display = true;
	    cmds += "clear";
	    extra_data->config_clear = true;
	    cmds += "help handler";
	    extra_data->config_handler = true;
	    cmds += "pwd";
	    extra_data->config_pwd = true;
	    cmds += "help setenv";
	    extra_data->config_setenv = true;
	    cmds += "help edit";
	    extra_data->config_edit = true;
	    cmds += "help make";
	    extra_data->config_make = true;
	    cmds += "help regs";
	    extra_data->config_regs = true;
	    cmds += "print \"" DDD_NAME "\"";
	    extra_data->config_named_values = true;
	    cmds += "help when";
	    extra_data->config_when_semicolon = true;
	    cmds += "delete " + print_cookie + " " + print_cookie;
	    extra_data->config_delete_comma = true;
	    cmds += "help run";
	    extra_data->config_err_redirection = true;
	    cmds += "help givenfile";
	    extra_data->config_givenfile = true;
	    cmds += "help cont";
	    extra_data->config_cont_sig = true;
	    cmds += "help examine";
	    extra_data->config_examine = true;
	    cmds += "help rerun";
	    extra_data->config_rerun = true;
	    cmds += "language";
	    extra_data->config_program_language = true;
	}

	cmds += "sh pwd";
	extra_data->refresh_pwd = true;
	cmds += "file";
	extra_data->refresh_file = true;
	cmds += "list";
	extra_data->refresh_line = true;
	cmds += "status";
	extra_data->refresh_breakpoints = true;
	break;

    case XDB:
	cmds += "L";
	extra_data->refresh_initial_line = true;

	if (config)
	{
	    cmds += "tm";
	    extra_data->config_xdb = true;
	}
	cmds += "!pwd";
	extra_data->refresh_pwd = true;
	cmds += "lb";
	extra_data->refresh_breakpoints = true;
	break;

    case JDB:
	extra_data->refresh_initial_line = true;

	cmds += "use";
	extra_data->refresh_class_path = true;
	break;

    case PYDB:
	extra_data->refresh_initial_line = true;

	cmds += "pwd";
	extra_data->refresh_pwd = true;
	cmds += "info breakpoints";
	extra_data->refresh_breakpoints = true;
	break;

    case PERL:
	// Perl starts immediately with the execution.
	cmd_data->new_exec_pos = true;

	cmds += gdb->pwd_command();
	extra_data->refresh_pwd = true;
	cmds += "L";
	extra_data->refresh_breakpoints = true;
	break;
    }

    while (dummy.size() < cmds.size())
	dummy += (void *)0;

    gdb->start_plus (partial_answer_received,
		     command_completed,
		     cmd_data,
		     cmds,
		     dummy,
		     cmds.size(),
		     extra_completed,
		     (void *)extra_data);

    if (config && (app_data.roulette || app_data.russian_roulette))
    {
	// Load random program
	static string program;
	program = random_program();
	Command c(gdb->debug_command(program));
	c.priority = COMMAND_PRIORITY_INIT;

	if (app_data.russian_roulette)
	{
	    c.callback = RussianRouletteHP;
	    c.data     = (void *)&program;
	    c.prompt   = false;
	}

	gdb_command(c);
    }

    // Enqueue restart and settings commands.  Since we're starting up
    // and don't care for detailed diagnostics, we allow the GDB
    // `source' command.
    init_session(restart, settings, app_data.source_init_commands);

    // One last command to clear the delay, set up breakpoints and
    // issue prompt
    Command c("# reset");
    c.priority = COMMAND_PRIORITY_INIT;
    c.echo     = false;
    c.verbose  = false;
    c.prompt   = false;
    c.check    = true;
    gdb_command(c);
}

struct InitSessionInfo {
    string restart;
    string settings;
    string tempfile;
};

static void SourceDoneCB(const string& answer, void *qu_data)
{
    InitSessionInfo *info = (InitSessionInfo *)qu_data;
    unlink(info->tempfile);

    string a = downcase(answer);
    if (a.contains(info->tempfile) && a.contains("error"))
    {
	// We've had an error while sourcing the file.  This keeps GDB
	// from reading the entire file, so we issue commands the
	// ordinary way.
	init_session(info->restart, info->settings, false);
    }

    delete info;
}

// Enqueue init commands
void init_session(const string& restart, const string& settings, 
		  bool try_source)
{
    string init_commands = restart + settings;

    InitSessionInfo *info = 0;

    if (try_source && !remote_gdb() && gdb->type() == GDB)
    {
	// Source start-up commands from temp file
	info = new InitSessionInfo;
	info->restart  = restart;
	info->settings = settings;
	info->tempfile = tmpnam(0);

	string file_commands = "";

	{
	    ofstream os(info->tempfile);
	    while (init_commands != "")
	    {
		string cmd = init_commands.before('\n');
		init_commands = init_commands.after('\n');

		if (is_file_cmd(cmd, gdb) || is_core_cmd(cmd) || 
		    cmd.contains("set confirm", 0))
		{
		    // Use this command the ordinary way
		    file_commands += cmd + "\n";
		}
		else
		{
		    // Source this command
		    fix_symbols(cmd);
		    if (is_graph_cmd(cmd))
			add_auto_command_prefix(cmd);
		    os << cmd << "\n";
		}
	    }
	}

	init_commands = file_commands;
    }

    // Process all start-up commands (load file, etc.)
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

    if (info != 0)
    {
	// Source remaining commands (settings, etc.)
	Command c("source " + info->tempfile, Widget(0), 
		  SourceDoneCB, (void *)info);
	c.priority = COMMAND_PRIORITY_INIT;
	c.check    = true;
	gdb_command(c);
    }
}



//-----------------------------------------------------------------------------
// Send the CMD to GDB, without any special processing.
//-----------------------------------------------------------------------------

void send_gdb_ctrl(string cmd, Widget origin)
{
    CmdData* cmd_data      = new CmdData(origin, TryFilter);
    cmd_data->command      = cmd;
    cmd_data->disp_buffer  = new DispBuffer;
    cmd_data->pos_buffer   = new PosBuffer;
    cmd_data->new_exec_pos = true;
    cmd_data->origin       = origin;

    if (cmd == '\004' && gdb_input_at_prompt)
	gdb_is_exiting = true;

    bool send_ok = gdb->send_user_ctrl_cmd(cmd, cmd_data);
    if (!send_ok)
	post_gdb_busy(origin);
}



//-----------------------------------------------------------------------------
// Handle DDD commands
//-----------------------------------------------------------------------------

// Do internal command; return reply
string internal_command(const string& command)
{
    if (command.contains("graph history ", 0))
    {
	string name = command.after("history ");
	strip_space(name);
	return undo_buffer.display_history(name);
    }

    return "";			// Nothing
}

bool is_internal_command(const string& command)
{
    return command.contains("graph history ", 0);
}

void internal_command(const string& command, OQCProc callback, void *data,
		      bool echo, bool verbose, bool do_prompt)
{
    if (echo && verbose)
	gdb_out(command + "\n");

    string answer = internal_command(command);

    if (verbose)
    {
	// Show answer
	_gdb_out(answer);
    }

    if (callback != 0)
    {
	// Invoke user-defined callback
	callback(answer, data);
    }

    if (do_prompt)
	prompt();
}


//-----------------------------------------------------------------------------
// Send user command to GDB
//-----------------------------------------------------------------------------

// True iff last command was cancelled
static bool command_was_cancelled = false;

// Send user command CMD to GDB.  Invoke CALLBACK with DATA upon
// completion of CMD; invoke EXTRA_CALLBACK with DATA when all extra
// commands (see CHECK) are done.  If ECHO and VERBOSE are set, issue
// command in GDB console.  If VERBOSE is set, issue answer in GDB
// console.  If PROMPT is set, issue prompt.  If CHECK is set, add
// extra GDB commands to get GDB state.
void send_gdb_command(string cmd, Widget origin,
		      OQCProc callback, OACProc extra_callback, void *data,
		      bool echo, bool verbose, bool prompt, bool check)
{
    string echoed_cmd = cmd;

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
    cmd_data->recorded      = gdb->recording();

    ExtraData* extra_data = new ExtraData;
    extra_data->command       = cmd;
    extra_data->user_callback = extra_callback;
    extra_data->user_data     = data;


    // Breakpoints may change any time
    if (gdb->has_volatile_breakpoints())
	extra_data->refresh_breakpoints = true;

    // User command output may change any time
    extra_data->refresh_user    = true;

    // Addresses may change any time
    extra_data->refresh_addr    = true;

    // Any command may break the `undo' state
    bool abort_undo = true;

    if (source_view->where_required())
    {
	extra_data->refresh_where = true;
	extra_data->refresh_frame = true;
    }

    if (source_view->register_required())
    {
	extra_data->refresh_registers = true;
    }

    if (source_view->thread_required())
    {
	extra_data->refresh_threads = true;
    }

    if (data_disp->count_data_displays() == 0 || 
	!gdb->has_display_command())
    {
	// No displays
	cmd_data->filter_disp = NoFilter;
    }

    if (is_list_cmd(cmd))
    {
	string arg = cmd.after(rxwhite);
	strip_space(arg);
	if (arg == "" || 
	    arg.contains('-', 0) || 
	    arg.contains('+', 0) || 
	    arg.matches(rxlist_range))
	{
	    // Ordinary `list', `list +', `list -', or `list N, M'.
	    // Nothing special.
	}
	else
	{
	    // `list ARG'
	    if (have_source_window())
	    {
		// Lookup ARG in source window, too
		switch (gdb->type())
		{
		case GDB:
		case PYDB:
		    // No need to list lines in the debugger console;
		    // translate `list' to `info line'.
		    cmd = "info line " + arg;
		    break;

		case DBX:
		case XDB:
		case JDB:
		case PERL:
		    // Use `list ARG' as directed, but as a side effect,
		    // lookup ARG in source window, too.
		    cmd_data->lookup_arg = arg;
		    break;
		}
	    }
	}

	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;

	abort_undo = false;
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
	{
	    string name = cmd.after(rxwhite);
	    strip_space(name);

	    update_define_later(name);
	    set_need_save_defines(true);
	}

	extra_data->refresh_breakpoints = ends_recording(cmd);
	extra_data->refresh_addr        = false;
	extra_data->refresh_user        = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;

	if (is_graph_cmd(cmd))
	{
	    cmd_data->graph_cmd = cmd;
	}
	else if (gdb->type() == GDB && starts_recording(cmd))
	{
	    gdb->recording(true);
	}

	if (gdb->recording())
	    echoed_cmd = cmd;

	abort_undo = false;
    }
    else if (is_file_cmd(cmd, gdb))
    {
	// File may change: display main() function and update displays
	bool is_reset_cmd = (cmd == "# reset");
	if (!is_reset_cmd)
	    extra_data->refresh_initial_line = true;

	extra_data->refresh_data = true;
	extra_data->refresh_recent_files = true;

	if (gdb->has_display_command())
	    extra_data->refresh_disp_info = true;
	
	switch (gdb->type())
	{
	case DBX:
	    extra_data->refresh_file = true;
	    extra_data->refresh_line = true;
	    break;

	case PERL:
	    if (!is_reset_cmd)
		cmd_data->new_exec_pos = true;
	    extra_data->refresh_initial_line = false;
	    break;

	case GDB:
	case XDB:
	case JDB:
	case PYDB:
	    break;		// FIXME
	}
    }
    else if (is_single_display_cmd(cmd, gdb))
    {
	// No new displays
	cmd_data->filter_disp = NoFilter;

	// Breakpoints, Frames, Code and Registers won't change
	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;
    }
    else if (is_data_cmd(cmd))
    {
	extra_data->refresh_data      = true;

	// Breakpoints, Frames, Code and Registers won't change
	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;
    }
    else if (is_running_cmd(cmd, gdb) || is_pc_cmd(cmd))
    {
	// New displays and new exec position
	if (gdb->has_display_command())
	    cmd_data->filter_disp = Filter;
	cmd_data->new_exec_pos = true;
	if (gdb->type() == DBX)
	{
	    extra_data->refresh_file  = true;
	    // extra_data->refresh_line  = true;
	    if (gdb->has_frame_command())
		extra_data->refresh_frame = true;
	}
	if (is_pc_cmd(cmd))
	{
	    extra_data->refresh_frame = true;
	    extra_data->refresh_pc    = true;
	}
	if (!gdb->has_display_command())
	    extra_data->refresh_data = true;

#if 0
	// Allow undoing `kill' and `run'
	if (is_run_cmd(cmd))
	    cmd_data->undo_command = gdb->kill_command();
	else if (is_kill_cmd(cmd))
	    cmd_data->undo_command = gdb->rerun_command();
	cmd_data->undo_is_exec = true;
#endif

	// Following input is not at prompt
	gdb_input_at_prompt = false;
    }
    else if (is_thread_cmd(cmd) || is_core_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp   = NoFilter;
	cmd_data->new_frame_pos = true;
	cmd_data->new_exec_pos  = true;

	extra_data->refresh_breakpoints = is_thread_cmd(cmd);
	extra_data->refresh_where       = true;
	extra_data->refresh_frame       = true;
	extra_data->refresh_data        = true;
	extra_data->refresh_threads     = true;
    }
    else if (is_frame_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp   = NoFilter;
	cmd_data->new_frame_pos = true;

	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = true;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_data        = true;

	if (gdb->type() == DBX)
	{
	    // We need to get the current file as well...
	    extra_data->refresh_file  = true;
	}
	if (gdb->type() == JDB)
	{
	    // Get the current frame via `where'
	    extra_data->refresh_where = true;
	}

	abort_undo = false;
    }
    else if (is_assign_cmd(cmd, gdb))
    {
	// Update displays
	extra_data->refresh_data = true;

	// Addresses won't change
	extra_data->refresh_addr = false;

	// Set up appropriate undoing command
	string var = get_assign_variable(cmd);
	if (var != "")
	{
	    string value = assignment_value(gdbValue(var));
	    if (value != NO_GDB_ANSWER)
	    {
		cmd_data->undo_command = gdb->assign_command(var, value);
		cmd_data->undo_is_exec = true;
	    }
	}
    }
    else if (is_lookup_cmd(cmd))
    {
	if (gdb->type() == DBX)
	{
	    // In DBX, `func' changes the stack frame
	    cmd_data->new_frame_pos   = true;
	    extra_data->refresh_frame = true;
	    extra_data->refresh_file  = true;
	    extra_data->refresh_line  = true;
	}
	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;

	if (!gdb->has_display_command())
	    extra_data->refresh_data = true;

	abort_undo = false;
    }
    else if (is_cd_cmd(cmd))
    {
	extra_data->refresh_pwd         = true;
	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;

	abort_undo = false;
    }
    else if (gdb->type() == JDB && is_use_cmd(cmd))
    {
	extra_data->refresh_class_path  = true;
	extra_data->set_command         = cmd;
	extra_data->refresh_breakpoints = false;
	extra_data->refresh_where       = false;
	extra_data->refresh_frame       = false;
	extra_data->refresh_registers   = false;
	extra_data->refresh_threads     = false;
	extra_data->refresh_addr        = false;
    }
    else if (is_setting_cmd(cmd))
    {
	get_settings(gdb->type());
	extra_data->refresh_setting     = true;
	extra_data->set_command         = cmd;
	extra_data->refresh_data        = false;
	extra_data->refresh_addr        = false;
	extra_data->refresh_breakpoints = false;

	if (gdb->type() == GDB && cmd.contains("history"))
	{
	    // Refresh history settings, too
	    extra_data->refresh_history_filename = true;
	    extra_data->refresh_history_size     = true;
	}

	abort_undo = false;
    }
    else if (is_handle_cmd(cmd))
    {
	(void) get_signals(gdb->type());
	extra_data->refresh_handle      = true;
	extra_data->refresh_data        = false;
	extra_data->refresh_addr        = false;
	extra_data->refresh_breakpoints = false;

	abort_undo = false;
    }
    else if (is_quit_cmd(cmd))
    {
	gdb_is_exiting = true;
    }
    else if (is_break_cmd(cmd))
    {
	extra_data->break_arg = get_break_expression(cmd);
	extra_data->refresh_breakpoints = true;

	abort_undo = false;
    }
    else if (is_print_cmd(cmd, gdb))
    {
	// A printing command - be sure to abort current undo
	abort_undo = true;
    }
    else if (is_other_builtin_cmd(cmd, gdb))
    {
	// Some other built-in command -- nothing special
	abort_undo = false;
    }
    else if (is_defined_cmd(cmd))
    {
	// User-defined command -- refresh everything
	if (gdb->has_display_command())
	    cmd_data->filter_disp = Filter;

	cmd_data->new_frame_pos = true;
	cmd_data->new_exec_pos  = true;

	extra_data->refresh_breakpoints = true;
	extra_data->refresh_where       = true;
	extra_data->refresh_frame       = true;
	extra_data->refresh_data        = true;
	extra_data->refresh_threads     = true;

	// Following input is not at prompt
	gdb_input_at_prompt = false;
    }

    if (undo_buffer.showing_earlier_state() && !cmd_data->new_exec_pos)
    {
	// Showing earlier state.  Don't update anything related to
	// program state.
	extra_data->refresh_where     = false;
	extra_data->refresh_frame     = false;
	extra_data->refresh_data      = false;
	extra_data->refresh_user      = false;
	extra_data->refresh_threads   = false;
	extra_data->refresh_registers = false;
	extra_data->refresh_addr      = false;
    }

    if (cmd_data->new_exec_pos
	|| extra_data->refresh_frame 
	|| extra_data->refresh_data)
    {
	// New program state: clear value cache
	clear_value_cache();
	DispValue::clear_type_cache();
    }

    if (!gdb->has_named_values() && is_print_cmd(cmd, gdb))
    {
	// The debugger `print NAME' does not prepend 'NAME = ' before
	// the value.  Fix this.
	cmd_data->user_answer = cmd.after(rxwhite) + " = ";
    }

    if (extra_data->refresh_frame && 
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

	extra_data->refresh_frame = false;

	if (!gdb->has_display_command())
	    extra_data->refresh_data = true;
    }

    if (!gdb->has_regs_command())
    {
	// No `regs' command => no refresh
	extra_data->refresh_registers = false;
    }

    if (gdb->type() != GDB && gdb->type() != JDB)
    {
	// No threads
	extra_data->refresh_threads = false;
    }

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
	strip_auto_command_prefix(echoed_cmd);
	gdb_out(echoed_cmd + "\n");
    }

    if (abort_undo)
	undo_buffer.restore_current_state();

    StringArray cmds;
    VoidArray dummy;

    assert(extra_data->n_init == 0);
    assert(!extra_data->config_frame);
    assert(!extra_data->config_func);
    assert(!extra_data->config_run_io);
    assert(!extra_data->config_print_r);
    assert(!extra_data->config_where_h);
    assert(!extra_data->config_display);
    assert(!extra_data->config_clear);
    assert(!extra_data->config_handler);
    assert(!extra_data->config_pwd);
    assert(!extra_data->config_setenv);
    assert(!extra_data->config_edit);
    assert(!extra_data->config_make);
    assert(!extra_data->config_regs);
    assert(!extra_data->config_named_values);
    assert(!extra_data->config_when_semicolon);
    assert(!extra_data->config_delete_comma);
    assert(!extra_data->config_err_redirection);
    assert(!extra_data->config_givenfile);
    assert(!extra_data->config_cont_sig);
    assert(!extra_data->config_examine);
    assert(!extra_data->config_rerun);
    assert(!extra_data->config_xdb);
    assert(!extra_data->config_output);
    assert(!extra_data->config_program_language);
    
    // Setup additional trailing commands
    switch (gdb->type())
    {
    case GDB:
	if (extra_data->refresh_initial_line)
	{
	    cmds += "info line";	// Fails if no symbol table is loaded.
	    cmds += "list";		// But works just fine after a `list'.
	    cmds += "info line";
	}
	if (extra_data->refresh_pwd)
	    cmds += "pwd";
	assert(!extra_data->refresh_class_path);
	assert(!extra_data->refresh_file);
	assert(!extra_data->refresh_line);
	if (extra_data->refresh_breakpoints)
	    cmds += "info breakpoints";
	if (extra_data->refresh_where)
	    cmds += "where";
	if (extra_data->refresh_frame)
	    cmds += gdb->frame_command();
	if (extra_data->refresh_registers)
	    cmds += source_view->refresh_registers_command();
	if (extra_data->refresh_threads)
	    cmds += "info threads";
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (extra_data->refresh_disp_info)
	    cmds += gdb->info_display_command();
	if (extra_data->refresh_history_filename)
	    cmds += "show history filename";
	if (extra_data->refresh_history_size)
	    cmds += "show history size";
	if (extra_data->refresh_setting)
	    cmds += show_command(cmd, gdb->type());
	if (extra_data->refresh_handle)
	{
	    string sig = cmd.after(rxwhite);
	    sig = sig.before(rxwhite);
	    if (sig == "all")
		sig = "";
	    cmds += "info handle " + sig;
	}
	break;

    case DBX:
	if (extra_data->refresh_pwd)
	    cmds += "pwd";
	assert(!extra_data->refresh_class_path);
	if (extra_data->refresh_file)
	    cmds += "file";
	if (extra_data->refresh_line)
	    cmds += "list";
	if (extra_data->refresh_breakpoints)
	    cmds += "status";
	if (extra_data->refresh_where)
	    cmds += "where";
	if (extra_data->refresh_frame)
	    cmds += gdb->frame_command();
	assert (!extra_data->refresh_registers);
	assert (!extra_data->refresh_threads);
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (extra_data->refresh_disp_info)
	    cmds += gdb->info_display_command();
	assert (!extra_data->refresh_history_filename);
	assert (!extra_data->refresh_history_size);
	if (extra_data->refresh_setting)
	    cmds += show_command(cmd, gdb->type());
	assert (!extra_data->refresh_handle);
	break;

    case XDB:
	if (extra_data->refresh_initial_line)
	    cmds += "L";
	if (extra_data->refresh_pwd)
	    cmds += "!pwd";
	assert(!extra_data->refresh_class_path);
	assert(!extra_data->refresh_file);
	assert(!extra_data->refresh_line);
	if (extra_data->refresh_breakpoints)
	    cmds += "lb";
	if (extra_data->refresh_where)
	    cmds += "t";
	if (extra_data->refresh_frame)
	    cmds += gdb->frame_command();
	assert (!extra_data->refresh_registers);
	assert (!extra_data->refresh_threads);
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (extra_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!extra_data->refresh_history_filename);
	assert (!extra_data->refresh_history_size);
	assert (!extra_data->refresh_setting);
	assert (!extra_data->refresh_handle);
	break;

    case JDB:
	assert (!extra_data->refresh_pwd);
	if (extra_data->refresh_class_path)
	    cmds += "use";
	assert(!extra_data->refresh_file);
	assert(!extra_data->refresh_line);
	if (extra_data->refresh_breakpoints)
	    cmds += "clear";
	if (extra_data->refresh_where)
	    cmds += "where";
	assert (!extra_data->refresh_registers);
	if (extra_data->refresh_threads)
	    cmds += "threads";
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	assert (!extra_data->refresh_history_filename);
	assert (!extra_data->refresh_history_size);
	assert (!extra_data->refresh_setting);
	assert (!extra_data->refresh_handle);
	break;

    case PYDB:
	if (extra_data->refresh_pwd)
	    cmds += "pwd";
	if (extra_data->refresh_breakpoints)
	    cmds += "info breakpoints";
	if (extra_data->refresh_where)
	    cmds += "where";
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (extra_data->refresh_disp_info)
	    cmds += gdb->info_display_command();
	break;

    case PERL:
	if (extra_data->refresh_pwd)
	    cmds += gdb->pwd_command();
	if (extra_data->refresh_breakpoints)
	    cmds += "L";
	if (extra_data->refresh_data)
	    extra_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (extra_data->refresh_user)
	    extra_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
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

    cmd_data->user_data    = data;
    cmd_data->user_verbose = verbose;
    cmd_data->user_prompt  = prompt;
    cmd_data->user_check   = check;

    extra_data->extra_commands = cmds;

    // Send commands
    bool send_ok = gdb->send_user_cmd_plus(cmds, dummy, cmds.size(),
					   extra_completed, (void*)extra_data,
					   cmd, (void *)cmd_data);

    if (!send_ok)
	post_gdb_busy(origin);
}


//-----------------------------------------------------------------------------
// Part of the answer has been received
//-----------------------------------------------------------------------------

static void partial_answer_received(const string& answer, void* data) 
{
    string ans = answer;
    CmdData *cmd_data = (CmdData *) data;

    bool verbose = cmd_data->user_verbose;

    // Filter displays and position
    if (cmd_data->pos_buffer)
	cmd_data->pos_buffer->filter(ans);

    if (cmd_data->filter_disp != NoFilter)
	cmd_data->disp_buffer->filter(ans);
    
    cmd_data->user_answer += ans;

    // Output remaining answer
    if (verbose && cmd_data->graph_cmd == "")
    {
	gdb_out(ans);
    }
    else if (cmd_data->user_answer.contains("(y or n) ", -1))
    {
	// GDB wants confirmation for a batch command
	gdb->send_user_ctrl_cmd("y\n");
    }
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
static void command_completed(void *data)
{
    gdb_is_exiting = false;

    CmdData *cmd_data = (CmdData *) data;
    PosBuffer *pos_buffer = cmd_data->pos_buffer;
    bool check     = cmd_data->user_check;
    bool verbose   = cmd_data->user_verbose;
    bool do_prompt = cmd_data->user_prompt;

    if (verbose && !cmd_data->recorded)
    {
	// Begin a new undo command
	undo_buffer.set_source(cmd_data->command);
    }

    string answer = "";
    if (pos_buffer)
    {
	answer = pos_buffer->answer_ended();
	cmd_data->user_answer += answer;
    }

    if (cmd_data->undo_command != "")
    {
	undo_buffer.add_command(cmd_data->undo_command, 
				cmd_data->undo_is_exec);
    }

    if (pos_buffer && pos_buffer->started_found())
    {
	// Program has been restarted - clear position history
	undo_buffer.clear_exec_pos();
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
	if (!auto_commands.contains('\n', -1))
	    auto_commands += '\n';

	while (auto_commands != "")
	{
	    string command = auto_commands.before('\n');
	    auto_commands = auto_commands.after('\n');

	    Command c(command, cmd_data->origin);
	    c.priority = COMMAND_PRIORITY_BATCH;
	    c.echo    = false;
	    c.verbose = true;
	    c.prompt  = do_prompt && (auto_commands == "");
	    gdb_command(c);
	}

	// Don't issue prompt now; let the last auto command do this
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
	    case PYDB:
	    case PERL:
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
	assert(cmd_data->filter_disp == TryFilter || 
	       gdb->has_display_command());

	if (verbose)
	    gdb_out(cmd_data->disp_buffer->answer_ended());

	if (cmd_data->filter_disp == Filter
	    || cmd_data->disp_buffer->displays_found())
	{
	    string displays = cmd_data->disp_buffer->get_displays();
	    string not_my_displays = 
		data_disp->process_displays(displays, 
					    cmd_data->disabling_occurred);

	    if (verbose)
		gdb_out(not_my_displays);

	    cmd_data->disp_buffer->clear();
	}
    }

    // If GDB disabled any display, try once more
    if (check && cmd_data->disabling_occurred)
    {
	cmd_data->filter_disp = Filter;
	cmd_data->user_prompt = false;	// No more prompts
	gdb->send_user_cmd(gdb->display_command());
	return;
    }

    if (cmd_data->user_callback != 0)
    {
	// Invoke user-defined callback
	cmd_data->user_callback(cmd_data->user_answer, cmd_data->user_data);
    }

    if (gdb->type() == JDB)
    {
	// Get a current program info to update the `recent files' list.
	// (In JDB, there will be no debugger interaction.)
	ProgramInfo info;
    }

    if (cmd_data->lookup_arg != "")
    {
	// As a side effect of `list X', lookup X in the source
	source_view->lookup(cmd_data->lookup_arg, false);
    }

    delete cmd_data;

    if (do_prompt)
	prompt();
}


//-----------------------------------------------------------------------------
// Process DDD `graph' commands (graph display, graph refresh).
//-----------------------------------------------------------------------------

// Fetch display numbers from ARG into NUMBERS
static bool read_displays(string arg, IntArray& numbers, bool verbose)
{
    while (has_nr(arg))
	numbers += atoi(read_nr_str(arg));

    strip_space(arg);
    if (arg != "")
    {
	int nr = data_disp->display_number(arg, verbose);
	if (nr == 0)
	    return false;	// No such display

	// Get all displays with name ARG
	data_disp->get_display_numbers(arg, numbers);
    }

    return true;		// Ok
}

// Handle graph command in CMD, with WHERE_ANSWER being the GDB reply
// to a `where 1' command; return true iff recognized
static bool handle_graph_cmd(string& cmd, const string& where_answer, 
			     Widget origin, bool verbose, bool do_prompt)
{
    string scope;
    if (gdb->has_func_command())
	scope = ((string&)where_answer).before('\n'); // `func' output
    else
	scope = get_scope(where_answer); // `where' or `frame' output

    cmd = cmd.after("graph ");
    if (is_display_cmd(cmd) || cmd.contains("plot", 0))
    {
	string rcmd = reverse(cmd);

	string depends_on = "";
	string when_in    = "";
	DeferMode deferred = DeferNever;
	bool clustered = false;
	BoxPoint *pos = 0;
	bool plotted = cmd.contains("plot", 0);

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

	    if (rcmd.contains("deretsulc", 0))
	    {
		clustered = true;
		rcmd = rcmd.after("deretsulc");
		continue;
	    }

	    break;
	}

	cmd = reverse(rcmd);
	string display_expression = get_display_expression(cmd);

	if (display_expression == "")
	{
	    // No argument.
	    // GDB gives no diagnostics in this case.  So, nor do we.
	    if (do_prompt)
		prompt();
	    return true;
	}

	if (when_in != "" && when_in != scope)
	{
	    data_disp->new_displaySQ(display_expression, when_in, pos,
				     depends_on, deferred, clustered, plotted,
				     origin, verbose, do_prompt);
	}
	else
	{
	    data_disp->new_displaySQ(display_expression, scope, pos,
				     depends_on, deferred, clustered, plotted,
				     origin, verbose, do_prompt);
	}
    }
    else if (is_refresh_cmd(cmd))
    {
	data_disp->refresh_displaySQ(origin, verbose, do_prompt);
    }
    else if (is_data_cmd(cmd))
    {
	IntArray numbers;
	bool ok = read_displays(cmd.after("display"), numbers, verbose);
	if (ok)
	{
	    // If no arg is given, apply command to all numbers
	    if (numbers.size() == 0)
		data_disp->get_all_display_numbers(numbers);
	    
	    if (is_delete_display_cmd(cmd))
	    {
		data_disp->delete_displaySQ(numbers, verbose, do_prompt);
	    }
	    else if (is_disable_display_cmd(cmd))
	    {
		data_disp->disable_displaySQ(numbers, verbose, do_prompt);
	    }
	    else if (is_enable_display_cmd(cmd))
	    {
		data_disp->enable_displaySQ(numbers, verbose, do_prompt);
	    }
	    else
	    {
		// Unknown command
		return false;
	    }
	}
    }
    else
    {
	// Unknown command
	return false;
    }

    // Command done
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

    if (ans.contains("program is not active")) // DBX
	return true;

    if (ans.contains("syntax"))	              // DEC DBX, Perl
	return false;

    if (ans.contains("invalid keyword"))      // DEC DBX
	return false;

    if (ans.contains("undefined command"))    // GDB
	return false;

    if (ans.contains("ambiguous command"))    // GDB
	return false;

    if (ans.contains("not found"))            // SUN DBX 3.0
	return false;

    if (ans.contains("is unknown"))           // SUN DBX 3.0
	return false;

    if (ans.contains("is a shell keyword"))   // SUN DBX 3.0
	return false;

    if (ans.contains("not a known"))          // AIX DBX 3.1
	return false;

    if (ans.contains("unrecognized"))         // AIX DBX & SUN DBX 1.0
	return false;

    if (ans.contains("no help available"))    // AIX DBX
	return false;

    if (ans.contains("expected"))             // SGI DBX
	return false;

    if (ans.contains("invoked in line mode")) // SCO DBX
	return false;

    if (ans.contains("huh?"))	              // JDB
	return false;

    if (ans.contains("can't locate"))         // Perl
	return false;

    if (ans.contains("unknown", 0))           // XDB
	return false;

    return true;
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

    // If `help run' contains `with the current arguments', then `run'
    // without args uses the current args.  Hence, `rerun' without
    // args must run the program without args.  SUN DBX feature.
    gdb->rerun_clears_args(answer.contains("with the current arg"));
}

static void process_config_givenfile(string& answer)
{
    gdb->has_givenfile_command(is_known_command(answer));
}

static void process_config_cont_sig(string& answer)
{
    gdb->has_cont_sig_command(answer.contains("[sig "));
}

static void process_config_examine(string& answer)
{
    gdb->has_examine_command(is_known_command(answer));
}

static void process_config_rerun(string& answer)
{
    gdb->has_rerun_command(is_known_command(answer));
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

static void extra_completed (const StringArray& answers,
			     const VoidArray& /* qu_datas */,
			     void*  data)
{
    int count = answers.size();

    ExtraData* extra_data = (ExtraData *)data;
    int qu_count = 0;
    string file;

    while (extra_data->n_init > 0)
    {
	// Handle output of initialization commands
	process_init(answers[qu_count++]);
	extra_data->n_init--;
    }

    if (extra_data->refresh_recent_files)
    {
	// Clear undo buffer.  Do this before setting the initial line,
	// such that it becomes part of the history.
	undo_buffer.clear();
    }

    if (extra_data->refresh_initial_line)
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
	case PYDB:
	case PERL:
	{
	    string dummy;
	    source_view->process_info_line_main(dummy);
	    break;
	}
	}
    }

    // Make sure XDB understands macros
    if (extra_data->config_xdb)
	process_config_tm(answers[qu_count++]);

    if (extra_data->config_frame)
	process_config_frame(answers[qu_count++]);

    if (extra_data->config_func)
	process_config_func(answers[qu_count++]);

    if (extra_data->config_run_io)
	process_config_run_io(answers[qu_count++]);

    if (extra_data->config_print_r)
	process_config_print_r(answers[qu_count++]);

    if (extra_data->config_where_h)
	process_config_where_h(answers[qu_count++]);

    if (extra_data->config_display)
	process_config_display(answers[qu_count++]);

    if (extra_data->config_clear)
	process_config_clear(answers[qu_count++]);

    if (extra_data->config_handler)
	process_config_handler(answers[qu_count++]);

    if (extra_data->config_pwd)
	process_config_pwd(answers[qu_count++]);

    if (extra_data->config_setenv)
	process_config_setenv(answers[qu_count++]);

    if (extra_data->config_edit)
	process_config_edit(answers[qu_count++]);

    if (extra_data->config_make)
	process_config_make(answers[qu_count++]);

    if (extra_data->config_regs)
	process_config_regs(answers[qu_count++]);

    if (extra_data->config_named_values)
	process_config_named_values(answers[qu_count++]);

    if (extra_data->config_when_semicolon)
	process_config_when_semicolon(answers[qu_count++]);

    if (extra_data->config_delete_comma)
	process_config_delete_comma(answers[qu_count++]);

    if (extra_data->config_err_redirection)
	process_config_err_redirection(answers[qu_count++]);

    if (extra_data->config_givenfile)
	process_config_givenfile(answers[qu_count++]);

    if (extra_data->config_cont_sig)
	process_config_cont_sig(answers[qu_count++]);

    if (extra_data->config_examine)
	process_config_examine(answers[qu_count++]);

    if (extra_data->config_rerun)
	process_config_rerun(answers[qu_count++]);

    if (extra_data->config_output)
	process_config_output(answers[qu_count++]);

    if (extra_data->config_program_language)
	process_config_program_language(answers[qu_count++]);

    if (extra_data->refresh_pwd)
	source_view->process_pwd(answers[qu_count++]);

    if (extra_data->refresh_class_path)
    {
	string ans = answers[qu_count++];
	source_view->process_use(ans);
	process_show(extra_data->set_command, ans);
    }

    if (extra_data->refresh_file)
    {
	assert (gdb->type() == DBX);

	file = answers[qu_count++];

	// Simple sanity check
	strip_trailing_space(file);
	if (file.contains('\n'))
	    file = file.before('\n');
	if (file.contains(' '))
	    file = "";

	if (file != "" && !extra_data->refresh_line)
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

    if (extra_data->refresh_line)
    {
	string listing = answers[qu_count++];

	if (file != "")
	{
	    int line;
	    if (extra_data->refresh_initial_line && atoi(listing) > 0)
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

    if (extra_data->refresh_breakpoints)
    {
	source_view->process_info_bp(answers[qu_count++], 
				     extra_data->break_arg);
	update_arg_buttons();
    }

    if (extra_data->refresh_where)
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
    else
    {
	undo_buffer.remove_where();
    }

    if (extra_data->refresh_frame)
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

	    if (extra_data->refresh_pc)
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
    else
    {
	undo_buffer.remove_frame();
    }

    if (extra_data->refresh_registers)
	source_view->process_registers(answers[qu_count++]);
    else
	undo_buffer.remove_registers();

    if (extra_data->refresh_threads)
	source_view->process_threads(answers[qu_count++]);
    else
	undo_buffer.remove_threads();

    if (extra_data->refresh_data)
    {
	string ans = "";
	for (int i = 0; i < extra_data->n_refresh_data; i++)
	{
	    if (!gdb->has_named_values())
	    {
		const string& cmd = extra_data->extra_commands[qu_count];
		ans += cmd.after(rxwhite) + " = ";
	    }
	    ans += answers[qu_count++];
	}

	if (extra_data->n_refresh_data > 0)
	{
	    bool disabling_occurred = false;
	    data_disp->process_displays(ans, disabling_occurred);
	    if (disabling_occurred)
		data_disp->refresh_displaySQ();
	}
    }

    if (extra_data->refresh_user)
    {
	StringArray answers(((string *)answers) + qu_count,
			    extra_data->n_refresh_user);
	data_disp->process_user(answers);
	qu_count += extra_data->n_refresh_user;
    }

    if (extra_data->refresh_addr)
	data_disp->refresh_addr();

    if (extra_data->refresh_disp_info)
	data_disp->process_info_display(answers[qu_count++]);

    if (extra_data->refresh_history_filename)
	process_history_filename(answers[qu_count++]);

    if (extra_data->refresh_history_size)
	process_history_size(answers[qu_count++]);

    if (extra_data->refresh_setting)
    {
	string ans = answers[qu_count++];
	process_show(extra_data->set_command, ans);

	// Just in case we've changed the source language
	PosBuffer pb;
	pb.filter(ans);
    }

    if (extra_data->refresh_handle)
    {
	process_handle(answers[qu_count++]);
    }

    if (extra_data->refresh_recent_files)
    {
	// Get a current program info to update the `recent files' list
	ProgramInfo info;

	// Update the source list in `Open Source'
	update_sources();
    }

    assert (qu_count == count);
    if (qu_count != count)
	abort();

    if (extra_data->user_callback != 0)
	(*extra_data->user_callback)(extra_data->user_data);

    delete extra_data;
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



//-----------------------------------------------------------------------------
// Russian Roulette
//-----------------------------------------------------------------------------

static void RussianRouletteDoneCB(XtPointer data, XtIntervalId *)
{
    string& program = *((string *)data);

    _gdb_out(program + ": you did not expect this to work, did you?\n");
    _gdb_out("\nProgram exited with code 01.\n");
    prompt();
}

static void RussianRouletteHP(const string&, void *data)
{
    // Run with ` -fr $HOME'
    string& program = *((string *)data);

    prompt();
    string args = string(" -fr ") + gethome();
    _gdb_out("run" + args + "\n");
    _gdb_out("Starting program: " + program + args + "\n");

    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 2000, 
		    RussianRouletteDoneCB, XtPointer(data));
}
