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

// $Log$
// Revision 1.1  1995/05/01 15:47:46  zeller
// Initial revision
//
// Revision 1.26  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.25  1994/01/11  15:47:32  zeller
// Neu: Bei start() lesen, was bereits da sein mag
//
// Revision 1.24  1994/01/11  12:00:12  zeller
// Neu: Ready Event handler
//
// Revision 1.23  1993/12/02  13:59:46  zeller
// Neu: write()/flush() sind jetzt public
//
// Revision 1.22  1993/10/15  12:45:41  zeller
// Neu: Blocking-Modi ueberarbeitet
//
// Revision 1.21  1993/08/27  15:48:07  zeller
// Neu: TypeInfo
//
// Revision 1.20  1993/07/28  14:17:57  zeller
// Fix: Bei "Dummy"-Konstruktor Anzahl Event-Typen weitergereicht
//
// Revision 1.19  1993/07/28  10:20:07  zeller
// Neu: Konstruktor fuer Kommunikation innerhalb eines Prozesses
//
// Revision 1.18  1993/05/22  20:20:11  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.17  1993/04/22  10:49:23  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/22  10:36:52  zeller
// Lizenz verbessert
// 
// Revision 1.15  1993/04/20  22:56:07  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.14  1993/04/16  11:28:31  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.13  1992/11/03  18:44:14  zeller
// friend-Funktionen durch static-Funktionen ersetzt (sauberer)
// 
// Revision 1.12  1992/11/03  18:22:27  zeller
// Friends korrekt deklariert
// 
// Revision 1.11  1992/10/16  04:26:53  zeller
// Eigene Header-Dateien eingebunden
// 
// Revision 1.10  1992/07/27  18:08:55  zeller
// activateIO() wird jetzt in Agent::start() aufgerufen,
// daher ist eigenes start() unnoetig;
// activateIO(), deactivateIO() jetzt virtuell (und bereits in Agent.h definiert)
// 
// Revision 1.9  1992/06/02  02:08:35  zeller
// #pragma interface eingefuehrt
// 
// Revision 1.8  1992/06/02  02:06:48  zeller
// #pragma once entfernt
// 
// Revision 1.7  1992/05/22  14:27:00  zeller
// Neu: EOF-Handling
// 
// Revision 1.6  1992/05/14  18:12:04  zeller
// Handling Input und Error aufgespalten
// 
// Revision 1.5  1992/05/14  14:35:21  zeller
// Neu: _read() fuer Unterklassen
// 
// Revision 1.4  1992/05/14  11:15:26  zeller
// Neuer Konstruktor fuer Agenten-Entwickler
// 
// Revision 1.3  1992/05/14  10:06:20  zeller
// DataLength ausgegliedert
// 
// Revision 1.2  1992/05/13  18:02:24  zeller
// I/O erst beim Starten aktivieren
// 
// Revision 1.1  1992/05/12  16:56:17  zeller
// Initial revision
// 

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
