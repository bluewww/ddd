// $Id$
// Filter position information from GDB output.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

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

void PosBuffer::filter (string& answer)
{
    // Positionsangabe abfangen und puffern, Rest zurueckgeben
    switch (already_read) {
    case PosComplete:
	// Nichts mehr zu filtern
	assert (pos_buffer != "");
	assert (answer_buffer == "");
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

		    if (index_p == answer.length() - 1) {
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

		    // Skip possible line number info
		    string stopped_info = answer;
		    if (stopped_info.contains('\n'))
			stopped_info = stopped_info.through('\n');
		    do {
			answer = answer.after('\n');
		    } while (answer != "" 
			     && atoi(answer) != 0 
			     && atoi(answer) == atoi(line));
		    answer = stopped_info + answer;
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
