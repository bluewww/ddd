// $Id$
// Asynchron Agent Interface

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

char AsyncAgent_rcsid[] = 
    "$Id$";

#include "AsyncAgent.h"
#include "TimeOut.h"
#include "misc.h"

#include <sys/types.h>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>

DEFINE_TYPE_INFO_1(AsyncAgent, Agent)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProc)
DEFINE_TYPE_INFO_0(AsyncAgentWorkProcInfo)

// Process child status change

#if ASYNC_CHILD_STATUS_CHANGE
void AsyncAgent::_childStatusChange(XtPointer client_data, XtSignalId *)
{
    AsyncAgent *a = (AsyncAgent *)client_data;

    // if we have a dummy agent running, prefer this one
    Agent *agent = Agent::runningAgents.search(a->pid());
    if (agent == 0)
	agent = a;

    agent->commit();
}
#endif

void AsyncAgent::statusChange()
{
    // Set new agent state
    hasNewStatus(new_status);

    // Check if we're still running
    running();

    // Clear `pending' flag
    status_change_pending = false;
}

void AsyncAgent::childStatusChange(Agent *agent, void *, void *call_data)
{
    AsyncAgent *a = (AsyncAgent *)agent;
    a->new_status  = (int)(long)call_data;

    // Process status change when back in event loop
#if ASYNC_CHILD_STATUS_CHANGE
    // Since we're called from a handler, this is the safe way to do it
    XtNoticeSignal(a->signal_id);
#endif

    // In X11R5 and earlier, we cannot call any Xt function -- simply
    // wait for the next AgentManager::commit()
    a->status_change_pending = true;
}

void AsyncAgent::commit()
{
    if (status_change_pending)
	statusChange();
}


// data from agent is ready to be read
#if defined(IF_XM)
void AsyncAgent::somethingHappened(XtPointer client_data, int *fid,
    XtInputId *inputId)
#else
bool AsyncAgent::somethingHappened(Glib::IOCondition cond, int type)

#endif
{
#if defined(IF_XM)
    AsyncAgent *agent = (AsyncAgent *)client_data;
    agent->dispatch(fid, inputId);
#else
    if (cond&Glib::IO_HUP) {
	std::cerr << "HANGUP ON AGENT " << this << "!\n";
	return false;
    }
    if (cond&Glib::IO_NVAL) {
	std::cerr << "INVALID FD ON AGENT " << this << "!\n";
	return false;
    }
    if (cond&Glib::IO_ERR) {
	std::cerr << "ERROR ON AGENT " << this << "!\n";
	return true;
    }
    dispatch(type);
    return true; // Do not delete this callback
#endif
}


// communication handlers

// Initialize handlers
void AsyncAgent::initHandlers()
{
    for (unsigned type = 0; type < AsyncAgent_NHandlers; type++)
    {
	_handlers[type] = 0;
#if defined(IF_XM)
	_ids[type] = 0;
#else
	_ids[type].disconnect();
#endif
    }

#if ASYNC_CHILD_STATUS_CHANGE
    signal_id = XtAppAddSignal(appContext(), _childStatusChange, 
			       XtPointer(this));
#endif
}

// Clear handlers
void AsyncAgent::clearHandlers()
{
    for (unsigned type = 0; type < AsyncAgent_NHandlers; type++)
	setHandler(type);

#if ASYNC_CHILD_STATUS_CHANGE
    XtRemoveSignal(signal_id);
#endif
}

#if defined(IF_XM)
// Process "Death of child" signal as soon as possible
void AsyncAgent::addDeathOfChildHandler()
{
    addHandler(_Died, childStatusChange, XtPointer(this));
}
#else
// Process "Death of child" signal as soon as possible
void AsyncAgent::addDeathOfChildHandler()
{
    addHandler(_Died, childStatusChange, this);
}
#endif


#if defined(IF_XM)
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
#else
// Set Handler
AsyncAgentHandler AsyncAgent::setHandler(unsigned type, AsyncAgentHandler h)
{
    // Remove old handler if set
    AsyncAgentHandler old_handler = handler(type);
    if (id(type))
    {
	_ids[type].disconnect();
    }

    // Register new handler
    FILE *sourcefp      = 0;
    int condition = 0;

    switch(type)
    {
	case OutputReady:
	    sourcefp  = outputfp();
	    condition = Glib::IO_OUT;
	    break;

	case InputReady:
	    sourcefp  = inputfp();
	    condition = Glib::IO_IN;
	    break;

	case ErrorReady:
	    sourcefp  = errorfp();
	    condition = Glib::IO_IN;
	    break;

	case OutputException:
	    sourcefp  = outputfp();
	    condition = (Glib::IO_PRI|Glib::IO_ERR);
	    break;

	case InputException:
	    sourcefp  = inputfp();
	    condition = (Glib::IO_PRI|Glib::IO_ERR);
	    break;

	case ErrorException:
	    sourcefp  = errorfp();
	    condition = (Glib::IO_PRI|Glib::IO_ERR);
	    break;

	default:
	    assert(0);		// illegal type
	    ::abort();
    }

    _handlers[type] = h;

    if (h && sourcefp) {
	const Glib::RefPtr<Glib::IOSource> source
	    = Glib::IOSource::create(fileno(sourcefp),
				     Glib::IOCondition(condition)|Glib::IO_HUP|Glib::IO_ERR|Glib::IO_NVAL);
	source->set_can_recurse(true);
	_ids[type] =
	    source->connect(sigc::bind(sigc::mem_fun(*this, &AsyncAgent::somethingHappened), type));
	Glib::RefPtr<Glib::MainContext> ctx = Glib::MainContext::get_default();
	source->attach(ctx);
    }

    return old_handler;
}
#endif


