// $Id$
// Three-channel Agent Interface

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

char Agent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "assert.h"
#include "config.h"
#include <errno.h>
#include <signal.h>
#include <iostream.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "sigName.h"
#include "Agent.h"
#include "AgentM.h"

DEFINE_TYPE_INFO_0(Agent)

// Child Status Change Handler
void Agent::childStatusChange(int sig)
{
    // Notify agents of their new status
    bool gotit = runningAgents.childStatusChange();

    if (gotit)
    {
	// Reinstall ourselves (for SVR4 and others)
	signal(sig, Agent::childStatusChange);
    }
}

// Default I/O error handler
void Agent::defaultHandler(Agent *source, void *client_data, void *call_data)
{
    string msg = (char *)call_data;
    if (msg != "Exit 0")
	cerr << source->name() << ": " << msg << "\n";
}

void Agent::addDefaultHandler(unsigned type)
{
    addHandler(type, defaultHandler);
}


// All running agents
AgentManager Agent::runningAgents(Agent::childStatusChange);

// Running states
void Agent::setRunning()
{
    bool was_running = _running;
    _running = true;

#if 0
    Agent *agent = runningAgents.search(pid());
    if (agent != 0)
	cerr << "Warning: Agent " << agent->name() << " started twice\n";
#endif

    runningAgents += this;
    activateIO();

    if (!was_running)
	callHandlers(Started, (char *)path());
}

void Agent::unsetRunning()
{
    if (_running)
	callHandlers(Stopped, (char *)path());

    deactivateIO();
    runningAgents -= this;
    _running = false;
}


// Duplicator
Agent::Agent(const Agent& c):
    _pid(c._pid), _inputfp(0), _outputfp(0), _errorfp(0),
    _beingTerminated(c._beingTerminated), _lastStatus(c._lastStatus),
    _terminateTimeOut(c._terminateTimeOut),
    _hangupTimeOut(c._hangupTimeOut),
    _killTimeOut(c._killTimeOut),
    handlers(c.handlers), next(0), _path(c._path)
{
    // Assume running unless otherwise proved
    setRunning();
}


void Agent::restoreParentIO()
{
    // nothing specific to do here
}

// start Agent
void Agent::start()
{
    // start child process if we're not listening to stdin
    if (pid() >= 0)
	startChildProcess();
    else
	setRunning();
}

// Start the child process
void Agent::startChildProcess()
{
    // Kill whatever's still running there
    terminate();

    if (setupCommunication())
    {
	raiseMsg("communication setup failed");
	return;
    }
    
    if ((_pid = fork()) == 0)
    {
	if (setupChildCommunication())
	{
	    raiseMsg("child communication setup failed");
	    exit(0);
	}
	
	executeChild();
    }

    if (pid() == -1)
    {
	_pid = 0;
	raiseIOMsg("cannot fork");
	return;
    }

    if (setupParentCommunication(_inputfp, _outputfp, _errorfp))
    {
	raiseMsg("parent communication setup failed");
	return;
    }

    // agent is now set up and running
    setRunning();
}


// Setup common communication
int Agent::setupCommunication()
{
    // open pipes for stdin, stdout and stderr
    if (pipe(to_child) < 0)
    {
	raiseIOMsg("cannot open stdin pipe");
	return -1;
    }

    if (pipe(to_parent) < 0)
    {
	raiseIOMsg("cannot open stdout pipe");
	return -1;
    }

    if (pipe(to_parent_error) < 0)
    {
	raiseIOMsg("cannot open stderr pipe");
	return -1;
    }

    return 0;
}

// Setup child communication
int Agent::setupChildCommunication()
{
    // I am the child:
    // close unused pipe ends
    close(to_child[WRITE]);
    close(to_parent[READ]);
    close(to_parent_error[READ]);

    // assign stdin to to_child, stdout to to_parent, and
    // stderr to to_parent_error
    dup2(to_child[READ], fileno(stdin));
    close(to_child[READ]);

    dup2(to_parent[WRITE], fileno(stdout));
    close(to_parent[WRITE]);

    dup2(to_parent_error[WRITE], fileno(stderr));
    close(to_parent_error[WRITE]);

    return 0;
}

// Setup parent communication
int Agent::setupParentCommunication(FILE *& _inputfp,
				    FILE *& _outputfp,
				    FILE *& _errorfp)
{
    // I am the parent: close unused pipe ends
    close(to_child[READ]);
    close(to_parent[WRITE]);
    close(to_parent_error[WRITE]);

    // access remaining pipe ends via stream I/O
    // using _errorfp, _inputfp and _outputfp...
    _errorfp = fdopen(to_parent_error[READ], "r");
    if (errorfp() == NULL)
    {
	raiseIOMsg("cannot fdopen child's stdin pipe");
	terminate();
	return -1;
    }

    _inputfp = fdopen(to_parent[READ], "r");
    if (inputfp() == NULL)
    {
	raiseIOMsg("cannot fdopen child's stdout pipe");
	terminate();
	return -1;
    }

    _outputfp = fdopen(to_child[WRITE], "w");
    if (outputfp() == NULL)
    {
	raiseIOMsg("cannot fdopen child's stderr pipe");
	terminate();
	return -1;
    }

    return 0;
}

