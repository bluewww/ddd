// $Id$
// Filter position information from GDB output.

// Copyright (C) 1995-1999 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003, 2005, 2008 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
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

char PosBuffer_rcsid[] =
    "$Id$";

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
#include "cmdtty.h"

#if RUNTIME_REGEX
// A regex for C addresses ("0xdead") and Modula-2 addresses ("0BEEFH");
regex rxaddress(RXADDRESS);
regex rxaddress_start(RXADDRESS_START);
#endif


// Filter all lines from ANSWER beginning with LINE.  This is required
// to suppress the line number output after a `stopping in' message.
void PosBuffer::filter_line(string& answer) const
{
    if (already_read != PosComplete)
	return;

    string line_s = pos_buffer;
    if (line_s.contains(':'))
	line_s = line_s.after(':');
    int line = atoi(line_s.chars());
    if (line <= 0)
	return;

    int pos = 0;
    do {
	if (atoi(answer.chars() + pos) == line)
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
    if (!buffer.empty())
	return;			// Already have an address

    while (index < int(answer.length()) && !is_address_start(answer[index]))
	index++;

    assert (is_address_start(answer[index]));

    int start = index;

    // Just fetch the first word -- no need to do big address matches here
    while (index < int(answer.length()) && !isspace(answer[index]))
	index++;

    buffer = answer.at(start, index - start);
}

// Store first function name in ANSWER after INDEX in BUFFER
static void fetch_function(const string& answer, int index, string& buffer,
			   bool in_required = false)
{
    if (!buffer.empty())
	return;			// Already have a function

    string line = answer.from(index);
    line = line.before('\n');
    if (in_required)
	line = line.after(" in ");

    // The function name is the word before the opening parenthesis
    line = line.before('(');
    strip_trailing_space(line);
    int ws_index = line.index(' ', -1) + 1;
    line = line.from(ws_index);
    strip_leading_space(line);
    if (!line.empty() && line.contains(rxidentifier, 0))
	buffer = line;
}

// Same, but requires " in " before function
inline void fetch_in_function(const string& answer, int index, string& buffer)
{
    fetch_function(answer, index, buffer, true);
}



// Fetch position from GDB output ANSWER.
void PosBuffer::filter (string& answer)
{
    if (answer.length() == 0)
	return;

    // Check program state
    switch (gdb->type())
    {
    case BASH:
    {
 	if (has_prefix(answer, "Reading "))
 	    started = true;
	break;
    }

    case GDB:
    {
	// If GDB prints a "Current function" line, it overrides whatever
	// came before (e.g. "stopped in").
	if (has_prefix(answer, "Current function is "))
	    already_read= Null;

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

	if (terminated)
	    annotate("exited");
	if (signaled)
	    annotate("signalled");
    }
    break;

    case DBG:
    {
 	// Check program state
 	if (has_prefix(answer, "Starting program: "))
 	    started = true;
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

    case MAKE:
    {
 	if (has_prefix(answer, "Reading makefiles..."))
 	    started = true;
	break;
    }

    case PERL:
    {
	if (has_prefix(answer, "Loading DB routines from perl5db.pl"))
	  recompiled = true;
    }
    break;

    case XDB:
    case JDB:
    case PYDB:
	break;			// Nothing special
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

    if (answer.contains("no active process") 
	|| answer.contains("execution completed")
	|| answer.contains("application exited"))
	terminated = true;

    // Check for auto command
    if (app_data.auto_commands)
    {
	answer.prepend(auto_cmd_part);
	auto_cmd_part = "";

	string pfx = app_data.auto_command_prefix;

	if (!auto_cmd_buffer.empty() && !auto_cmd_buffer.contains('\n', -1))
	{
	    // Complete pending auto command
	    if (answer.contains('\n'))
	    {
		auto_cmd_buffer += answer.through('\n');
		answer = answer.after('\n');
	    }
	    else
	    {
		auto_cmd_buffer += answer;
		answer = "";
	    }
	}

	int index;
	while ((index = answer.index(pfx)) >= 0)
	{
	    string cmd = answer.from(index);
	    if (cmd.contains('\n'))
		cmd = cmd.through('\n');
	    answer = 
		answer.before(index) + answer.from(int(index + cmd.length()));
	    cmd = cmd.after(pfx);
	    auto_cmd_buffer += cmd;
	}

	if (pfx.contains(answer, 0))
	{
	    auto_cmd_part = answer;
	    answer = "";
	}
    }

    // Fetch and store position info, return remainder
    switch (already_read)
    {
    case PosComplete:
	if (gdb->type() == GDB)
	    filter_gdb(answer);		// Slurp in the source annotation
	// Nothing more to filter - skip possible line number info
	filter_line(answer);
	break;

    case PosPart:
	answer.prepend (answer_buffer);
	answer_buffer = "";
	already_read = Null;
	// FALL THROUGH

    case Null:
    {
	// Now go for the actual position.
	switch (gdb->type())
	{
	case BASH:
	    filter_bash(answer);
	    break;
	
	case DBG:
	    filter_dbg(answer);
	    break;

	case DBX:
	    filter_dbx(answer);
	    break;

	case GDB:
	    filter_gdb(answer);
	    break;

	case JDB:
	    filter_jdb(answer);
	    break;

	case MAKE:
	    filter_make(answer);
	    break;

	case PERL:
	    filter_perl(answer);
	    break;

	case PYDB:
	    filter_pydb(answer);
	    break;

	case XDB:
	    filter_xdb(answer);
	    break;
	}

	filter_line(answer);
    }

    break;
    }
}

void PosBuffer::filter_bash(string& answer)
{
    // Check for regular source info
    int index1 = answer.index ("\032\032");
	    
    if (index1 < 0) 
    {
	int index_p = answer.index ("\032");
	if (index_p >= 0 && index_p == int(answer.length()) - 1)
	{
	    // Possible begin of position info at end of ANSWER
	    answer_buffer = "\032";
	    answer = answer.before (index_p);
	    already_read = PosPart;
	    return;
	}
    }
    else
    {
	// ANSWER contains position info
	int index2 = answer.index("\n", index1);
	    
	if (index2 == -1)
	{
	    // Position info is incomplete
	    answer_buffer = answer.from (index1);
	    answer = answer.before (index1);
	    already_read = PosPart;
	    return;
	}
	else
	{
	    assert (index1 < index2);
	    
	    // Position info is complete
	    already_read = PosComplete;
	    pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));
	    answer.at(index1, index2 - index1 + 1) = "";
	}
    }

    if (already_read != PosComplete)
    {
	// Try '(FILE:LINE):\n';

	// INDEX points at the start of a line
	int index = 0;
	while (index >= 0 && !answer.empty())
	{
	    string line = answer.from(index);
	    if (line.contains('\n'))
		line = line.before('\n');
	    strip_trailing_space(line);
		    
#if RUNTIME_REGEX
	    static regex rxbashpos("[(][^:]*:[1-9][0-9]*[)]:");
#endif
	    if (line.matches(rxbashpos))
	    {
		// Fetch position
		pos_buffer = line.after('(');
		pos_buffer = pos_buffer.before(')');
		already_read = PosComplete;

		// Delete this line from output
		int next_index = answer.index('\n', index);
		if (next_index < 0)
		    next_index = answer.length();
		else
		    next_index++;
		answer.at(index, next_index - index) = "";
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
    }
}

void PosBuffer::filter_gdb(string& answer)
{
    // Try to find out current PC even for non-existent source

    if (check_pc && pc_buffer.empty())
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
	    
    if (check_pc && pc_buffer.empty() || 
	check_func && func_buffer.empty())
    {
	// `Breakpoint N, ADDRESS in FUNCTION (ARGS...)'
#if RUNTIME_REGEX
	static regex rxstopped_addr("Breakpoint  *[1-9][0-9]*,  *"
				    RXADDRESS);
#endif
	int pc_index = index(answer, rxstopped_addr, "Breakpoint");
	if (pc_index >= 0)
	{
	    annotate("stopped");
	    pc_index = answer.index(',');
	    fetch_address(answer, pc_index, pc_buffer);
	    fetch_in_function(answer, pc_index, func_buffer);
	}
    }
	    
    if (check_pc && pc_buffer.empty() || 
	check_func && func_buffer.empty())
    {
	// `#FRAME ADDRESS in FUNCTION (ARGS...)'
#if RUNTIME_REGEX
	static regex rxframe_addr("#[0-9][0-9]*  *" RXADDRESS);
#endif
		
	int pc_index = index(answer, rxframe_addr, "#");
	if (pc_index == 0
	    || pc_index > 0 && answer[pc_index - 1] == '\n')
	{
	    pc_index = answer.index(' ');
	    fetch_address(answer, pc_index, pc_buffer);
	    fetch_in_function(answer, pc_index, func_buffer);
	}
    }
	    
    if (check_pc && pc_buffer.empty() || 
	check_func && func_buffer.empty())
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
	    if (func_buffer.empty())
	    {
		string line = answer.from(pc_index);
		line = line.after('<');
		line = line.before('>');
		if (!line.empty())
		    func_buffer = line;
	    }
	}
    }

    if (check_pc && pc_buffer.empty() && !answer.empty())
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
	    fetch_in_function(answer, pc_index, func_buffer);
	}
    }

    // Try to find out current function name, even for
    // non-existing addresses
    if (check_func && func_buffer.empty())
    {
	// `Breakpoint N, FUNCTION (ARGS...)'
	// This regex used for PYDB as well.
#if RUNTIME_REGEX
	static regex rxstopped_func("Breakpoint  *[1-9][0-9]*,  *");
#endif
	int bp_index = index(answer, rxstopped_func, "Breakpoint");
	if (bp_index >= 0)
	    fetch_function(answer, bp_index, func_buffer);
    }

    if (check_func && func_buffer.empty())
    {
	// `#FRAME FUNCTION'
#if RUNTIME_REGEX
	static regex rxframe_func("#[0-9][0-9]*  *[a-zA-Z_].*[(]");
#endif
	int frame_index = index(answer, rxframe_addr, "#");
	if (frame_index == 0
	    || frame_index > 0 && answer[frame_index - 1] == '\n')
	{
	    fetch_function(answer, frame_index, func_buffer);
	}
    }

    if (check_func && func_buffer.empty())
    {
	// FUNCTION (ARGS...) at FILE:POS
	int at_index = answer.index(" at ");
	if (at_index > 0)
	{
	    int nl_index = 
		answer.index('\n', at_index - answer.length() - 1) + 1;
	    fetch_function(answer, nl_index, func_buffer);

	    // Try to construct position from `at FILE:POS' (vxworks)
	    string file = answer.after(" at ");
	    file = file.before('\n');

	    if (file.contains(rxaddress, 0))
	    {
		// This is `at ADDRESS' (GDB output)
	    }
	    else if (file.contains(":") && !file.contains(": "))
	    {
		pos_buffer = file;
		already_read = PosComplete;
		return;
	    }
		
	}
    }
	    
    // Look for regular source info
    // (GDB's annotations are prefixed with "^Z^Z")
    int index1 = answer.index ("\032\032");
	    
    if (index1 < 0) 
    {
	int index_p = answer.index ("\032");
	if (index_p >= 0 && index_p == int(answer.length()) - 1)
	{
	    // Possible begin of position info at end of ANSWER
	    answer_buffer = "\032";
	    answer = answer.before (index_p);
	    already_read = PosPart;
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
	    pos_buffer = file + ":1";
	    already_read = PosComplete;
	    return;
	}

	// Try to construct position from `Line xxxx of "filename"' (vxworks)
	// which is the output of an 'info line' command
	string line = answer.after("Line ");
	string file = answer.after('\"');
	if (!line.empty() && !file.empty())
	{
	    line = line.before(" of");
	    file = file.before('\"') + ":" + line;
	    if (!line.empty() && !file.empty())
	    {
		pos_buffer = file;
		already_read = PosComplete;
		return;
	    }
	}
	// Try FUNCTION (ARGS...) at FILE:POS
	// here to properly handle up/down commands 
	int at_index = answer.index(" at ");
	int br_index = answer.index("Break");
	if ( (at_index > 0) && (br_index < 0) )
	{
	    int nl_index = 
		answer.index('\n', at_index - answer.length() - 1) + 1;
	    fetch_function(answer, nl_index, func_buffer);

	    // Try to construct position from `at FILE:POS' (vxworks)
	    string file = answer.after(" at ");
	    file = file.before('\n');
	    if (!file.empty())
	    {
		pos_buffer = file;
		already_read = PosComplete;
		return;
	    }
		
	}
	
	// Nothing found
	return;
    }
	    
    // ANSWER contains position info
    int index2 = answer.index ("\n", index1);

    if (index2 == -1)
    {
	// Position info is incomplete
	answer_buffer = answer.from (index1);
	answer = answer.before (index1);
	already_read = PosPart;
	return;
    }

    assert (index1 < index2);

    // Position info is complete
    pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));

    if (pos_buffer.contains("source ", 0))
    {
	// This happens with GDB in annotation level 2
	pos_buffer = pos_buffer.after("source ");
    }

    int last_colon = pos_buffer.index(':', -1);
    pc_buffer = pos_buffer.after(last_colon);
    if (!pc_buffer.contains(rxaddress_start, 0))
	pc_buffer = "0x" + pc_buffer;
    pc_buffer = pc_buffer.through(rxaddress);

    answer.at(index1, index2 - index1 + 1) = "";
    if (!pos_buffer.empty())
	already_read = PosComplete;
}

