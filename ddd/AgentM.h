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

// $Log$
// Revision 1.1  1995/05/01 15:47:08  zeller
// Initial revision
//
// Revision 1.14  1995/04/18  10:29:00  zeller
// New: childStatusChange() returns whether signal could be handled
//
// Revision 1.13  1995/03/30  12:35:03  zeller
// Fix: DECs require explicit signal argument casts
//
// Revision 1.12  1995/03/21  16:41:38  zeller
// New: handle child status change in AgentManager
//
// Revision 1.11  1993/12/14  12:52:00  zeller
// Neu: Funktions-Aufrufe (*p)() durch vereinfachte Syntax p() ersetzt
//
// Revision 1.10  1993/08/27  15:48:07  zeller
// Neu: TypeInfo
//
// Revision 1.9  1993/05/22  20:18:50  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.8  1993/04/22  10:49:20  zeller
// Lizenz verbessert
// 
// Revision 1.7  1993/04/22  10:36:49  zeller
// Lizenz verbessert
// 
// Revision 1.6  1993/04/20  22:55:58  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.5  1993/04/16  11:28:12  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.4  1992/10/29  13:15:56  zeller
// Neu: commit()
// 
// Revision 1.3  1992/06/02  02:08:28  zeller
// #pragma interface eingefuehrt
// 
// Revision 1.2  1992/06/02  02:06:39  zeller
// #pragma once entfernt
// 
// Revision 1.1  1992/05/12  16:56:12  zeller
// Initial revision
// 

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
