// $Id$
// Breakpoint management

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

char BreakPoint_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


//-----------------------------------------------------------------------------
// Breakpoint management
//-----------------------------------------------------------------------------

#include "BreakPoint.h"
#include <ctype.h>

// Misc includes
#include "assert.h"
#include "cook.h"

// DDD includes
#include "string-fun.h"
#include "comm-manag.h"
#include "ddd.h"
#include "dbx-lookup.h"
#include "question.h"
#include "GDBAgent.h"
#include "regexps.h"
#include "index.h"

#if RUNTIME_REGEX
static regex rxnl_int ("\n[1-9]");
static regex rxname_colon_int_nl ("[^ ]+:[0-9]+\n");
#endif

// Create new breakpoint from INFO_OUTPUT
BreakPoint::BreakPoint (string& info_output, string arg)
    : mynumber_str(""),
      mynumber(0),
      mytype(BREAKPOINT),
      mydispo(BPKEEP),
      myenabled(true),
      myfile_name(""),
      myline_nr(0),
      myaddress(""),
      myinfos(""),
      myignore_count(""),
      mycondition(""),
      myarg(arg),
      myfile_changed(true),
      myposition_changed(true),
      myaddress_changed(true),
      myselected(false),
      mysource_glyph(0),
      mycode_glyph(0)
{
    update(info_output);
}