void PosBuffer::filter_dbg(string& answer)
{
    int idx1, idx2;
    
    if (already_read != PosComplete && !answer.contains('\n'))
    {
	// Position info is incomplete
	answer_buffer = answer;
	answer = "";
	already_read = PosPart;
	return;
    }

    idx1 = 0;
    while (idx1 < (int)answer.length())
    {
        idx2 = answer.index('\n', idx1);
	if (idx2 < 0) idx2 = answer.length();	
	string line = answer.at(idx1, idx2 - idx1);
	if (line.contains('\n'))
	    line = line.before('\n');
	strip_trailing_space(line);
		
	// DBG uses a format like `test.php:4 <main>\n echo $a."hello world."'
#if RUNTIME_REGEX
	static regex rxdbgpos("[^ \t]*:[ \t]*[1-9][0-9]*[ \t]*<.*>");
	static regex rxdbgframepos("#[0-9]*[ \t]*<.*>[ \t]*at[ \t]*[^ \t]*:[ \t]*[1-9][0-9]*");
#endif
	if (line.matches(rxdbgpos)) 
	{
	    string file = line.before(':');
	    line = line.after(':');
		    
	    string line_no = line;
	    strip_leading_space(line_no);
	    line_no = line_no.before(' ');
	    
	    line = line.after('<');
	    func_buffer  = line.before('>');
	    strip_leading_space(func_buffer);
		    
	    pos_buffer   = file + ":" + line_no;
	    
	    // Delete this line from output
	    answer.at(idx1, idx2 - idx1 + 1) = "";
	    already_read = PosComplete;
	    break;

	} else if (line.matches(rxdbgframepos)) 
	{
	    string addr = line.before(">");
	    func_buffer = addr.after('<');
	    strip_leading_space(func_buffer);
	    
	    string file = line.after(">");
	    file = file.after("at");
	    strip_leading_space(file);
	    
	    string line_no = file.after(':');
	    strip_leading_space(line_no);
	
	    file = file.before(':');
	    	    
	    pos_buffer   = file + ":" + line_no;
	    
	    // Delete this line from output
	    answer.at(idx1, idx2 - idx1 + 1) = "";
	    already_read = PosComplete;
	    break;
	}
	
	idx1 = idx2+1;
    }
}
	
