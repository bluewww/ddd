// $Id$
// GDB communication manager.

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

char comm_manager_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// GDB communication manager
// Name conventions:
// ...SUC : calls send_user_cmd() of GDBAgent *gdb.
// ...OA  : an OAProc used in GDBAgent::on_answer
// ...OAC : an OACProc used in GDBAgent::on_answer_completion()
// ...HP  : A handler procedure; see HandlerL.h
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "comm-manag.h"

#include "bool.h"
#include "commandQ.h"
#include "ddd.h"
#include "dbx-lookup.h"
#include "disp-read.h"
#include "DispBuffer.h"
#include "PosBuffer.h"
#include "string-fun.h"
#include "post.h"
#include "cmdtty.h"
#include "history.h"
#include "SourceView.h"
#include "DataDisp.h"
#include "version.h"
#include "VoidArray.h"
#include "buttons.h"
#include "question.h"
#include "settings.h"
#include "AppData.h"

#include <ctype.h>

static void handle_graph_cmd (string cmd, Widget origin,
			      OQCProc callback, void *data, 
			      bool verbose, bool check);

//-----------------------------------------------------------------------------
// Sollen Displays aus der Antwort herausgefiltert werden ?
//
enum Filtering {NoFilter, TryFilter, Filter};

typedef struct CmdData {
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

    string      user_answer;	  // Buffer for the complete answer
    OQCProc     user_callback;	  // User callback
    void *      user_data;	  // User data
    bool        user_verbose;	  // Flag as given to user_cmdSUC

    CmdData (Filtering   fd = TryFilter,
	     DispBuffer* db = 0,
	     PosBuffer*  pb = 0,
	     bool     nep= false,
	     bool     /* nfp */ = false) :
	filter_disp(fd),
	disp_buffer(db),
	pos_buffer(pb),
	new_exec_pos(nep),
	new_frame_pos(nep),
	set_frame_pos(false),
	set_frame_arg(0),
	set_frame_func(""),
	user_answer(""),
	user_callback(0),
	user_data(0),
	user_verbose(true)
    {}
};


typedef struct PlusCmdData {
    int      n_init;	               // # of initialization commands

    bool     refresh_initial_line;     // send 'info line' / `func'
    bool     refresh_file;             // send 'file'
    bool     refresh_line;             // send 'list'
    bool     refresh_pwd;	       // send 'pwd'
    bool     refresh_bpoints;          // send 'info b'
    bool     refresh_where;            // send 'where'
    bool     refresh_frame;            // send 'frame'
    bool     refresh_registers;        // send 'info registers'
    bool     refresh_threads;          // send 'info threads'
    bool     refresh_data;             // send 'display'
    bool     refresh_user;             // send user-defined commands
    bool     refresh_addr;             // send commands to get addresses
    bool     refresh_disp_info;        // send 'info display'
    bool     refresh_history_filename; // send 'show history filename'
    bool     refresh_history_size;     // send 'show history size'
    bool     refresh_history_save;     // send 'show history save'
    bool     refresh_setting;	       // send 'show SETTING'
    string   set_command;	       // setting to update
    string   break_arg;		       // argument when setting breakpoint
    int      n_refresh_data;	       // # of data displays to refresh
    int      n_refresh_user;	       // # of user displays to refresh
    int      n_refresh_addr;	       // # of addresses to refresh

    bool     config_frame;	       // try 'frame'
    bool     config_run_io;	       // try 'dbxenv run_io'
    bool     config_print_r;	       // try 'print -r'
    bool     config_where_h;	       // try 'where -h'
    bool     config_display;	       // try 'display'
    bool     config_clear;	       // try 'clear'
    bool     config_pwd;	       // try 'pwd'
    bool     config_setenv;	       // try 'help setenv'
    bool     config_edit;	       // try 'help edit'
    bool     config_make;	       // try 'help make'
    bool     config_named_values;      // try 'print "ddd"'
    bool     config_when_semicolon;    // try 'help when'
    bool     config_delete_comma;      // try 'delete 4711 4712'
    bool     config_err_redirection;   // try 'help run'
    bool     config_xdb;	       // try XDB settings
    bool     config_output;            // try 'output'
    bool     config_program_language;  // try 'show language'

    PlusCmdData () :
	n_init(0),

	refresh_initial_line(false),
	refresh_file(false),
	refresh_line(false),
	refresh_pwd(false),
	refresh_bpoints(false),
	refresh_where(false),
	refresh_frame(false),
	refresh_registers(false),
	refresh_threads(false),
	refresh_data(false),
	refresh_user(false),
	refresh_addr(false),
	refresh_disp_info(false),
	refresh_history_filename(false),
	refresh_history_size(false),
	refresh_history_save(false),
	refresh_setting(false),
	set_command(""),
	break_arg(""),
	n_refresh_data(0),
	n_refresh_user(0),
	n_refresh_addr(0),

	config_frame(false),
	config_run_io(false),
	config_print_r(false),
	config_where_h(false),
	config_display(false),
	config_clear(false),
	config_pwd(false),
	config_setenv(false),
	config_edit(false),
	config_make(false),
	config_named_values(false),
	config_when_semicolon(false),
	config_delete_comma(false),
	config_err_redirection(false),
	config_xdb(false),
	config_output(false),
	config_program_language(false)
    {}
};

