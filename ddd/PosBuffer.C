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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

// Misc includes
#include "assert.h"
#include "cook.h"

// DDD includes
#include "PosBuffer.h"
#include "comm-manag.h"
#include "string-fun.h"
#include "ddd.h"
#include "GDBAgent.h"
#include "SourceView.h"

// A regex for C addresses ("0xdead") and Modula-2 addresses ("0BEEFH");
regex rxaddress(RXADDRESS);


// Filter all lines from ANSWER beginning with LINE.  This is required
// to suppress the line number output after a `stopping in' message.
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

// Fetch position from GDB output ANSWER.
void PosBuffer::filter (string& answer)
{
    if (gdb->type() == GDB)
    {
	// If gdb prints a "Current function" line, it overrides whatever
	// came before (e.g. "stopped in").

	int index;
	index = answer.index("Current function is ");
	if (index == 0 || index > 0 && answer[index - 1] == '\n')
	    already_read = Null;

	index = answer.index("Starting program: ");
	if (index == 0 || index > 0 && answer[index - 1] == '\n')
	    started = true;

	index = answer.index("has changed; re-reading symbols");
	if (index > 0)
	    recompiled = true;

	index = answer.index("Current language: ");
	if (index == 0 || index > 0 && answer[index - 1] == '\n')
	    gdb->program_language(answer);

	index = answer.index("The current source language is ");
	if (index == 0 || index > 0 && answer[index - 1] == '\n')
	    gdb->program_language(answer);
    }
    else if (gdb->type() == DBX)
    {
	int index;
	index = answer.index("Running: ");
	if (index == 0 || index > 0 && answer[index - 1] == '\n')
	    started = true;

	index = answer.index("has been recompiled");
	if (index > 0)
	    recompiled = true;
    }

    // Fetch and store position info, return remainder
    switch (already_read) {
    case PosComplete:
	// Nothing more to filter
	assert (pos_buffer != "");
	assert (answer_buffer == "");

	// Skip possible line number info
	switch (gdb->type())
	{
	case GDB:
	    break;

	case DBX:
	    {
		string line_s = pos_buffer;
		if (line_s.contains(':'))
		    line_s = line_s.after(':');
		int line = atoi(line_s);
		filter_line(answer, line);
	    }

	case XDB:
	    break;		// FIXME
	}
	break;

    case PosPart:
	answer.prepend (answer_buffer);
	answer_buffer = "";
	already_read = Null;
	// FALL THROUGH

    case Null:
	{
	    switch (gdb->type())
	    {
	    case GDB:
	    {
		// Try to find out current PC even for non-existent source

		if (pc_buffer == "")
		{
		    // `$pc = ADDRESS'
		    static regex rxpc("\\$pc  *=  *" RXADDRESS);
		    int pc_index = answer.index(rxpc);
		    if (pc_index >= 0)
		    {
			pc_buffer = answer.from(pc_index);
			pc_buffer = pc_buffer.from(rxaddress);
			pc_buffer = pc_buffer.through(rxaddress);

			// Strip this line from ANSWER
			int end_line = answer.index('\n', pc_index);
			int start_line = pc_index;
			while (start_line > 0 
			       && answer[start_line - 1] != '\n')
			    start_line--;

			if (end_line < 0)
			    answer.from(start_line) = "";
			else
			    answer.at(start_line, end_line - start_line + 1) 
				= "";
		    }
		}

		if (pc_buffer == "")
		{
		    // `Breakpoint N, ADDRESS in FUNCTION'
		    static regex rxstopped("Breakpoint  *[1-9][0-9]*,  *"
					   RXADDRESS);
		    int pc_index = answer.index(rxstopped);
		    if (pc_index >= 0)
		    {
			pc_buffer = answer.from(pc_index);
			pc_buffer = pc_buffer.from(rxaddress);
			pc_buffer = pc_buffer.through(rxaddress);
		    }
		}

		if (pc_buffer == "")
		{
		    // `#FRAME ADDRESS in FUNCTION'
		    static regex 
			rxframe("#[0-9][0-9]*  *" RXADDRESS);

		    int pc_index = answer.index(rxframe);
		    if (pc_index == 0
			|| pc_index > 0 && answer[pc_index - 1] == '\n')
		    {
			pc_buffer = answer.from(pc_index);
			pc_buffer = pc_buffer.from(rxaddress);
			pc_buffer = pc_buffer.through(rxaddress);
		    }
		}

		if (pc_buffer == "")
		{
		    // `No line number available for 
		    // address ADDRESS <FUNCTION>'
		    static regex rxaddr("address  *" RXADDRESS);

		    int pc_index = answer.index(rxaddr);
		    if (pc_index >= 0)
		    {
			pc_buffer = answer.from(pc_index);
			pc_buffer = pc_buffer.from(rxaddress);
			pc_buffer = pc_buffer.through(rxaddress);
		    }
		}

		if (pc_buffer == "")
		{
		    // `ADDRESS in FUNCTION'
		    int pc_index = answer.index(rxaddress);
		    if (pc_index == 0 
		        || pc_index > 0 && answer[pc_index - 1] == '\n')
		    {
			pc_buffer = answer.from(pc_index);
			pc_buffer = pc_buffer.from(rxaddress);
			pc_buffer = pc_buffer.through(rxaddress);
		    }
		}

		// Look for regular source info
		int index1 = answer.index ("\032\032");

		if (index1 == -1) 
		{
		    int index_p = answer.index ("\032");

		    if (index_p == int(answer.length()) - 1) {
			// Possible begin of position info at end of ANSWER
			already_read = PosPart;
			answer_buffer = "\032";
			answer = answer.before (index_p);

			return;
		    }

		    // Handle erroneous `info line' output like
		    // `Line number 10 is out of range for "t1.f".'
		    // At least get the file name.
		    static regex rxout_of_range(
	                "Line number [0-9]+ is out of range for ");
		    index_p = answer.index(rxout_of_range);
		    if (index_p >= 0)
		    {
			string file = answer.after('\"', index_p);
			file = file.before('\"');
			answer_buffer = file + ":1";
			already_read = PosComplete;
			return;
		    }

		    // Nothing found
		    return;
		}
		// ANSWER contains position info
		int index2 = answer.index ("\n", index1);

		if (index2 == -1) {
		    // Position info is incomplete
		    already_read = PosPart;
		    answer_buffer = answer.from (index1);
		    answer = answer.before (index1);

		    return;
		}
		assert (index1 < index2);

		// Position info is complete
		pos_buffer = answer.at (index1 + 2, index2 - (index1 + 2));
		int last_colon = pos_buffer.index(':', -1);
		pc_buffer = pos_buffer.after(last_colon);
		if (!pc_buffer.contains(rxaddress, 0))
		    pc_buffer = "0x" + pc_buffer;
		pc_buffer = pc_buffer.through(rxaddress);
		answer.at (index1, index2 - index1 + 1) = "";
		already_read = PosComplete;
	    }
	    break;

	    case DBX:
	    {
		string file;	// File name found
		string line;	// Line number found

		if (answer.contains('(', 0) || answer.contains('[', 0))
		{
		    // Get breakpoint position
		    string ans = answer;
		    int num = read_positive_nr(ans);
		    string pos = source_view->bp_pos(num);
		    if (pos != "")
		    {
			file = pos.before(':');
			line = pos.after(':');
		    }
		}

		static regex RXdbxfunc("[a-zA-Z_][^:]*: *[1-9][0-9]*  *.*");
		if (answer.matches(RXdbxfunc))
		{
		    // DEC DBX issues `up', `down' and `func' output
		    // in the format `FUNCTION: LINE  TEXT'

		    line = answer.after(":");
		    line = line.through(rxint);
		    already_read = PosComplete;

		    answer = answer.after("\n");
		}

		static regex RXdbxfunc2(".*line  *[1-9][0-9]*  *"
					"in  *\"[^\"]*\"\n.*");
		if (answer.matches(RXdbxfunc2))
		{
		    // AIX DBX issues `up', `down' and `func' output
		    // in the format `FUNCTION(ARGS), line LINE in "FILE"'

		    line = answer.after("line");
		    line = line.through(rxint);

		    file = answer.after("in ");
		    file = file.after('\"');
		    file = file.before('\"');

		    already_read = PosComplete;

		    // answer = answer.after("\n");
		}

		static regex RXdbxpos("[[][^]]*:[1-9][0-9]*[^]]*].*");
		if (answer.contains(RXdbxpos))
		{
		    // DEC dbx issues breakpoint lines in the format
		    // "[new_tree:113 ,0x400858] \ttree->right = NULL;"

		    line = answer.from(RXdbxpos);
		    line = line.after(":");
		    line = line.through(rxint);
		    already_read = PosComplete;

		    if (!answer.contains('[', 0))
			answer = answer.after("\n");
		}
		else if (answer.contains("stopped in ")
			 || answer.contains("stopped at "))
		{
		    // Stop reached
		    if (answer.contains("in file "))
		    {
			// File name given
			file = answer.after("in file ");
			if (file.contains('\n'))
			    file = file.before('\n');
			file = unquote(file);
		    }
		    else if (answer.contains("["))
		    {
			// DEC dbx and SGI dbx output format
			line = answer.after("[");
			func_buffer = line.before(":");
			line = line.after(":");
			line = line.through(rxint);
			// answer = answer.after("\n");
		    }
		    else
		    {
			// Function name given
			string func = answer.after("stopped in ");
			if (func.contains(" at "))
			    func = func.before(" at ");
			func_buffer = func;
		    }

		    if (line == "")
		    {
			line = answer.after("at line ");
			line = line.through(rxint);
			if (!answer.contains("at line "))
			    line = "0";
		    }

		    already_read = PosComplete;
		    filter_line(answer, atoi(line));
		}
		else if (answer.contains("Current function is "))
		{
		    // Up/Down command entered
		    string nr = answer.after("\n");
		    if (nr != "")
		    {
			line = itostring(atoi(nr));
			already_read = PosComplete;

			// Show current function only
			answer = answer.from("Current function is ");
			answer = answer.through("\n");
			func_buffer = answer.after("function is ");
			func_buffer = func_buffer.before("\n");
		    }
		    else
		    {
			answer_buffer = answer;
			answer = "";
			already_read = PosPart;
		    }
		}

		if (already_read == PosComplete && line != "")
		{
		    if (file != "")
			pos_buffer = file + ":" + line;
		    else
			pos_buffer = line;
		}
	    }
	    break;

	    case XDB:
		{
		    // INDEX points at the start of a line
		    int index = 0;
		    while (index >= 0 && answer != "")
		    {
			string line = answer.from(index);
			if (line.contains('\n'))
			    line = line.before('\n');
			strip_final_blanks(line);

			static regex 
			    RXxdbpos("[^: \t]*:[^:]*: [1-9][0-9]*[: ].*");
			if (line.matches(RXxdbpos))
			{
			    string file = line.before(':');
			    line = line.after(':');
			    string func = line.before(':');
			    line = line.after(':');
			    string line_no = line.before(':');
			
			    read_leading_blanks(func);
			    read_leading_blanks(line_no);
			    line_no = line_no.through(rxint);

			    pos_buffer   = file + ":" + line_no;
			    func_buffer  = func;
			    already_read = PosComplete;

			    // Delete this line from output
			    answer.through('\n', index) = "";
			    break;
			}
			else
			{
			    // Look at next line
			    index = answer.index('\n', index);
			    if (index >= 0)
				index++;
			}
		    }

		    if (already_read != PosComplete && answer.contains(':'))
		    {
			answer_buffer = answer;
			answer = "";
			already_read = PosPart;
		    }
		}
		break;
	    }
	}
	break;

    default:
	// This can't happen.
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

    case Null:
	assert (pos_buffer == "");
	return "";

    case PosComplete:
	assert (pos_buffer != "");
	return "";

    default:
	// This can't happen.
	assert(0);
	break;
    }
    return "";
}
