// $Id$
// GDB communication manager.

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

char comm_manager_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementierung von comm-manag.h
// Namenskonventionen: 
// ...SUC : ruft send_user_cmd() des gdb auf.
// ...OA  : fuer on_answer, Typ: OAProc, siehe gdbAgent.
// ...OAC : fuer on_answer_completion, Typ: OACProc, siehe gdbAgent.
// ...HP  : Handler-Prozedur. Typ: HandlerProc, siehe HandlerL.h
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "comm-manag.h"

#include "bool.h"
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

#include <ctype.h>


void handle_graph_cmd (string cmd, Widget origin);
void send_display_cmdHP (void*, void* client_data, void* call_data);

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

    CmdData (Filtering   fd = TryFilter,
	     DispBuffer* db = 0,
	     PosBuffer*  pb = 0,
	     bool     nep= false,
	     bool     /* nfp */ = false) :
	filter_disp (fd),
	disp_buffer (db),
	pos_buffer (pb),
	new_exec_pos (nep),
	new_frame_pos (nep),
	set_frame_pos (false),
	set_frame_arg (0)
    {}
};


typedef struct PlusCmdData {
    bool     refresh_main;             // send 'info line main' / `func main'
    bool     refresh_file;             // send 'file'
    bool     refresh_line;             // send 'list' (DBX 1.x) or 'line'
    bool     refresh_pwd;	       // send 'pwd'
    bool     refresh_bpoints;          // send 'info b'
    bool     refresh_where;            // send 'where'
    bool     refresh_frame;            // send 'frame'
    bool     refresh_register;         // send 'info registers'
    bool     refresh_disp;             // send 'display'
    bool     refresh_disp_info;        // send 'info display'
    bool     refresh_history_filename; // send 'show history filename'
    bool     refresh_history_size;     // send 'show history size'
    bool     refresh_history_save;     // send 'show history save'

    bool     config_frame;	       // try 'frame'
    bool     config_line;	       // try 'line'
    bool     config_run_io;	       // try 'dbxenv run_io'
    bool     config_print_r;	       // try 'print -r'
    bool     config_where_h;	       // try 'where -h'
    bool     config_display;	       // try 'display'
    bool     config_clear;	       // try 'clear'
    bool     config_pwd;	       // try 'pwd'
    bool     config_named_values;      // try 'print "ddd"'
    bool     config_func_pos;          // try 'func main'
    bool     config_when_semicolon;    // try 'help when'
    bool     config_page;	       // try 'set $page = 0'

    PlusCmdData () :
	refresh_main(false),
	refresh_file(false),
	refresh_line(false),
	refresh_pwd(false),
	refresh_bpoints(false),
	refresh_where(false),
	refresh_frame(false),
	refresh_register(false),
	refresh_disp(false),
	refresh_disp_info(false),
	refresh_history_filename(false),
	refresh_history_size(false),
	refresh_history_save(false),

	config_frame(false),
	config_line(false),
	config_run_io(false),
	config_print_r(false),
	config_where_h(false),
	config_display(false),
	config_clear(false),
	config_pwd(false),
	config_named_values(false),
	config_func_pos(false),
	config_when_semicolon(false),
	config_page(false)
    {}
};

void user_cmdOA  (const string&, void *);
void user_cmdOAC (void *);
void plusOQAC (string [], void *[], int, void *);

static string print_r_cookie = "4711";

