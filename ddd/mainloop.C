// $Id$ -*- C++ -*-
// DDD main event loop

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char mainloop_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "mainloop.h"
#include "exit.h"
#include "status.h"
#include "ddd.h"

#include <setjmp.h>

// Resume after fatal errors
bool main_loop_entered = false;

static jmp_buf main_loop_env;

// DDD main loop.  This is placed in a separate module to avoid
// warnings about longjmp() clobbering local variables.
void ddd_main_loop()
{
    main_loop_entered = false;
    static int sig = 0;
    if ((sig = setjmp(main_loop_env)) != 0)
    {
	main_loop_entered = false;
	ddd_show_signal(sig);
	reset_status_lock();
    }

    // Set `main_loop_entered' to true as soon 
    // as DDD becomes idle again.
    XtAppAddWorkProc(XtWidgetToApplicationContext(gdb_w), ddd_setup_done, 0);

    // Main Loop
    for (;;)
	process_next_event();
}

// Return to DDD main loop, using longjmp()
void goto_main_loop(int sig)
{
    longjmp(main_loop_env, sig);
}
