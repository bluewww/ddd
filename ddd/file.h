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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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


// Capture information on current program
struct ProgramInfo {
    string file;		// Filename of current debuggee
    string core;		// Current core file
    int    pid;			// PID of debuggee
    bool   attached;		// Flag: is GDB attached to process?
    bool   running;		// Flag: is program running?
    string state;		// Verbose state description

    ProgramInfo();		// Initialize
};

#endif // _DDD_file_h
// DON'T ADD ANYTHING BEHIND THIS #endif
