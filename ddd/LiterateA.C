// $Id$
// Agent interface on a callback basis

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char LiterateAgent_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>		// On Linux, includes _G_config.h
#include <ctype.h>

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


DEFINE_TYPE_INFO_1(LiterateAgent, AsyncAgent)

// ---------------------------------------------------------------------------
// TTY blocking
// ---------------------------------------------------------------------------

// This package reads data in non-blocking mode, reading only data
// that is currently present.  Unfortunately, this is not a good idea
// for ttys, since if the process is run in the background, it will
// set non-blocking mode for its controlling tty.  Since this mode
// change affects all processes reading from this tty, they will
// detect an EOF on input and (most likely) exit.

// Consequently, we provide a special flag, BLOCK_TTY_INPUT.  If
// BLOCK_TTY_INPUT is TRUE, we use blocking mode for TTYs and read
// only one line at a time.  This works fine unless the process
// runs in raw or cbreak mode.  If BLOCK_TTY_INPUT is FALSE, we
// have no special treatment for TTYs.

// Determine an appropriate default setting
bool LiterateAgent::default_block_tty_input()
{
#if defined(BLOCK_TTY_INPUT)
    return BLOCK_TTY_INPUT;

#elif defined(linux) && __GLIBC__ >= 2
    // According to Ray Dassen <jdassen@wi.LeidenUniv.nl>, Linux with
    // GNU libc 6 (that is, glibc 2.x or later) wants BLOCK_TTY_INPUT
    // to be unset.
    return false;

#elif defined(_LINUX_C_LIB_VERSION_MAJOR)
    // On Linux with Linux libc, the correct setting depends on the
    // libc version.

    // We get the version number from the library, rather than from
    // the include file, since the (dynamic) library may have changed
    // since compilation.
    extern char *__linux_C_lib_version;

    // If this doesn't link or compile, replace __linux_C_lib_version by "".
    char *s = __linux_C_lib_version;

    // Find start of version number
    while (*s && !isdigit(*s))
	s++;
    if (atoi(s) == 0)
    {
	// No library version - try the included one
	s = _LINUX_C_LIB_VERSION;
    }

    int version_major = atoi(s);
    while (*s && *s != '.')
	s++;
    if (*s == '.')
	s++;
    int version_minor = atoi(s);
    while (*s && *s != '.')
	s++;
    if (*s == '.')
	s++;
    int version_subminor = atoi(s);

    // According to Ray Dassen <jdassen@wi.LeidenUniv.nl>, Linux with GNU
    // libc 6 wants BLOCK_TTY_INPUT to be unset.
    if (version_major > 5)
	return false;

    // According to Terence Spielman <terence@globeset.com>, this is also
    // true for Linux with GNU libc 5.4.35.  Anders Wegge Jakobsen
    // <wegge@wegge.dk> reports the same for GNU libc 5.4.38.
    if (version_major == 5 && version_minor > 4)
	return false;
    if (version_major == 5 && version_minor == 4 && version_subminor > 33)
	return false;

    // On the other hand, Linux with GNU libc 5.4.33 and earlier,
    // needs BLOCK_TTY_INPUT being set.  Hence, we set BLOCK_TTY_INPUT
    // only for GNU libc 5.4.33 and earlier.
    return true;

#else  // !LINUX && !BLOCK_TTY_INPUT
    // For all other systems, the default is BLOCK_TTY_INPUT set.  (I
    // don't know whether this is the `best' setting, but I have no
    // reason to change a default that has been around successfully
    // for so long...)

    return true;
#endif // !LINUX && !BLOCK_TTY_INPUT
}


// ---------------------------------------------------------------------------
// I/O functions
// ---------------------------------------------------------------------------

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

