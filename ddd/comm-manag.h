// $Id$
// GDB communication manager.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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


//-----------------------------------------------------------------------------
// GDB communication manager
// Name conventions:
// ...OA  : an OAProc used in GDBAgent::on_answer
// ...OAC : an OACProc used in GDBAgent::on_answer_completion()
//-----------------------------------------------------------------------------

#ifndef _DDD_comm_manag_h
#define _DDD_comm_manag_h

#include "GDBAgent.h"
#include "Command.h"

// Note: `Command.h' is the preferred way of interacting with GDB.

// Call gdb->start() with required parameters
void start_gdb(bool config = true);

// Send user command CMD to GDB.  Invoke CALLBACK with DATA upon
// completion of CMD; invoke EXTRA_CALLBACK with DATA when all extra
// commands (see CHECK) are done.  If ECHO and VERBOSE are set, issue
// command in GDB console.  If VERBOSE is set, issue answer in GDB
// console.  If PROMPT is set, issue prompt.  If CHECK is set, add
// extra GDB commands to get GDB state.
void send_gdb_command(string cmd, Widget origin,
		      OQCProc callback, OACProc extra_callback, void *data,
		      bool echo, bool verbose, bool prompt, bool check,
		      bool undo_source);

// Return FALSE if ANSWER is an error message indicating an unknown command
bool is_known_command(const string& answer);

// Send commands to initialize a session.  If TRY_SOURCE is set, try
// the GDB `source' command.
void init_session(const string& restart, const string& settings,
		  Boolean try_source = False);

// Let DDD handle COMMAND
void internal_command(const string& command, OQCProc callback, void *data,
		      bool echo, bool verbose, bool do_prompt);

string internal_command(const string& command);
bool is_internal_command(const string& command);

// Return GDB output that has not been echoed yet
string buffered_gdb_output();

// True if the debuggee is running
extern bool debuggee_running;

// True if GDB prompts with `(y or n) '
bool gdb_prompts_y_or_n();

// Configure JDB
extern void configure_jdb(const string& all_help);

#endif // _DDD_comm_manag_h
// DON'T ADD ANYTHING BEHIND THIS #endif
