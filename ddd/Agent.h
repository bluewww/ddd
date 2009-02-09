// $Id$
// Three-Channel Agent Interface 

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2004 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
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

#ifndef _DDD_Agent_h
#define _DDD_Agent_h


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
#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>

#if !HAVE_STRERROR_DECL
extern "C" char *strerror(int);
#endif

#include "casts.h"
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
    pid_t _pid;		    	// process id (0: not running, <0: no process)

protected:
    FILE *_inputfp;	    	// read from child
    FILE *_outputfp;	    	// write to child
    FILE *_errorfp;	    	// read errors from child

private:
    bool _running;	      	// flag: is child still running?
    bool _beingTerminated;      // flag: is child just being terminated?

    int _lastStatus;        	// Status after exiting

    int _terminateTimeOut; 	// terminate TimeOut (in s)
    int _hangupTimeOut;    	// hangup TimeOut (in s)
    int _killTimeOut;      	// kill TimeOut (in s)

protected:
    HandlerList handlers;   	// error handlers

private:
    int to_child[2];		// pipes
    int to_parent[2];
    int to_parent_error[2];

    Agent *next;	    	// used in agent manager

    Agent& operator = (const Agent&);

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
    void _raise(const char *msg_,
		int handler, 
		bool system_error, 
		bool check_if_running)
    {
        if (system_error) {
	    const string msg = string(msg_) + ": " + strerror(errno);
	    callHandlers(handler, STATIC_CAST(void *,
					      CONST_CAST(char*,msg.chars())));
	} else
	    callHandlers(handler, STATIC_CAST(void *,CONST_CAST(char*,msg_)));

	if (check_if_running)
	    (void)running();
    }

protected:
    string _path;	    		   // process path

    // custom error report functions
    // Msg invokes error handler, Warning invokes warning handler
    // IO means include system error, _ means don't check if running.


#define DECL_RAISE(NAME, TYPE, ERROR, RUNNING) \
    void NAME(const string& msg){ _raise(msg.chars(), TYPE, ERROR, RUNNING); }\
    void NAME(const char *msg)  { _raise(msg,         TYPE, ERROR, RUNNING); }

    DECL_RAISE(_raiseMsg,      Panic,   false, false)
    DECL_RAISE(_raiseWarning,  Strange, false, false)
    DECL_RAISE(_raiseIOMsg,    Panic,   true,  false)
    DECL_RAISE(_raiseIOWarning,Strange, true,  false)

    DECL_RAISE(raiseMsg,       Panic,   false, true)
    DECL_RAISE(raiseWarning,   Strange, false, true)
    DECL_RAISE(raiseIOMsg,     Panic,   true,  true)
    DECL_RAISE(raiseIOWarning, Strange, true,  true)
#undef DECL_RAISE

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
    bool inputIsTerminal() const
    {
	return inputfp() != 0 && isatty(fileno(inputfp()));
    }    
    bool outputIsTerminal() const
    {
	return outputfp() != 0 && isatty(fileno(outputfp()));
    }
    bool onTerminal() const
    {
	return inputIsTerminal() && outputIsTerminal();
    }

public:
    // all running agents
    static AgentManager runningAgents;

    // Constructor for Agent users
    Agent(const string& pth, unsigned nTypes = Agent_NTypes):
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
	_pid(-pid_t((unsigned long)UniqueId())), _inputfp(in), _outputfp(out),
	_errorfp(err), _running(false), _beingTerminated(false),
	_lastStatus(-1), _terminateTimeOut(-1), _hangupTimeOut(-1), 
	_killTimeOut(-1), handlers(nTypes), next(0), _path("this")
    {
	addDefaultHandler(Panic);    // report errors only
    }

    // "Dummy" Constructor without any communication
    Agent(bool, unsigned nTypes = Agent_NTypes):
	_pid(-pid_t((unsigned long)UniqueId())), _inputfp(0), _outputfp(0),
	_errorfp(0), _running(false), _beingTerminated(false),
	_lastStatus(-1), _terminateTimeOut(-1), _hangupTimeOut(-1),
	_killTimeOut(-1), handlers(nTypes), next(0), _path("this")
    {
	// no need to report anything
    }

    // Duplicator
    Agent(const Agent& c);
    virtual Agent *dup() const { return new Agent(*this); }

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
    const string& path() const		 { return _path;    }
    pid_t pid()	const		 { return _pid;     }
    int& terminateTimeOut()	 { return _terminateTimeOut; }
    int& hangupTimeOut()	 { return _hangupTimeOut; }
    int& killTimeOut()		 { return _killTimeOut; }
    int terminateTimeOut() const { return _terminateTimeOut; }
    int hangupTimeOut()	const    { return _hangupTimeOut; }
    int killTimeOut()	const	 { return _killTimeOut; }
    bool beingTerminated() const { return _beingTerminated; }
    int lastStatus() const	 { return _lastStatus; }

    string name() const
    {
	std::ostringstream os;
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
    virtual bool OK() const
    {
	return true;
    }
};

#endif // _DDD_Agent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