void PosBuffer::filter_dbx(string& answer)
{
    string file;		// File name found
    string line;		// Line number found

    // When reaching a breakpoint, DBX issues the breakpoint
    // number before the status line.  Check for this and
    // initialize defaults from breakpoint position.
    strip_leading_space(answer);
    if (answer.contains('(', 0) || answer.contains('[', 0))
    {
	// Get breakpoint position
	string ans = answer;
	int num = read_positive_nr(ans);
	string pos = source_view->bp_pos(num);
	if (!pos.empty())
	{
	    file = pos.before(':');
	    line = pos.after(':');
	}
    }

    // DEC DBX way issue warnings like
    // `warning: "./cxxtest.C":157 has no code associated with it'
    // right within the position info.

    int start_of_warning = answer.index("\nwarning");
    if (start_of_warning >= 0)
    {
	int open_bracket  = answer.index('[');
	int close_bracket = answer.index(']');
	if (open_bracket >= 0 && open_bracket < start_of_warning &&
	    close_bracket >= 0 && close_bracket > start_of_warning)
	{
	    // Remove warning

	    int end_of_warning = answer.index('\n', start_of_warning + 1);
	    while (end_of_warning < int(answer.length()) && 
		   answer[end_of_warning] == '\n')
		end_of_warning++;

	    while (start_of_warning > 0 && 
		   answer[start_of_warning - 1] == '\n')
		start_of_warning--;

	    int warning_length = end_of_warning - start_of_warning;
	    answer.at(start_of_warning, warning_length) = "";
	}
    }

#if RUNTIME_REGEX
    static regex rxdbxfunc2(
	".*line  *[1-9][0-9]*  *in  *(file  *)?\"[^\"]*\"\n.*");
#endif
    if (already_read != PosComplete && answer.matches(rxdbxfunc2))
    {
	// AIX DBX issues `up', `down' and `func' output
	// in the format `FUNCTION(ARGS), line LINE in "FILE"'.
	// SUN DBX uses `line LINE in file "FILE"' instead.
	// We check for the `line LINE' part.

	line = answer.after(" line ");
	line = line.through(rxint);

	file = answer.after('\"');
	file = file.before('\"');

	if (!line.empty())
	{
	    already_read = PosComplete;
	    // answer = answer.after("\n");
	}
    }

#if RUNTIME_REGEX
    static regex rxdbxpos("[[][^]]*:[1-9][0-9]*[^]]*[]].*");
#endif
    int dbxpos_index = -1;
    if (already_read != PosComplete && 
	(dbxpos_index = index(answer, rxdbxpos, "[")) >= 0)
    {
	// DEC DBX issues breakpoint lines in the format
	// "[new_tree:113 ,0x400858] \ttree->right = NULL;"
		
	line = answer.from(dbxpos_index);
	if (line.contains("[#", 0))
	{
	    // This is a Ladebug breakpoint, no position info.
	}
	else
	{
	    // Note that the function name may contain "::" sequences.
	    while (line.contains("::"))
		line = line.after("::");
	    line = line.after(":");
	    line = line.through(rxint);
	    if (!line.empty())
	    {
		if (answer.index('\n', dbxpos_index) >= 0)
		{
		    already_read = PosComplete;

		    // Strip position info and line
		    strip_leading_space(answer);
		    if (answer.contains('[', 0))
			answer = answer.after("\n");
		}
		else
		{
		    // Wait for `\n' such that we can delete the line
		    answer_buffer = answer;
		    answer = "";
		    already_read = PosPart;
		    return;
		}
	    }
	}
    }

    if (already_read != PosComplete && 
	(answer.contains("stopped in ") || 
	 answer.contains("stopped at ")))
    {
	int stopped_index = answer.index("stopped");
	assert(stopped_index >= 0);

	// Stop reached
	// AIX DBX issues lines like
	// `[4] stopped in unnamed block $b382 at line 4259 in file
	//      "/msdev/sms/ms7/riosqa/src/tffi/fi2tofu.c" ($t1)'
	int in_file_index = answer.index("in file ", stopped_index);
	int bracket_index = answer.index("[", stopped_index);

	if (in_file_index >= 0)
	{
	    // File name given
	    file = answer.from(in_file_index);
	    file = file.after("in file ");
	    if (file.contains('\n'))
		file = file.before('\n');
	    if (file.contains('"', 0))
	    {
		file = file.after('"');
		file = file.before('"');
	    }
	}
	else if (bracket_index >= 0)
	{
	    // DEC DBX and SGI DBX output format:
	    // `[3] Process  1852 (cxxtest) 
	    // stopped at [::main:266 ,0x1000a028]'
	    line = answer.after(bracket_index);
	    func_buffer = line;
	    while (line.contains("::"))
		line = line.after("::");
	    line = line.from(":");
	    func_buffer = func_buffer.before(line);
	    line = line.after(":");
	    line = line.through(rxint);
	    // answer = answer.after("\n");
	}
	else
	{
	    // Function name given
	    string func = answer.after(stopped_index);
	    func = func.after("stopped");
	    if (func.contains(" at "))
		func = func.before(" at ");
	    func_buffer = func;
	}

	if (!func_buffer.empty())
	{
	    // With DEC's `ladebug', the function name is fully qualified,
	    // as in `stopped at [void tree_test(void):277 0x120003f44]'
	    // We use only the base name (`tree_test' in this case).

	    // (We could avoid this if `ladebug' offered a way to look
	    // up fully qualified names.  Does it? - AZ)
	    if (func_buffer.contains('('))
		func_buffer = func_buffer.before('(');
	    while (func_buffer.contains(' '))
		func_buffer = func_buffer.after(' ');
	}

	if (line.empty())
	{
	    line = answer.after("at line ", stopped_index);
	    line = line.through(rxint);
	    if ((!file.empty() || !func_buffer.empty()) &&
		!answer.contains("at line "))
		line = "0";
	}

	if (!line.empty())
	    already_read = PosComplete;
    }

#if RUNTIME_REGEX
    static regex rxdbxfunc("[a-zA-Z_][^[]*: *[1-9][0-9]*  *.*");
#endif
    if (already_read != PosComplete && answer.matches(rxdbxfunc))
    {
	// DEC DBX issues `up', `down' and `func' output
	// in the format `FUNCTION: LINE  TEXT'
		
	// Note that the function name may contain "::" sequences.
	line = answer;
	while (line.contains("::"))
	    line = line.after("::");

	line = line.after(":");
	strip_leading_space(line);
	if (line.contains(rxint, 0))
	{
	    line = line.through(rxint);
	    if (!line.empty())
	    {
		if (answer.contains('\n'))
		{
		    // Got it!
		    already_read = PosComplete;
		    answer = answer.after("\n");
		}
		else
		{
		    // Wait for `\n' such that we can delete the line
		    answer_buffer = answer;
		    answer = "";
		    already_read = PosPart;
		    return;
		}
	    }
	}
    }

    if (already_read != PosComplete && 
	answer.contains("Current function is "))
    {
	// Up/Down command entered
	string nr = answer.after("\n");
	if (!nr.empty())
	{
	    line = itostring(atoi(nr.chars()));
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
	    return;
	}
    }

    if (already_read != PosComplete && 
	(!answer.contains('\n') ||
	 (answer.contains('[') && !answer.contains(']'))))
    {
	// Position info is incomplete
	answer_buffer = answer;
	answer = "";
	already_read = PosPart;
	return;
    }

    if (already_read == PosComplete && !line.empty())
    {
	if (!file.empty())
	    pos_buffer = file + ":" + line;
	else
	    pos_buffer = line;
    }

    if (already_read == PosComplete && pos_buffer.empty())
	already_read = Null;
}

