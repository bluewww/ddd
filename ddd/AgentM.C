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

char AgentM_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "AgentM.h"
#include "Agent.h"
#include "SignalB.h"
#include "config.h"

extern "C" {
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
}

DEFINE_TYPE_INFO_0(AgentManager)

#if 0
#include "SignalC.h"

// Make sure to call all destructors, even when a signal comes
static SignalCleanup hasta_la_vista_you_filthy_agents;
#endif


// Add an agent
void AgentManager::operator += (Agent *key)
{
    for (Agent *c = first; c != 0; c = c->next)
	if (c == key)
	    return;

    SignalBlocker sb(SIGCHLD);

    key->next = first;
    first     = key;
}

// Remove an agent
void AgentManager::operator -= (Agent *key)
{
    Agent *prev = 0;
    for (Agent *c = first; c != 0; c = c->next)
    {
	if (c == key)
	{
	    SignalBlocker sb(SIGCHLD);

	    if (prev == 0)
		first = c->next;
	    else
		prev->next = c->next;
	}
	else
	{
	    prev = c;
	}
    }
}


// Search an agent
Agent *AgentManager::search(int pid)
{
    for (Agent *c = first; c != 0; c = c->next)
	if (pid == c->pid())
	    return c;

    return 0;
}

// Commit all pending changes
void AgentManager::commit()
{
    for (Agent *c = first; c != 0; c = c->next)
	c->commit();
}

// Agent Manager Destructor: called at the end of the show
AgentManager::~AgentManager()
{
    // Terminate using low-level functions
    for (Agent *c = first; c != 0; c = c->next)
	c->terminate(true);
}

// Process status change
bool AgentManager::childStatusChange()
{
    bool gotit = false;

    for (Agent *c = first; c != 0; c = c->next)
    {
	pid_t pid = c->pid();
	int status;

	if (pid > 0 && waitpid(pid, &status, WNOHANG) == pid)
	{
	    c->callHandlers(_Died, (void *)status);
	    gotit = true;
	}
    }

    return gotit;
}
