// $Id$
// Call destructors on signal

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

char SignalCatcher_rcsid[] = 
    "$Id$";

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "SignalC.h"
#include "sigName.h"

#ifdef __GNUG__
extern "C" {
#if __GNUG__ < 2
    void __do_global_cleanup(void);
#else
    void __do_global_dtors(void);
#endif
    void _exit(int);
    void _cleanup();
}
#endif


// Kill all agents when an interrupt signal is received
static void cleanupOnSignal(int sig)
{
    // restore default action (if we're killed another time, this will make it)
    signal(sig, (void (*)(int))SIG_DFL);

    fprintf(stderr, "%s\nCleaning up... ", sigName(sig));

#ifdef __GNUG__
#if __GNUG__ < 2
    // Using a GNU compiler, we can call the cleanup actions
    // directly, rather than going through exit().
    __do_global_cleanup();
#else
    // same for version 2.0 and above
    __do_global_dtors();
#endif
#endif

    fprintf(stderr, "done.\n");

#ifdef __GNUG__
#ifdef sun
    // Using a SUN, we can also close all open files
    _cleanup();
#endif
    // Now exit gracefully, without calling destructors
    _exit(127);
#else
    // Exit gracefully: this calls all destructors (but leaves no core)
    exit(127);
#endif
}



// Constructor: catch them all
SignalCleanup::SignalCleanup()
{
    // if $NO_SIGNAL_CLEANUP is set, don't do anything
    if (getenv("NO_SIGNAL_CLEANUP") != NULL)
	return;

    // catch "typical" interrupt signals

#ifdef SIGHUP
    signal(SIGHUP,  cleanupOnSignal);	// hangup
#endif

#ifdef SIGINT
    signal(SIGINT,  cleanupOnSignal);	// interrupt
#endif

#ifdef SIGQUIT
    signal(SIGQUIT, cleanupOnSignal);	// quit
#endif

#ifdef SIGILL
    signal(SIGILL,  cleanupOnSignal);	// illegal instruction
#endif

#ifdef SIGABRT
    signal(SIGABRT, cleanupOnSignal);	// abort
#endif

#ifdef SIGFPE
    signal(SIGFPE,  cleanupOnSignal);	// floating point exception
#endif

#ifdef SIGBUS
    signal(SIGBUS,  cleanupOnSignal);	// bus error
#endif

#ifdef SIGSEGV
    signal(SIGSEGV, cleanupOnSignal);	// segmentation violation
#endif

#ifdef SIGSYS
    signal(SIGSYS,  cleanupOnSignal);	// bad argument to system call
#endif

#ifdef SIGTERM
    signal(SIGTERM, cleanupOnSignal);	// software termination signal
#endif

}
