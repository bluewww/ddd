// $Id$
// Asynchron Agent Interface

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

char AsyncAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "AsyncAgent.h"

#include <sys/types.h>
#include <strstream.h>
#include <unistd.h>

DEFINE_TYPE_INFO_1(AsyncAgent, Agent)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProc)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProcInfo)

// process child status change

void AsyncAgent::_childStatusChange(XtPointer client_data, XtIntervalId *timer)
{
    AsyncAgent *a = (AsyncAgent *)client_data;

    int agent_pid    = a->pid();
    int agent_status = a->new_status;

    // if we have a dummy agent running, prefer this one
    Agent *agent = Agent::runningAgents.search(agent_pid);
    if (agent == 0)
	agent = a;

    // set new agent state
    agent->hasNewStatus(agent_status);

    // check if agent is still running
    (void)(agent->running());
}

void AsyncAgent::childStatusChange(Agent *agent, void *client_data,
    void *call_data)
{
    AsyncAgent *a = (AsyncAgent *)agent;
    a->new_status  = (int)call_data;

    // process status change when back in event loop
    // Note: this may be delayed until the next event comes in
    XtAppAddTimeOut(a->appContext(), 1, _childStatusChange, XtPointer(a));
}

// data from agent is ready to be read
void AsyncAgent::somethingHappened(XtPointer client_data, int *fid,
    XtInputId *inputId)
{
    AsyncAgent *agent = (AsyncAgent *)client_data;
    agent->dispatch(fid, inputId);
}


// communication handlers

// Initialize handlers
void AsyncAgent::initHandlers()
{
    for (unsigned type = 0; type < NHandlers; type++)
    {
	_handlers[type] = 0;
	_ids[type] = 0;
    }
}

// Process "Death of child" signal as soon as possible
void AsyncAgent::addDeathOfChildHandler()
{
    addHandler(_Died, childStatusChange, XtPointer(this));
}


// Set Handler
AsyncAgentHandler AsyncAgent::setHandler(unsigned type, AsyncAgentHandler h)
{
    // Remove old handler if set
    AsyncAgentHandler old_handler = handler(type);
    if (id(type))
    {
	XtRemoveInput(id(type));
	_ids[type] = 0;
    }

    // Register new handler
    FILE *sourcefp      = 0;
    XtPointer condition = 0;

    switch(type)
    {
	case OutputReady:
	    sourcefp  = outputfp();
	    condition = XtPointer(XtInputWriteMask);
	    break;

	case InputReady:
	    sourcefp  = inputfp();
	    condition = XtPointer(XtInputReadMask);
	    break;

	case ErrorReady:
	    sourcefp  = errorfp();
	    condition = XtPointer(XtInputReadMask);
	    break;

	case OutputException:
	    sourcefp  = outputfp();
	    condition = XtPointer(XtInputExceptMask);
	    break;

	case InputException:
	    sourcefp  = inputfp();
	    condition = XtPointer(XtInputExceptMask);
	    break;

	case ErrorException:
	    sourcefp  = errorfp();
	    condition = XtPointer(XtInputExceptMask);
	    break;

	default:
	    assert(0);		// illegal type
	    ::abort();
    }

    _handlers[type] = h;

    if (h && sourcefp)
	_ids[type] = XtAppAddInput(appContext(), fileno(sourcefp), condition,
	    somethingHappened, (XtPointer)this);

    return old_handler;
}


// Dispatcher
void AsyncAgent::dispatch(int *fid, XtInputId *inputId)
{
    // search handler
    for (unsigned type = 0; type < NHandlers && id(type) != *inputId; type++)
	;
    
    // call it
    if (type < NHandlers)
    {
	(*(handler(type)))(this);
    }
    else
    {
	ostrstream os;
	os << "unhandled input id " << *inputId;
	string s(os);
	raiseMsg(s);
    }
}

// Abort
void AsyncAgent::abort()
{
    // remove previously installed handlers
    for (unsigned type = 0; type < NHandlers; type++)
	removeInput(type);

    // inhibit further communication
    Agent::abort();
}


