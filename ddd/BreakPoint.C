// $Id$
// Breakpoint management

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
BreakPoint::BreakPoint(string& info_output, char *arg, int number, char *file)
    : mynumber(number),
      mytype(BREAKPOINT),
      mydispo(BPKEEP),
      myenabled(true),
      myfile_name(file),
      myline_nr(0),
      myaddress(""),
      myfunc(""),
      myexpr(""),
      myinfos(""),
      myignore_count(0),
      mycondition(""),
      mycommands(0),
      myarg(arg),
      mywatch_mode(WATCH_CHANGE),
      myenabled_changed(true),
      myfile_changed(true),
      myposition_changed(true),
      myaddress_changed(true),
      myselected(false),
      mysource_glyph(0),
      mycode_glyph(0)
{
    if (gdb->has_numbered_breakpoints())
    {
	// Read leading breakpoint number
	strip_leading_space(info_output);
	string number_str = read_nr_str (info_output);
	int number = get_positive_nr (number_str);
	if (number < 0)
	    return;

	mynumber = number;
    }

    strip_leading_space(info_output);

    switch(gdb->type())
    {
    case GDB:
	process_gdb(info_output);
	break;

    case DBX:
	process_dbx(info_output);
	break;

    case XDB:
	process_xdb(info_output);
	break;

    case JDB:
	process_jdb(info_output);
	break;

    case PYDB:
	process_pydb(info_output);
	break;

    case PERL:
	process_perl(info_output);
	break;
    }
}

void BreakPoint::process_gdb(string& info_output)
{
    // Read type (`breakpoint' or `watchpoint')
    // The type may be prefixed by `hw ' or other details.
    string word1 = info_output.before('\n');
    string word2 = word1.after(rxblanks_or_tabs);

    if (word1.contains("watchpoint", 0) || 
	word2.contains("watchpoint", 0))
    {
	mytype = WATCHPOINT;

	// Fetch breakpoint mode detail (`acc' or `read')
	if (word1.contains("acc ", 0))
	    mywatch_mode = WATCH_ACCESS;
	else if (word1.contains("read ", 0))
	    mywatch_mode = WATCH_READ;
	else
	    mywatch_mode = WATCH_CHANGE;
    }
    else if (word1.contains("breakpoint", 0) || 
	     word2.contains("breakpoint", 0))
    {
	mytype = BREAKPOINT;
    }
    info_output = info_output.after("point");
    info_output = info_output.after(rxblanks_or_tabs);

    // Read disposition (`dis', `del', or `keep')
    if (info_output.contains("dis", 0))
    {
	mydispo = BPDIS;
    }
    else if (info_output.contains("del", 0))
    {
	mydispo = BPDEL;
    }
    else if (info_output.contains("keep", 0))
    {
	mydispo = BPKEEP;
    }
    info_output = info_output.after(rxblanks_or_tabs);

    // Read enabled flag (`y' or `n')
    if (info_output.contains('y', 0))
    {
	myenabled = true;
    }
    else if (info_output.contains('n', 0))
    {
	myenabled = false;
    }
    info_output = info_output.after(rxblanks_or_tabs);

    string new_info = "";
    if (mytype == BREAKPOINT) 
    {
	// Read address
	myaddress = info_output.through(rxalphanum);

	info_output = info_output.after(myaddress);
	strip_leading_space(info_output);

	if (info_output.contains("in ", 0))
	{
	    // Function name
	    string func = info_output.after("in ");
	    if (func.contains('\n'))
		func = func.before('\n');
	    if (func.contains(" at "))
		func = func.before(" at ");
	    strip_space(func);

	    myfunc = func;
	}

	// Location
	info_output = info_output.from(rxname_colon_int_nl);
	myfile_name = info_output.before(":");

	info_output = info_output.after (":");
	if (info_output != "" && isdigit(info_output[0]))
	    myline_nr = get_positive_nr(info_output);
    }
    else if (mytype == WATCHPOINT)
    {
	// Read watched expression
	myexpr = info_output.before('\n');
    }

    // That's all in this line
    info_output = info_output.after('\n');

    int ignore_count = 0;
    string cond      = "";
    StringArray commands;

    if (info_output != "" && !isdigit(info_output[0]))
    {
	// Extra info follows
	int next_nl = index(info_output, rxnl_int, "\n");
	if (next_nl == -1)
	{
	    new_info += info_output;
	    info_output = "";
	}
	else
	{
	    new_info += info_output.through(next_nl);
	    info_output = info_output.after(next_nl);
	}

	int n = new_info.freq('\n');
	string *lines = new string[n + 1];
	split(new_info, lines, n + 1, '\n');
	string newer_info = "";

	for (int i = 0; i < n; i++)
	{
	    bool save_info = true;

	    string line = lines[i];
	    bool starts_with_space = (line != "" && isspace(line[0]));
	    strip_leading_space(line);

	    if (line.contains("ignore next ", 0))
	    {
		// Fetch ignore count
		string count = line.after("ignore next ");
		count = count.before(" hits");
		ignore_count = atoi(count);
	    }
	    else if (line.contains("stop only if ", 0))
	    {
		// Fetch condition
		cond = line.after("stop only if ");
	    }
	    else if (line.contains("stop ", 0))
	    {
		// Any info (no GDB command starts with `stop')
	    }
	    else if (line.contains("breakpoint ", 0))
	    {
		// Any info (no GDB command starts with `breakpoint')
	    }
	    else if (starts_with_space)
	    {
		// A command (GDB indents all commands)
		commands += line;
		save_info = false;
	    }
	    else
	    {
		// Some other info
	    }

	    if (save_info)
		newer_info += line + '\n';
	}

	new_info = newer_info;
	delete[] lines;
    }

    myinfos = new_info;
    myignore_count = ignore_count;
    mycondition = cond;
    mycommands = commands;
}

