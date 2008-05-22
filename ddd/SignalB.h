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

#ifndef _DDD_SignalBlocker_h
#define _DDD_SignalBlocker_h

#include <sys/types.h>
#include <signal.h>
#include "assert.h"

// A SignalBlocker is used to protect critical sections:
// 
// {
//     SignalBlocker sb(<signal name>);
//     ... critical section protected against <signal name>...
// }
//
// For instance, to protect a critical section against SIGCHLD signals,
// you would write:
// 
// {
//     SignalBlocker sb(SIGCHLD);
//     ... critical section ...
// }
//
// A SignalBlocker without arguments blocks all signals.

class SignalBlocker {
private:
#ifdef SIG_SETMASK
    sigset_t old_set;		// POSIX interface
#else
    int old_mask;               // BSD interface
#endif

    // No assignments and copies
    SignalBlocker& operator = (const SignalBlocker&);

    SignalBlocker(const SignalBlocker&);

public:
    // Constructor - block signal SIGNUM
    SignalBlocker(int signum);

    // Constructor - block all signals
    SignalBlocker();

    // Destructor - restore state
    ~SignalBlocker();
};
  
#endif // _DDD_SignalBlocker_h
// DON'T ADD ANYTHING BEHIND THIS #endif
