// $Id$
// Filter display information from GDB output

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

char DispBuffer_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementation von DispBuffer.h
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

    // Displays abfangen und puffern, Rest zurueckgeben
    switch (already_read) {
    case DisplayPart:
	answer.prepend (answer_buffer);
	answer_buffer = "";
	already_read = Null;
	// weiter wie bei Null
    case Null:
	index = display_index(answer, gdb);

	if (index == 0) {
	    // Antwort besteht nur aus displays
	    display_buffer = answer;
	    already_read = DisplayFound;

	    answer = "";
	}
	else if (index > 0) {
	    // Ein Teil der Antwort sind Displays
	    display_buffer = answer.from(index);
	    already_read = DisplayFound;

	    answer = answer.before(index);
	}
	else {
	    // Vielleicht ein Display-Teil am Ende der Antwort ?
	    index = possible_begin_of_display (answer, gdb);

	    if (index == -1) {
		// nichts gefunden -> answer bleibt unveraendert
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
	// Fehler!
	assert(0);
	break;
    }

    // clog << "Display buffer = " << quote(display_buffer) << "\n";
}

string DispBuffer::answer_ended ()
{
    switch (already_read) {
    case DisplayPart:
	assert (display_buffer == "");
	return answer_buffer;
	break;
    case Null:
	assert (display_buffer == "");
	return "";
	break;
    case DisplayFound:
	return "";
	break;
    default:
	// Fehler!
	assert(0);
	break;
    }
    return "";
}


