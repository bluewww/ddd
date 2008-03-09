// $Id$
// Filter display information from GDB output

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_DispBuffer_h
#define _DDD_DispBuffer_h

//-----------------------------------------------------------------------------
// A `DispBuffer' filters display information from GDB output
//-----------------------------------------------------------------------------

// Misc includes
#include "bool.h"
#include "strclass.h"

class DispBuffer {
    // What was in the previous answer?
    enum ReadState {Null, DisplayPart, DisplayFound};
    
    string display_buffer;
    string answer_buffer;      // possible parts of display output
    ReadState already_read;

public:
    // Constructor
    DispBuffer () :
	display_buffer(""),
	answer_buffer(""),
	already_read(Null)
    {}

    // Filter `display' output from ANSWER and buffer them.
    // After returning, ANSWER contains non-display output.
    void filter (string& answer);

    // The answer has ended; return buffered output.
    string answer_ended ();

    // Were there any displays in the last output?
    bool displays_found () const { return !display_buffer.empty(); }
    bool partial_displays_found () const
    {
	return !displays_found() && already_read == DisplayPart;
    }

    // Return any displays found
    const string& get_displays () const { return display_buffer; }

    // Call before filtering next answer
    void clear () {
	display_buffer = "";
	answer_buffer = "";
	already_read = Null;
    }

};

#endif // _DDD_DispBuffer_h
// DON'T ADD ANYTHING BEHIND THIS #endif
