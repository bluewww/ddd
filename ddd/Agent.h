// $Id$
// Three-Channel Agent Interface 

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

#ifndef _Nora_Agent_h
#define _Nora_Agent_h

#ifdef __GNUG__
#pragma interface
#endif


/*
    Agent(p) opens a three-channel communication interface to a process p
    (called an "agent"). The calling process can write to the agent's stdin
    and can read from the agent's stdout and stderr.

    The process is started using start(). The pipe is closed
    using shutdown(), sending an EOF to the agent's stdin, on
    which the agent should terminate. shutdown() does not wait
    for the agent to terminate.

    Upon destruction ~Agent() or terminate(), the pipe is closed;
    the calling process waits for the agent to terminate.
    If it agent has not terminated after hangupTimeOut (default: 5) seconds,
    the agent is sent a SIGHUP signal.
    If it has not terminated after terminateTimeOut (default: 10) seconds,
    the agent is sent a SIGTERM signal.
    If it has not terminated after killTimeOut (default: 15) seconds,
    the agent is sent a SIGKILL signal.
    Signals are sent only if the appropriate timeout is >= 0.

    Each Agent can be associated with handlers for these events:
    Agent::Panic    -- a serious I/O error occured
    Agent::Started  -- the agent was started (is now running)
    Agent::Stopped  -- the agent was stopped (is no more running)
    Agent::InputEOF -- EOF on input channel detected
    Agent::ErrorEOF -- EOF on error channel detected
    Agent::Died     -- the agent died
    Agent::_Died    -- the agent has a new status (called asynchronously)

    A handler takes the form
    void handler(Agent *source, void *client_data, void *call_data)
    where source is the agent causing the event and client_data is arbitrary
    data passed to addHandler() when the handler was added.
    In Agent::Panic, char *s = (char *)call_data is the I/O error message,
    In Agent::Died, char *s = (char *)call_data is the reason for the 
	agent to die (in plain text). lastStatus() gives the status itself.
    In Agent::_Died, int status = (int)call_data is the new agent state.

    Note that Agent::_Died is invoked asynchronously from signal();
    thus the event handlers must not call any non-reentrant routines.

    For Agent writers, an alternative interface is provided with the
    constructor Agent(FILE *in = stdin, FILE *out = stdout).
    No process is started and no pipe is created; instead, 
    the interface listens to <in> as it would to the stdandard
    output of a child process and calls appropriate handlers.
    <out> defaults to stdout; output via this interface is also
    logged using the standard handlers. The default parameters are chosen
    so that they need not be specified by standard agents.

    For Client writers, a third interface is provided with the
    constructor Agent(bool dummy). This is used for in-process 
    communication. No inter-process communication ever takes place.
*/

#include "config.h"

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream.h>
#include <strstream.h>
#include <errno.h>
#include <string.h>

#ifndef HAVE_STRERROR_DECL
extern "C" char *sterror(int errno);
#endif

#include "strclass.h"
#include "bool.h"
#include "AgentM.h"
#include "HandlerL.h"
#include "UniqueId.h"
#include "SignalB.h"
#include "TypeInfo.h"


typedef void (*AgentHandlerProc)(class Agent *source, void *client_data,
    void *call_data);


// Event types
const unsigned Panic    = 0;	        // I/O error occurred
const unsigned Strange  = Panic + 1;    // Strange I/O condition occurred
const unsigned Started  = Strange + 1;  // Process started
const unsigned Stopped  = Started + 1;  // Process stopped
const unsigned InputEOF = Stopped + 1;  // EOF on input detected
const unsigned ErrorEOF = InputEOF + 1; // EOF on error detected
const unsigned Died     = ErrorEOF + 1; // Process died
const unsigned _Died    = Died + 1;     // New Status (async call)

const unsigned Agent_NTypes = _Died + 1;    // number of events

const int READ = 0;		// constants for pipe indexing
const int WRITE = 1;