// Close a channel
void AsyncAgent::closeChannel(FILE *fp)
{
    if (fp)
    {
	if (fp == inputfp())
	{
	    removeInput(InputReady);
	    removeInput(InputException);
	}
	if (fp == errorfp())
	{
	    removeInput(ErrorReady);
	    removeInput(ErrorException);
	}
	if (fp == outputfp())
	{
	    removeInput(OutputReady);
	    removeInput(OutputException);
	}
    }

    Agent::closeChannel(fp);
}


// Terminator
static void terminateProcess(XtPointer client_data, XtIntervalId *timer)
{
    AsyncAgent *agent = (AsyncAgent *)client_data;
    agent->_terminate();
}

static void hangupProcess(XtPointer client_data, XtIntervalId *timer)
{
    AsyncAgent *agent = (AsyncAgent *)client_data;
    agent->_hangup();
}

static void killProcess(XtPointer client_data, XtIntervalId *timer)
{
    AsyncAgent *agent = (AsyncAgent *)client_data;
    agent->_kill();
}

void AsyncAgent::waitToTerminate()
{
    // Copy agent to a "dummy" agent. This agent is used only
    // for handling the terminating sequence. It cannot be used for I/O.

    AsyncAgent *dummy = new AsyncAgent(*this);

    if (terminateTimeOut() >= 0)
	XtAppAddTimeOut(appContext(), terminateTimeOut() * 1000,
	    terminateProcess, XtPointer(dummy));

    if (hangupTimeOut() >= 0)
	XtAppAddTimeOut(appContext(), hangupTimeOut() * 1000,
	    hangupProcess, XtPointer(dummy));

    if (killTimeOut() >= 0)
	XtAppAddTimeOut(appContext(), killTimeOut() * 1000,
	    killProcess, XtPointer(dummy));

    // The "dummy" agent is never deleted.
    // It will be checked and waited for termination when exiting.
}


// Delayed Event Handling

void AsyncAgent::callTheHandlersIfIdle(XtPointer client_data, XtIntervalId *id)
{
    AsyncAgentWorkProcInfo *info = (AsyncAgentWorkProcInfo *)client_data;

    if (info->agent->isIdle())
    {
	// call handlers
	info->agent->callHandlers(info->type, info->call_data);
	info->agent->deleteWorkProc(info, false);
    }
    else
    {
	// try again in 10 ms
	XtAppAddTimeOut(info->agent->appContext(), 10, callTheHandlersIfIdle,
			XtPointer(info));
    }
}

Boolean AsyncAgent::callTheHandlers(XtPointer client_data)
{
    AsyncAgentWorkProcInfo *info = (AsyncAgentWorkProcInfo *)client_data;
    XtAppAddTimeOut(info->agent->appContext(), 1, callTheHandlersIfIdle,
		    XtPointer(info));

    return true;
}

void AsyncAgent::callHandlersWhenIdle(int type, void *call_data)
{
    // Create required callback information
    AsyncAgentWorkProcInfo *info =
	new AsyncAgentWorkProcInfo(this, type, call_data);

    // Register background work procedure (called when idle)
    XtWorkProcId workProcId =
	XtAppAddWorkProc(appContext(), callTheHandlers, info);

    // Memoize work procedure so that it may be cancelled
    workProcs = new AsyncAgentWorkProc(workProcId, info, workProcs);
}

void AsyncAgent::deleteWorkProc(AsyncAgentWorkProcInfo *info,
				bool remove)
{
    AsyncAgentWorkProc *prev = 0;
    for (AsyncAgentWorkProc *c = workProcs; c != 0; c = c->next)
    {
        if (c->info == info)
        {
            if (prev == 0)
                workProcs = c->next;
            else
                prev->next = c->next;

            if (remove)
		XtRemoveWorkProc(c->proc_id);
	    delete c->info;
	    delete c;
        }
        else
        {
            prev = c;
        }
    }
}

void AsyncAgent::deleteAllWorkProcs()
{
    while (workProcs)
	deleteWorkProc(workProcs->info);
}


// reference XtToolkitInitialize to avoid these stupid
// "ld.so: Undefined symbol: __XtInherit" messages

void _just_make_sure___XtInherit_is_loaded()
{
    XtToolkitInitialize();
}