// Actually execute the child
void Agent::executeChild()
{
    // start child
    string exec_str = "exec " + path();
    char *exec_cmd = exec_str;
    execl("/bin/sh", "sh", "-c", exec_cmd, (char *)0);

    // could not find child: send message to parent via stderr
    perror("/bin/sh");
    _exit(1);
}


// process all status changes since last call and check if still running
bool Agent::running()
{
    if (_running && pid() >= 0)
    {
	// Ignore interrupts for a while
	typedef void (*SignalHandler)(int sig);

	SignalHandler istat = signal(SIGINT,  (void (*)(int))SIG_IGN);
	SignalHandler qstat = signal(SIGQUIT, (void (*)(int))SIG_IGN);
	SignalHandler hstat = signal(SIGHUP,  (void (*)(int))SIG_IGN);

	// Query current process state
	int r, status;
	if ((r = waitpid(pid(), &status, WNOHANG)) > 0)
	{
	    // Agent stopped or terminated
	    assert(r == pid());
	    hasNewStatus(status);
	}
	else if (r < 0)
	{
	    if (errno == ECHILD)
	    {
		// No such child: agent is not running
		abort();
	    }
	    else
		_raiseIOMsg("wait failed");
	}

        // restore interrupts
	signal(SIGINT,  istat);
	signal(SIGQUIT, qstat);
	signal(SIGHUP,  hstat);
    }

    return _running;
}


// child status change
void Agent::hasNewStatus(int status)
{
    _lastStatus = status;

    if (WIFEXITED(status) || WIFSIGNALED(status))
    {
	// agent died: inhibit further communication
	abort();
    }
}


// terminate process
void Agent::terminate(bool onExit)
{
    if (onExit)
    {
	// We're exiting: call only the default handlers
	removeAllHandlers();
	addDefaultHandler(Panic);
	addDefaultHandler(Died);
    }

    if (!running())
    {
	restoreParentIO();
	return;
    }

    _beingTerminated = true;
    if (onExit)
    {
	// close files
	Agent::abort();
    }
    else
    {
	// call subclasses abort handlers
	abort();
    }
    _beingTerminated = false;

    if (pid() >= 0)
    {
	// wait until we're terminated
	if (onExit)
	    Agent::waitToTerminate();
	else
	    waitToTerminate();
    }

    // now, we're really done.
    unsetRunning();
}


// send KILL signal
void Agent::_kill(int sig)
{
    if (running() && pid() >= 0)
    {
	// Hasta la vista, agent

	if (kill(pid(), sig) < 0)
	    raiseIOMsg("Could not kill");
    }
}


// Wait for agent to terminate, sending signals
void Agent::waitToTerminate()
{
    int sig = 0;

    for (unsigned seconds = 0; running(); seconds++)
    {
	sig = 0;

	if (seconds == terminateTimeOut())
	    sig = SIGTERM;

	if (seconds == hangupTimeOut())
	    sig = SIGHUP;

	if (seconds == killTimeOut())
	    sig = SIGKILL;

        if (sig)
	    _kill(sig);

        if (running())
	    sleep(1);
    }

    if (sig)
	raiseMsg(string("Agent wouldn't die (") + sigName(sig) + ")");
}


// Wait for agent to terminate, not sending signals
void Agent::wait()
{
    while (running())
    {
	int status;
	int ret = waitpid(pid(), &status, 0);
	if (ret > 0)
	{
	    // Agent stopped or terminated
	    assert(ret == pid());
	    hasNewStatus(status);
	}
    }
}


// Close channel
// This handles several file pointers addressing one file
void Agent::closeChannel(FILE *fp)
{
    if (fp != 0)
    {
	bool err = (fclose(fp) == EOF);
    
	if (fp == inputfp())
	{
	    _inputfp = 0;
	    if (err)
		raiseIOMsg("couldn't close input channel");
	}

	if (fp == errorfp())
	{
	    _errorfp = 0;
	    if (err)
		raiseIOMsg("couldn't close error channel");
	}

	if (fp == outputfp())
	{
	    _outputfp = 0;
	    if (err)
		raiseIOMsg("couldn't close output channel");
	}
    }
}

// send an EOF to agent
void Agent::shutdown()
{
    // We NEVER close stdout and stderr
    if (outputfp() == stdout || outputfp() == stderr)
	return;

    // Close output pipe
    closeChannel(outputfp());
}

// EOF on input detected
void Agent::inputEOF()
{
    // Check if we're still running
    running();

    // Call handlers
    callHandlers(InputEOF);

    // Close input pipe
    closeChannel(inputfp());
}

// EOF on error detected
void Agent::errorEOF()
{
    // Check if we're still running
    running();

    // Call handlers
    callHandlers(ErrorEOF);

    // Close error pipe
    closeChannel(errorfp());
}
	
// inhibit further communication
void Agent::abort()
{
    restoreParentIO();

    // close pipes
    // we deliberately ignore any error messages here
    shutdown();
    closeChannel(inputfp());
    closeChannel(errorfp());

    if (!_beingTerminated)
    {
	// declare agent as "not running"
	unsetRunning();
    }

    if (_lastStatus >= 0)
    {
	// call "Died" message handlers
	callHandlers(Died, statusName(_lastStatus));
	_lastStatus = -1;
    }
}

// Destructor
Agent::~Agent()
{
    // terminate agent gracefully
    terminate();
}
