// $Id$
// Agent Manager

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

// This class is used to perform process-wide actions;
// especially terminating child agents upon exiting
// and searching an agent with given pid

#ifndef _DDD_AgentManager_h
#define _DDD_AgentManager_h

#ifdef __GNUG__
#pragma interface
#endif


#include <signal.h>
#include "TypeInfo.h"
#include "bool.h"

#if !defined(SIGCHLD) && defined(SIGCLD)
#define SIGCHLD SIGCLD
#endif

typedef void (*SignalProc)(...);

class Agent;

class AgentManager {
    friend class Agent;

private:
    Agent *first;
    SignalProc old_pipe_handler;
    SignalProc old_chld_handler;

    // Add an agent
    void operator += (Agent *key);

    // Remove an agent
    void operator -= (Agent *key);

public:
    DECLARE_TYPE_INFO

    // Constructor
    AgentManager(SignalProc new_chld_handler = SignalProc(SIG_IGN)):
	first(0)
    {
	// ignore "Broken Pipe" signals
	old_pipe_handler = SignalProc(signal(SIGPIPE, SignalProc(SIG_IGN)));

	// catch "Death of child" signals
	old_chld_handler = SignalProc(signal(SIGCHLD, new_chld_handler));
    }

    // Destructor
    virtual ~AgentManager();

    // Find an agent with given pid
    Agent *search(int pid);

    // Process status changes; return true if signal was handled
    bool childStatusChange();

    // Commit all pending changes
    void commit();
};

#endif
