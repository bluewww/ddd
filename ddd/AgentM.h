// $Id$
// Agent Manager

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

// This class is used to perform process-wide actions;
// especially terminating child agents upon exiting
// and searching an agent with given pid

#ifndef _DDD_AgentManager_h
#define _DDD_AgentManager_h


#include <signal.h>
#include "config.h"
#include "TypeInfo.h"
#include "bool.h"
#include "assert.h"

#if !defined(SIGCHLD) && defined(SIGCLD)
#define SIGCHLD SIGCLD
#endif

extern "C" {
// Sun C++ wants `SignalProc' to be a C funtion
typedef void (*SignalProc)(SIGHANDLERARGS);
}

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

private:
    AgentManager(const AgentManager&);
    AgentManager& operator = (const AgentManager&);

public:
    DECLARE_TYPE_INFO

    // Constructor
    AgentManager(SignalProc new_chld_handler = SignalProc(SIG_IGN)):
	first(0), 
	// Ignore "Broken Pipe" signals
	old_pipe_handler(SignalProc(signal(SIGPIPE, SignalProc(SIG_IGN)))),
	// Catch "Death of child" signals
	old_chld_handler(SignalProc(signal(SIGCHLD, new_chld_handler)))
    {}

    // Destructor
    virtual ~AgentManager();

    // Find an agent with given pid
    Agent *search(int pid);

    // Process status changes; return true if signal was handled
    bool childStatusChange();

    // Commit all pending changes
    void commit();
};

#endif // _DDD_AgentManager_h
// DON'T ADD ANYTHING BEHIND THIS #endif
