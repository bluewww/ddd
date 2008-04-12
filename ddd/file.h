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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_file_h
#define _DDD_file_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef IF_MOTIF

#include <X11/Intrinsic.h>

#else

#include <gtkmm/filechooser.h>

#endif

#if !defined(IF_XM)
#include <GUI/FileSelectionDialog.h>
#endif

#include "gtk_wrapper.h"

#include "strclass.h"
#include "StringA.h"

// Open file, process, core, source ...
#if defined(IF_XM)

extern void gdbOpenRecentCB   (Widget, XtPointer, XtPointer);
extern void gdbOpenProcessCB  (Widget, XtPointer, XtPointer);
extern void gdbOpenClassCB    (Widget, XtPointer, XtPointer);
extern void gdbOpenFileCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenCoreCB     (Widget, XtPointer, XtPointer);
extern void gdbOpenSourceCB   (Widget, XtPointer, XtPointer);
extern void gdbLookupSourceCB (Widget, XtPointer, XtPointer);

#else

extern void gdbOpenRecentCB   (int index);
extern void gdbOpenProcessCB  (GUI::Widget *w);
extern void gdbOpenClassCB    (GUI::Widget *w);
extern void gdbOpenFileCB     (GUI::Widget *w);
extern void gdbOpenCoreCB     (GUI::Widget *w);
extern void gdbOpenSourceCB   (GUI::Widget *w);
extern void gdbLookupSourceCB (GUI::Widget *w);

#endif


// Get all sources from GDB
void get_gdb_sources(StringArray& sources_list);

// Update sources list
void update_sources();

// When entering `cd', change path in file selection boxes
extern void process_cd(const string& pwd);

#if defined(IF_XM)
// Get the file name from the file selection box W
string get_file(Widget w, XtPointer, XtPointer call_data);
#else
// Get the file name from the file selection box W
string get_file(GUI::FileSelectionDialog *w);
#endif

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
