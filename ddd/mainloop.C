// $Id$ -*- C++ -*-
// DDD main event loop

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2006 Free Software Foundation, Inc.
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

char mainloop_rcsid[] = 
    "$Id$";

#include "mainloop.h"
#include "exit.h"
#include "status.h"
#include "ddd.h"
#include "AppData.h"
#include "SignalB.h"

#include <setjmp.h>

// Resume after fatal errors
bool main_loop_entered = false;

static jmp_buf main_loop_env;
/*
 * Wrap ddd_setup_done so we can install it as a timer rather than a
 * workproc.  It seems that workprocs on Linux burn a lot of CPU
 * and starve GDB
 */
static void wrap_ddd_setup_done(XtPointer, XtIntervalId *)
{
    if (!ddd_setup_done(0))
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 10, 
					wrap_ddd_setup_done, 0);
}

// DDD main loop.  This is placed in a separate module to avoid
// warnings about longjmp() clobbering local variables.
void ddd_main_loop()
{
    main_loop_entered = false;
    static int sig = 0;
    if ((sig = setjmp(main_loop_env)) != 0)
    {
	// Got a fatal signal
	main_loop_entered = false;
	ddd_show_signal(sig);
	reset_status_lock();

	if (sig < 0)
	{
	    // We had a core dump
	    if (app_data.debug_core_dumps)
		DDDDebugCB(gdb_w, XtPointer(True), 0);
	    report_core(dddlog);
	}

	// Bring X in a consistent state
	XUngrabPointer(XtDisplay(gdb_w), CurrentTime);
	XUngrabKeyboard(XtDisplay(gdb_w), CurrentTime);
	XUngrabServer(XtDisplay(gdb_w));

	// Enable maintenance menu
	if (!app_data.maintenance)
	{
	    app_data.maintenance = true;
	    update_options();
	}

	// Unblock the given signal, such that we can handle it again
	if (sig < 0)
	    sig = -sig;

#ifdef SIG_SETMASK
	// POSIX interface
	sigset_t new_set;
	sigemptyset(&new_set);
	sigaddset(&new_set, sig);
	sigprocmask(SIG_UNBLOCK, &new_set, 0);
#else
	// BSD interface
	sigsetmask(sigblock(0) & ~sigmask(sig));
#endif
    }

    // Set `main_loop_entered' to true as soon 
    // as DDD becomes idle again.

    // Something about Linux's implementation of workprocs makes them burn
    // a lot of CPU, which has the nasty side effect of slowing down the
    // app we're waiting for (GDB), which has the nasty side effect of
    // giving us more time to burn even more CPU.  So don't use workprocs.
    // We'll poll ddd_setup_done every 10ms instead of the bazillion times
    // per second that it as being polled via the workproc.
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 10, wrap_ddd_setup_done, 0);

    // Main Loop
    for (;;)
	process_next_event();
}

// Return to DDD main loop, using longjmp()
void goto_main_loop(int sig)
{
    longjmp(main_loop_env, sig);
}
