// $Id$ -*- C++ -*-
// GDB initialization

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

#ifndef _DDD_gdbinit_h
#define _DDD_gdbinit_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "GDBAgent.h"
#include "AppData.h"

#if defined(IF_MOTIF)
#include <X11/Intrinsic.h>
#endif

#include "gtk_wrapper.h"

#if !defined(IF_XM)
#include <GUI/Main.h>
#endif

#if defined(IF_XM)
// Create new GDB process
extern GDBAgent *new_gdb(DebuggerType type,
			 const AppData& app_data,
			 XtAppContext app_context,
			 int argc, char *argv[]);
#else
// Create new GDB process
extern GDBAgent *new_gdb(DebuggerType type,
			 const AppData& app_data,
			 GUI::Main *app_context,
			 int argc, char *argv[]);
#endif

struct DebuggerInfo {
    // Guess an appropriate debugger TYPE from ARGC/ARGV.
    // Set ARG if debugger type could be deduced from arg ARG.

    DebuggerType type;
    string arg;

    DebuggerInfo(int argc, const char * const argv[]);
};

// Determine debugger type from given DEBUGGER_NAME
extern bool get_debugger_type(const string& debugger_name, DebuggerType& type);

// Return default program for TYPE
extern const char *default_debugger(const string& debugger_name, DebuggerType type);

inline const char *default_debugger(DebuggerType type)
{
    return default_debugger("", type);
}

#endif // _DDD_gdbinit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