static void user_cmdOA  (const string&, void *);
static void user_cmdOAC (void *);
static void plusOQAC (string [], void *[], int, void *);

static string print_cookie = "4711";

// ***************************************************************************
//

inline string str(String s)
{
    return s != 0 ? s : "";
}

void start_gdb()
{
    CmdData* cmd_data     = new CmdData;
    cmd_data->filter_disp = NoFilter;       // keine Display-Ausgaben
    cmd_data->pos_buffer  = new PosBuffer(); // ggf. Position lesen

    PlusCmdData* plus_cmd_data = new PlusCmdData ();
    StringArray cmds;
    VoidArray dummy;

    // Set up initialization commands
    string init;

    switch (gdb->type())
    {
    case GDB:
	init = str(app_data.gdb_initial_cmds) + str(app_data.gdb_settings);
	break;

    case DBX:
	init = str(app_data.dbx_initial_cmds) + str(app_data.dbx_settings);
	break;

    case XDB:
	init = str(app_data.xdb_initial_cmds) + str(app_data.xdb_settings);
	break;
    }

    plus_cmd_data->n_init = init.freq('\n');
    while (init != "")
    {
	cmds += init.before('\n');
	init = init.after('\n');
    }

    // Some additional commands with reply handling
    switch (gdb->type())
    {
    case GDB:
	cmds += "list";		// Required to load symbol table
	cmds += "info line";
	plus_cmd_data->refresh_initial_line = true;
	cmds += "output " + print_cookie;
	plus_cmd_data->config_output = true;
	cmds += "show language";
	plus_cmd_data->config_program_language = true;
	cmds += "pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "info breakpoints";
	plus_cmd_data->refresh_bpoints = true;
	cmds += "show history filename";
	plus_cmd_data->refresh_history_filename = true;
	cmds += "show history size";
	plus_cmd_data->refresh_history_size = true;
	cmds += "show history save";
	plus_cmd_data->refresh_history_save = true;
	break;

    case DBX:
	plus_cmd_data->refresh_initial_line = true;

	cmds += "frame";
	plus_cmd_data->config_frame = true;
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
	cmds += "pwd";
	plus_cmd_data->config_pwd = true;
	cmds += "help setenv";
	plus_cmd_data->config_setenv = true;
	cmds += "help edit";
	plus_cmd_data->config_edit = true;
	cmds += "help make";
	plus_cmd_data->config_make = true;
	cmds += "print \"" DDD_NAME "\"";
	plus_cmd_data->config_named_values = true;
	cmds += "help when";
	plus_cmd_data->config_when_semicolon = true;
	cmds += "delete " + print_cookie + " " + print_cookie;
	plus_cmd_data->config_delete_comma = true;
	cmds += "help run";
	plus_cmd_data->config_err_redirection = true;

	cmds += "sh pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "file";
	plus_cmd_data->refresh_file = true;
	cmds += "list";
	plus_cmd_data->refresh_line = true;
	cmds += "status";
	plus_cmd_data->refresh_bpoints = true;
	break;

    case XDB:
	cmds += "L";
	plus_cmd_data->refresh_initial_line = true;
	cmds += "tm";
	plus_cmd_data->config_xdb = true;
	cmds += "!pwd";
	plus_cmd_data->refresh_pwd = true;
	cmds += "lb";
	plus_cmd_data->refresh_bpoints = true;
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
}



// ***************************************************************************
// Send the GDB command CMD to GDB.
// No special processing whatsoever.

void user_rawSUC (string cmd, Widget origin)
{
    CmdData* cmd_data      = new CmdData(NoFilter);
    cmd_data->disp_buffer  = new DispBuffer;
    cmd_data->pos_buffer   = new PosBuffer;
    cmd_data->new_exec_pos = true;

    bool send_ok = gdb->send_user_ctrl_cmd(cmd, cmd_data);
    if (!send_ok)
	post_gdb_busy(origin);
}



// ***************************************************************************
// Send user command CMD to GDB.  Invoke CALLBACK with DATA upon
// completion.  If VERBOSE is set, issue command in GDB console.
// If CHECK is set, add appropriate GDB commands to get GDB state.

void user_cmdSUC (string cmd, Widget origin,
		  OQCProc callback, void *data,
		  bool verbose, bool check)
{
    // Pass control commands unprocessed to GDB.
    if (cmd.length() == 1 && iscntrl(cmd[0]))
    {
	// Don't issue control characters at the GDB prompt
	if (verbose && !gdb->isReadyWithPrompt())
	{
	    char c = cmd[0];

	    if (c < ' ')
		gdb_out("^" + string('@' + int(c)));
	    else
		gdb_out("^?");	// DEL
	}

	user_rawSUC(cmd, origin);
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
	    // We do not wait for GDB output.  Pass CMD unprocessed to
	    // GDB, leaving current user_data unharmed.
	    cmd += '\n';
	    send_ok = gdb->send_user_ctrl_cmd(cmd);
	}

	if (!send_ok)
	    post_gdb_busy(origin);
	return;
    }

    // Catch internal commands
    if (is_graph_cmd (cmd))
    {
	handle_graph_cmd(cmd, origin, callback, data, verbose, check);
	return;
    }

    // Ordinary GDB command

    // Setup extra command information
    CmdData* cmd_data       = new CmdData;
    cmd_data->disp_buffer   = new DispBuffer;
    cmd_data->pos_buffer    = new PosBuffer;
    cmd_data->user_callback = callback;
    cmd_data->user_data     = data;
    cmd_data->user_verbose  = verbose;

    PlusCmdData* plus_cmd_data     = new PlusCmdData;

    // Breakpoints may change any time
    plus_cmd_data->refresh_bpoints = true;

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

    if (data_disp->count_data_displays() == 0)
    {
	// No displays
	cmd_data->filter_disp = NoFilter;
    }

    if (!check || is_nop_cmd(cmd))
    {
	cmd_data->filter_disp            = NoFilter;
	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_addr      = false;
	plus_cmd_data->refresh_user      = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_frame     = false;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_addr      = false;
    }
    else if (is_file_cmd(cmd, gdb))
    {
	// File may change: display main() function and update displays
	plus_cmd_data->refresh_disp_info    = true;
	plus_cmd_data->refresh_initial_line = true;
	plus_cmd_data->refresh_data         = true;
	
	switch (gdb->type())
	{
	case DBX:
	    plus_cmd_data->refresh_file      = true;
	    plus_cmd_data->refresh_line      = true;
	    break;
	case GDB:
	    break;
	case XDB:
	    break;		// FIXME
	}
    }
    else if (is_single_display_cmd(cmd, gdb))
    {
	// No new displays
	cmd_data->filter_disp = NoFilter;

	// Breakpoints, Frames, Code and Registers won't change
	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_frame     = false;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_addr      = false;
    }
    else if (is_data_cmd(cmd))
    {
	plus_cmd_data->refresh_data      = true;

	// Breakpoints, Frames, Code and Registers won't change
	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_frame     = false;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_addr      = false;
    }
    else if (is_running_cmd(cmd, gdb))
    {
	// New displays and new exec position
	cmd_data->filter_disp = Filter;
	cmd_data->new_exec_pos = true;
	if (gdb->type() == DBX)
	{
	    plus_cmd_data->refresh_file  = true;
	    // plus_cmd_data->refresh_line  = true;
	    if (gdb->has_frame_command())
		plus_cmd_data->refresh_frame = true;
	}
	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_frame_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp           = NoFilter;
	cmd_data->new_frame_pos         = true;

	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_frame     = true;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_data      = true;

	switch (gdb->type())
	{
	case GDB:
	case XDB:
	    break;

	case DBX:
	    // We need to get the current file as well...
	    plus_cmd_data->refresh_file  = true;
	    break;
	}
	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_thread_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp            = NoFilter;
	cmd_data->new_frame_pos          = true;

	plus_cmd_data->refresh_bpoints   = true;
	plus_cmd_data->refresh_where     = true;
	plus_cmd_data->refresh_frame     = true;
	plus_cmd_data->refresh_data      = true;
	plus_cmd_data->refresh_threads   = true;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_set_cmd(cmd))
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
	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_addr      = false;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_data = true;
    }
    else if (is_cd_cmd(cmd))
    {
	plus_cmd_data->refresh_pwd       = true;
	plus_cmd_data->refresh_bpoints   = false;
	plus_cmd_data->refresh_where     = false;
	plus_cmd_data->refresh_frame     = false;
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
	plus_cmd_data->refresh_addr      = false;
    }
    else if (is_setting_cmd(cmd))
    {
	get_settings(gdb->type());
	plus_cmd_data->refresh_setting = true;
	plus_cmd_data->set_command     = cmd;
	plus_cmd_data->refresh_data    = false;
	plus_cmd_data->refresh_addr    = false;
    }

    if (is_break_cmd(cmd))
    {
	plus_cmd_data->break_arg = get_break_expression(cmd);
    }

    if (cmd_data->new_exec_pos
	|| plus_cmd_data->refresh_frame 
	|| plus_cmd_data->refresh_data)
    {
	// New program state: clear value cache
	clear_value_cache();
    }

    if (plus_cmd_data->refresh_frame && !gdb->has_frame_command())
    {
	// We have a backtrace window open, but DBX has no ``frame''
	// command to set the selected frame.  Use this hack instead.
	string arg_s = cmd.after(RXblanks_or_tabs);

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
    {
	plus_cmd_data->refresh_registers = false;
	plus_cmd_data->refresh_threads   = false;
    }

    if (verbose)
    {
	gdb_out(cmd);
	gdb_out("\n");
    }

    StringArray cmds;
    VoidArray dummy;

    assert(plus_cmd_data->n_init == 0);
    assert(!plus_cmd_data->config_frame);
    assert(!plus_cmd_data->config_run_io);
    assert(!plus_cmd_data->config_print_r);
    assert(!plus_cmd_data->config_where_h);
    assert(!plus_cmd_data->config_display);
    assert(!plus_cmd_data->config_clear);
    assert(!plus_cmd_data->config_pwd);
    assert(!plus_cmd_data->config_setenv);
    assert(!plus_cmd_data->config_edit);
    assert(!plus_cmd_data->config_make);
    assert(!plus_cmd_data->config_named_values);
    assert(!plus_cmd_data->config_when_semicolon);
    assert(!plus_cmd_data->config_delete_comma);
    assert(!plus_cmd_data->config_err_redirection);
    assert(!plus_cmd_data->config_xdb);
    assert(!plus_cmd_data->config_output);
    assert(!plus_cmd_data->config_program_language);
    
    // Setup additional trailing commands
    switch (gdb->type())
    {
    case GDB:
	if (plus_cmd_data->refresh_initial_line)
	{
	    cmds += "list";	// Required to load symbol table
	    cmds += "info line";
	}
	if (plus_cmd_data->refresh_pwd)
	    cmds += "pwd";
	assert(!plus_cmd_data->refresh_file);
	assert(!plus_cmd_data->refresh_line);
	if (plus_cmd_data->refresh_bpoints)
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
	if (plus_cmd_data->refresh_addr)
	    plus_cmd_data->n_refresh_addr = 
		data_disp->add_refresh_addr_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += "info display";
	if (plus_cmd_data->refresh_history_filename)
	    cmds += "show history filename";
	if (plus_cmd_data->refresh_history_size)
	    cmds += "show history size";
	if (plus_cmd_data->refresh_history_save)
	    cmds += "show history save";
	if (plus_cmd_data->refresh_setting)
	{
	    string show_command = "show ";
	    if (cmd.contains("set ", 0))
		show_command += cmd.after("set ");
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
	if (plus_cmd_data->refresh_file)
	    cmds += "file";
	if (plus_cmd_data->refresh_line)
	    cmds += "list";
	if (plus_cmd_data->refresh_bpoints)
	    cmds += "status";
	if (plus_cmd_data->refresh_where)
	    cmds += "where";
	if (plus_cmd_data->refresh_frame)
	{
	    assert(gdb->has_frame_command());
	    cmds += gdb->frame_command();
	}
	assert (!plus_cmd_data->refresh_registers);
	assert (!plus_cmd_data->refresh_threads);
	if (plus_cmd_data->refresh_data)
	    plus_cmd_data->n_refresh_data = 
		data_disp->add_refresh_data_commands(cmds);
	if (plus_cmd_data->refresh_user)
	    plus_cmd_data->n_refresh_user = 
		data_disp->add_refresh_user_commands(cmds);
	if (plus_cmd_data->refresh_addr)
	    plus_cmd_data->n_refresh_addr = 
		data_disp->add_refresh_addr_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_history_save);
	if (plus_cmd_data->refresh_setting)
	    cmds += cmd.before(rxwhite);
	break;

    case XDB:
	if (plus_cmd_data->refresh_initial_line)
	    cmds += "L";
	if (plus_cmd_data->refresh_pwd)
	    cmds += "!pwd";
	assert(!plus_cmd_data->refresh_file);
	assert(!plus_cmd_data->refresh_line);
	if (plus_cmd_data->refresh_bpoints)
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
	if (plus_cmd_data->refresh_addr)
	    plus_cmd_data->n_refresh_addr = 
		data_disp->add_refresh_addr_commands(cmds);
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_history_save);
	assert (!plus_cmd_data->refresh_setting);
	break;
    }

    while (dummy.size() < cmds.size())
	dummy += (void *)0;

    // Send commands
    bool send_ok;
    if (cmds.size() > 0)
    {
	send_ok = 
	    gdb->send_user_cmd_plus(cmds, dummy, cmds.size(),
				    plusOQAC, (void*)plus_cmd_data,
				    cmd, (void *)cmd_data);
    }
    else
    {
	send_ok = gdb->send_user_cmd (cmd, (void *)cmd_data);
	delete plus_cmd_data;
    }

    if (!send_ok)
	post_gdb_busy(origin);
}


