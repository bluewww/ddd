// $Id$ -*- C++ -*-
// Block signals for the lifetime of this object (protect critical sections)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _Nora_SignalBlocker_h
#define _Nora_SignalBlocker_h

#ifdef __GNUG__
#pragma interface
#endif

#include <signal.h>

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


#ifdef SIG_SETMASK

// POSIX interface
class SignalBlocker {
private:
    sigset_t old_set;

    // Avoid assignments
    void operator = (const SignalBlocker&) {}

public:
    // Constructor
    SignalBlocker(int signum)
    {
	sigset_t new_set;

	sigemptyset(&new_set);
	sigaddset(&new_set, signum);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
    }
    SignalBlocker()
    {
	sigset_t new_set;

	sigfillset(&new_set);
	sigprocmask(SIG_BLOCK, &new_set, &old_set);
    }

    // Destructor
    ~SignalBlocker()
    {
	sigprocmask(SIG_SETMASK, &old_set, (sigset_t *)0);
    }
};

#else

// BSD interface
class SignalBlocker {
private:
    int _mask;

    // Avoid assignments
    void operator = (const SignalBlocker& b) {}

public:
    // Constructor
    SignalBlocker(int signum)
    {
	_mask = sigblock(sigmask(signum));
    }
    SignalBlocker()
    {
	_mask = sigblock(~0);
    }

    // Destructor
    ~SignalBlocker()
    {
	sigsetmask(_mask);
    }
};
#endif // SIG_SETMASK
  
#endif // _Nora_SignalBlocker_h
// DON'T ADD ANYTHING BEHIND THIS #endif
