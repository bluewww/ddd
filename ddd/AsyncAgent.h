// $Id$ -*- C++ -*-
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

#ifndef _DDD_AsyncAgent_h
#define _DDD_AsyncAgent_h

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

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "assert.h"
#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Main.h>
#include "gtk_wrapper.h"
#endif
#include "Agent.h"

// Set to 1 if you want asynchronous notification
// #define ASYNC_CHILD_STATUS_CHANGE

#ifndef ASYNC_CHILD_STATUS_CHANGE
#define ASYNC_CHILD_STATUS_CHANGE (XtSpecificationRelease >= 6)
#endif // !defined(ASYNC_CHILD_STATUS_CHANGE)


class AsyncAgent;

typedef void (*AsyncAgentHandler)(AsyncAgent *agent);

struct AsyncAgentWorkProcInfo {
    DECLARE_TYPE_INFO

    AsyncAgent *agent;
    unsigned int type;
    void *call_data;

    AsyncAgentWorkProcInfo(AsyncAgent *a, unsigned t, void *c)
	: agent(a), type(t), call_data(c)
    {}

private:
    AsyncAgentWorkProcInfo(const AsyncAgentWorkProcInfo&);
    AsyncAgentWorkProcInfo& operator = (const AsyncAgentWorkProcInfo&);
};

#if defined(IF_XM)
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

private:
    AsyncAgentWorkProc(const AsyncAgentWorkProc&);
    AsyncAgentWorkProc& operator = (const AsyncAgentWorkProc&);
};
#else
struct AsyncAgentWorkProc {
    DECLARE_TYPE_INFO

    GUI::connection proc_id; 	        // id of running background proc
    AsyncAgentWorkProcInfo *info;	// info
    AsyncAgentWorkProc *next;		// next one

    // Constructor
    AsyncAgentWorkProc(GUI::connection i, AsyncAgentWorkProcInfo *inf,
		       AsyncAgentWorkProc *n):
	proc_id(i), info(inf), next(n)
    {}

private:
    AsyncAgentWorkProc(const AsyncAgentWorkProc&);
    AsyncAgentWorkProc& operator = (const AsyncAgentWorkProc&);
};
#endif

// Events
const unsigned AsyncAgent_NTypes = Agent_NTypes;

// Handlers
const unsigned OutputReady     = 0;	// agent is ready for data
const unsigned InputReady      = 1;	// data from agent's stdout is ready
const unsigned ErrorReady      = 2;	// data from agent's stdin is ready
const unsigned OutputException = 3;	// I/O exception (urgent msg) on output
const unsigned InputException  = 4;	// I/O exception (urgent msg) on input
const unsigned ErrorException  = 5;	// I/O exception (urgent msg) on error

const unsigned AsyncAgent_NHandlers = 6; // # of handler types

#if defined(IF_XM)
class AsyncAgent: public Agent {
public:
    DECLARE_TYPE_INFO

private:

    XtAppContext _appContext;		// the application context

    AsyncAgentHandler _handlers[AsyncAgent_NHandlers]; // handlers
    XtInputId _ids[AsyncAgent_NHandlers];	       // their ids

    AsyncAgentWorkProc *workProcs;	// working procedures

    // dispatch event
    void dispatch(int *fid, XtInputId *id);

    // used in childStatusChange()
    int new_status;
    bool status_change_pending;

#if ASYNC_CHILD_STATUS_CHANGE
    XtSignalId signal_id;
#else
    XtIntervalId signal_id;		// Just for padding
#endif

    bool killing_asynchronously;

    void initHandlers();
    void addDeathOfChildHandler();

    // delete remaining work procedure(s)
    void deleteWorkProc(AsyncAgentWorkProcInfo *info, bool remove = true);
    void deleteAllWorkProcs();

    // Call when NEW_STATUS has been set
    void statusChange();

#if ASYNC_CHILD_STATUS_CHANGE
    static void _childStatusChange(XtPointer client_data, XtSignalId *id);
#endif

    // X Event Handlers
    static void somethingHappened(XtPointer client_data,
				  int *fid, XtInputId *id);
    static void childStatusChange(Agent *agent, void *client_data,
				  void *call_data);
    static Boolean callTheHandlers(XtPointer client_data);
    static void callTheHandlersIfIdle(XtPointer client_data, XtIntervalId *id);

    // Helping functions
    static void terminateProcess(XtPointer, XtIntervalId *);
    static void hangupProcess(XtPointer, XtIntervalId *);
    static void killProcess(XtPointer, XtIntervalId *);

protected:
    // Set handler
    AsyncAgentHandler setHandler(unsigned type, AsyncAgentHandler handler = 0);

    // Clear all handlers
    void clearHandlers();

    // resources
    XtInputId id(unsigned type) const
    {
	assert(type < AsyncAgent_NHandlers);
	return _ids[type]; 
    }

