// $Id$ -*- C++ -*-
// Block signals for the lifetime of this object (protect critical sections)

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
// Revision 1.1  1995/05/01 15:47:50  zeller
// Initial revision
//
// Revision 1.3  1993/10/15  12:04:49  zeller
// Neu: SignalBlocker fuer ALLE Signale
//
// Revision 1.2  1993/08/03  14:09:43  zeller
// Neu: POSIX-Version mit sigprocmask() und sigset_t
//
// Revision 1.1  1993/08/03  13:45:56  zeller
// Initial revision
//

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
    void operator = (const SignalBlocker& b) {}

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
