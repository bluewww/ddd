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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char PosBuffer_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

// Misc includes
#include "assert.h"
#include "cook.h"

// DDD includes
#include "AppData.h"
#include "PosBuffer.h"
#include "comm-manag.h"
#include "string-fun.h"
#include "ddd.h"
#include "GDBAgent.h"
#include "SourceView.h"
#include "regexps.h"
#include "index.h"

#if RUNTIME_REGEX
// A regex for C addresses ("0xdead") and Modula-2 addresses ("0BEEFH");
regex rxaddress(RXADDRESS);
regex rxaddress_start(RXADDRESS_START);
#endif


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

// Return true iff ANSWER has a line beginning with PREFIX
static bool has_prefix(const string& answer, const string& prefix)
{
    int index = answer.index(prefix);
    return index == 0 || index > 0 && answer[index - 1] == '\n';
}

// Store first address in ANSWER after INDEX in BUFFER
static void fetch_address(const string& answer, int index, string& buffer)
{
    while (index < int(answer.length()) && !is_address_start(answer[index]))
	index++;

    assert (is_address_start(answer[index]));

    int start = index;

    // Just fetch the first word -- no need to do big address matches here
    while (index < int(answer.length()) && !isspace(answer[index]))
	index++;

    buffer = ((string&)answer).at(start, index - start);
}