// ***************************************************************************
// Filtert und buffert ggf. die Displays aus der Antwort und schreibt uebrige
// Antwort ins gdb_w. Das data-Argument bestimmt das Filtering.
//
void user_cmdOA (const string& answer, void* data) 
{
    string ans = answer;
    CmdData* cmd_data = (CmdData *) data;
    cmd_data->pos_buffer->filter(ans);

    if (cmd_data->filter_disp != NoFilter)
    {
	// Displays abfangen und buffern, Rest ausgeben
	cmd_data->disp_buffer->filter(ans);
    }

    cmd_data->user_answer += ans;

    if (cmd_data->user_verbose)
	gdb_out(ans);
}

// ***************************************************************************
// Schreibt den prompt ins gdb_w, nachdem ggf. gebufferte Displays abgearbeitet
// sind und evtl. Restantworten ausgegeben sind (ins gdb_w).
//

// These two are required for the DBX `file' command.
// DBX does not issue file names when stopping, so use these instead.
static string last_pos_found;	// Last position found
static bool last_new_exec_pos;	// True if last command was new exec position
static bool last_new_frame_pos;	// True if last command was new frame position

void user_cmdOAC (void* data)
{
    CmdData* cmd_data = (CmdData *) data;

    string answer = cmd_data->pos_buffer->answer_ended();
    cmd_data->user_answer += answer;

    if (cmd_data->pos_buffer->started_found())
    {
	// Program has been restarted - clear position history
	source_view->clear_history();
    }

    if (cmd_data->pos_buffer->terminated_found())
    {
	// Program has been terminated - clear execution position
	source_view->clear_execution_position();
    }

    if (cmd_data->pos_buffer->recompiled_found())
    {
	// Program has been recompiled - clear code and source cache,
	// clear execution position, and reload current source.
	source_view->clear_code_cache();
	source_view->clear_file_cache();
	source_view->clear_execution_position();
	source_view->reload();
    }

    if (cmd_data->pos_buffer->auto_cmd_found())
    {
	// Program (or GDB) issued command to be executed by DDD
	gdb_batch(cmd_data->pos_buffer->get_auto_cmd());
    }

    // Set execution position
    if (cmd_data->pos_buffer->pos_found())
    {
	string pos  = cmd_data->pos_buffer->get_position();
	string func = cmd_data->pos_buffer->get_function();

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
	    }

	    if (file != "")
		pos = file + ':' + pos;
	}

	last_new_exec_pos =  cmd_data->new_exec_pos;
	last_new_frame_pos = cmd_data->new_frame_pos;

	if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	{
	    source_view->show_execution_position(pos, cmd_data->new_exec_pos);
	}
	else
	{
	    // Lookup command: do not change exec position
	    source_view->show_position(pos);
	}
    }
    else
    {
	// Delete old position
	if (cmd_data->new_exec_pos || cmd_data->pos_buffer->pc_found())
	    source_view->show_execution_position("", true);
    }

    // up/down is done: set frame position in backtrace window
    if (cmd_data->set_frame_pos)
	if (cmd_data->set_frame_func != "")
	    source_view->set_frame_func(cmd_data->set_frame_func);
	else
	    source_view->set_frame_pos(cmd_data->set_frame_arg);

    // Set PC position
    if (cmd_data->pos_buffer->pc_found())
    {
	string pc = cmd_data->pos_buffer->get_pc();
	if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	    source_view->show_pc(pc, XmHIGHLIGHT_SELECTED);
	else
	    source_view->show_pc(pc, XmHIGHLIGHT_NORMAL);
    }

    if (cmd_data->user_callback != 0)
    {
	// Invoke user-defined callback
	cmd_data->user_callback(cmd_data->user_answer, cmd_data->user_data);
    }

    if (cmd_data->user_verbose)
    {
	// Show answer
	gdb_out(answer);
    }

    // Process displays
    if (cmd_data->filter_disp != NoFilter)
    {
	if (cmd_data->user_verbose)
	    gdb_out(cmd_data->disp_buffer->answer_ended());

	if (cmd_data->filter_disp == Filter
	    || cmd_data->disp_buffer->displays_found())
	{
	    bool disabling_occurred;
	    string displays = cmd_data->disp_buffer->get_displays();
	    string not_my_displays = 
		data_disp->process_displays(displays, disabling_occurred);

	    if (cmd_data->user_verbose)
		gdb_out(not_my_displays);
	    
	    cmd_data->disp_buffer->clear();

	    // Did GDB disable any display?
	    if (disabling_occurred)
	    {
		cmd_data->filter_disp = Filter;
		gdb->send_user_cmd(gdb->display_command());
	    }
	}
    }

    prompt();

    cmd_data->pos_buffer->clear();
    delete cmd_data;
}

