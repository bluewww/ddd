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
#include "disp-read.h"
#include "DispBuffer.h"
#include "PosBuffer.h"
#include "string-fun.h"

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
    bool     refresh_line;             // send 'list'
    bool     refresh_bpoints;          // send 'info b'
    bool     refresh_where;            // send 'where'
    bool     refresh_frame;            // send 'frame'
    bool     refresh_register;         // send 'info registers'
    bool     refresh_disp;             // send 'display'
    bool     refresh_disp_info;        // send 'info display'
    bool     refresh_history_filename; // send 'show history filename'
    bool     refresh_history_size;     // send 'show history size'
    bool     refresh_history_save;     // send 'show history save'

    PlusCmdData () :
	refresh_main(false),
	refresh_file(false),
	refresh_line(false),
	refresh_bpoints(false),
	refresh_where(false),
	refresh_frame(false),
	refresh_register(false),
	refresh_disp_info(false),
	refresh_history_filename(false),
	refresh_history_size(false),
	refresh_history_save(false)
    {}
};

void user_cmdOA  (const string&, void *);
void user_cmdOAC (void *);
void plusOQAC (string [], void *[], int, void *);

// ***************************************************************************
//
void start_gdb()
{
    CmdData* cmd_data = new CmdData ();
    cmd_data->filter_disp = NoFilter;       // keine Display-Ausgaben
    cmd_data->pos_buffer = new PosBuffer(); // ggf. Position lesen

    PlusCmdData* plus_cmd_data = new PlusCmdData ();
    string cmds[10];
    void* dummy[10];
    int qu_count = 0;

    switch (gdb->type())
    {
    case GDB:
	cmds[qu_count++] = "info line main";
	plus_cmd_data->refresh_main = true;
	cmds[qu_count++] = "info breakpoints";
	plus_cmd_data->refresh_bpoints = true;
	cmds[qu_count++] = "show history filename";
	plus_cmd_data->refresh_history_filename = true;
	cmds[qu_count++] = "show history size";
	plus_cmd_data->refresh_history_size = true;
	cmds[qu_count++] = "show history save";
	plus_cmd_data->refresh_history_save = true;
	break;

    case DBX:
	plus_cmd_data->refresh_main = true;
	cmds[qu_count++] = "file";
	plus_cmd_data->refresh_file = true;
	cmds[qu_count++] = "list";
	plus_cmd_data->refresh_line = true;
	cmds[qu_count++] = "status";
	plus_cmd_data->refresh_bpoints = true;
	break;
    }

    gdb->start_plus (user_cmdOA,
		     user_cmdOAC,
		     cmd_data,
		     cmds,
		     dummy,
		     qu_count,
		     plusOQAC,
		     (void*)plus_cmd_data);
}



// ***************************************************************************
// Send the GDB command CMD to GDB.
// No special processing whatsoever.

