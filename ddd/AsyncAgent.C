// $Id$
// Asynchron Agent Interface

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

char AsyncAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "AsyncAgent.h"
#include "TimeOut.h"
#include "misc.h"

#include <sys/types.h>
#include <strstream.h>
#include <unistd.h>
#include <stdlib.h>

DEFINE_TYPE_INFO_1(AsyncAgent, Agent)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProc)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProcInfo)

// process child status change

void AsyncAgent::_childStatusChange(XtPointer client_data, XtIntervalId *)
{
    AsyncAgent *a = (AsyncAgent *)client_data;

    pid_t agent_pid  = a->pid();
    int agent_status = a->new_status;

    // if we have a dummy agent running, prefer this one
    Agent *agent = Agent::runningAgents.search(int(agent_pid));
    if (agent == 0)
	agent = a;

    // set new agent state
    agent->hasNewStatus(agent_status);

    // check if agent is still running
    (void)(agent->running());
}

void AsyncAgent::childStatusChange(Agent *agent, void *, void *call_data)
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
    for (unsigned type = 0; type < AsyncAgent_NHandlers; type++)
    {
	_handlers[type] = 0;
	_ids[type] = 0;
    }
}

// Clear handlers
void AsyncAgent::clearHandlers()
{
    for (unsigned type = 0; type < AsyncAgent_NHandlers; type++)
	setHandler(type);
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
void AsyncAgent::dispatch(int *, XtInputId *inputId)
{
    // search handler
    unsigned type;
    for (type = 0; type < AsyncAgent_NHandlers && id(type) != *inputId; type++)
	;
    
    // call it
    if (type < AsyncAgent_NHandlers)
    {
	(*(handler(type)))(this);
    }
#if 0
    else
    {
	ostrstream os;
	os << "unhandled input id " << *inputId;
	string s(os);
	raiseMsg(s);
    }
#endif
}

// Abort
void AsyncAgent::abort()
{
    // remove previously installed handlers
    for (unsigned type = 0; type < AsyncAgent_NHandlers; type++)
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
void AsyncAgent::terminateProcess(XtPointer client_data, XtIntervalId *)
{
    pid_t pid = (pid_t)client_data;
    kill(pid, SIGTERM);
}

void AsyncAgent::hangupProcess(XtPointer client_data, XtIntervalId *)
{ 
    pid_t pid = (pid_t)client_data;
    kill(pid, SIGHUP);
}

void AsyncAgent::killProcess(XtPointer client_data, XtIntervalId *)
{
    pid_t pid = (pid_t)client_data;
    kill(pid, SIGKILL);
}

void AsyncAgent::terminate(bool onExit)
{
    Agent::terminate(onExit);

    if (onExit)
	Agent::waitToTerminate();
    else if (pid() > 0)
    {
	// Kill asynchronously.  We don't want to wait until the
	// process dies, so we just send out some signals and pretend
	// the process has terminated gracefully.
	if (terminateTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), terminateTimeOut() * 1000,
			    terminateProcess, XtPointer(pid()));

	if (hangupTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), hangupTimeOut() * 1000,
			    hangupProcess, XtPointer(pid()));

	if (killTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), killTimeOut() * 1000,
			    killProcess, XtPointer(pid()));

	// Inhibit further communication
	abort();
	callHandlers(Died, "Exit 0");
    }
}

void AsyncAgent::waitToTerminate()
{
    // Do nothing
}


// Delayed Event Handling

void AsyncAgent::callTheHandlersIfIdle(XtPointer client_data, XtIntervalId *)
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

void _just_make_sure_XtInherit_is_loaded()
{
    XtToolkitInitialize();
}
