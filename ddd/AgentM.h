// $Id$
// Agent Manager

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

// This class is used to perform process-wide actions;
// especially terminating child agents upon exiting
// and searching an agent with given pid

#ifndef _Nora_AgentManager_h
#define _Nora_AgentManager_h

#ifdef __GNUG__
#pragma interface
#endif


#include <signal.h>
#include "TypeInfo.h"
#include "bool.h"

#if !defined(SIGCHLD) && defined(SIGCLD)
#define SIGCHLD SIGCLD
#endif

class Agent;

class AgentManager {
    friend class Agent;

private:
    Agent *first;
    void (*old_pipe_handler)(int sig);
    void (*old_chld_handler)(int sig);

    // Add an agent
    void operator += (Agent *key);

    // Remove an agent
    void operator -= (Agent *key);

public:
    DECLARE_TYPE_INFO

    // Constructor
    AgentManager(void (*new_chld_handler)(int sig) = (void (*)(int))SIG_IGN):
	first(0)
    {
	// ignore "Broken Pipe" signals
	old_pipe_handler = signal(SIGPIPE, (void (*)(int))SIG_IGN);

	// catch "Death of child" signals
	old_chld_handler = signal(SIGCHLD, new_chld_handler);
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