void PosBuffer::filter_jdb(string& answer)
{
    if (already_read != PosComplete && !answer.contains('\n'))
    {
	// Position info is incomplete
	answer_buffer = answer;
	answer = "";
	already_read = PosPart;
	return;
    }

    int index = 0;
    while (index >= 0 && !answer.empty())
    {
	string line = answer.from(index);
	if (line.contains('\n'))
	    line = line.before('\n');
	strip_trailing_space(line);

	// Having reached a breakpoint, JDB 1.1 uses a format like
	// `(HelloWorld:3)'.
	// Having loaded a class, JDB uses `class(foo.bar.HelloWorld)'.

	// This may be prefixed by the fully qualified class
	// name (`path'), as in
	// `GlobalView.Map.MapController.createMap (MapController:53)'.
	// In such a case, prefer the fully qualified name.
#if RUNTIME_REGEX
	static regex 
	    rxjdbpos("((class|interface)[(][A-Za-z][A-Za-z0-9.]*[)]|"
		     "[(][A-Za-z][A-Za-z0-9.]*:[1-9][0-9]*[)])");
#endif
	if (gdb->has_debug_command() && line.matches(rxjdbpos))
	{
	    string file = line.after('(');
	    file = file.before(')');
	    string line_no = "0";
	    if (file.contains(':'))
	    {
		line_no = file.after(':');
		file = file.before(':');
	    }
 
	    // Check whether a fully qualified class name is prepended
	    int class_index = line.index('(') - 1;
	    while (class_index >= 0 && line[class_index] == ' ')
		class_index--;
	    while (class_index >= 1 && line[class_index - 1] != ' ')
		class_index--;
	    if (class_index >= 0)
	    {
		string class_name = line.from(class_index);
		class_name = class_name.before('(');
		strip_trailing_space(class_name);
		if (class_name.contains('.') && 
		    class_name.matches(rxchain))
		{
		    // Strip method
		    class_name = class_name.before('.', -1);

		    if (class_name.contains("." + file))
		    {
			// CLASS_NAME is more qualified - use it
			file = class_name;
		    }
		}
	    }

	    pos_buffer	 = file + ":" + line_no;
	    already_read = PosComplete;

#if 0
	    // Delete this line from output
	    int next_index = answer.index('\n', index);
	    if (next_index < 0)
		next_index = answer.length();
	    else
		next_index++;
	    answer.at(index, next_index - index) = "";
#endif
	    break;
	}
	else if (line.contains("line="))
	{
	    // JDB 1.2 output format:
	    // `Step completed: thread="main", jtest.array_test(), 
	    //  line=77, bci=206'

	    string line_no = line.after("line=");
	    line_no = line_no.before(", ");

	    string class_name = line.after(", ");
	    class_name = class_name.before(", ");
	    if (class_name.contains("("))
	    {
		// Within a method
		class_name = class_name.before('(');
		if (class_name.contains("."))
		    class_name = class_name.before('.', -1);
	    }

	    string file = class_name;
	    strip_space(file);

	    pos_buffer	 = file + ":" + line_no;
	    already_read = PosComplete;

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
}

void PosBuffer::filter_make(string& answer)
{
    // Check for regular source info
    int index1 = answer.index ("\032\032");
	    
    if (index1 < 0) 
    {
	int index_p = answer.index ("\032");
	if (index_p >= 0 && index_p == int(answer.length()) - 1)
	{
	    // Possible begin of position info at end of ANSWER
	    answer_buffer = "\032";
	    answer = answer.before (index_p);
	    already_read = PosPart;
	    return;
	}
    }
    else
    {
	// ANSWER contains position info
	int index2 = answer.index("\n", index1);
	    
	if (index2 == -1)
	{
	    // Position info is incomplete
	    answer_buffer = answer.from (index1);
	    answer = answer.before (index1);
	    already_read = PosPart;
	    return;
	}
	else
	{
	    assert (index1 < index2);
	    
	    // Position info is complete
	    already_read = PosComplete;
	    pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));
	    answer.at(index1, index2 - index1 + 1) = "";
	}
    }

    if (already_read != PosComplete)
    {
	// Try '(FILE:LINE):\n';

	// INDEX points at the start of a line
	int index = 0;
	while (index >= 0 && !answer.empty())
	{
	    string line = answer.from(index);
	    if (line.contains('\n'))
		line = line.before('\n');
	    strip_trailing_space(line);
		    
#if RUNTIME_REGEX
	    static regex rxmakepos("[(][^:]*:[0-9][0-9]*[)]:");
#endif
	    if (line.matches(rxmakepos))
	    {
		// Fetch position
		pos_buffer = line.after('(');
		pos_buffer = pos_buffer.before(')');
		already_read = PosComplete;

		// Delete this line from output
		int next_index = answer.index('\n', index);
		if (next_index < 0)
		    next_index = answer.length();
		else
		    next_index++;
		answer.at(index, next_index - index) = "";
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
    }
}

void PosBuffer::filter_perl(string& answer)
{
    // Check for regular source info
    int index1 = answer.index ("\032\032");
	    
    if (index1 < 0) 
    {
	int index_p = answer.index ("\032");
	if (index_p >= 0 && index_p == int(answer.length()) - 1)
	{
	    // Possible begin of position info at end of ANSWER
	    answer_buffer = "\032";
	    answer = answer.before (index_p);
	    already_read = PosPart;
	    return;
	}
    }
    else
    {
	// ANSWER contains position info
	int index2 = answer.index("\n", index1);
	    
	if (index2 == -1)
	{
	    // Position info is incomplete
	    answer_buffer = answer.from (index1);
	    answer = answer.before (index1);
	    already_read = PosPart;
	    return;
	}
	else
	{
	    assert (index1 < index2);
	    
	    // Position info is complete
	    already_read = PosComplete;
	    pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));
	    answer.at(index1, index2 - index1 + 1) = "";
	}
    }

    if (already_read != PosComplete)
    {
	// Try 'PACKAGE::FUNCTION(FILE:LINE):\n'; FUNCTION is optional

	// INDEX points at the start of a line
	int index = 0;
	while (index >= 0 && !answer.empty())
	{
	    string line = answer.from(index);
	    if (line.contains('\n'))
		line = line.before('\n');
	    strip_trailing_space(line);
		    
#if RUNTIME_REGEX
	    static regex rxperlpos("[^(]*::[^(]*[(][^:]*:[1-9][0-9]*[)]:");
#endif
	    if (line.matches(rxperlpos))
	    {
		// Fetch position
		pos_buffer = line.after('(');
		pos_buffer = pos_buffer.before(')');
		already_read = PosComplete;

		// Delete this line from output
		int next_index = answer.index('\n', index);
		if (next_index < 0)
		    next_index = answer.length();
		else
		    next_index++;
		answer.at(index, next_index - index) = "";
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
    }
}

void PosBuffer::filter_pydb(string& answer)
{
    // Check for regular source info
    int index1 = answer.index ("\032\032");
	    
    if (index1 < 0) 
    {
	int index_p = answer.index ("\032");
	if (index_p >= 0 && index_p == int(answer.length()) - 1)
	{
	    // Possible begin of position info at end of ANSWER
	    answer_buffer = "\032";
	    answer = answer.before (index_p);
	    already_read = PosPart;
	    return;
	}
    }
    else
    {
	// ANSWER contains position info
	int index2 = answer.index("\n", index1);
	    
	if (index2 == -1)
	{
	    // Position info is incomplete
	    answer_buffer = answer.from (index1);
	    answer = answer.before (index1);
	    already_read = PosPart;
	    return;
	}
	else
	{
	    assert (index1 < index2);
	    
	    // Position info is complete
	    already_read = PosComplete;
	    pos_buffer = answer.at(index1 + 2, index2 - (index1 + 2));
	    answer.at(index1, index2 - index1 + 1) = "";
	}
    }

    if (already_read != PosComplete)
    {
	// Try '(FILE:LINE):\n';

	// INDEX points at the start of a line
	int index = 0;
	while (index >= 0 && !answer.empty())
	{
	    string line = answer.from(index);
	    if (line.contains('\n'))
		line = line.before('\n');
	    strip_trailing_space(line);
		    
#if RUNTIME_REGEX
	    static regex rxpypos("[(][^:]*:[1-9][0-9]*[)]:");
#endif
	    if (line.matches(rxpypos))
	    {
		// Fetch position
		pos_buffer = line.after('(');
		pos_buffer = pos_buffer.before(')');
		already_read = PosComplete;

		// Delete this line from output
		int next_index = answer.index('\n', index);
		if (next_index < 0)
		    next_index = answer.length();
		else
		    next_index++;
		answer.at(index, next_index - index) = "";
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
    }
}

void PosBuffer::filter_xdb(string& answer)
{
    if (already_read != PosComplete && !answer.contains('\n'))
    {
	// Position info is incomplete
	answer_buffer = answer;
	answer = "";
	already_read = PosPart;
	return;
    }

    // INDEX points at the start of a line
    int index = 0;
    while (index >= 0 && !answer.empty())
    {
	string line = answer.from(index);
	if (line.contains('\n'))
	    line = line.before('\n');
	strip_trailing_space(line);
		
	// XDB uses a format like `ctest.c: main: 4: int a = 33;'
#if RUNTIME_REGEX
	static regex rxxdbpos("[^ \t]*:.*: [1-9][0-9]*[: ].*");
#endif
	if (line.matches(rxxdbpos))
	{
	    string file = line.before(':');
	    line = line.after(':');
		    
	    // The function name may contain "::"
	    string func = line;
	    while (line.contains("::"))
		line = line.after("::");
	    line = line.from(':');
	    func = func.before(line);
		    
	    line = line.after(':');
	    string line_no = line.before(':');
		    
	    strip_leading_space(func);
	    strip_leading_space(line_no);
	    line_no = line_no.through(rxint);
		    
	    pos_buffer   = file + ":" + line_no;
	    func_buffer  = func;
	    already_read = PosComplete;
		    
	    // Delete this line from output
	    int next_index = answer.index('\n', index);
	    if (next_index < 0)
		next_index = answer.length();
	    else
		next_index++;
	    answer.at(index, next_index - index) = "";
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
	    
    // Check for trailing `:' in last line
    index = answer.index('\n', -1) + 1;
    if (already_read != PosComplete 
	&& answer.index(':', index) >= 0)
    {
	answer_buffer = answer.from(index);
	answer.from(index) = "";
	already_read = PosPart;
	return;
    }
}
	
string PosBuffer::answer_ended ()
{
    switch (already_read) 
    {
    case Null:
    {
	assert (pos_buffer.empty());
	return auto_cmd_part;
    }

    case PosPart:
    {
	assert (pos_buffer.empty());
	const string ans = auto_cmd_part + answer_buffer;
	answer_buffer = "";
	return ans;
    }

    case PosComplete:
    {
	assert (!pos_buffer.empty());
	return auto_cmd_part;
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
