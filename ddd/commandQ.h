// $Id$ -*- C++ -*-
// DDD command queue

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

#ifndef _DDD_commandQueue_h
#define _DDD_commandQueue_h

#ifdef __GNUG__
#pragma interface
#endif

#include "GDBAgent.h"

// Send COMMAND to GDB, if GDB is ready; queue in COMMAND, otherwise.
// Upon completion, invoke CALLBACK with DATA (if CALLBACK is
// non-zero).  If VERBOSE is set, issue command in GDB console.  If
// CHECK is set, add appropriate GDB commands to get GDB state.
extern void gdb_command(const string& command, Widget origin,
			OQCProc callback, void *data = 0, 
			bool verbose = false, bool check = false);

// Custom call.  Send COMMAND to GDB, if GDB is ready; queue in
// COMMAND, otherwise.
inline void gdb_command(const string& command, Widget origin = 0)
{
    gdb_command(command, origin, OQCProc(0), 0, true, true);
}


// Send COMMAND to GDB (unconditionally)
extern void _gdb_command(string command, Widget origin,
			 OQCProc callback, void *data = 0,
			 bool verbose = false, bool check = false);

// Custom call.  Send COMMAND to GDB (unconditionally)
inline void _gdb_command(const string& command, Widget origin = 0)
{
    _gdb_command(command, origin, OQCProc(0), 0, true, true);
}

// Execute commands in CMD as soon as GDB is idle
extern void gdb_batch(const string& command);

// Pass the COMMAND given in CLIENT_DATA to gdb_command()
void gdbCommandCB(Widget w, XtPointer call_data, XtPointer client_data);

// Check if command queue is empty
bool emptyCommandQueue();

// Clear command queue
void clearCommandQueue();

// Synchronize with command queue
void syncCommandQueue();

// Return a shell widget according to last command origin
Widget find_shell(Widget w = 0);

// Process next element from command queue
void processCommandQueue(XtPointer = 0, XtIntervalId *id = 0);

#endif // _DDD_commandQueue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
