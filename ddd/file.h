// $Id$ -*- C++ -*-
// DDD file handling

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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

#ifndef _DDD_file_h
#define _DDD_file_h

#include <X11/Intrinsic.h>
#include "strclass.h"
#include "StringA.h"

// Open file, process, core, source ...
extern void gdbOpenFileCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenRecentCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenClassCB    (Widget, XtPointer, XtPointer);
extern void gdbOpenProcessCB  (Widget, XtPointer, XtPointer);
extern void gdbOpenCoreCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenSourceCB   (Widget, XtPointer, XtPointer);
extern void gdbLookupSourceCB (Widget, XtPointer, XtPointer);

// Get all sources from GDB
void get_gdb_sources(StringArray& sources_list);

// Update sources list
void update_sources();

// When entering `cd', change path in file selection boxes
extern void process_cd(const string& pwd);

// Get the file name from the file selection box W
string get_file(Widget w, XtPointer, XtPointer call_data);

// Capture information on current program
struct ProgramInfo {
    string file;		// Filename of current debuggee
    string core;		// Current core file
    int    pid;			// PID of debuggee
    bool   attached;		// Flag: is GDB attached to process?
    bool   running;		// Flag: is program running?
    string state;		// Verbose state description

    ProgramInfo();		// Initialize

    // This should avoid link warnings on SGI
    // as reported by Mike Tosti <tosti@cinesite.com>
    ~ProgramInfo() {}
};

// Last opened file
extern string open_file_reply;

#endif // _DDD_file_h
// DON'T ADD ANYTHING BEHIND THIS #endif
