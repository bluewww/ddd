// $Id$ -*- C++ -*-
// An agent interface using ptys (pseudo ttys)

// Copyright (C) 1994 Technische Universitaet Braunschweig, Germany.
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

#ifndef _Nora_TTYAgent_h
#define _Nora_TTYAgent_h

#ifdef __GNUG__
#pragma interface
#endif

#include "LiterateA.h"
#include <unistd.h>

const unsigned TTYAgent_NTypes = LiterateAgent_NTypes;

class TTYAgent: public LiterateAgent {
public:
    DECLARE_TYPE_INFO

private:
    string _master_tty;  // master side of terminal
    string _slave_tty;   // slave side of terminal
    int master;		 // master file descriptor
    bool push;           // true if extra devices must be pushed

    int open_master();   // find and open master tty
    int open_slave();	 // open slave tty 

protected:
    // hooks for alternative communication schemes
    virtual int setupCommunication();
    virtual int setupChildCommunication();
    virtual int setupParentCommunication();

public:
    // Constructors
    TTYAgent(XtAppContext app_context, char *pth,
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, pth, nTypes),
	master(-1),
	push(false)
    {}

    TTYAgent(XtAppContext app_context, FILE *in = stdin,
	     FILE *out = stdout, FILE *err = 0, 
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, in, out, err, nTypes),
	master(-1),
	push(false)
    {}

    TTYAgent(XtAppContext app_context, bool dummy,
	     unsigned nTypes = TTYAgent_NTypes):
        LiterateAgent(app_context, dummy, nTypes),
	master(-1),
	push(false)
    {}

    // Return the name of the used tty
    const string& master_tty() const { return _master_tty; }
    const string& slave_tty() const  { return _slave_tty; }
};

#endif // _Nora_TTYAgent_h
// DON'T ADD ANYTHING BEHIND THIS #endif