// Fetch position from GDB output ANSWER.
void PosBuffer::filter (string& answer)
{
    if (answer.length() == 0)
	return;

    switch (gdb->type())
    {
    case GDB:
    {
	// If GDB prints a "Current function" line, it overrides whatever
	// came before (e.g. "stopped in").
	if (has_prefix(answer, "Current function is "))
	    already_read = Null;

	// Check program state
	if (has_prefix(answer, "Starting program: "))
	    started = true;

	if (has_prefix(answer, "The program no longer exists"))
	    terminated = true;

	if (has_prefix(answer, "Program received signal"))
	    signaled = true;

	if (has_prefix(answer, "Program terminated with signal"))
	    signaled = terminated = true;

	if (answer.contains("has changed; re-reading symbols"))
	    recompiled = true;

	if (has_prefix(answer, "Current language: "))
	    gdb->program_language(answer);

	if (has_prefix(answer, "The current source language is "))
	    gdb->program_language(answer);
    }
    break;

    case DBX:
    {
	if (has_prefix(answer, "Running: "))
	    started = true;

	if (answer.contains("has been recompiled"))
	    recompiled = true;

	if (has_prefix(answer, "signal "))
	    signaled = true;
    }
    break;
    
    case XDB:
	break;			// FIXME
    }

    // Check for terminated program
    int i = -1;
    while ((i = answer.index("rogram", i + 1)) > 0)
    {
	int j = i;
	while (j > 0 && answer[j - 1] != '\n')
	    j--;
	
#if RUNTIME_REGEX
	static regex rxterminated("([Tt]he )?[Pp]rogram "
				  "(exited|terminated"
				  "|is not being run|no longer exists).*");
#endif
	if (answer.matches(rxterminated, j))
	    terminated = true;
    }

    if (answer.contains("no active process") ||
	answer.contains("execution completed"))
	terminated = true;


    // Check for auto command
    if (app_data.auto_commands)
    {
	while (has_prefix(answer, app_data.auto_command_prefix))
	{
	    int index = answer.index(app_data.auto_command_prefix);
	    string cmd = answer.from(index);
	    if (cmd.contains('\n'))
		cmd = cmd.through('\n');
	    answer = 
		answer.before(index) + answer.from(int(index + cmd.length()));
	    cmd = cmd.after(app_data.auto_command_prefix);
	    auto_cmd_buffer += cmd;
	}
    }

    // Fetch and store position info, return remainder
    switch (already_read)
    {
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
#if RUNTIME_REGEX
		    static regex rxpc("\\$pc  *=  *" RXADDRESS);
#endif
		    int pc_index = index(answer, rxpc, "$pc ");
		    if (pc_index >= 0)
		    {
			int addr_index = answer.index('=');
			fetch_address(answer, addr_index, pc_buffer);

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
#if RUNTIME_REGEX
		    static regex rxstopped("Breakpoint  *[1-9][0-9]*,  *"
					   RXADDRESS);
#endif
		    int pc_index = index(answer, rxstopped, "Breakpoint");
		    if (pc_index >= 0)
		    {
			pc_index = answer.index(',');
			fetch_address(answer, pc_index, pc_buffer);
		    }
		}

		if (pc_buffer == "")
		{
		    // `#FRAME ADDRESS in FUNCTION'
#if RUNTIME_REGEX
		    static regex rxframe("#[0-9][0-9]*  *" RXADDRESS);
#endif

		    int pc_index = index(answer, rxframe, "#");
		    if (pc_index == 0
			|| pc_index > 0 && answer[pc_index - 1] == '\n')
		    {
			pc_index = answer.index(' ');
			fetch_address(answer, pc_index, pc_buffer);
		    }
		}

		if (pc_buffer == "")
		{
		    // `No line number available for 
		    // address ADDRESS <FUNCTION>'
#if RUNTIME_REGEX
		    static regex rxaddr("address  *" RXADDRESS);
#endif

		    int pc_index = index(answer, rxaddr, "address ");
		    if (pc_index >= 0)
		    {
			pc_index = answer.index(' ');
			fetch_address(answer, pc_index, pc_buffer);
		    }
		}

		if (pc_buffer == "" && answer != "")
		{
		    // `ADDRESS in FUNCTION'
#if RUNTIME_REGEX
		    static regex rxaddress_in(RXADDRESS " in ");
#endif
		    int pc_index = -1;
		    if (is_address_start(answer[0]) 
			&& answer.contains(rxaddress_in, 0))
		    {
			pc_index = 0;
		    }
		    else
		    {
#if RUNTIME_REGEX
			static regex rxnladdress_in("\n" RXADDRESS " in ");
#endif
			pc_index = index(answer, rxnladdress_in, "\n");
		    }

		    if (pc_index >= 0)
		    {
			fetch_address(answer, pc_index, pc_buffer);
		    }
		}

		// Look for regular source info
		int index1 = answer.index ("\032\032");

		if (index1 < 0) 
		{
		    int index_p = answer.index ("\032");
		    if (index_p >= 0 && index_p == int(answer.length()) - 1)
		    {
			// Possible begin of position info at end of ANSWER
			already_read = PosPart;
			answer_buffer = "\032";
			answer = answer.before (index_p);

			return;
		    }

		    // Handle erroneous `info line' output like
		    // `Line number 10 is out of range for "t1.f".'
		    // At least get the file name.
#if RUNTIME_REGEX
		    static regex rxout_of_range(
	                "Line number [0-9]+ is out of range for ");
#endif
		    index_p = index(answer, rxout_of_range, "Line number");
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

		if (index2 == -1)
		{
		    // Position info is incomplete
		    already_read = PosPart;
		    answer_buffer = answer.from (index1);
		    answer = answer.before (index1);
		    return;
		}

		assert (index1 < index2);

		// Position info is complete
		pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));
		int last_colon = pos_buffer.index(':', -1);
		pc_buffer = pos_buffer.after(last_colon);
		if (!pc_buffer.contains(rxaddress_start, 0))
		    pc_buffer = "0x" + pc_buffer;
		pc_buffer = pc_buffer.through(rxaddress);
		answer.at(index1, index2 - index1 + 1) = "";
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

#if RUNTIME_REGEX
		static regex rxdbxfunc("[a-zA-Z_][^:]*: *[1-9][0-9]*  *.*");
#endif
		if (answer.matches(rxdbxfunc))
		{
		    // DEC DBX issues `up', `down' and `func' output
		    // in the format `FUNCTION: LINE  TEXT'

		    line = answer.after(":");
		    line = line.through(rxint);
		    already_read = PosComplete;

		    answer = answer.after("\n");
		}

#if RUNTIME_REGEX
		static regex rxdbxfunc2(
		    ".*line  *[1-9][0-9]*  *in  *\"[^\"]*\"\n.*");
#endif
		if (answer.matches(rxdbxfunc2))
		{
		    // AIX DBX issues `up', `down' and `func' output
		    // in the format `FUNCTION(ARGS), line LINE in "FILE"'

		    line = answer.after("line ");
		    line = line.through(rxint);

		    file = answer.after("in ");
		    file = file.after('\"');
		    file = file.before('\"');

		    already_read = PosComplete;

		    // answer = answer.after("\n");
		}

#if RUNTIME_REGEX
		static regex rxdbxpos("[[][^]]*:[1-9][0-9]*[^]]*].*");
#endif
		if (answer.contains(rxdbxpos))
		{
		    // DEC dbx issues breakpoint lines in the format
		    // "[new_tree:113 ,0x400858] \ttree->right = NULL;"

		    line = answer.from(rxdbxpos);
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

#if RUNTIME_REGEX
			static regex rxxdbpos(
			    "[^: \t]*:[^:]*: [1-9][0-9]*[: ].*");
#endif
			if (line.matches(rxxdbpos))
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