// ***************************************************************************
// Process DDD `graph' commands (graph display, graph refresh).

// Fetch display numbers from COMMAND
void read_numbers(string command, IntArray& numbers)
{
    while (has_nr(command))
	numbers += atoi(read_nr_str(command));
}

void handle_graph_cmd (string cmd, Widget origin, 
		       OQCProc callback, void *data, 
		       bool verbose, bool check)
{
    if (verbose)
	gdb_out(cmd + "\n");

    cmd = cmd.after ("graph ");
    if (is_display_cmd(cmd))
    {
	string rcmd = reverse(cmd);

	string depends_on = "";
	BoxPoint *pos = 0;

	for (;;)
	{
	    read_leading_blanks(rcmd);

	    {
		// Check for `dependent on DISPLAY'
		static regex rxdep("[ \t]+no[ \t]+tnedneped[ \t]+");
		int index = rcmd.index(rxdep);
		if (index >= 0)
		{
		    depends_on = reverse(rcmd.before(index));
		    read_leading_blanks(depends_on);
		    strip_final_blanks(depends_on);

		    rcmd = rcmd.after(index);
		    rcmd = rcmd.after("tnedneped");
		    continue;
		}
	    }

	    {
		// Check for `at X, Y' or `at (X, Y)'
		static regex 
		    rxat("[)]?[0-9]*[1-9]-?[ \t]*,[ \t]*[0-9]*[1-9]-?[(]?"
			 "[ \t]+ta[ \t]+.*");
		if (rcmd.matches(rxat))
		{
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
	    }

	    break;
	}

	cmd = reverse(rcmd);

	string display_expression = get_display_expression(cmd);
	data_disp->new_displaySQ(display_expression, pos, depends_on, origin);
    }
    else if (is_refresh_cmd(cmd))
    {
	data_disp->refresh_displaySQ(origin);
    }
    else if (is_data_cmd(cmd))
    {
	IntArray numbers;
	read_numbers(cmd.after("display"), numbers);

	if (is_delete_display_cmd (cmd))
	{
	    data_disp->delete_displaySQ(numbers);
	}
	else if (is_disable_display_cmd (cmd))
	{
	    data_disp->disable_displaySQ(numbers);
	}
	else if (is_enable_display_cmd (cmd))
	{
	    data_disp->enable_displaySQ(numbers);
	}
    }
    else
    {
	user_cmdSUC(cmd, origin, callback, data, verbose, check);
    }

    // Make sure the command queue is processed further
    processCommandQueue();
}


// ***************************************************************************
// Process output of configuration commands

bool is_known_command(const string& answer)
{
    string ans = downcase(answer);

    read_leading_blanks(ans);
    strip_final_blanks(ans);

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
	    && !ans.contains("unrecognized")         // AIX DBX & SUN DBX 1.0
	    && !ans.contains("no help available")    // AIX DBX
	    && !ans.contains("expected")             // SGI DBX
	    && !ans.contains("invoked in line mode") // SCO DBX
	    && !ans.contains("unknown", 0));         // XDB
}

