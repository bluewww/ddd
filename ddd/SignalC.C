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

// $Log$
// Revision 1.1  1995/05/01 15:47:40  zeller
// Initial revision
//
// Revision 1.12  1995/03/30  14:18:58  zeller
// Fix: DECs require explicit signal argument casts
//
// Revision 1.11  1993/05/22  20:12:14  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.10  1993/04/22  11:02:11  zeller
// Lizenz verbessert
// 
// Revision 1.9  1993/04/16  11:40:05  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.8  1992/06/02  02:32:32  zeller
// Anpassung an gcc 2.1
// 
// Revision 1.7  1992/05/20  15:29:42  zeller
// Dateinamen verkuerzt
// 
// Revision 1.6  1992/05/14  14:57:08  zeller
// Neu: selbst-kill() abgeschaltet (macht keinen Sinn,
// wenn beim Aufraeumen alles verloren geht); stattdessen Meldung ausgegeben
// 
// Revision 1.5  1992/05/13  19:56:22  zeller
// Aufruf _exit() unterlassen (kill() wirkt erst nach Rueckkehr);
// Ausgabe Signalname ebenfalls unterlassen.
// 
// Revision 1.4  1992/05/13  19:52:11  zeller
// Meldung bei Interrupt ausgeben;
// 2x kill() aufrufen (haelt besser);
// Signal-Namen nur verwenden, wenn auch definiert.
// 
// Revision 1.3  1992/05/13  19:29:25  zeller
// Neu: _cleanup() aufgerufen, um Dateien zu schliessen
// 
// Revision 1.2  1992/05/13  19:14:13  zeller
// __do_global_cleanup() ist in C geschrieben
// 
// Revision 1.1  1992/05/13  19:09:20  zeller
// Initial revision
// 

static const char rcsid[] = 
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