void BreakPoint::process_pydb(string& info_output)
{
    // PYDB has the same output format as GDB.
    process_gdb(info_output);
}

void BreakPoint::process_dbx(string& info_output)
{
    if (info_output.contains("stop ", 0) || 
	info_output.contains("stopped ", 0))
    {
	info_output = info_output.after(rxblanks_or_tabs);
	strip_leading_space (info_output);
	if (info_output.contains ("at ", 0))
	{
	    info_output = info_output.after(rxblanks_or_tabs);
	    string file_name;
	    if (info_output.contains('"', 0))
	    {
		// `stop at "FILE":LINE'
		file_name = unquote(info_output.before(":"));
		info_output = info_output.after (":");
	    }
	    else if (info_output.contains('[', 0))
	    {
		// `stop at [file:line ...]'
		file_name = info_output.before(":");
		file_name = file_name.after('[');
		info_output = info_output.after (":");
	    }
	    else
	    {
		// `stop at LINE'
		file_name = "";
	    }

	    int new_line_nr = 0;
	    if (info_output != "" && isdigit(info_output[0]))
		new_line_nr = get_positive_nr(info_output);

	    if (file_name != "")
		myfile_name = file_name;

	    if (new_line_nr != 0)
		myline_nr = new_line_nr;

	    // DBX issues either locations or functions
	    myfunc = "";
	}
	else if (info_output.contains ("in ", 0))
	{
	    // `stop in FUNC'
	    string func = info_output.after(rxblanks_or_tabs);
	    if (func.contains('\n'))
		func = func.before('\n');
	    strip_space(func);
	    myfunc = func;

	    myfile_name = "";
	    myline_nr = 0;

	    // Attempt to get exact position
	    string pos = dbx_lookup(func);
	    if (pos != "")
	    {
		string file_name = pos.before(":");
		string line_s    = pos.after(":");
		int new_line_nr  = get_positive_nr(line_s);

		myfile_name = file_name;
		if (new_line_nr != 0)
		    myline_nr = new_line_nr;
	    }
	}
	else
	{
	    // `stop VAR'
	    mytype       = WATCHPOINT;
	    mywatch_mode = WATCH_CHANGE;

	    string expr = info_output;
	    if (expr.contains('\n'))
		expr = expr.before('\n');
	    if (expr.contains(rxblanks_or_tabs))
		expr = expr.before(rxblanks_or_tabs);

	    myexpr = expr;
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
	myenabled = new_enabled;

	myinfos = "";
	if (options.contains(" -count "))
	{
	    string count = options.after(" -count ");
	    strip_leading_space(count);
	    if (count.contains(' '))
		count = count.before(' ');

	    myinfos = "count " + count;
	    if (count.contains('/'))
		count = count.after('/');
	    myignore_count = atoi(count);
	}

	if (options.contains(" if ") || options.contains(" -if "))
	{
	    string cond = options.after("if ");
	    if (myinfos != "")
		myinfos += '\n';
	    myinfos += "stop only if " + cond;
	    mycondition = cond;
	}
    }
    info_output = info_output.after('\n');
}


void BreakPoint::process_xdb(string& info_output)
{
    // Strip leading `:'.
    // Bob Wiegand <robert.e.wiegand.1@gsfc.nasa.gov>
    if (info_output.contains(':', 0))
	info_output = info_output.after(0);

    strip_leading_space(info_output);

    // Skip `count: N'
    if (info_output.contains("count:", 0))
    {
	info_output = info_output.after("count:");
	strip_leading_space(info_output);
	string count = info_output.before(rxblanks_or_tabs);
	info_output = info_output.after(rxblanks_or_tabs);

	myignore_count = atoi(count);
    }
	    
    // Check for `Active' or `Suspended' and strip them
    // Bob Wiegand <robert.e.wiegand.1@gsfc.nasa.gov>
    if (info_output.contains("Active", 0))
    {
	info_output = info_output.after("Active");
	myenabled   = true;
    }
    else if (info_output.contains("Suspended", 0))
    {
	info_output = info_output.after("Suspended");
	myenabled   = false;
    }

    // Get function name and position
    info_output = info_output.after(rxblanks_or_tabs);
    myfunc = info_output.before(": ");

    string pos = dbx_lookup(myfunc);
    if (pos != "")
    {
	myfile_name = pos.before(":");
    }

    info_output = info_output.after(": ");
    myline_nr = get_positive_nr(info_output);

    info_output = info_output.after('\n');

    // Examine commands for condition
    string commands = info_output;
    strip_leading_space(commands);
    if (commands.contains('{', 0))
    {
	// A condition has the form `{if COND {} {Q; c}}'.
	if (commands.contains("{if ", 0))
	{
	    string cond = commands.after("{if ");
	    cond = cond.before('{');
	    strip_space(cond);
	    mycondition = cond;
	}

	// Skip this line, too
	info_output = info_output.after('\n');
    }
}

void BreakPoint::process_jdb(string& info_output)
{
    int colon = info_output.index(':');
    if (colon >= 0)
    {
	string class_name = info_output.before(colon);
	int line_no = get_positive_nr(info_output.after(colon));
	if (line_no >= 0 && class_name != "")
	{
	    myfile_name = class_name;
	    myline_nr   = line_no;

	    // Kill this line
	    int beginning_of_line = colon;
	    while (beginning_of_line >= 0 && 
		   info_output[beginning_of_line] != '\n')
		beginning_of_line--;
	    beginning_of_line++;

	    int next_nl = info_output.index('\n', colon);
	    if (next_nl >= 0)
		info_output = info_output.before(beginning_of_line)
		    + info_output.from(next_nl);
	    else
		info_output = info_output.before(beginning_of_line);
	}
    }
}

void BreakPoint::process_perl(string& info_output)
{
    // Format: LINE_NO: LINE
    //           INFO 1
    //           INFO 2 ...
    myline_nr = atoi(info_output);
    info_output = info_output.after('\n');
    while (info_output.contains(' ', 0))
    {
	string info = info_output.before('\n');
	info_output = info_output.after('\n');

	strip_space(info);
	if (info.contains("break if ", 0))
	{
	    string cond = info.after(" if ");
	    while (cond.contains('(', 0) && cond.contains(')', -1))
		cond = unquote(cond);
	    if (cond == "1")
		cond = "";
	    mycondition = cond;
	}
	else if (info.contains("action: ", 0))
	{
	    string command = info.after(':');
	    strip_space(command);
	    mycommands += command;
	}
	else
	{
	    myinfos += info + '\n';
	}
    }
}

static bool equal(const StringArray& s1, const StringArray& s2)
{
    if (s1.size() != s2.size())
	return false;
    for (int i = 0; i < s1.size(); i++)
	if (s1[i] != s2[i])
	    return false;

    return true;
}

// Update breakpoint information
bool BreakPoint::update(string& info_output,
			ostream& undo_commands,
			bool& need_total_undo)
{
    BreakPoint new_bp(info_output);

    bool changed       = false;
    myenabled_changed  = false;
    myposition_changed = false;
    myfile_changed     = false;
    myaddress_changed  = false;
    need_total_undo    = false;

    string num = "@" + itostring(number()) + "@";

    if (new_bp.number() != number())
    {
	mynumber = new_bp.number();
	need_total_undo = changed = true;
    }

    if (new_bp.type() != type())
    {
	mytype = new_bp.type();
	need_total_undo = changed = myenabled_changed = true;
    }

    if (new_bp.dispo() != dispo())
    {
	need_total_undo = changed = myenabled_changed = true;
	mydispo = new_bp.dispo();
    }

    if (new_bp.watch_mode() != watch_mode())
    {
	need_total_undo = changed = myenabled_changed = true;
	mywatch_mode = new_bp.watch_mode();
    }

    if (new_bp.myenabled != myenabled)
    {
	changed = myenabled_changed = true;
	myenabled = new_bp.enabled();

	if (new_bp.myenabled)
	{
	    if (gdb->has_disable_command())
		undo_commands << gdb->disable_command(num) << "\n";
	    else
		need_total_undo = true;
	}
	else
	{
	    if (gdb->has_enable_command())
		undo_commands << gdb->enable_command(num) << "\n";
	    else
		need_total_undo = true;
	}
    }

    if (type() == BREAKPOINT)
    {
	if (new_bp.address() != address())
	{
	    changed = myaddress_changed = true;
	    myaddress = new_bp.address();
	}

	if (new_bp.func() != func())
	{
	    changed = myposition_changed = true;
	    myfunc = new_bp.func();
	}

	if (new_bp.file_name() != file_name())
	{
	    changed = myposition_changed = myfile_changed = true;
	    myfile_name = new_bp.file_name();
	}

	if (new_bp.line_nr() != line_nr())
	{
	    changed = myposition_changed = true;
	    myline_nr = new_bp.line_nr();
	}
    }
    else if (type() == WATCHPOINT)
    {
	if (new_bp.expr() != expr())
	{
	    changed = true;
	    myexpr = new_bp.expr();
	}
    }

    if (new_bp.infos() != infos())
    {
	changed = true;
	myinfos = new_bp.infos();
    }

    if (new_bp.ignore_count() != ignore_count())
    {
	if (gdb->has_ignore_command())
	    undo_commands << gdb->ignore_command(num, myignore_count) << "\n";
	else
	    need_total_undo = true;

	changed = myenabled_changed = true;
	myignore_count = new_bp.ignore_count();
    }

    if (new_bp.real_condition() != real_condition())
    {
	if (gdb->has_condition_command())
	    undo_commands << gdb->condition_command(num, condition()) << "\n";
	else
	    need_total_undo = true;

	changed = myenabled_changed = true;
	mycondition = new_bp.real_condition();
    }

    if (!equal(new_bp.commands(), commands()))
    {
	if (gdb->type() == GDB)
	{
	    undo_commands << "commands " << num << '\n';
	    for (int i = 0; i < commands().size(); i++)
		undo_commands << commands()[i] << '\n';
	    undo_commands << "end\n";
	}

	changed = myenabled_changed = true;
	mycommands = new_bp.commands();
    }

    return changed;
}


//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------

string BreakPoint::pos() const
{
    if (line_nr() == 0)
	return "*" + address();
    else if (file_name() == "")
	return itostring(line_nr());
    else
	return file_name() + ":" + itostring(line_nr());
}

string BreakPoint::symbol() const
{
    char c;
    if (!enabled())
	c = '_';
    else if (condition() != "" || ignore_count() != 0)
	c = '?';
    else
	c = '#';

    return c + itostring(number()) + c;
}

string BreakPoint::condition() const
{
    if (is_false(real_condition()))
	return real_condition().after(and_op());
    else
	return real_condition();
}

bool BreakPoint::enabled() const
{
    if (is_false(real_condition()))
	return false;
    else
	return myenabled;
}



//-----------------------------------------------------------------------------
// Condition stuff
//-----------------------------------------------------------------------------

// Return "0" (or appropriate)
string BreakPoint::false_value()
{
    switch (gdb->program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_PYTHON:
    case LANGUAGE_OTHER:
	return "0";

    case LANGUAGE_PERL:
	// In Perl, giving a breakpoint a condition of `0' is not
	// accepted by the debugger.  So, we use the string "0"
	// instead, which Perl also evaluates to False.
	return "\"0\"";

    case LANGUAGE_FORTRAN:
	return ".FALSE.";

    case LANGUAGE_JAVA:
	return "false";

    case LANGUAGE_CHILL:	// ??
    case LANGUAGE_PASCAL:
    case LANGUAGE_ADA:
	return "FALSE";
    }

    return "0";
}

// Return " && " (or appropriate)
string BreakPoint::and_op()
{
    switch (gdb->program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_PERL:
    case LANGUAGE_JAVA:
    case LANGUAGE_OTHER:
	return " && ";

    case LANGUAGE_FORTRAN:
	return " .AND. ";

    case LANGUAGE_CHILL:	// ??
    case LANGUAGE_PASCAL:
    case LANGUAGE_ADA:
	return " AND ";

    case LANGUAGE_PYTHON:
	return " and ";
    }

    return " && ";
}

// True if COND is `false' or starts with `false and'
bool BreakPoint::is_false(const string& cond)
{
    if (cond == false_value())
	return true;

    string c = downcase(cond);
    string prefix = downcase(false_value() + and_op());

    return c.contains(prefix, 0);
}

// Make COND `false' or `false and COND'
string BreakPoint::make_false(const string& cond)
{
    if (is_false(cond))
	return cond;
    else if (cond == "")
	return false_value();
    else
	return false_value() + and_op() + cond;
}

//-----------------------------------------------------------------------------
// Session stuff
//-----------------------------------------------------------------------------

// Return commands to restore this breakpoint, using the dummy number
// NR.  If AS_DUMMY is set, delete the breakpoint immediately in order
// to increase the breakpoint number.  If ADDR is set, use ADDR as
// (fake) address.  If COND is set, use COND as (fake) condition.
// Return true iff successful.
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
	cond = real_condition();

    string num = "@" + itostring(nr) + "@";

    switch (gdb->type())
    {
    case GDB:
    case PYDB:
    {
	switch (type())
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
	    os << gdb->watch_command(expr(), watch_mode()) << "\n";
	    break;
	}
	}

	if (!as_dummy)
	{
	    // Extra infos
	    if (!enabled() && gdb->has_disable_command())
		os << gdb->disable_command(num) << "\n";
	    int ignore = ignore_count();
	    if (ignore > 0 && gdb->has_ignore_command())
		os << gdb->ignore_command(num, ignore) << "\n";
	    if (cond != "" && gdb->has_condition_command())
		os << gdb->condition_command(num, cond) << "\n";
	    if (commands().size() != 0)
	    {
		os << "commands " << num << "\n";
		for (int i = 0; i < commands().size(); i++)
		    os << commands()[i] << "\n";
		os << "end\n";
	    }
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

	switch (type())
	{
	case BREAKPOINT:
	    if (func() != "")
	    {
		os << "stop in " << func() << "\n";
	    }
	    else if (pos.contains('*', 0))
	    {
		os << "stop at " << pos.after('*') << cond_suffix << '\n';
	    }
	    else
	    {
		os << "file "    << pos.before(':') << "\n";
		os << "stop at " << pos.after(':')  << cond_suffix << "\n";
	    }
	    break;

	case WATCHPOINT:
	    os << "stop " << expr() << cond_suffix << '\n';
	    break;
	}

	if (!as_dummy)
	{
	    // Extra infos
	    if (!enabled() && gdb->has_disable_command())
		os << gdb->disable_command(num) << "\n";
	    int ignore = ignore_count();
	    if (ignore > 0 && gdb->has_ignore_command())
		os << gdb->ignore_command(num, ignore) << "\n";
	}
	break;
    }

    case JDB:
    {
	os << "stop at " << pos << "\n";
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
	    int ignore = ignore_count();
	    if (ignore > 0 && gdb->has_ignore_command())
		os << gdb->ignore_command(num, ignore) << "\n";
	}
	break;
    }

    case PERL:
    {
	string cond_suffix;
	if (cond != "")
	    cond_suffix = " " + cond;

	os << "f " << pos.before(':') << "\n";
	os << "b " << pos.after(':')  << cond_suffix << "\n";

	if (commands().size() != 0)
	{
	    os << "a " << pos.after(':') << " ";
	    for (int i = 0; i < commands().size(); i++)
		os << commands()[i] << ";";
	    os << "\n";
	}

	break;
    }
    }

    if (as_dummy && gdb->has_delete_command())
    {
	// Delete the breakpoint just created
	os << gdb->delete_command(num) << "\n";
    }

    return true;
}
