// $Id$
// Agent interface on a callback basis

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

#ifndef _Nora_LiterateAgent_h
#define _Nora_LiterateAgent_h

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

class LiterateAgent: public AsyncAgent {
public:
    DECLARE_TYPE_INFO

    // Event types
    const unsigned Ready  = AsyncAgent::NTypes;	// Ready for output
    const unsigned Output = Ready + 1;          // Output written
    const unsigned Input  = Output + 1;         // Input read
    const unsigned Error  = Input + 1;	        // Error read

    const unsigned NTypes = Error + 1;	        // number of events

private:
    void _outputException();	// exception on output channel
    void _inputException();	// exception on input channel
    void _errorException();	// exception on error channel

    bool activeIO;           // Flag: I/O handlers active?
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
    static void outputException(AsyncAgent *c);
    static void inputException(AsyncAgent *c);
    static void errorException(AsyncAgent *c);

protected:

    // input data handling
    int readInput(char*& data)
    {
	data = "";
	if (inputfp() == 0 || !activeIO)
	    return -1;

	return _readInput(data);
    }
    virtual int _readInput(char *& data);
    virtual void readAndDispatchInput(bool expectEOF = false);

    // error data handling
    int readError(char*& data)
    {
	data = "";
	if (errorfp() == 0 || !activeIO)
	    return -1;

	return _readError(data);
    }
    virtual int _readError(char *& data);
    virtual void readAndDispatchError(bool expectEOF = false);

    // event management
    virtual void dispatch(int type, char *data, int length); // dispatch data

    // called when handlers were changed
    void handlerChange();

public:
    // Constructors
    LiterateAgent(XtAppContext app_context, char *pth,
	 unsigned nTypes = NTypes):
	AsyncAgent(app_context, pth, nTypes), activeIO(false)
    {}

    LiterateAgent(XtAppContext app_context, FILE *in = stdin,
	FILE *out = stdout, FILE *err = 0, unsigned nTypes = NTypes):
	AsyncAgent(app_context, in, out, err, nTypes), activeIO(false)
    {}

    LiterateAgent(XtAppContext app_context, bool dummy,
	unsigned nTypes = NTypes):
	AsyncAgent(app_context, dummy, nTypes), activeIO(false)
    {}

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