// ***************************************************************************
//
void start_gdb()
{
    CmdData* cmd_data = new CmdData ();
    cmd_data->filter_disp = NoFilter;       // keine Display-Ausgaben
    cmd_data->pos_buffer = new PosBuffer(); // ggf. Position lesen

    PlusCmdData* plus_cmd_data = new PlusCmdData ();
    StringArray cmds;
    VoidArray dummy;

    switch (gdb->type())
    {
    case GDB:
	cmds += "info line main";
	plus_cmd_data->refresh_main = true;
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
	plus_cmd_data->refresh_main = true;
	cmds += "frame";
	plus_cmd_data->config_frame = true;
	cmds += "line";
	plus_cmd_data->config_line = true;
	cmds += "dbxenv run_io";
	plus_cmd_data->config_run_io = true;
	cmds += "print -r " + print_r_cookie;
	plus_cmd_data->config_print_r = true;
	cmds += "where -h";
	plus_cmd_data->config_where_h = true;
	cmds += "display";
	plus_cmd_data->config_display = true;
	cmds += "clear";
	plus_cmd_data->config_clear = true;
	cmds += "pwd";
	plus_cmd_data->config_pwd = true;
	cmds += "print \"" DDD_NAME "\"";
	plus_cmd_data->config_named_values = true;
	cmds += "func main";
	plus_cmd_data->config_func_pos = true;
	cmds += "help when";
	plus_cmd_data->config_when_semicolon = true;
	cmds += "set $page = 0";
	plus_cmd_data->config_page = true;
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
	cmds += "v main";
	plus_cmd_data->refresh_main = true;
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
// Send the GDB command CMD to GDB.
// Handle filters, internal commands, control characters, echoing etc.

void user_cmdSUC (string cmd, Widget origin)
{
    // Pass control commands unprocessed to GDB.
    if (cmd.length() == 1 && iscntrl(cmd[0]))
    {
	// Don't issue control characters at the GDB prompt
	if (!gdb->isReadyWithPrompt())
	{
	    char c = cmd[0];

	    if (c < ' ')
		gdb_out(string("^") + string('@' + int(c)));
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
	handle_graph_cmd (cmd, origin);
	cmd += '\n';
	gdb_out(cmd);
	prompt();
	return;
    }

    // Ordinary GDB command

    // Setup extra command information
    CmdData* cmd_data              = new CmdData;
    PlusCmdData* plus_cmd_data     = new PlusCmdData;
    cmd_data->disp_buffer          = new DispBuffer;
    cmd_data->pos_buffer           = new PosBuffer;
    plus_cmd_data->refresh_bpoints = true;

    if (source_view->where_required())
    {
	plus_cmd_data->refresh_where = true;
	plus_cmd_data->refresh_frame = true;
    }

    if (source_view->register_required())
    {
	plus_cmd_data->refresh_register = true;
    }

    if (data_disp->count_all() == 0)
    {
	// No displays
	cmd_data->filter_disp = NoFilter;
    }

    if (is_file_cmd (cmd, gdb))
    {
	// File may change: display main() function and update displays
	plus_cmd_data->refresh_disp_info = true;
	plus_cmd_data->refresh_main      = true;
	plus_cmd_data->refresh_disp      = true;
	
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
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_frame    = false;
	plus_cmd_data->refresh_register = false;
    }
    else if (is_running_cmd(cmd, gdb))
    {
	// New displays and new exec position
	cmd_data->filter_disp = Filter;
	cmd_data->new_exec_pos = true;
	if (gdb->type() == DBX) {
	    plus_cmd_data->refresh_file  = true;
	    if (gdb->has_line_command())
		plus_cmd_data->refresh_line  = true;
	    if (gdb->has_frame_command())
		plus_cmd_data->refresh_frame = true;
	}
	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_disp = true;
    }
    else if (is_frame_cmd(cmd))
    {
	// Update displays
	cmd_data->filter_disp           = NoFilter;
	cmd_data->new_frame_pos         = true;

	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_frame    = true;
	plus_cmd_data->refresh_register = false;
	plus_cmd_data->refresh_disp     = true;

	switch (gdb->type())
	{
	case GDB:
	    break;

	case DBX:
	    // We need to get the current file as well...
	    plus_cmd_data->refresh_file  = true;
	    if (gdb->has_line_command())
		plus_cmd_data->refresh_line  = true;
	    break;

	case XDB:
	    break;		// FIXME
	}
	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_disp = true;
    }
    else if (is_set_cmd(cmd))
    {
	// Update displays
	plus_cmd_data->refresh_disp = true;
    }
    else if (is_lookup_cmd(cmd))
    {
	if (gdb->type() == DBX)
	{
	    if (!gdb->has_frame_command())
	    {
		// Check if this 'func' command should select a stack frame
		string func = cmd.after(RXblanks_or_tabs);
		if (func != "" && source_view->set_frame_func(func))
		{
		    // Func found in backtrace; everything okay
		}
		else
		{
		    // Update position
		    plus_cmd_data->refresh_file = true;

		    if (!gdb->has_func_pos())
			plus_cmd_data->refresh_line = true;
		}
	    }
	    else
	    {
		plus_cmd_data->refresh_file  = true;
		plus_cmd_data->refresh_line  = true;
		plus_cmd_data->refresh_frame = true;
	    }
	}
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_register = false;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_disp = true;
    }
    else if (is_cd_cmd(cmd))
    {
	plus_cmd_data->refresh_pwd      = true;
    }
    else if (is_nop_cmd(cmd))
    {
	cmd_data->filter_disp           = NoFilter;
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_frame    = false;
	plus_cmd_data->refresh_register = false;
    }
	
    if (plus_cmd_data->refresh_frame && !gdb->has_frame_command())
    {
	// We have a backtrace window open, but DBX has no ``frame''
	// command to set the selected frame.  Use this hack instead.
	if (is_up_cmd(cmd) || is_down_cmd(cmd))
	{
	    // Set the new selected frame from the `up'/`down'
	    // argument.

	    int arg;
	    string arg_s = cmd.after(RXblanks_or_tabs);
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
	else
	{
	    // Make bottom frame the selected frame
	    cmd_data->set_frame_pos = true;
	    cmd_data->set_frame_arg = 0;
	}

	plus_cmd_data->refresh_frame = false;

	if (!gdb->has_display_command())
	    plus_cmd_data->refresh_disp = true;
    }

    if (gdb->type() == DBX)
    {
	plus_cmd_data->refresh_register = false;
    }

    gdb_out(cmd);
    gdb_out("\n");

    StringArray cmds;
    VoidArray dummy;

    assert(!plus_cmd_data->config_frame);
    assert(!plus_cmd_data->config_line);
    assert(!plus_cmd_data->config_run_io);
    assert(!plus_cmd_data->config_print_r);
    assert(!plus_cmd_data->config_where_h);
    assert(!plus_cmd_data->config_display);
    assert(!plus_cmd_data->config_clear);
    assert(!plus_cmd_data->config_pwd);
    assert(!plus_cmd_data->config_named_values);
    assert(!plus_cmd_data->config_func_pos);
    assert(!plus_cmd_data->config_when_semicolon);
    assert(!plus_cmd_data->config_page);
    
    // Setup additional trailing commands
    switch (gdb->type())
    {
    case GDB:
	if (plus_cmd_data->refresh_main)
	    cmds += "info line main";
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
	if (plus_cmd_data->refresh_register)
	    cmds += "info registers";
	if (plus_cmd_data->refresh_disp)
	    cmds += data_disp->refresh_display_command();
	if (plus_cmd_data->refresh_disp_info)
	    cmds += "info display";
	if (plus_cmd_data->refresh_history_filename)
	    cmds += "show history filename";
	if (plus_cmd_data->refresh_history_size)
	    cmds += "show history size";
	if (plus_cmd_data->refresh_history_save)
	    cmds += "show history save";
	break;

    case DBX:
	if (plus_cmd_data->refresh_pwd)
	    cmds += "pwd";
	if (plus_cmd_data->refresh_file)
	    cmds += "file";
	if (plus_cmd_data->refresh_line)
	{
	    if (gdb->has_line_command())
		cmds += "line";
	    else
		cmds += "list";
	}
	if (plus_cmd_data->refresh_bpoints)
	    cmds += "status";
	if (plus_cmd_data->refresh_where)
	    cmds += "where";
	if (plus_cmd_data->refresh_frame)
	{
	    assert(gdb->has_frame_command());
	    cmds += gdb->frame_command();
	}
	assert (!plus_cmd_data->refresh_register);
	if (plus_cmd_data->refresh_disp)
	    cmds += data_disp->refresh_display_command();
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_history_save);
	break;

    case XDB:
	if (plus_cmd_data->refresh_main)
	    cmds += "v main";
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
	assert (!plus_cmd_data->refresh_register);
	if (plus_cmd_data->refresh_disp)
	    cmds += data_disp->refresh_display_command();
	if (plus_cmd_data->refresh_disp_info)
	    cmds += gdb->display_command();
	assert (!plus_cmd_data->refresh_history_filename);
	assert (!plus_cmd_data->refresh_history_size);
	assert (!plus_cmd_data->refresh_history_save);
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
    cmd_data->pos_buffer->filter (ans);

    if (cmd_data->filter_disp == NoFilter)
	gdb_out(ans);
    else {
	// Displays abfangen und buffern, Rest ausgeben
	cmd_data->disp_buffer->filter (ans);
	gdb_out(ans);
    }
}



// For DBXes that issue positions on `func' commands, save the line found.
static int last_lookup_line = 0;

// ***************************************************************************
// Schreibt den prompt ins gdb_w, nachdem ggf. gebufferte Displays abgearbeitet
// sind und evtl. Restantworten ausgegeben sind (ins gdb_w).
//

void user_cmdOAC (void* data)
{
    last_lookup_line = 0;

    CmdData* cmd_data = (CmdData *) data;

    string answer = cmd_data->pos_buffer->answer_ended();

    if (cmd_data->pos_buffer->started_found())
    {
	// Program has been restarted - clear position history
	source_view->clear_history();
    }

    if (cmd_data->pos_buffer->recompiled_found())
    {
	// Program has been recompiled - 
	// clear code and source cache and reload current source.
	source_view->clear_code_cache();
	source_view->clear_file_cache();
	source_view->reload();
    }

    // Set execution/frame position
    if (cmd_data->pos_buffer->pos_found())
    {
	string pos  = cmd_data->pos_buffer->get_position();
	string func = cmd_data->pos_buffer->get_function();

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

	if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	{
	    source_view->show_execution_position(pos, cmd_data->new_exec_pos);

	    // Up/Down succeeded: set frame position in backtrace window
	    if (cmd_data->set_frame_pos)
		source_view->set_frame_pos(cmd_data->set_frame_arg);
	}
	else
	{
	    // Lookup command: do not change exec position
	    source_view->show_position(pos);
	}
    }
    else if (cmd_data->pos_buffer->lookup_found())
    {
	last_lookup_line = atoi(cmd_data->pos_buffer->get_lookup());
    }
    else
    {
	// Delete old position
	if (cmd_data->new_exec_pos)
	    source_view->show_execution_position("", true); 
    }

    gdb_out(answer);

    // Set PC position
    if (cmd_data->pos_buffer->pc_found())
    {
	string pc = cmd_data->pos_buffer->get_pc();
	if (cmd_data->new_exec_pos || cmd_data->new_frame_pos)
	    source_view->show_pc(pc, XmHIGHLIGHT_SELECTED);
	else
	    source_view->show_pc(pc, XmHIGHLIGHT_NORMAL);
    }

    // Process displays
    if (cmd_data->filter_disp == NoFilter)
    {
	prompt();
    }
    else 
    {
	gdb_out(cmd_data->disp_buffer->answer_ended());

	if (cmd_data->filter_disp == Filter
	    || cmd_data->disp_buffer->displays_found())
	{

	    bool disabling_occurred;
	    string displays = cmd_data->disp_buffer->get_displays();
	    string not_my_displays = 
		data_disp->process_displays(displays, disabling_occurred);
	    gdb_out(not_my_displays);

	    cmd_data->disp_buffer->clear();

	    // War letztes display disabling.... ?
 	    if (disabling_occurred)
	    {
		cmd_data->filter_disp = Filter;
		gdb->send_user_cmd(gdb->display_command());
 		return;
 	    }
	}

	prompt();
    }

    cmd_data->pos_buffer->clear();
    delete cmd_data;
}

// ***************************************************************************
// Behandelt die Programmeigenen graph-Befehle (insbes. graph display ... und
// graph refresh).
//
void handle_graph_cmd (string cmd, Widget origin)
{
    cmd = cmd.after ("graph ");
    if (is_display_cmd (cmd)) {
	string display_expression = get_display_expression (cmd);
	data_disp->new_displaySQ(display_expression, 0, origin);
    }
    else if (is_refresh_cmd (cmd)) {
	data_disp->refresh_displaySQA(origin);
    }
    else {
	user_cmdSUC(cmd, origin);
    }
}


// ***************************************************************************
// Process output of configuration commands

static bool is_known_command(string& answer)
{
    return answer.contains("program is not active") // DBX
	|| (!answer.contains("syntax")              // DEC DBX
	    && !answer.contains("help")             // GDB & DBX 1.0
	    && !answer.contains("not found")        // DBX 3.0
	    && !answer.contains("Unknown", 0));     // XDB
}

static void process_config_frame(string& answer)
{
    gdb->has_frame_command(is_known_command(answer));
}

static void process_config_line(string& answer)
{
    gdb->has_line_command(is_known_command(answer));
}

static void process_config_run_io(string& answer)
{
    gdb->has_run_io_command(is_known_command(answer));
}

static void process_config_print_r(string& answer)
{
    gdb->has_print_r_command(is_known_command(answer) 
			     && answer.contains(print_r_cookie));
}

static void process_config_where_h(string& answer)
{
    gdb->has_where_h_command(is_known_command(answer));
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

static void process_config_named_values(string& answer)
{
    gdb->has_named_values(answer.contains(" = "));
}

static void process_config_func_pos(string& answer)
{
    static regex RXcolon_and_line_number(": *[0-9][0-9]*");
    if (answer.contains(RXcolon_and_line_number))
	gdb->has_func_pos(true);
}

static void process_config_when_semicolon(string& answer)
{
    gdb->has_when_semicolon(answer.contains(';'));
}

static void process_config_page(string&)
{
    // Nothing yet...
}


// ***************************************************************************
// Behandelt die Antworten auf die hinterhergeschickten Anfragen
//
void plusOQAC (string answers[],
	       void*  qu_datas[],
	       int    count,
	       void*  data)
{
    PlusCmdData* plus_cmd_data = (PlusCmdData*)data;
    int qu_count = 0;
    string file;

    if (plus_cmd_data->refresh_main) {
	assert (qu_count < count);
	switch (gdb->type())
	{
	case GDB:
	case XDB:
	    source_view->process_info_line_main(answers[qu_count++]);
	    break;

	case DBX:
	    break;
	}
    }

    if (plus_cmd_data->config_frame) {
	assert (qu_count < count);
	process_config_frame(answers[qu_count++]);
    }

    if (plus_cmd_data->config_line) {
	assert (qu_count < count);
	process_config_line(answers[qu_count++]);
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

    if (plus_cmd_data->config_named_values) {
	assert (qu_count < count);
	process_config_named_values(answers[qu_count++]);
    }

    if (plus_cmd_data->config_func_pos) {
	assert (qu_count < count);
	process_config_func_pos(answers[qu_count++]);
    }

    if (plus_cmd_data->config_when_semicolon) {
	assert (qu_count < count);
	process_config_when_semicolon(answers[qu_count++]);
    }

    if (plus_cmd_data->config_page) {
	assert (qu_count < count);
	process_config_page(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_pwd) {
	assert (qu_count < count);
	source_view->process_pwd(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_file) {
	assert (gdb->type() == DBX);
	assert (qu_count < count);

	file = answers[qu_count++];
	if (file.contains('\n'))
	    file = file.before('\n');

	if (!plus_cmd_data->refresh_line)
	{
	    // Set the correct lookup position now.
	    string pos = file + ":" + itostring(last_lookup_line);
	    source_view->lookup(pos);
	}
    }

    if (plus_cmd_data->refresh_line)
    {
	assert (gdb->type() == DBX);
	assert (qu_count < count);

	string listing = answers[qu_count++];

	string message = "";
	while (listing != "" && atoi(listing) == 0)
	{
	    message += listing.through('\n');
	    listing = listing.after('\n');
	}

	if (message != "")
	    post_gdb_message(message);

	int line = atoi(listing);
	if (line == 0)
	{
	    // Weird.  No source?
	    line = 1;
	}
	else if (!plus_cmd_data->refresh_main)
	{
	    // Older DBX 1.0 lists 10 lines; the current line is the
	    // 5th one.  With DBX 3.0, we use the "line" command; and
	    // even if "list" was used (as on startup) we don't add 5.
	    if (!gdb->has_line_command())
	    {
		line += 5;
	    }
	}

	if (file != "")
	    source_view->lookup(file + ":" + itostring(line));
    }

    if (plus_cmd_data->refresh_bpoints) {
	assert (qu_count < count);
	source_view->process_info_bp(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_where) {
	assert (qu_count < count);
	source_view->process_where(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_frame) {
	assert (qu_count < count);
	source_view->process_frame(answers[qu_count++]);
    }

    if (plus_cmd_data->refresh_register) {
	assert (qu_count < count);
	source_view->process_register(answers[qu_count++]);
    }

    bool disabling_occurred = false;
    if (plus_cmd_data->refresh_disp) {
	assert (qu_count < count);
	data_disp->process_displays(answers[qu_count++], disabling_occurred);
	if (disabling_occurred) {
	    data_disp->refresh_displaySQ();
	}
    }

    if (plus_cmd_data->refresh_disp_info) {
	assert (qu_count < count);
	if (!disabling_occurred)
	    data_disp->process_info_display(answers[qu_count++]);
	else
	    qu_count++;
    }

    if (plus_cmd_data->refresh_history_filename) {
	assert (qu_count < count);
	if (!disabling_occurred)
	    process_history_filename(answers[qu_count++]);
	else
	    qu_count++;
    }

    if (plus_cmd_data->refresh_history_size) {
	assert (qu_count < count);
	if (!disabling_occurred)
	    process_history_size(answers[qu_count++]);
	else
	    qu_count++;
    }

    if (plus_cmd_data->refresh_history_save) {
	assert (qu_count < count);
	if (!disabling_occurred)
	    process_history_save(answers[qu_count++]);
	else
	    qu_count++;
    }

    assert (qu_count == count);
    if (qu_count != count)
	abort();

    delete plus_cmd_data;
    delete[] answers;
    delete[] qu_datas;
}
