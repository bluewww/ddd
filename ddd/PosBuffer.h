// $Id$
// Filter position information from GDB output.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_PosBuffer_h
#define _DDD_PosBuffer_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// A `PosBuffer' filters position infos from GDB output.
//-----------------------------------------------------------------------------

// Misc includes
#include "bool.h"
#include "strclass.h"

class PosBuffer {
    // What was in the previous answer?
    enum ReadState {Null, PosPart, PosComplete};
    
    string pos_buffer;
    string func_buffer;
    string answer_buffer;	// Possible parts of positions
    string pc_buffer;
    ReadState already_read;

    bool started;		// Program has started
    bool terminated;		// Program has terminated
    bool recompiled;		// Program has been recompiled

public:
    // Constructor
    PosBuffer () :
	pos_buffer(""),
	func_buffer(""),
	answer_buffer(""),
	pc_buffer(""),
	already_read(Null),
	started(false),
	terminated(false),
	recompiled(false)
    {}

    // Filter positions from ANSWER and buffer them.  ANSWER contains
    // any remaining parts.
    void filter (string& answer);

    // GDB output has ended.  Return any non-position parts.
    string answer_ended ();

    // Did we find a position in the last output?
    bool pos_found () const { return already_read == PosComplete; }
    bool pc_found ()  const { return pc_buffer != ""; }

    // Return the position found.
    const string& get_position () const { return pos_buffer; }
    const string& get_function () const { return func_buffer; }
    const string& get_pc ()       const { return pc_buffer; }

    // Other properties.
    bool started_found()    const { return started; }
    bool terminated_found() const { return terminated; }
    bool recompiled_found() const { return recompiled; }

    // Call this before filtering any new output.
    void clear ()
    {
	pos_buffer    = "";
	func_buffer   = "";
	answer_buffer = "";
	pc_buffer     = "";
	already_read  = Null;
	started       = false;
	terminated    = false;
	recompiled    = false;
    }
};

// A regex for C addresses ("0xdead"), Modula-2 addresses ("0BEEFH"),
// and Chill addresses ("H'AFFE").  XDB uses the pattern `00000000' for
// nil pointers; XBD uses `nil' and `NIL'.  The origin of the last
// format "16_FFFF" is lost in the mist of time.
#define RXADDRESS "(0x[0-9a-fA-F]+|0[0-9a-fA-F]+[hH]|H'[0-9a-fA-F]+" \
                  "|00+|[(]nil[)]|NIL|16_[0-9a-f]+)"
extern regex rxaddress;

#endif // _DDD_PosBuffer_h
// DON'T ADD ANYTHING BEHIND THIS #endif
