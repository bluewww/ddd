// $Id$
// Agent interface on a callback basis

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

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

    int _read(char*& data, FILE *fp);	// simple read function
    int _readNonBlocking(char* data, int size, FILE *fp); // read what's there

    // Event Handlers
    static void outputReady(AsyncAgent *c);
    static void inputReady(AsyncAgent *c);
    static void errorReady(AsyncAgent *c);

protected:

    // input data handling
    int readInput(char*& data);
    virtual int _readInput(char *& data);
    virtual void readAndDispatchInput(bool expectEOF = false);

    // error data handling
    int readError(char*& data);
    virtual int _readError(char *& data);
    virtual void readAndDispatchError(bool expectEOF = false);

    // event management
    virtual void dispatch(int type, char *data, int length); // dispatch data

    // called when handlers were changed
    void handlerChange();

public:
    // Constructors
    LiterateAgent(XtAppContext app_context, string pth,
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, pth, nTypes), activeIO(false)
    {}

    LiterateAgent(XtAppContext app_context, FILE *in = stdin,
		  FILE *out = stdout, FILE *err = 0, 
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, in, out, err, nTypes), activeIO(false)
    {}

    LiterateAgent(XtAppContext app_context, bool dummy,
		  unsigned nTypes = LiterateAgent_NTypes):
	AsyncAgent(app_context, dummy, nTypes), activeIO(false)
    {}

    // Duplicator
    LiterateAgent(const LiterateAgent& lit)
	: AsyncAgent(lit), activeIO(lit.activeIO)
    {}
    virtual Agent *dup() const { return new LiterateAgent(*this); }

    // output data handling
    virtual int write(const char *data, int length);

    // flush output
    int flush();

    // Starter
    virtual void start();
    
    // Terminator
    virtual void abort();
};

#endif