class Agent {
    friend class AgentManager;

public:
    DECLARE_TYPE_INFO

private:
    int _pid;		    	// process id (0: not running, <0: no process)

protected:
    FILE *_inputfp;	    	// read from child
    FILE *_outputfp;	    	// write to child
    FILE *_errorfp;	    	// read errors from child

private:
    bool _running;	      	// flag: is child still running?
    bool _beingTerminated;   // flag: is child just being terminated?

    int _lastStatus;        	// Status after exiting

    int _terminateTimeOut; 	// terminate TimeOut (in s)
    int _hangupTimeOut;    	// hangup TimeOut (in s)
    int _killTimeOut;      	// kill TimeOut (in s)

    HandlerList handlers;   	// error handlers

    int to_child[2];		// pipes
    int to_parent[2];
    int to_parent_error[2];

    Agent *next;	    	// used in agent manager


protected:
    // call Handlers
    void callHandlers(int type, void *call_data = 0)
    {
	handlers.call(type, this, call_data);
    }

    // dito, but delayed
    virtual void callHandlersWhenIdle(int type, void *call_data = 0)
    {
	callHandlers(type, call_data);
    }


private:
    // check if agent is dead
    void checkIfDead();

    // set new states for _running
    void setRunning();
    void unsetRunning();

    // start new process
    void startChildProcess();

    // Handle status change
    static void childStatusChange(int sig);

    // generic error report function
    void _raise(string msg, 
		int handler, 
		bool system_error, 
		bool check_if_running)
    {
	if (system_error)
	    msg += string(": ") + strerror(errno);
	char *s = msg;

	callHandlers(handler, (void *)s);

	if (check_if_running)
	    (void)running();
    }

protected:
    string _path;	    		   // process path

    // custom error report functions
    // Msg invokes error handler, Warning invokes warning handler
    // IO means include system error, _ means don't check if running.

    void _raiseMsg(string msg)        { _raise(msg, Panic,   false, false); }
    void _raiseWarning(string msg)    { _raise(msg, Strange, false, false); }
    void _raiseIOMsg(string msg)      { _raise(msg, Panic,   true,  false); }
    void _raiseIOWarning(string msg)  { _raise(msg, Strange, true,  false); }
    void raiseMsg(string msg)         { _raise(msg, Panic,   false, true); }
    void raiseWarning(string msg)     { _raise(msg, Strange, false, true); }
    void raiseIOMsg(string msg)       { _raise(msg, Panic,   true,  true); }
    void raiseIOWarning(string msg)   { _raise(msg, Strange, true,  true); }

    // Terminator
    virtual void waitToTerminate();	// wait until agent dead

    // Terminator 2
    virtual void abort();		// inhibit further communication

    // default handler
    static void defaultHandler(Agent *source, 
			       void *client_data, 
			       void *call_data);
    void addDefaultHandler(unsigned type);

    // restore I/O for parent
    virtual void restoreParentIO();

    // setup I/O
    virtual void activateIO()   {}
    virtual void deactivateIO() {}

    // hooks for alternative communication schemes
    virtual int setupCommunication();
    virtual int setupChildCommunication();
    virtual int setupParentCommunication();

    // hook for alternative child execution (never return)
    virtual void executeChild();

    // hook for channel closing
    virtual void closeChannel(FILE *fp);

    // EOF handlers
    virtual void inputEOF();    // EOF on input detected
    virtual void errorEOF();    // EOF on error detected

    // Check if we're on a terminal
    bool inputIsTerminal() 
    {
	return inputfp() != 0 && isatty(fileno(inputfp()));
    }    
    bool outputIsTerminal() 
    {
	return outputfp() != 0 && isatty(fileno(outputfp()));
    }
    bool onTerminal()
    {
	return inputIsTerminal() && outputIsTerminal();
    }

public:
    // all running agents
    static AgentManager runningAgents;

    // Constructor for Agent users
    Agent(string pth, unsigned nTypes = Agent_NTypes):
	_pid(0), _inputfp(0), _outputfp(0), _errorfp(0),
	_running(false), _beingTerminated(false), _lastStatus(-1),
	_terminateTimeOut(10), _hangupTimeOut(5), _killTimeOut(15),
	handlers(nTypes), next(0), _path(pth)
    {
	addDefaultHandler(Panic);    // report errors
	addDefaultHandler(Strange);  // report warnings
	addDefaultHandler(Died);     // report death
    }