void user_rawSUC (string cmd, Widget origin)
{
    CmdData* cmd_data     = new CmdData(NoFilter);
    cmd_data->disp_buffer = new DispBuffer;
    cmd_data->pos_buffer  = new PosBuffer;

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

    if (is_file_cmd (cmd, gdb->type()))
    {
	// File may change: display main() function and update displays
	plus_cmd_data->refresh_disp_info = true;
	plus_cmd_data->refresh_main      = true;

	switch (gdb->type())
	{
	case DBX:
	    plus_cmd_data->refresh_file      = true;
	    plus_cmd_data->refresh_line      = true;
	    break;

	case GDB:
	    plus_cmd_data->refresh_disp      = true;
	    break;
	}
    }
    else if (is_single_display_cmd(cmd, gdb->type()))
    {
	// No new displays
	cmd_data->filter_disp = NoFilter;

	// Breakpoints, Frames, Code and Registers won't change
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_frame    = false;
	plus_cmd_data->refresh_register = false;
    }
    else if (is_running_cmd(cmd, gdb->type()))
    {
	// New displays and new exec position
	cmd_data->filter_disp = Filter;
	cmd_data->new_exec_pos = true;
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

	switch (gdb->type())
	{
	case GDB:
	    // Update displays
	    plus_cmd_data->refresh_disp    = true;
	    break;

	case DBX:
	    // We need to get the current file as well...
	    plus_cmd_data->refresh_file  = true;
	    break;
	}
    }
    else if (is_set_cmd(cmd))
    {
	// Update displays
	plus_cmd_data->refresh_disp    = true;
    }
    else if (is_lookup_cmd(cmd))
    {
	if (gdb->type() == DBX)
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
		plus_cmd_data->refresh_file      = true;
		plus_cmd_data->refresh_line      = true;
	    }
	}
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_register = false;
    }
    else if (is_nop_cmd(cmd))
    {
	cmd_data->filter_disp           = NoFilter;
	plus_cmd_data->refresh_bpoints  = false;
	plus_cmd_data->refresh_where    = false;
	plus_cmd_data->refresh_frame    = false;
	plus_cmd_data->refresh_register = false;
    }
	
    if (gdb->type() == DBX && plus_cmd_data->refresh_frame)
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
		if (is_up_cmd(cmd))
		    cmd_data->set_frame_arg = -arg;
		else
		    cmd_data->set_frame_arg = arg;
	    }
	}
	else
	{
	    // Make bottom frame the selected frame
	    cmd_data->set_frame_pos = true;
	    cmd_data->set_frame_arg = 0;
	}

	plus_cmd_data->refresh_frame = false;
    }

    if (gdb->type() == DBX)
    {
	plus_cmd_data->refresh_register = false;
    }

    gdb_out(cmd);
    gdb_out("\n");

    string cmds[20];
    void*  dummy[20];
    int qu_count = 0;
    
    // Setup additional trailing commands
    switch (gdb->type())
    {
    case GDB:
	if (plus_cmd_data->refresh_main)
	    cmds[qu_count++] = "info line main";
	if (plus_cmd_data->refresh_file)
	    assert(0);
	if (plus_cmd_data->refresh_line)
	    assert(0);
	if (plus_cmd_data->refresh_bpoints)
	    cmds[qu_count++] = "info breakpoints";
	if (plus_cmd_data->refresh_where)
	    cmds[qu_count++] = "where";
	if (plus_cmd_data->refresh_frame)
	    cmds[qu_count++] = "frame";
	if (plus_cmd_data->refresh_register)
	    cmds[qu_count++] = "info registers";
	if (plus_cmd_data->refresh_disp)
	    cmds[qu_count++] = "display";
	if (plus_cmd_data->refresh_disp_info)
	    cmds[qu_count++] = "info display";
	if (plus_cmd_data->refresh_history_filename)
	    cmds[qu_count++] = "show history filename";
	if (plus_cmd_data->refresh_history_size)
	    cmds[qu_count++] = "show history size";
	if (plus_cmd_data->refresh_history_save)
	    cmds[qu_count++] = "show history save";
	break;

    case DBX:
	if (plus_cmd_data->refresh_file)
	    cmds[qu_count++] = "file";
	if (plus_cmd_data->refresh_line)
	    cmds[qu_count++] = "list";
	if (plus_cmd_data->refresh_bpoints)
	    cmds[qu_count++] = "status";
	if (plus_cmd_data->refresh_where)
	    cmds[qu_count++] = "where";
	if (plus_cmd_data->refresh_frame)
	    assert(0);
	if (plus_cmd_data->refresh_register)
	    assert(0);
	if (plus_cmd_data->refresh_disp)
	    assert(0);
	if (plus_cmd_data->refresh_disp_info)
	    cmds[qu_count++] = "display";
	if (plus_cmd_data->refresh_history_filename)
	    assert(0);
	if (plus_cmd_data->refresh_history_size)
	    assert(0);
	if (plus_cmd_data->refresh_history_save)
	    assert(0);
	break;
    }

    // Send commands
    bool send_ok;
    if (qu_count) {
	send_ok = 
	    gdb->send_user_cmd_plus(cmds, dummy, qu_count,
				    plusOQAC, (void*)plus_cmd_data,
				    cmd, (void *)cmd_data);
    }
    else {
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



// ***************************************************************************
// Schreibt den prompt ins gdb_w, nachdem ggf. gebufferte Displays abgearbeitet
// sind und evtl. Restantworten ausgegeben sind (ins gdb_w).
//
void user_cmdOAC (void* data)
{
    CmdData* cmd_data = (CmdData *) data;

    string answer = cmd_data->pos_buffer->answer_ended();

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
	    source_view->show_execution_position(pos);

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
    else
    {
	if (cmd_data->new_exec_pos)
	    source_view->show_execution_position(); // alte Anzeige loeschen
    }

    gdb_out(answer);

    // Set PC position
    if (cmd_data->pos_buffer->pc_found())
    {
	source_view->show_pc(cmd_data->pos_buffer->get_pc());
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
		gdb->send_user_cmd("display");
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
	    source_view->process_info_line_main(answers[qu_count++]);
	    break;

	case DBX:
	    break;
	}
    }

    if (plus_cmd_data->refresh_file) {
	assert (gdb->type() == DBX);
	assert (qu_count < count);

	file = answers[qu_count++];
	if (file.contains('\n'))
	    file = file.before('\n');

	if (!plus_cmd_data->refresh_line)
	{
	    // Set the correct file now.
	    string pos = file + ":0";
	    source_view->show_execution_position(pos);
	}
    }

    if (plus_cmd_data->refresh_line) {
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
	    // DBX lists 10 lines; the current line is the 5th one.
	    line += 5;
	}

	string pos;
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