// Write a whole string
int LiterateAgent::write(const char *data, int length)
{
    if (outputfp() == 0 || !activeIO)
	return -1;

    int failures = 0;

    while (length > 0)
    {
	errno = 0;
	int nitems = ::write(fileno(outputfp()), data, length);

	if (nitems <= 0)
	{
	    if (nitems == 0 && ++failures <= 3)
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

	assert(nitems > 0);
	dispatch(Output, (char *)data, nitems);

	length -= nitems;
	data += nitems;
    }

    if (failures)
	raiseMsg("write ok");

    return 0;
}

// Flush output buffers
int LiterateAgent::flush()
{
    // Not needed, since we use immediate write()
    return 0;
}


// Read from fp without delay
int LiterateAgent::_readNonBlocking(char *buffer, int nelems, FILE *fp)
{
    // Avoid being stopped when file is non-blocking
    SignalBlocker sb;

#if HAVE_FCNTL && defined(O_NONBLOCK)
    // Make file non-blocking
    int flags = fcntl(fileno(fp), F_GETFL, 0);
    if (flags == -1)
	_raiseIOWarning("cannot get file descriptor status flags");
    if (fcntl(fileno(fp), F_SETFL, flags | O_NONBLOCK) == -1)
	_raiseIOWarning("cannot set file to non-blocking mode");
#endif

    // Read stuff
    int nitems = fread(buffer, sizeof(char), nelems, fp);

    if (nitems <= 0)
    {
	if (false
#ifdef EAGAIN
	|| errno == EAGAIN
#endif
#ifdef EWOULDBLOCK
	|| errno == EWOULDBLOCK
#endif
	)
	{
	    // Resource temporarily unavailable: an operation that
	    // would block was attempted on an object that has
	    // non-blocking mode selected.  Trying the same operation
	    // again will block until some external condition makes it
	    // possible to read, write, or connect (whatever the
	    // operation).  So, just try again next time.
	    nitems = 0;

	    // Linux libc 5.4.39 and later treats EAGAIN and
	    // EWOULDBLOCK as EOF condition.  This is a bad idea.
	    clearerr(fp);
	}
    }


#if HAVE_FCNTL && defined(F_SETFL)
    // Reset file state
    if (fcntl(fileno(fp), F_SETFL, flags) == -1)
	_raiseIOWarning("cannot restore file mode");
#endif

    return nitems;
}


// Read from fp
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
	// Otherwise, read and accumulate whatever's there - up to
	// BUFSIZ characters
	int length = -1;
	while (queue.length() < BUFSIZ
	       && (length = _readNonBlocking(buffer, BUFSIZ, fp)) > 0)
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



// ---------------------------------------------------------------------------
// Dispatchers
// ---------------------------------------------------------------------------

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
    

// ---------------------------------------------------------------------------
// Handlers
// ---------------------------------------------------------------------------

// Data handlers

void LiterateAgent::outputReady(AsyncAgent *c)
{
    ptr_cast(LiterateAgent, c)->callHandlers(Ready);
}

void LiterateAgent::inputReady(AsyncAgent *c)
{
    char data[BUFSIZ];
    char *datap = data;
    LiterateAgent *lc = ptr_cast(LiterateAgent, c);
    if (lc != 0)
    {
	int length = lc->readInput(datap);
	if (length > 0)
	    lc->dispatch(Input, datap, length);
	else if (length == 0 && lc->inputfp() != 0 && feof(lc->inputfp()))
	    lc->inputEOF();
    }
}

void LiterateAgent::errorReady(AsyncAgent *c)
{
    char data[BUFSIZ];
    char *datap = data;
    LiterateAgent *lc = ptr_cast(LiterateAgent, c);
    if (lc != 0)
    {
	int length = lc->readError(datap);
	if (length > 0)
	    lc->dispatch(Error, datap, length);
	else if (length == 0 && lc->errorfp() != 0 && feof(lc->errorfp()))
	    lc->errorEOF();
    }
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
    setHandler(InputReady,  inputReady);
    setHandler(ErrorReady,  errorReady);
}

// Deactivate Handlers
void LiterateAgent::_deactivateIO()
{
    setHandler(OutputReady, 0);
    setHandler(InputReady,  0);
    setHandler(ErrorReady,  0);
}


// Starter
void LiterateAgent::start()
{
    AsyncAgent::start();
    
    // Dispatch input data that may already be there
    if (inputfp() != 0 && !blocking_tty(inputfp()))
	readAndDispatchInput();

    if (errorfp() != 0 && !blocking_tty(errorfp()))
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