// Update breakpoint information
bool BreakPoint::update (string& info_output)
{
    bool changed       = false;
    myenabled_changed  = false;
    myposition_changed = false;
    myfile_changed     = false;
    myaddress_changed  = false;

    read_leading_blanks(info_output);
    string number_str = read_nr_str (info_output);
    int number = get_positive_nr (number_str);
    if (number < 0)
	return false;

    if (number_str != mynumber_str)
    {
	mynumber_str = number_str;
	changed = true;
    }
    if (number != mynumber)
    {
	mynumber = number;
	changed = true;
    }

    switch(gdb->type())
    {
    case GDB:
	{
	    // Read "Type" 
	    read_leading_blanks (info_output);
	    if (info_output.contains ("watchpoint", 0))
	    {
		changed |= (mytype != WATCHPOINT);
		mytype = WATCHPOINT;
	    }
	    else if (info_output.contains ("breakpoint", 0))
	    {
		changed |= (mytype != BREAKPOINT);
		mytype = BREAKPOINT;
	    }
	    info_output = info_output.after(rxblanks_or_tabs);

	    // Read "Disp"
	    if (info_output.contains("dis", 0))
	    {
		changed |= (mydispo != BPDIS);
		mydispo = BPDIS;
	    }
	    else if (info_output.contains("del", 0))
	    {
		changed |= (mydispo != BPDEL);
		mydispo = BPDEL;
	    }
	    else if (info_output.contains("keep", 0))
	    {
		changed |= (mydispo != BPKEEP);
		mydispo = BPKEEP;
	    }
	    info_output = info_output.after(rxblanks_or_tabs);

	    // Read "Enb"
	    if (info_output.index ("y") == 0) {
		if (myenabled != true) {
		    changed = myenabled_changed = true;
		    myenabled = true;
		}
	    }
	    else if (info_output.contains("n", 0))
	    {
		if (myenabled != false) {
		    changed = myenabled_changed = true;
		    myenabled = false;
		}
	    }
	    info_output = info_output.after(rxblanks_or_tabs);

	    string new_info = "";
	    if (mytype == BREAKPOINT) 
	    {
		// Read address
		string new_address = info_output.through(rxalphanum);

		if (myaddress != new_address)
		{
		    changed = myaddress_changed = true;
		    myaddress = new_address;
		}

		info_output = info_output.from (rxname_colon_int_nl);
		if (myfile_name != info_output.before(":")) {
		    changed = myposition_changed = myfile_changed = true;
		    myfile_name = info_output.before(":");
		}
		info_output = info_output.after (":");
		if (info_output != "" && isdigit(info_output[0]))
		{
		    int new_line_nr = read_positive_nr (info_output);
		    if (new_line_nr < 0)
			return false;

		    if (myline_nr != new_line_nr) {
			changed = myposition_changed = true;
			myline_nr = new_line_nr;
		    }
		}
	    }
	    else if (mytype == WATCHPOINT)
	    {
		// Read Address
		info_output = info_output.after(rxblanks_or_tabs);
		new_info = info_output.through ("\n");
	    }

	    if (info_output != "" && !isdigit(info_output[0]))
	    {
		// Extra info may follow
		int next_nl = index(info_output, rxnl_int, "\n");
		if (next_nl == -1)
		{
		    // That's all, folks!
		    new_info += info_output;
		    info_output = "";
		}
		else {
		    new_info += info_output.through(next_nl);
		    info_output = info_output.after(next_nl);
		}
	    }

	    if (new_info != myinfos)
	    {
		changed = true;
		myinfos = new_info;
	    }
	}
	break;

    case DBX:
	{
	    read_leading_blanks (info_output);
	    if (info_output.contains ("stop ", 0)
		|| info_output.contains ("stopped ", 0))
	    {
		info_output = info_output.after(rxblanks_or_tabs);
		read_leading_blanks (info_output);
		if (info_output.contains ("at ", 0))
		{
		    info_output = info_output.after(rxblanks_or_tabs);
		    string file_name;
		    if (info_output.contains('"', 0))
		    {
			// ``stop at "FILE":LINE''
			file_name = unquote(info_output.before(":"));
			info_output = info_output.after (":");
		    }
		    else if (info_output.contains('[', 0))
		    {
			// ``stop at [file:line ...]''
			file_name = info_output.before(":");
			file_name = file_name.after('[');
			info_output = info_output.after (":");
		    }
		    else
		    {
			// ``stop at LINE''
			file_name = "";
		    }

		    int new_line_nr = 0;
		    if (info_output != "" && isdigit(info_output[0]))
			new_line_nr = get_positive_nr(info_output);

		    if (file_name != "" && file_name != myfile_name)
		    {
			myfile_name = file_name;
			changed = myposition_changed = myfile_changed = true;
		    }

		    if (new_line_nr && new_line_nr != myline_nr)
		    {
			myline_nr = new_line_nr;
			changed = myposition_changed = true;
		    }
		}
		else if (info_output.contains ("in ", 0))
		{
		    // ``stop in FUNC''
		    string func = info_output.after(rxblanks_or_tabs);
		    func = func.before('\n');
		    string pos = dbx_lookup(func);
		    if (pos != "")
		    {
			string file_name = pos.before(":");
			string line_s    = pos.after(":");
			int new_line_nr  = get_positive_nr(line_s);

			if (file_name != myfile_name)
			{
			    myfile_name = file_name;
			    changed 
				= myposition_changed 
				= myfile_changed 
				= true;
			}

			if (new_line_nr && new_line_nr != myline_nr)
			{
			    myline_nr = new_line_nr;
			    changed = myposition_changed = true;
			}
		    }
		}

		// Sun DBX 3.0 issues extra characters like 
		// (2) stop in main -count 0/10
		// [3] stop in main -disable
		string options;
		if (info_output.contains('\n'))
		    options = info_output.before('\n');
		else
		    options = info_output;
		bool new_enabled = !options.contains(" -disable");
		if (new_enabled != myenabled)
		{
		    myenabled = new_enabled;
		    changed = myenabled_changed = true;
		}

		myinfos = "";
		if (options.contains(" -count "))
		{
		    string count = options.after(" -count ");
		    read_leading_blanks(count);
		    if (count.contains(' '))
			count = count.before(' ');

		    myinfos = "count " + count;
		    if (count.contains('/'))
			count = count.after('/');
		    count = read_nr_str(count);

		    if (count != myignore_count)
		    {
			myignore_count = count;
			changed = true;
		    }
		}

		if (options.contains(" if ") || options.contains(" -if "))
		{
		    string cond = options.after("if ");
		    if (myinfos != "")
			myinfos += '\n';
		    myinfos += "stop only if " + cond;
		    if (cond != mycondition)
		    {
			mycondition = cond;
			changed = true;
		    }
		}
	    }
	    info_output = info_output.after('\n');
	}
	break;

    case XDB:
	{
	    // Strip leading `:'.
	    // Bob Wiegand <robert.e.wiegand.1@gsfc.nasa.gov>
	    if (info_output.contains(':', 0))
		info_output = info_output.after(0);

	    read_leading_blanks(info_output);

	    // Skip `count: N'
	    if (info_output.contains("count:", 0))
	    {
		info_output = info_output.after("count:");
		read_leading_blanks(info_output);
		string ignore_count = info_output.before(rxblanks_or_tabs);
		info_output = info_output.after(rxblanks_or_tabs);
		if (myignore_count != ignore_count)
		{
		    changed = true;
		    myignore_count = ignore_count;
		}
	    }
	    
	    // Check for `Active' or `Suspended' and strip them
	    // Bob Wiegand <robert.e.wiegand.1@gsfc.nasa.gov>
	    if (info_output.contains("Active", 0))
	    {
		if (!myenabled)
		    changed = myenabled_changed = true;
		info_output = info_output.after("Active");
		myenabled   = true;
	    }
	    else if (info_output.contains("Suspended", 0))
	    {
		if (myenabled)
		    changed = myenabled_changed = true;
		info_output = info_output.after("Suspended");
		myenabled   = false;
	    }

	    // Get function name and position
	    info_output = info_output.after(rxblanks_or_tabs);
	    string func = info_output.before(": ");
	    string pos  = dbx_lookup(func);
	    if (pos != "")
	    {
		string file_name = pos.before(":");
		if (file_name != myfile_name)
		{
		    changed = myposition_changed = myfile_changed = true;
		    myfile_name = file_name;
		}
	    }

	    info_output = info_output.after(": ");
	    int line_nr = get_positive_nr(info_output);
	    if (line_nr != myline_nr)
	    {
		changed = myposition_changed = true;
		myline_nr = line_nr;
	    }
	    
	    info_output = info_output.after('\n');

	    // Examine commands for condition
	    string commands = info_output;
	    read_leading_blanks(commands);
	    if (commands.contains('{', 0))
	    {
		// A condition has the form `{if COND {} {Q; c}}'.
		if (commands.contains("{if ", 0))
		{
		    string cond = commands.after("{if ");
		    cond = cond.before('{');
		    strip_final_blanks(cond);
		    if (cond != mycondition)
		    {
			mycondition = cond;
			changed = true;
		    }
		}

		// Skip this line, too
		info_output = info_output.after('\n');
	    }
	}
	break;
    }

    return changed;
}