    AsyncAgentHandler handler(unsigned type) const
    {
	assert(type < AsyncAgent_NHandlers);
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

    AsyncAgent& operator = (const AsyncAgent&);

public:
    // Resources
    XtAppContext appContext() const { return _appContext; }

    // Constructors
    AsyncAgent(XtAppContext app_context, const string& pth, 
	       unsigned nTypes = AsyncAgent_NTypes):
	Agent(pth, nTypes), _appContext(app_context), workProcs(0), 
	new_status(0), status_change_pending(false), 
	signal_id(0), killing_asynchronously(false)
    {
	initHandlers();
	addDeathOfChildHandler();
    }

    AsyncAgent(XtAppContext app_context, FILE *in = stdin, FILE *out = stdout,
	FILE *err = 0, unsigned nTypes = AsyncAgent_NTypes):
	Agent(in, out, err, nTypes), _appContext(app_context), workProcs(0),
	new_status(0), status_change_pending(false),
	signal_id(0), killing_asynchronously(false)
    {
	initHandlers();
    }

    AsyncAgent(XtAppContext app_context, bool dummy,
	unsigned nTypes = AsyncAgent_NTypes):
	Agent(dummy, nTypes), _appContext(app_context), workProcs(0),
	new_status(0), status_change_pending(false),
	signal_id(0), killing_asynchronously(false)
    {
	initHandlers();
    }

    // Duplicator
    AsyncAgent(const AsyncAgent& c):
	Agent(c), _appContext(c.appContext()), workProcs(0), 
	new_status(0), status_change_pending(false),
	signal_id(0), killing_asynchronously(false)
    {
	initHandlers();
    }
    virtual Agent *dup() const { return new AsyncAgent(*this); }

    // Destructor
    ~AsyncAgent()
    {
	// Make sure the work procedure won't be called
	deleteAllWorkProcs();

	// Inhibit further Xt selection
	clearHandlers();
    }

    // Commit pending status changes
    virtual void commit();

    // These need special management:
    virtual void abort();
    virtual void terminate(bool onExit = false);
};
#else
class AsyncAgent: public Agent {
public:
    DECLARE_TYPE_INFO

private:

    GUI::Main *_appContext;

    AsyncAgentHandler _handlers[AsyncAgent_NHandlers]; // handlers
    GUI::connection _ids[AsyncAgent_NHandlers];       // their ids

    AsyncAgentWorkProc *workProcs;	// working procedures

    // dispatch event
    void dispatch(int type);

    // used in childStatusChange()
    int new_status;
    bool status_change_pending;

    GUI::connection signal_id;		// Just for padding

    bool killing_asynchronously;

    void initHandlers();
    void addDeathOfChildHandler();

    // delete remaining work procedure(s)
    void deleteWorkProc(AsyncAgentWorkProcInfo *info, bool remove = true);
    void deleteAllWorkProcs();

    // Call when NEW_STATUS has been set
    void statusChange();

#if ASYNC_CHILD_STATUS_CHANGE
    static void _childStatusChange(XtPointer client_data, XtSignalId *id);
#endif

    // X Event Handlers
    static void childStatusChange(Agent *agent, void *client_data,
				  void *call_data);

    bool somethingHappened(Glib::IOCondition cond, int type);
    static Boolean callTheHandlers(AsyncAgentWorkProcInfo *info);
    static bool callTheHandlersIfIdle(AsyncAgentWorkProcInfo *info);

    // Helping functions

    static bool terminateProcess(pid_t);
    static bool hangupProcess(pid_t);
    static bool killProcess(pid_t);

protected:
    // Set handler
    AsyncAgentHandler setHandler(unsigned type, AsyncAgentHandler handler = 0);

    // Clear all handlers
    void clearHandlers();

    // resources
    GUI::connection id(unsigned type) const
    {
	assert(type < AsyncAgent_NHandlers);
	return _ids[type]; 
    }

    AsyncAgentHandler handler(unsigned type) const
    {
	assert(type < AsyncAgent_NHandlers);
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
	    id(type).disconnect();
        }
    }

    AsyncAgent& operator = (const AsyncAgent&);

public:
    // Resources
    GUI::Main *appContext() const { return _appContext; }

    // Constructors
    AsyncAgent(GUI::Main *app_context, const string& pth, 
	       unsigned nTypes = AsyncAgent_NTypes):
	Agent(pth, nTypes), _appContext(app_context), workProcs(0), 
	new_status(0), status_change_pending(false), 
	killing_asynchronously(false)
    {
	initHandlers();
	addDeathOfChildHandler();
    }

    AsyncAgent(GUI::Main *app_context, FILE *in = stdin, FILE *out = stdout,
	FILE *err = 0, unsigned nTypes = AsyncAgent_NTypes):
	Agent(in, out, err, nTypes), _appContext(app_context), workProcs(0),
	new_status(0), status_change_pending(false),
	killing_asynchronously(false)
    {
	initHandlers();
    }

    AsyncAgent(GUI::Main *app_context, bool dummy,
	unsigned nTypes = AsyncAgent_NTypes):
	Agent(dummy, nTypes), _appContext(app_context), workProcs(0),
	new_status(0), status_change_pending(false),
	killing_asynchronously(false)
    {
	initHandlers();
    }

    // Duplicator
    AsyncAgent(const AsyncAgent& c):
	Agent(c), _appContext(c.appContext()), workProcs(0), 
	new_status(0), status_change_pending(false),
	killing_asynchronously(false)
    {
	initHandlers();
    }
    virtual Agent *dup() const { return new AsyncAgent(*this); }

    // Destructor
    ~AsyncAgent()
    {
	// Make sure the work procedure won't be called
	deleteAllWorkProcs();

	// Inhibit further Xt selection
	clearHandlers();
    }

    // Commit pending status changes
    virtual void commit();

    // These need special management:
    virtual void abort();
    virtual void terminate(bool onExit = false);
};
#endif

#endif // _DDD_AsyncAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
