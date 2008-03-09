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

char DispBuffer_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
// Filter displays from GDB output
//-----------------------------------------------------------------------------

#include "DispBuffer.h"

// Misc includes
#include "assert.h"
#include "cook.h"

// DDD includes
#include "disp-read.h"
#include "comm-manag.h"
#include "ddd.h"

void DispBuffer::filter (string& answer)
{
    int index;

    // Fetch and buffer displays, return remainder
    switch (already_read) {
    case DisplayPart:
	answer.prepend (answer_buffer);
	answer_buffer = "";
	already_read = Null;
	// FALL THROUGH

    case Null:
	index = display_index(answer, gdb);

	if (index == 0) {
	    // We only have displays
	    display_buffer = answer;
	    already_read = DisplayFound;

	    answer = "";
	}
	else if (index > 0) {
	    // Displays are a part of the answer
	    display_buffer = answer.from(index);
	    already_read = DisplayFound;

	    answer = answer.before(index);
	}
	else {
	    // Maybe there is a display part at the end of the answer?
	    index = possible_begin_of_display (answer, gdb);

	    if (index == -1) {
		// nothing found -> answer remains unchanged
	    }
	    else {
		answer_buffer = answer.from(index);
		already_read = DisplayPart;

		answer = answer.before(index);
	    }
	}
	break;

    case DisplayFound:
	display_buffer += answer;

	answer = "";
	break;

    default:
	assert(0);		// This can't happen.
	::abort();
	break;
    }

    // std::clog << "Display buffer = " << quote(display_buffer) << "\n";
}

string DispBuffer::answer_ended ()
{
    switch (already_read) {
    case DisplayPart:
    {
	assert (display_buffer.empty());
	string ans = answer_buffer;
	answer_buffer = "";
	return ans;
    }

    case Null:
    {
	assert (display_buffer.empty());
	return "";
    }

    case DisplayFound:
    {
	return "";
    }

    default:
    {
	assert(0);		// This can't happen
	::abort();
	break;
    }
    }

    return "";
}


