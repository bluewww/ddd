// $Id$ -*- C++ -*-
// DDD command queue

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

#ifndef _DDD_commandQueue_h
#define _DDD_commandQueue_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "strclass.h"
#include <X11/Intrinsic.h>

// Send COMMAND to GDB, if GDB is ready; queue in COMMAND, otherwise
extern void gdb_command(const string& command, Widget origin = 0);

// Send COMMAND to GDB (unconditionally)
extern void _gdb_command(string command, Widget origin = 0);

// Pass the COMMAND given in CLIENT_DATA to gdb_command()
void gdbCommandCB(Widget w, XtPointer call_data, XtPointer client_data);

// Check if command queue is empty
bool emptyCommandQueue();

// Clear command queue
void clearCommandQueue();

// Return a shell widget according to last command origin
Widget find_shell(Widget w = 0);

void processCommandQueue(XtPointer, XtIntervalId *);

#endif // _DDD_commandQueue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
