// $Id$ -*- C++ -*-
// Exit DDD

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001-2004 Free Software Foundation, Inc.
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

#ifndef _DDD_exit_h
#define _DDD_exit_h

#include <X11/Intrinsic.h>
#include <iostream>

#include "bool.h"
#include "ExitCB.h"

// Cleanup and signal handling
extern void ddd_cleanup();
extern void ddd_show_signal(int sig);
extern void ddd_show_exception(const char *cls = 0, const char *what = 0);
extern void ddd_install_fatal(const char *program_name = 0);
extern void ddd_install_signal();
extern void ddd_install_x_fatal();
extern void ddd_install_x_error();
extern void ddd_install_xt_error(XtAppContext app_context);

// Callbacks
extern void _DDDExitCB   (Widget, XtPointer, XtPointer); // GDB exited normally
extern void DDDExitCB    (Widget, XtPointer, XtPointer); // Exit DDD
extern void DDDRestartCB (Widget, XtPointer, XtPointer); // Restart DDD
extern void DDDDebugCB   (Widget, XtPointer, XtPointer); // Debug DDD
extern void DDDDumpCoreCB(Widget, XtPointer, XtPointer); // Dump Core

// Valgrind
extern void dddValgrindLeakCheckCB(Widget, XtPointer, XtPointer);
extern bool ValgrindLeakBuiltin();
extern bool RunningOnValgrind();

// Core dumps
extern void report_core(std::ostream& os);

// Helpers
class Agent;
extern void gdb_eofHP      (Agent *, void *, void *);
extern void gdb_diedHP     (Agent *, void *, void *);
extern void gdb_exceptionHP(Agent *, void *, void *);

// True if GDB is about to exit
extern bool gdb_is_exiting;

// True if DDD is about to exit
extern bool ddd_is_exiting;

// True if DDD is about to restart
extern bool ddd_is_restarting;

// True if DDD is about to shut down (saving current settings)
extern bool ddd_is_shutting_down;

// True if DDD has crashed and needs restarting
extern bool ddd_has_crashed;

#endif // _DDD_exit_h
// DON'T ADD ANYTHING BEHIND THIS #endif