    // Constructor for Agent writers
    Agent(FILE *in = stdin, FILE *out = stdout, FILE *err = 0,
	unsigned nTypes = Agent_NTypes):
	_pid(-(unsigned long)UniqueId()), _inputfp(in), _outputfp(out),
	_errorfp(err), _running(false), _beingTerminated(false),
	_lastStatus(-1), _terminateTimeOut(-1), _hangupTimeOut(-1), 
	_killTimeOut(-1), handlers(nTypes), next(0), _path("this")
    {
	addDefaultHandler(Panic);    // report errors only
    }

    // "Dummy" Constructor without any communication
    Agent(bool dummy, unsigned nTypes = Agent_NTypes):
	_pid(-(unsigned long)UniqueId()), _inputfp(0), _outputfp(0),
	_errorfp(0), _running(false), _beingTerminated(false),
	_lastStatus(-1), _terminateTimeOut(-1), _hangupTimeOut(-1),
	_killTimeOut(-1), handlers(nTypes), next(0), _path("this")
    {
	// no need to report anything
    }

    // Duplicator
    Agent(const Agent& c);

    // Destructor
    virtual ~Agent();

    // Start agent
    virtual void start();

    // File access
    FILE *inputfp()  const { return _inputfp;  }
    FILE *outputfp() const { return _outputfp; }
    FILE *errorfp()  const { return _errorfp;  }

    // Shutdown agent (sending EOF)
    virtual void shutdown();

    // Wait until terminated
    virtual void wait();

    // High-Level Terminator (using signals)
    virtual void terminate(bool onExit = false);

    // Low-Level Terminators
    void _kill(int sig = SIGKILL);	   // send signal
    void _hangup()    { _kill(SIGHUP); }   // send Hangup signal
    void _terminate() { _kill(SIGTERM); }  // send Terminate signal

    // Hook to be called when handler list has changed
    virtual void handlerChange() {}

    // Add event handler
    void addHandler(unsigned type, 
			    AgentHandlerProc proc,
			    void *client_data = 0)
    {
	SignalBlocker sb(SIGCHLD);

	handlers.add(type, (HandlerProc)proc, client_data);
	handlerChange();
    }
    void addHandler(const Agent& a)
    {
	SignalBlocker sb(SIGCHLD);

	handlers.add(a.handlers);
	handlerChange();
    }

    // Remove event handler
    void removeHandler(unsigned type, AgentHandlerProc proc,
			       void *client_data = 0)
    {
	SignalBlocker sb(SIGCHLD);

	handlers.remove(type, (HandlerProc)proc, client_data);
	handlerChange();
    }

    // Remove all event handlers
    void removeAllHandlers(unsigned type)
    {
	SignalBlocker sb(SIGCHLD);

	handlers.removeAll(type);
	handlerChange();
    }

    // Remove all handlers for all event types
    void removeAllHandlers()
    {
	SignalBlocker sb(SIGCHLD);

	handlers.removeAll();
	handlerChange();
    }

    // Check if we have a handler
    bool hasHandler(unsigned type) const
    {
	return handlers.has(type);
    }

    // inform agent about new status
    void hasNewStatus(int status);

    // Resources
    string path()		{ return _path;    }
    pid_t pid()			{ return _pid;     }
    int& terminateTimeOut()	{ return _terminateTimeOut; }
    int& hangupTimeOut()	{ return _hangupTimeOut; }
    int& killTimeOut()		{ return _killTimeOut; }
    bool beingTerminated()	{ return _beingTerminated; }
    int lastStatus()		{ return _lastStatus; }

    string name()
    {
	ostrstream os;
	os << path();
	if (pid() > 0)
	    os << " [" << pid() << "]";
	return string(os);
    }

    // Check if still running
    bool running();

    // Commit pending changes
    virtual void commit()       
    {
	// default: do nothing
    }

    // Debugging
    virtual bool OK()
    {
	return true;
    }
};

#endif
