// $Id$
// Filter position information from GDB output.

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char PosBuffer_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementation von PosBuffer.h
//-----------------------------------------------------------------------------
#include "PosBuffer.h"
#include "comm-manag.h"
#include "string-fun.h"
#include "ddd.h"

// includes von Nora-Zeugs
#include "assert.h"
#include "cook.h"

// Filter all lines from ANSWER beginning with LINE
// This is required to suppress the line number output
// after a `stopping in' message.
static void filter_line(string& answer, int line)
{
    if (line <= 0)
	return;

    int pos = 0;
    do {
	if (atoi((char *)answer + pos) == line)
	{
	    answer = answer.before(pos) + answer.after('\n', pos);
	    break;
	}
	pos = answer.index('\n', pos) + 1;
    } while (pos > 0);
}

void PosBuffer::filter (string& answer)
{
    // Positionsangabe abfangen und puffern, Rest zurueckgeben
    switch (already_read) {
    case PosComplete:
	// Nichts mehr zu filtern
	assert (pos_buffer != "");
	assert (answer_buffer == "");

	// Skip possible line number info
	switch (gdb->type())
	{
	case GDB:
	    break;

	case DBX:
	    string line_s = pos_buffer;
	    if (line_s.contains(':'))
		line_s = line_s.after(':');
	    int line = atoi(line_s);
	    filter_line(answer, line);
	}
	break;
    case PosPart:
	answer.prepend (answer_buffer);
	answer_buffer = "";
	already_read = Null;
	// weiter wie bei Null
    case Null:
	{
	    switch (gdb->type())
	    {
	    case GDB:
	    {
		int index1 = answer.index ("\032\032");

		if (index1 == -1) 
		{
		    int index_p = answer.index ("\032");

		    if (index_p == int(answer.length()) - 1) {
			// moegl. Beginn einer Positionsangabe
			// am Ende von answer
			already_read = PosPart;
			answer_buffer = "\032";
			answer = answer.before (index_p);

			return;
		    }
		    // nichts gefunden
		    return;
		}
		// Antwort enthaelt Positionsangabe
		int index2 = answer.index ("\n", index1);

		if (index2 == -1) {
		    // keine komplette Positionsangabe
		    already_read = PosPart;
		    answer_buffer = answer.from (index1);
		    answer = answer.before (index1);

		    return;
		}
		assert (index1 < index2);

		// Positionsangabe komplett
		pos_buffer = answer.at (index1 + 2, index2 - (index1 + 2));
		answer.at (index1, index2 - index1 + 1) = "";
		already_read = PosComplete;
	    }
	    break;

	    case DBX:
	    {
		if (answer.contains("stopped in "))
		{
		    string file;

		    // Stop reached
		    if (answer.contains("in file "))
		    {
			// File name given
			file = answer.after("in file ");
			if (file.contains('\n'))
			    file = file.before('\n');
			file = unquote(file);
		    }
		    else
		    {
			// Function name given
			string func = answer.after("stopped in ");
			if (func.contains(" at line"))
			    func = func.before(" at line");
			func_buffer = func;
			file = "";
		    }

		    string line = answer.after("at line ");
		    line = line.through(rxint);

		    if (file != "")
			pos_buffer = file + ":" + line;
		    else
			pos_buffer = line;
		    already_read = PosComplete;
		    filter_line(answer, atoi(line));
		}
		else if (answer.contains("Current function is "))
		{
		    // Up/Down command entered
		    string nr = answer.after("\n");
		    pos_buffer = itostring(atoi(nr));
		    already_read = PosComplete;

		    // Show current function only
		    answer = answer.from("Current function is ");
		    answer = answer.through("\n");
		}
	    }
	    break;
	    }
	}
	break;

    default:
	// Fehler!
	assert(0);
	break;
    }
}

string PosBuffer::answer_ended ()
{
    switch (already_read) {
    case PosPart:
	assert (pos_buffer == "");
	return answer_buffer;
	break;
    case Null:
	assert (pos_buffer == "");
	return "";
	break;
    case PosComplete:
	assert (pos_buffer != "");
	return "";
	break;
    default:
	// Fehler!
	assert(0);
	break;
    }
    return "";
}
