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
// Revision 1.27  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.26  1994/01/08  17:13:16  zeller
// Anpassungen fuer TTYAgent:
// Neu: inputfp, errorfp, outputfp koennen jetzt gemeinsam eine Datei
// referenzieren (closeChannel)
//
// Revision 1.25  1993/10/15  12:46:07  zeller
// Neu: Blocking-Modi ueberarbeitet
//
// Revision 1.24  1993/10/15  07:55:36  zeller
// Neu: Behandlung von STOP- und CONT-Signalen
//
// Revision 1.23  1993/08/27  15:48:07  zeller
// Neu: TypeInfo
//
// Revision 1.22  1993/07/28  10:20:07  zeller
// Neu: Konstruktor fuer Kommunikation innerhalb eines Prozesses
//
// Revision 1.21  1993/05/22  20:18:56  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.20  1993/04/22  10:49:22  zeller
// Lizenz verbessert
// 
// Revision 1.19  1993/04/22  10:36:51  zeller
// Lizenz verbessert
// 
// Revision 1.18  1993/04/20  22:55:58  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.17  1993/04/16  11:28:13  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.16  1992/11/19  15:49:17  zeller
// Neu: Prompt erst ausgeben, wenn auch wirklich idle
// 
// Revision 1.15  1992/11/03  18:44:10  zeller
// friend-Funktionen durch static-Funktionen ersetzt (sauberer)
// 
// Revision 1.14  1992/11/03  18:22:20  zeller
// Friends korrekt deklariert
// 
// Revision 1.13  1992/10/16  04:26:45  zeller
// Eigene Header-Dateien eingebunden
// 
// Revision 1.12  1992/07/27  17:46:45  zeller
// Neu: callHandlersWhenIdle schon in Agent.h virtuell definiert
// 
// Revision 1.11  1992/06/02  02:08:28  zeller
// #pragma interface eingefuehrt
// 
// Revision 1.10  1992/06/02  02:06:40  zeller
// #pragma once entfernt
// 
// Revision 1.9  1992/05/22  15:20:20  zeller
// removeInput() nach hinten gesetzt,
// um Warnung "can't inline" zu vermeiden
// 
// Revision 1.8  1992/05/22  14:26:37  zeller
// Neu: EOF-Handling
// 
// Revision 1.7  1992/05/22  09:45:00  zeller
// Neu: callHandlersWhenIdle
// 
// Revision 1.6  1992/05/20  16:54:01  zeller
// Neu: shutdown()
// 
// Revision 1.5  1992/05/14  21:01:20  zeller
// AsyncAgent::XtAppAddTimeOut entfernt
// (Literatur sagt: keine Sorgen machen)
// 
// Revision 1.4  1992/05/14  14:35:05  zeller
// Neu: setupParentIO(), restoreParentIO()
// 
// Revision 1.3  1992/05/14  11:15:16  zeller
// Neuer Konstruktor fuer Agenten-Entwickler
// 
// Revision 1.2  1992/05/14  10:21:15  zeller
// start() entfernt (schon in Oberklasse)
// 
// Revision 1.1  1992/05/12  16:56:16  zeller
// Initial revision
// 

#ifndef _Nora_AsyncAgent_h
#define _Nora_AsyncAgent_h

#ifdef __GNUG__
#pragma interface
#endif


/*
    AsyncAgent(app_context, p)
    opens a three-way communication channel to the agent p (see Agent).

    In order to cooperate with other event-driven libraries (especially X),
    communication is handled asynchronously: every time that input data
    (from the agent's stdout) or error data (from the agent's stderr)
    is available, an input handler is called passing the agent as parameter.

    The outputHandler is called whenever the agent is ready to read data
    from its stdin. Its use is discouraged, since on some operating systems
    the outputHandler is called even when the agent is not ready yet.

    Upon terminate(), the agent is killed asynchronously, i.e. terminate()
    does not wait for the agent to die.

    Handlers are declared with addHandler().

    Asynchronus communication is implemented using XtAppAddInput(3).
*/


#include "assert.h"
#include <X11/Intrinsic.h>
#include "Agent.h"

class AsyncAgent;


typedef void (*AsyncAgentHandler)(AsyncAgent *agent);

struct AsyncAgentWorkProcInfo {
    DECLARE_TYPE_INFO

    AsyncAgent *agent;
    unsigned type;
    void *call_data;

