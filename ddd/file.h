// $Id$ -*- C++ -*-
// DDD file handling

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

#ifndef _DDD_file_h
#define _DDD_file_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "strclass.h"

// Open file, process, core, source ...
extern void gdbOpenFileCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenProcessCB(Widget, XtPointer, XtPointer);
extern void gdbOpenCoreCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenSourceCB (Widget, XtPointer, XtPointer);

// When entering `cd', change path in file selection boxes
extern void process_cd(string pwd);



// Get FILENAME and PID of current debuggee.  NO_GDB_ANSWER for
// FILENAME means debuggee cannot be determined; "" means no debuggee.
// ATTACHED is true iff we're debugging an attached process.
// RUNNING is true iff the program is running.
// STATE is a verbose description of the program state.
extern void get_program_state(string& filename, int& pid, 
			      bool& attached, bool& running, string& state);


// Compatibility custom calls
inline void get_current_file(string& filename, int& pid, bool& attached)
{
    bool running;
    string state;
    get_program_state(filename, pid, attached, running, state);
}

inline bool program_running(string& state)
{
    string filename;
    int pid;
    bool attached;
    bool running;
    get_program_state(filename, pid, attached, running, state);
    return running;
}

inline bool program_running()
{
    string state;
    return program_running(state);
}

#endif // _DDD_file_h
// DON'T ADD ANYTHING BEHIND THIS #endif
