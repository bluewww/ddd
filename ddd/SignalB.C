// $Id$ -*- C++ -*-
// Block signals for the lifetime of this object (protect critical sections)

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

char SignalBlocker_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SignalB.h"

// The LINUX `sigfillset' macro definition causes a GCC warning.
#if defined(__linux__) && defined(sigfillset)
#undef sigfillset
#endif

// Constructor - block signal SIGNUM
SignalBlocker::SignalBlocker(int signum)
    : 
#ifdef SIG_SETMASK
    old_set()
#else
    // BSD interface
    old_mask(sigblock(sigmask(signum)));
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
    :
#ifdef SIG_SETMASK
    old_set()
#else
    // BSD interface
    old_mask(sigblock(~0));
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