    AsyncAgentWorkProcInfo(AsyncAgent *a, unsigned t, void *c):
	agent(a), type(t), call_data(c)
    {}
};

struct AsyncAgentWorkProc {
    DECLARE_TYPE_INFO

    XtWorkProcId proc_id; 	        // id of running background proc
    AsyncAgentWorkProcInfo *info;	// info
    AsyncAgentWorkProc *next;		// next one

    // Constructor
    AsyncAgentWorkProc(XtWorkProcId i, AsyncAgentWorkProcInfo *inf,
		       AsyncAgentWorkProc *n):
	proc_id(i), info(inf), next(n)
    {}
};

class AsyncAgent: public Agent {
public:
    DECLARE_TYPE_INFO

    const unsigned NTypes = Agent::NTypes; // number of events

protected:
    // Handlers
    const unsigned OutputReady     = 0;	// agent is ready for data
    const unsigned InputReady      = 1;	// data from agent's stdout is ready
    const unsigned ErrorReady      = 2;	// data from agent's stdin is ready
    const unsigned OutputException = 3;	// I/O error on output
    const unsigned InputException  = 4;	// I/O error on input
    const unsigned ErrorException  = 5;	// I/O error on error

private:
    const unsigned NHandlers   = 6;     // number of handler types

    XtAppContext _appContext;		// the application context

    AsyncAgentHandler _handlers[NHandlers]; // handlers
    XtInputId _ids[NHandlers];		// their ids

    AsyncAgentWorkProc *workProcs;	// working procedures

    // dispatch event
    void dispatch(int *fid, XtInputId *id);

    // used in childStatusChange()
    int new_status;

    void initHandlers();
    void addDeathOfChildHandler();

    // delete remaining work procedure(s)
    void deleteWorkProc(AsyncAgentWorkProcInfo *info, bool remove = true);
    void deleteAllWorkProcs();

    // X Event Handlers
    static void somethingHappened(XtPointer client_data,
				  int *fid, XtInputId *id);
    static void _childStatusChange(XtPointer client_data, XtIntervalId *iid);
    static void childStatusChange(Agent *agent, void *client_data,
				  void *call_data);
    static Boolean callTheHandlers(XtPointer client_data);
    static void callTheHandlersIfIdle(XtPointer client_data, XtIntervalId *id);
    

protected:
    // set handler
    AsyncAgentHandler setHandler(unsigned type, AsyncAgentHandler handler);

    // resources
    XtInputId id(unsigned type)
    {
	assert(type < NHandlers);
	return _ids[type]; 
    }

    AsyncAgentHandler handler(unsigned type)
    {
	assert(type < NHandlers);
	return _handlers[type];
    }

    virtual void waitToTerminate();

    // Delayed event handling
    virtual void callHandlersWhenIdle(int type, void *call_data = 0);
    virtual bool isIdle() { return true; }

    // Hook for channel closing
    virtual void closeChannel(FILE *fp);

private:
    // remove input
    void removeInput(unsigned type)
    {
	if (id(type))
	{
	    XtRemoveInput(id(type));
            _ids[type] = 0;
        }
    }

public:
    // Constructors
    AsyncAgent(XtAppContext app_context, char *pth, unsigned nTypes = NTypes):
	Agent(pth, nTypes), _appContext(app_context), workProcs(0)
    {
	initHandlers();
	addDeathOfChildHandler();
    }

    AsyncAgent(XtAppContext app_context, FILE *in = stdin, FILE *out = stdout,
	FILE *err = 0, unsigned nTypes = NTypes):
	Agent(in, out, err, nTypes), _appContext(app_context), workProcs(0)
    {
	initHandlers();
    }

    AsyncAgent(XtAppContext app_context, bool dummy,
	unsigned nTypes = NTypes):
	Agent(dummy, nTypes), _appContext(app_context), workProcs(0)
    {
	initHandlers();
    }

    // Destructor
    ~AsyncAgent()
    {
	// make sure the work procedure won't be called
	deleteAllWorkProcs();
    }

    // Duplicator
    AsyncAgent(const AsyncAgent& c):
	Agent(c), _appContext(c._appContext)
    {
	initHandlers();
    }

    // These need special management:
    virtual void abort();

    // resources
    XtAppContext appContext()  { return _appContext; }
};

#endif
