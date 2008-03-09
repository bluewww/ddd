// $Id$ -*- C++ -*-
// An agent interface using ptys (pseudo ttys)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_TTYAgent_h
#define _DDD_TTYAgent_h

#include "LiterateA.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>		// O_RDWR

const unsigned TTYAgent_NTypes = LiterateAgent_NTypes;

class TTYAgent: public LiterateAgent {
public:
    DECLARE_TYPE_INFO

private:
    string _master_tty;   // master side of terminal
    string _slave_tty;    // slave side of terminal
    int master;		  // master file descriptor
    int slave;		  // slave file descriptor
    bool push;            // true if extra devices must be pushed

    void open_master();   // find and open master tty
    void open_slave();	  // open slave tty 

protected:
    // Hooks for alternative communication schemes
    virtual int setupCommunication();
    virtual int setupChildCommunication();
    virtual int setupParentCommunication();

    // Open a tty.
    // Like open(TTY, FLAGS), but care for EAGAIN and EWOULDBLOCK conditions
    virtual int open_tty(const char *tty, int flags = O_RDWR);

    // Check whether a TTY is usable
    virtual bool tty_ok(const char *tty);

public:
    // Constructors
    TTYAgent(XtAppContext app_context, const string& pth,
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, pth, nTypes),
	_master_tty(), _slave_tty(),
	master(-1), slave(-1),
	push(false)
    {}

    TTYAgent(XtAppContext app_context, FILE *in = stdin,
	     FILE *out = stdout, FILE *err = 0, 
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, in, out, err, nTypes),
	_master_tty(), _slave_tty(),
	master(-1), slave(-1),
	push(false)
    {}

    TTYAgent(XtAppContext app_context, bool dummy,
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, dummy, nTypes),
	_master_tty(), _slave_tty(),
	master(-1), slave(-1),
	push(false)
    {}

    // Duplicator
    TTYAgent (const TTYAgent& tty)
	: LiterateAgent(tty),
	  _master_tty(tty.master_tty()),
	  _slave_tty(tty.slave_tty()),
	  master(tty.master),
	  slave(tty.slave),
	  push(tty.push)
    {};
    virtual Agent *dup() const { return new TTYAgent(*this); }

    // Return the name of the used tty
    const string& master_tty() const { return _master_tty; }
    const string& slave_tty() const  { return _slave_tty; }
    const char *slave_tty_c() const { return _slave_tty.chars(); }
};

#endif // _DDD_TTYAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