static void process_init(string&)
{
    // Nothing yet...
}

static void process_config_frame(string& answer)
{
    gdb->has_frame_command(is_known_command(answer));
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
    static regex rxsemicolon_and_brace("; *[}]");
    gdb->has_when_semicolon(answer.contains(rxsemicolon_and_brace));
}

static void process_config_delete_comma(string& answer)
{
    gdb->has_delete_comma(!is_known_command(answer));
}

static void process_config_err_redirection(string& answer)
{
    gdb->has_err_redirection(answer.contains(">&"));
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


// ***************************************************************************
// Handle GDB answers to DDD questions sent after GDB command
//
void plusOQAC (string answers[],
	       void*  qu_datas[],
	       int    count,
	       void*  data)
{
    PlusCmdData* plus_cmd_data = (PlusCmdData*)data;
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
		// Handle `list' output
		assert (qu_count < count);
		string list = answers[qu_count++];

		// Skip initial message lines like `Reading symbols...'
		while (list != "" && !has_nr(list))
		    list = list.after('\n');

		if (atoi(list) == 0)
		{
		    // No listing => no source => ignore `info line' output
		    assert (qu_count < count);
		    qu_count++;
		}
		else
		{
		    // Handle `info line' output
		    assert (qu_count < count);
		    source_view->process_info_line_main(answers[qu_count++]);
		}
	    }
	    break;

	case XDB:
	    assert (qu_count < count);
	    source_view->process_info_line_main(answers[qu_count++]);
	    break;

	case DBX:
	    break;
	}
    }

    if (plus_cmd_data->config_xdb) {
	// Make sure XDB understands macros
	assert (qu_count < count);
	process_config_tm(answers[qu_count++]);
    }

    if (plus_cmd_data->config_frame) {
	assert (qu_count < count);
	process_config_frame(answers[qu_count++]);
    }

    if (plus_cmd_data->config_run_io) {
	assert (qu_count < count);
	process_config_run_io(answers[qu_count++]);
    }

    if (plus_cmd_data->config_print_r) {
	assert (qu_count < count);
	process_config_print_r(answers[qu_count++]);
    }

    if (plus_cmd_data->config_where_h) {
	assert (qu_count < count);
	process_config_where_h(answers[qu_count++]);
    }

    if (plus_cmd_data->config_display) {
	assert (qu_count < count);
	process_config_display(answers[qu_count++]);
    }

    if (plus_cmd_data->config_clear) {
	assert (qu_count < count);
	process_config_clear(answers[qu_count++]);
    }

    if (plus_cmd_data->config_pwd) {
	assert (qu_count < count);
	process_config_pwd(answers[qu_count++]);
    }

    if (plus_cmd_data->config_setenv) {
	assert (qu_count < count);
	process_config_setenv(answers[qu_count++]);
    }

    if (plus_cmd_data->config_edit) {
	assert (qu_count < count);
	process_config_edit(answers[qu_count++]);
    }

    if (plus_cmd_data->config_make) {
	assert (qu_count < count);
	process_config_make(answers[qu_count++]);
    }

    if (plus_cmd_data->config_named_values) {
	assert (qu_count < count);
	process_config_named_values(answers[qu_count++]);
    }

    if (plus_cmd_data->config_when_semicolon) {
	assert (qu_count < count);
	process_config_when_semicolon(answers[qu_count++]);
    }

    if (plus_cmd_data->config_delete_comma) {
	assert (qu_count < count);
	process_config_delete_comma(answers[qu_count++]);
    }

    if (plus_cmd_data->config_err_redirection) {
	assert (qu_count < count);
	process_config_err_redirection(answers[qu_count++]);
    }

    if (plus_cmd_data->config_output) {
	assert (qu_count < count);
	process_config_output(answers[qu_count++]);
    }

    if (plus_cmd_data->config_program_language) {
	assert (qu_count < count);
	process_config_program_language(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_pwd) {
	assert (qu_count < count);
	source_view->process_pwd(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_file) {
	assert (gdb->type() == DBX);
	assert (qu_count < count);

	file = answers[qu_count++];

	// Simple sanity check
	strip_final_blanks(file);
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
							 true);
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
	assert (qu_count < count);

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

    if (plus_cmd_data->refresh_bpoints) {
	assert (qu_count < count);
	source_view->process_info_bp(answers[qu_count++], 
				     plus_cmd_data->break_arg);
	update_arg_buttons();
    }

    if (plus_cmd_data->refresh_where) {
	assert (qu_count < count);
	source_view->process_where(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_frame) {
	assert (qu_count < count);
	source_view->process_frame(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_registers) {
	assert (qu_count < count);
	source_view->process_registers(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_threads) {
	assert (qu_count < count);
	source_view->process_threads(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_data) {
	string ans = "";
	for (int i = 0; i < plus_cmd_data->n_refresh_data; i++)
	{
	    assert (qu_count < count);
	    ans += answers[qu_count++];
	}
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
    {
	StringArray answers(((string *)answers) + qu_count,
			    plus_cmd_data->n_refresh_addr);
	data_disp->process_addr(answers);
	qu_count += plus_cmd_data->n_refresh_addr;
    }

    if (plus_cmd_data->refresh_disp_info) {
	assert (qu_count < count);
	data_disp->process_info_display(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_history_filename) {
	assert (qu_count < count);
	process_history_filename(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_history_size) {
	assert (qu_count < count);
	process_history_size(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_history_save) {
	assert (qu_count < count);
	process_history_save(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_setting) {
	assert (qu_count < count);
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
    delete[] answers;
    delete[] qu_datas;
}