#if defined(IF_XM)
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
#else
// Dispatcher
void AsyncAgent::dispatch(int type)
{
    // call it
    if (type < AsyncAgent_NHandlers)
    {
	(*(handler(type)))(this);
    }
}
#endif

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


#if defined(IF_XM)
// Terminator
void AsyncAgent::terminateProcess(XtPointer client_data, XtIntervalId *)
{
    pid_t pid = (pid_t)(long)client_data;
    kill(pid, SIGTERM);
}

void AsyncAgent::hangupProcess(XtPointer client_data, XtIntervalId *)
{ 
    pid_t pid = (pid_t)(long)client_data;
    kill(pid, SIGHUP);
}

void AsyncAgent::killProcess(XtPointer client_data, XtIntervalId *)
{
    pid_t pid = (pid_t)(long)client_data;
    kill(pid, SIGKILL);
}
#else
// Terminator
bool AsyncAgent::terminateProcess(pid_t pid)
{
    kill(pid, SIGTERM);
}


bool AsyncAgent::hangupProcess(pid_t pid)
{ 
    kill(pid, SIGHUP);
}


bool AsyncAgent::killProcess(pid_t pid)
{
    kill(pid, SIGKILL);
}
#endif

void AsyncAgent::terminate(bool onExit)
{
    bool was_running = (pid() > 0 && running());

    Agent::terminate(onExit);

    if (onExit)
    {
	Agent::waitToTerminate();
    }
    else if (was_running && !killing_asynchronously)
    {
	// Kill asynchronously.  We don't want to wait until the
	// process dies, so we just send out some signals and pretend
	// the process has terminated gracefully.
	killing_asynchronously = true;

#if defined(IF_XM)
	if (terminateTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), terminateTimeOut() * 1000,
			    terminateProcess, XtPointer(pid()));
#else
	if (terminateTimeOut() >= 0)
	    Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(terminateProcess), pid()), terminateTimeOut() * 1000);
#endif

#if defined(IF_XM)
	if (hangupTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), hangupTimeOut() * 1000,
			    hangupProcess, XtPointer(pid()));
#else
	if (hangupTimeOut() >= 0)
	    Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(hangupProcess), pid()), hangupTimeOut() * 1000);
#endif

#if defined(IF_XM)
	if (killTimeOut() >= 0)
	    XtAppAddTimeOut(appContext(), killTimeOut() * 1000,
			    killProcess, XtPointer(pid()));
#else
	if (killTimeOut() >= 0)
	    Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(killProcess), pid()), killTimeOut() * 1000);
#endif

	// Inhibit further communication
	hasNewStatus(-1);
	abort();
	callHandlers(Died, STATIC_CAST(void*,CONST_CAST(char*,"Exit 0")));
    }
}

void AsyncAgent::waitToTerminate()
{
    // Do nothing
}


// Delayed Event Handling

#if defined(IF_XM)
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
#else
bool AsyncAgent::callTheHandlersIfIdle(AsyncAgentWorkProcInfo *info)
{
    if (info->agent->isIdle())
    {
	// call handlers
	info->agent->callHandlers(info->type, info->call_data);
	info->agent->deleteWorkProc(info, false);
    }
    else
    {
	// try again in 10 ms
	return true;
    }
    return false;
}
#endif

#if defined(IF_XM)
Boolean AsyncAgent::callTheHandlers(XtPointer client_data)
{
    AsyncAgentWorkProcInfo *info = (AsyncAgentWorkProcInfo *)client_data;
    XtAppAddTimeOut(info->agent->appContext(), 1, callTheHandlersIfIdle,
		    XtPointer(info));

    return true;
}
#else
Boolean AsyncAgent::callTheHandlers(AsyncAgentWorkProcInfo *info)
{
    Glib::signal_timeout().connect(sigc::bind(sigc::ptr_fun(callTheHandlersIfIdle), info), 10);

    return true;
}
#endif

void AsyncAgent::callHandlersWhenIdle(int type, void *call_data)
{
    // Create required callback information
    AsyncAgentWorkProcInfo *info =
	new AsyncAgentWorkProcInfo(this, type, call_data);

    // Register background work procedure (called when idle)
#if defined(IF_XM)
    XtWorkProcId workProcId =
	XtAppAddWorkProc(appContext(), callTheHandlers, info);
#else
    GUI::connection workProcId =
	GUI::signal_idle().connect(sigc::bind(sigc::ptr_fun(&AsyncAgent::callTheHandlers), info));
#endif

    // Memoize work procedure so that it may be cancelled
    workProcs = new AsyncAgentWorkProc(workProcId, info, workProcs);
}

#if defined(IF_XM)
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
#else
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
		c->proc_id.disconnect();
	    delete c->info;
	    delete c;
        }
        else
        {
            prev = c;
        }
    }
}
#endif

void AsyncAgent::deleteAllWorkProcs()
{
    while (workProcs)
	deleteWorkProc(workProcs->info);
}


#if defined(IF_XM)
// reference XtToolkitInitialize to avoid these stupid
// "ld.so: Undefined symbol: __XtInherit" messages

void _just_make_sure_XtInherit_is_loaded()
{
    XtToolkitInitialize();
}
#endif
