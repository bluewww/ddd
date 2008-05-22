// $Id$ -*- C++ -*-
// Block signals for the lifetime of this object (protect critical sections)

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

char SignalBlocker_rcsid[] = 
    "$Id$";

#include "SignalB.h"

// The LINUX `sigfillset' macro definition causes a GCC warning.
#if defined(__linux__) && defined(sigfillset)
#undef sigfillset
#endif

// Constructor - block signal SIGNUM
SignalBlocker::SignalBlocker(int signum)
#ifdef SIG_SETMASK
    // Initializing `old_set' causes an error in GCC 2.8.1
    // Reported by "Derks, Victor" <Victor.Derks@nl2.vdogrp.de>
    // : old_set()			// POSIX interface
#else
    : old_mask(sigblock(sigmask(signum))); // BSD interface
#endif
{
#ifdef SIG_SETMASK
    // POSIX interface
    sigset_t new_set;

    sigemptyset(&new_set);
    sigaddset(&new_set, signum);
    sigprocmask(SIG_BLOCK, &new_set, &old_set);
#endif
}

// Constructor - block all signals
SignalBlocker::SignalBlocker()
#ifdef SIG_SETMASK
    // Initializing `old_set' causes an error in GCC 2.8.1
    // Reported by "Derks, Victor" <Victor.Derks@nl2.vdogrp.de>
    // : old_set()			// POSIX interface
#else
    : old_mask(sigblock(~0));	// BSD interface
#endif
{
#ifdef SIG_SETMASK
    // POSIX interface
    sigset_t new_set;

    sigfillset(&new_set);
    sigprocmask(SIG_BLOCK, &new_set, &old_set);
#endif
}

// Destructor - restore state
SignalBlocker::~SignalBlocker()
{
#ifdef SIG_SETMASK
    // POSIX interface
    sigprocmask(SIG_SETMASK, &old_set, (sigset_t *)0);
#else
    // BSD interface
    sigsetmask(old_mask);
#endif
}
