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

// $Log$
// Revision 1.1  1995/05/01 15:47:30  zeller
// Initial revision
//
// Revision 1.30  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.29  1995/03/17  07:36:57  zeller
// Removed unused variables
//
// Revision 1.28  1994/07/20  09:15:11  zeller
// Fix: #pragma implementation was invisible when using gcc
//
// Revision 1.27  1994/07/19  23:35:32  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.26  1994/01/08  17:13:24  zeller
// Anpassungen fuer TTYAgent:
// Neu: inputfp, errorfp, outputfp koennen jetzt gemeinsam eine Datei
// referenzieren (closeChannel)
//
// Revision 1.25  1993/10/15  12:45:32  zeller
// Neu: Blocking-Modi ueberarbeitet
//
// Revision 1.24  1993/10/15  07:52:56  zeller
// Neu: Behandlung von STOP- und CONT-Signalen
//
// Revision 1.23  1993/08/27  15:48:07  zeller
// Neu: TypeInfo
//
// Revision 1.22  1993/07/02  11:46:42  zeller
// Fix: __XtInherit wird jetzt in jedem Fall referenziert
//
// Revision 1.21  1993/06/30  06:35:03  zeller
// AsyncAgent::setHandler: Fix: sichergestellt, dass Funktion
// bei illegalem `type'-Parameter abbricht
//
// Revision 1.20  1993/06/16  16:42:53  zeller
// Bemerkung wg. event handling eingefuegt
//
// Revision 1.19  1993/06/16  16:07:27  zeller
// Neu: Zeiten fuer XtAppAddTimeOut() ueberarbeitet
// 
// Revision 1.18  1993/05/22  20:18:53  zeller
// %Q% added by apply-q-flag.sh 1.5
// 
// Revision 1.17  1993/04/22  10:49:37  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/22  10:37:05  zeller
// Lizenz verbessert
// 
// Revision 1.15  1993/04/21  15:14:13  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// 
// Revision 1.14  1993/04/16  11:28:12  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.13  1992/11/26  10:44:08  zeller
// Fix: "block mode set failed" sind Warnungen, keine Fehler
// 
// Revision 1.12  1992/11/19  15:48:59  zeller
// Neu: Prompt erst ausgeben, wenn auch wirklich idle
// 
// Revision 1.11  1992/11/03  18:44:09  zeller
// friend-Funktionen durch static-Funktionen ersetzt (sauberer)
// 
// Revision 1.10  1992/06/02  02:38:07  zeller
// #pragma implementation eingefuehrt
// 
// Revision 1.9  1992/05/22  14:26:31  zeller
// Neu: EOF-Handling
// 
// Revision 1.8  1992/05/22  09:45:30  zeller
// Neu: callHandlersWhenIdle
// 
// Revision 1.7  1992/05/20  16:53:53  zeller
// Neu: shutdown()
// 
// Revision 1.6  1992/05/15  10:46:58  zeller
// Neu: I/O nur waehrend des Einlesens in non-blocking mode
// 
// Revision 1.5  1992/05/14  21:01:14  zeller
// AsyncAgent::XtAppAddTimeOut entfernt
// (Literatur sagt: keine Sorgen machen)
// 
// Revision 1.4  1992/05/14  14:34:56  zeller
// Neu: setupParentIO(), restoreParentIO()
// 
// Revision 1.3  1992/05/14  11:15:12  zeller
// Neuer Konstruktor fuer Agenten-Entwickler
// 
// Revision 1.2  1992/05/14  10:21:09  zeller
// start() entfernt (schon in Oberklasse)
// 
// Revision 1.1  1992/05/12  16:56:15  zeller
// Initial revision
// 

char AsyncAgent_rcsid[] = "$Id$";

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