//-----------------------------------------------------------------------------
// Session stuff
//-----------------------------------------------------------------------------

// Return ignore count ("" if none)
string BreakPoint::ignore_count() const
{
    switch (gdb->type())
    {
    case GDB:
    {
	string info = gdb_question("info breakpoint " + number_str());
	if (info == NO_GDB_ANSWER)
	    return "";

	string ignore = info.after("ignore next ");
	return ignore.before(" hits");
    }
    break;

    case DBX:
    case XDB:
	return myignore_count;
    }

    return "";			// Never reached
}

// Return condition ("" if none)
string BreakPoint::condition() const
{
    switch (gdb->type())
    {
    case GDB:
    {
	string info = gdb_question("info breakpoint " + number_str());
	if (info == NO_GDB_ANSWER)
	    return "";

	string cond = info.after("stop only if ");
	if (cond.contains('\n'))
	    cond = cond.before("\n");

	return cond;
    }
    case DBX:
    case XDB:
	return mycondition;
    }

    return "";			// Never reached
}

// Return commands to restore this breakpoint.  If AS_DUMMY is set,
// delete the breakpoint immediately in order to increase the
// breakpoint number.  If ADDR is set, use ADDR as (fake) address.  If
// COND is set, use COND as (fake) condition.
bool BreakPoint::get_state(ostream& os, int nr, bool as_dummy,
			   string pos, string cond)
{
    if (pos == "")
    { 
	if (line_nr() > 0)
	    pos = file_name() + ":" + itostring(line_nr());
	else
	    pos = string('*') + address();
    }

    if (cond == char(-1))
	cond = condition();

    string num = "@" + itostring(nr) + "@";

    switch (gdb->type())
    {
    case GDB:
    {
	switch (BPType())
	{
	case BREAKPOINT:
	{
	    switch (dispo())
	    {
	    case BPKEEP:
	    case BPDIS:
		os << "break " << pos << "\n";
		break;

	    case BPDEL:
		os << "tbreak " << pos << "\n";
		break;
	    }
	    break;
	}

	case WATCHPOINT:
	{
	    os << "watch " << infos() << "\n";
	    break;
	}
	}

	if (!as_dummy)
	{
	    // Extra infos
	    if (!enabled() && gdb->has_disable_command())
		os << gdb->disable_command(num) << "\n";
	    string ignore = ignore_count();
	    if (ignore != "" && gdb->has_ignore_command())
		os << gdb->ignore_command(num, atoi(ignore)) << "\n";
	    if (cond != "" && gdb->has_condition_command())
		os << gdb->condition_command(num, cond) << "\n";
	}
	break;
    }

    case DBX:
    {
	string cond_suffix = "";
	if (cond != "")
	{
	    if (gdb->has_handler_command())
		cond_suffix = " -if " + cond;
	    else
		cond_suffix = " if " + cond;
	}

	if (pos.contains('*', 0))
	{
	    os << "stop at " << pos.after('*') << cond_suffix << '\n';
	}
	else
	{
	    os << "file "    << pos.before(':') << "\n";
	    os << "stop at " << pos.after(':')  << cond_suffix << "\n";
	}

	if (!as_dummy)
	{
	    // Extra infos
	    if (!enabled() && gdb->has_disable_command())
		os << gdb->disable_command(num) << "\n";
	    string ignore = ignore_count();
	    if (ignore != "" && gdb->has_ignore_command())
		os << gdb->ignore_command(num, atoi(ignore)) << "\n";
	}
	break;
    }

    case XDB:
    {
	string cond_suffix;
	if (cond != "" && !gdb->has_condition_command())
	    cond_suffix = " {if " + cond + " {} {Q;c}}";

	if (pos.contains('*', 0))
	    os << "ba " << pos.after('*') << cond_suffix << '\n';
	else
	    os << "b " << pos << cond_suffix << "\n";

	if (!as_dummy)
	{
	    // Extra infos
	    if (!enabled() && gdb->has_disable_command())
		os << gdb->disable_command(num) << "\n";
	    string ignore = ignore_count();
	    if (ignore != "" && gdb->has_ignore_command())
		os << gdb->ignore_command(num, atoi(ignore)) << "\n";
	}
	break;
    }
    }

    if (as_dummy)
    {
	// Delete the breakpoint just created
	os << gdb->delete_command(num) << "\n";
    }

    return true;
}
