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

char LiterateAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if defined(HAVE_FCNTL) && !defined(HAVE_FCNTL_DECL) && !defined(fcntl)
extern "C" int fcntl(int fd, int command, ...);
#endif

#include "bool.h"
#include "LiterateA.h"
#include "SignalB.h"
#include "ChunkQueue.h"

// This package reads data in non-blocking mode, reading
// only data that is currently present. However, this is not a good
// idea for ttys, since if the process is run in the background, it
// will set non-blocking mode for its controlling tty. Since
// this mode change affects all processes reading from this tty,
// they will detect an EOF on input and (most likely) exit.
// Thus, for ttys, we use blocking mode and read only one line at a time.
// This works fine unless the process runs in raw or cbreak mode.

#ifndef BLOCK_TTY_INPUT
#define BLOCK_TTY_INPUT 1
#endif

DEFINE_TYPE_INFO_1(LiterateAgent, AsyncAgent)

// Check if fp is a tty and wants blocking input
inline bool blocking_tty(FILE *fp)
{
    return BLOCK_TTY_INPUT && isatty(fileno(fp));
}

// I/O functions

// Input data handling
int LiterateAgent::readInput(char*& data)
{
    data = "";
    if (inputfp() == 0 || !activeIO)
	return -1;

    return _readInput(data);
}

// Error data handling
int LiterateAgent::readError(char*& data)
{
    data = "";
    if (errorfp() == 0 || !activeIO)
	return -1;

    return _readError(data);
}

// write a whole string
int LiterateAgent::write(const char *data, int length)
{
    if (outputfp() == 0 || !activeIO)
	return -1;

    int failures = 0;

    while (length > 0)
    {
	errno = 0;
	int nitems = ::write(fileno(outputfp()), data, length);

	if (nitems == 0)
	{
	    if (errno)
	    {
		if (++failures <= 3)
		{
		    ostrstream os;
		    os << "write failed (attempt #" 
		       << failures << ", still trying)";
		    string s(os);
		    raiseIOMsg(s);
		    sleep(1);
		    continue;
		}
		else
		{
		    raiseIOMsg("write failed");
		    return -1;
		}
	    }
	}

	dispatch(Output, (char *)data, nitems);

	length -= nitems;
	data += nitems;
    }

    if (failures)
	raiseMsg("write ok");

    return 0;
}

// flush output buffers
int LiterateAgent::flush()
{
    // Not needed, since we use immediate write()
    return 0;
}


// read from fp without delay
int LiterateAgent::_readNonBlocking(char *buffer, int nelems, FILE *fp)
{
    // Avoid being stopped when file is non-blocking
    SignalBlocker sb;

    // Make file non-blocking
    int flags = fcntl(fileno(fp), F_GETFL, 0);
    if (flags == -1)
	_raiseIOWarning("cannot get file descriptor status flags");
    if (fcntl(fileno(fp), F_SETFL, flags | O_NONBLOCK) == -1)
	_raiseIOWarning("cannot set file to non-blocking mode");

    // Read stuff
    int nitems = fread(buffer, sizeof(char), nelems, fp);

    // Reset file state
    if (fcntl(fileno(fp), F_SETFL, flags) == -1)
	_raiseIOWarning("cannot restore file mode");

    return nitems;
}


// read from fp
int LiterateAgent::_read(char*& data, FILE *fp)
{
    static ChunkQueue queue;

    queue.discard();
    char buffer[BUFSIZ + 1];
    
    if (blocking_tty(fp))
    {
	// Non-blocking ttys are nasty, so we read only the 
	// single line available here and now.
	char *s = fgets(buffer, BUFSIZ, fp);

	if (s != 0)
	    queue.append(buffer, strlen(buffer));
    }
    else
    {
	// Otherwise, read and accumulate whatever's there
	int length;
	while ((length = _readNonBlocking(buffer, BUFSIZ, fp)) > 0)
	    queue.append(buffer, length);

	if (length < 0)
	    raiseIOMsg("read from agent failed");
    }

    data = queue.data();
    return queue.length();
}

int LiterateAgent::_readInput(char*& data)
{
    return _read(data, inputfp());
}

int LiterateAgent::_readError(char*& data)
{
    return _read(data, errorfp());
}



// Dispatchers

// dispatch data to <type> handler 
void LiterateAgent::dispatch(int type, char *data, int length)
{
    char c = data[length];
    if (c != '\0')
	data[length] = '\0';

    DataLength dl(data, length);

    // call global handlers
    callHandlers(type, &dl);

    if (c != '\0')
	data[length] = c;
}
    

// Handlers

// Data handlers

void LiterateAgent::outputReady(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->callHandlers(Ready);
}

void LiterateAgent::inputReady(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->readAndDispatchInput();
}

void LiterateAgent::errorReady(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->readAndDispatchError();
}

// Input Data is available: read all and call Input handlers of current job
void LiterateAgent::readAndDispatchInput(bool expectEOF)
{
    char *data;

    int length = readInput(data);
    if (length > 0)
	dispatch(Input, data, length);
    else if (!expectEOF && length == 0 && inputfp() != 0 && feof(inputfp()))
	inputEOF();
}

// Error Data is available: read all and call Error handlers of current job
void LiterateAgent::readAndDispatchError(bool expectEOF)
{
    char *data;

    int length = readError(data);
    if (length > 0)
	dispatch(Error, data, length);
    else if (!expectEOF && length == 0 && errorfp() != 0 && feof(errorfp()))
	errorEOF();
}


// Exception handlers

void LiterateAgent::outputException(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->_outputException();
}

void LiterateAgent::inputException(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->_inputException();
}

void LiterateAgent::errorException(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->_errorException();
}

void LiterateAgent::_outputException()
{
    raiseWarning("I/O exception when writing to agent");
}

void LiterateAgent::_inputException()
{
    raiseWarning("I/O exception when reading from agent's stdin");
}

void LiterateAgent::_errorException()
{
    raiseWarning("I/O exception when reading from agent's stderr");
}

// (Re)set I/O handlers
void LiterateAgent::handlerChange()
{
    if (activeIO)
	_activateIO();
}

// Activate Handlers
void LiterateAgent::_activateIO()
{
    // We do select this event only if a handler is present
    // Otherwise, outputReady() may be called all the time
    setHandler(OutputReady, hasHandler(Ready) ? outputReady : 0);
    
    setHandler(InputReady,      inputReady);
    setHandler(ErrorReady,      errorReady);
    
    setHandler(OutputException, outputException);
    setHandler(InputException,  inputException);
    setHandler(ErrorException,  errorException);
}

// Deactivate Handlers
void LiterateAgent::_deactivateIO()
{
    setHandler(OutputReady,     0);
    setHandler(InputReady,      0);
    setHandler(ErrorReady,      0);

    setHandler(OutputException, 0);
    setHandler(InputException,  0);
    setHandler(ErrorException,  0);
}


// Starter
void LiterateAgent::start()
{
    AsyncAgent::start();
    
    // dispatch input data that may already be there
    if (inputfp() != NULL && !blocking_tty(inputfp()))
	readAndDispatchInput();

    if (errorfp() != NULL && !blocking_tty(errorfp()))
	readAndDispatchError();
}

// Terminator
void LiterateAgent::abort()
{
    // dispatch remaining input data (if there is some remaining)
    activateIO();
    readAndDispatchInput(true);
    readAndDispatchError(true);

    // now, clean up
    deactivateIO();
    AsyncAgent::abort();
}
