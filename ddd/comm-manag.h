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


//-----------------------------------------------------------------------------
// GDB communication manager
// Name conventions:
// ...OA  : an OAProc used in GDBAgent::on_answer
// ...OAC : an OACProc used in GDBAgent::on_answer_completion()
//-----------------------------------------------------------------------------

#ifndef _DDD_comm_manag_h
#define _DDD_comm_manag_h

#ifdef __GNUG__
#pragma interface
#endif

#include "GDBAgent.h"

// Note: `Command.h' is the preferred way of interacting with GDB.

// Call gdb->start() with required parameters
void start_gdb ();

// Send user command CMD to GDB.  Invoke CALLBACK with DATA upon
// completion.  If ECHO and VERBOSE are set, issue command in GDB
// console.  If VERBOSE is set, issue answer in GDB console.  If
// PROMPT is set, issue prompt.  If CHECK is set, add appropriate GDB
// commands to get GDB state.
void send_gdb_command(string cmd, Widget origin,
		      OQCProc callback, void *data,
		      bool echo, bool verbose, bool prompt, bool check);

// Send user input CMD to GDB (unchanged).
void send_gdb_ctrl(string cmd, Widget origin = 0);

// Return FALSE if ANSWER is an error message indicating an unknown command
bool is_known_command(const string& answer);

// Send commands to initialize a session.  If TRY_SOURCE is set, try
// the GDB `source' command.
void init_session(const string& restart, const string& settings,
		  bool try_source = false);

#endif // _DDD_comm_manag_h
// DON'T ADD ANYTHING BEHIND THIS #endif
