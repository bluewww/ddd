// $Id$
// Agent interface on a callback basis

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_LiterateAgent_h
#define _DDD_LiterateAgent_h

#ifdef __GNUG__
#pragma interface
#endif

/*
    LiterateAgent(app_context, p) opens an asynchronous three-way communication
    channel to the agent p (see Agent, AsyncAgent).

    Communication with the agent is achieved by writing data
    to the agent (write()).

    Each LiterateAgent can be associated with handlers for the events
    defined in Agent, plus:
    LiterateAgent::Output -- data was written to the agent
    LiterateAgent::Input  -- data was received from the agent
    LiterateAgent::Error  -- data was received from the agent's stderr

    A handler takes the form
    void handler(Agent *source, void *client_data, void *call_data)
    where source is the agent causing the event,
    DataLength *d = (DataLength *)call_data is the data sent or received,
    and client_data is arbitrary data passed to addHandler() when
    the handler was added.

    This class dispatches data to handlers as soon as it is available.
    Use a more elaborate subclass to dispatch data in appropriate units.
*/

#include "assert.h"
#include <stdio.h>
#include <string.h>
#include "strclass.h"

#include "AsyncAgent.h"
#include "DataLength.h"


// Event types
const unsigned Ready  = AsyncAgent_NTypes; // Ready for output
const unsigned Output = Ready + 1;         // Output written
const unsigned Input  = Output + 1;        // Input read
const unsigned Error  = Input + 1;	   // Error read

const unsigned LiterateAgent_NTypes = Error + 1; // number of events


class LiterateAgent: public AsyncAgent {
public:
    DECLARE_TYPE_INFO

private:
    bool activeIO;		// Flag: I/O handlers active?
    void _activateIO();         // activate I/O handlers
    void _deactivateIO();       // deactivate I/O handlers

    virtual void activateIO()   
    {
	if (!activeIO) 
	{
	    _activateIO();
	    activeIO = true; 
	}
    }
    virtual void deactivateIO() 
    {
	if (activeIO) 
	{
	    _deactivateIO();
	    activeIO = false; 
	}
    }

    int _read(char*& data, FILE *fp); // Simple read function
    int _readNonBlocking(char* data, int size, FILE *fp); // Read what's there

    // Event Handlers
    static void outputReady(AsyncAgent *c);
    static void inputReady(AsyncAgent *c);
    static void errorReady(AsyncAgent *c);

    // Flag: do we want TTYs to be read in blocking mode?
    bool _block_tty_input;

    // Return default value for _block_tty_input
    static bool default_block_tty_input();

public:
    // Resources
    bool block_tty_input() const     { return _block_tty_input; }
    bool block_tty_input(bool state) { return _block_tty_input = state; }

protected:
    // Input data handling
    int readInput(char*& data);
    virtual int _readInput(char *& data);
    virtual void readAndDispatchInput(bool expectEOF = false);

    // Error data handling
    int readError(char*& data);
    virtual int _readError(char *& data);
    virtual void readAndDispatchError(bool expectEOF = false);

    // Event management
    virtual void dispatch(int type, char *data, int length); // dispatch data

    // Called when handlers were changed
    void handlerChange();

    // Check if fp is a tty and wants blocking input
    bool blocking_tty(FILE *fp) const
    {
	return block_tty_input() && isatty(fileno(fp));
    }

public:
    // Constructor for Agent users
    LiterateAgent(XtAppContext app_context, const string& pth,
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, pth, nTypes), activeIO(false),
	_block_tty_input(default_block_tty_input())
    {}

    // Constructor for Agent writers
    LiterateAgent(XtAppContext app_context, FILE *in = stdin,
		  FILE *out = stdout, FILE *err = 0, 
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, in, out, err, nTypes), activeIO(false),
	// When reading from stdin, always block TTY input.
	_block_tty_input(in == stdin || default_block_tty_input())
    {}

    // "Dummy" Constructor without any communication
    LiterateAgent(XtAppContext app_context, bool dummy,
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, dummy, nTypes), activeIO(false),
	_block_tty_input(default_block_tty_input())
    {}

    // Duplicator
    LiterateAgent(const LiterateAgent& lit)
	: AsyncAgent(lit), activeIO(lit.activeIO),
	  _block_tty_input(lit.block_tty_input())
    {}
    virtual Agent *dup() const { return new LiterateAgent(*this); }

    // Output data handling
    virtual int write(const char *data, int length);

    // Flush output
    int flush();

    // Starter
    virtual void start();
    
    // Terminator
    virtual void abort();
};

#endif // _DDD_LiterateAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
