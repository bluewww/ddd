// $Id$
// Breakpoint management

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003, 2005 Free Software Foundation, Inc.
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

char BreakPoint_rcsid[] =
    "$Id$";

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
#include "value-read.h"

#if RUNTIME_REGEX
static regex rxnl_int ("\n[1-9]");
static regex rxname_colon_int_nl ("[^ ]+:[0-9]+\n");
#endif

// Create new breakpoint from INFO_OUTPUT
BreakPoint::BreakPoint(string& info_output, const string& arg, 
		       int number, string& file)
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

    if (gdb->type() != PERL)
	strip_leading_space(info_output);

    switch(gdb->type())
    {
    case GDB:
    case MAKE:
    case BASH:
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

    case DBG:
	process_dbg(info_output);
	break;
    }

    // If we found a file name, propagate it to next breakpoint
    file = file_name();
}


// Parse the output of a a gdb "info break" line. This routine is 
// also used for BASH, MAKE and possibly others (e.g. DBG, PYDB)
//
// Sample gdb info output:
// 1   breakpoint     keep y   0x080696fa in main at ddd.C:3160
//
// Sample bashdb output: 
// 1   breakpoint     keep y   /etc/init.d/network:20

void BreakPoint::process_gdb(string& info_output)
{
    // Read type (`breakpoint' or `watchpoint')
    // The type may be prefixed by `hw ' or other details.
    const string word1 = info_output.before('\n');
    const string word2 = word1.after(rxblanks_or_tabs);

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
        if (MAKE != gdb->type() && BASH != gdb->type())
	{
	    // Read address
	    myaddress = info_output.through(rxalphanum);
	  
	    info_output = info_output.after(myaddress);
	    strip_leading_space(info_output);
	    
	}
	  
        if (BASH != gdb->type()) {
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
	}

	// Location
 	string remainder = info_output.through('\n');
	info_output = info_output.after('\n');

	// GDB 5.0 may issue an (indented) file name in the following line
	if (!remainder.contains(rxname_colon_int_nl))
	{
	    remainder += info_output.through('\n');
	    if (remainder.contains(rxname_colon_int_nl))
		info_output = info_output.after('\n');
	}

 	remainder = remainder.from(rxname_colon_int_nl);
 	myfile_name = remainder.before(":");

 	remainder = remainder.after(":");
 	if (!remainder.empty() && isdigit(remainder[0]))
 	    myline_nr = get_positive_nr(remainder);
    }
    else if (mytype == WATCHPOINT)
    {
	// Read watched expression
	myexpr = info_output.before('\n');
	info_output = info_output.after('\n');
    }

    int ignore_count = 0;
    string cond      = "";
    StringArray commands;

    if (!info_output.empty() && !isdigit(info_output[0]))
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
	    bool starts_with_space = (!line.empty() && isspace(line[0]));
	    strip_leading_space(line);

	    if (line.contains("ignore next ", 0))
	    {
		// Fetch ignore count
		string count = line.after("ignore next ");
		count = count.before(" hits");
		ignore_count = atoi(count.chars());
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

void BreakPoint::process_dbg(string& info_output)
{
    // DBG has the same output format as GDB.
    process_gdb(info_output);
}

void BreakPoint::process_dbx(string& info_output)
{
    if (info_output.contains("PC==", 0) ||
	info_output.contains("stop ", 0) ||
	info_output.contains("stopped ", 0))
    {
	// Breakpoint

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
	    if (!info_output.empty() && isdigit(info_output[0]))
		new_line_nr = get_positive_nr(info_output);

	    if (!file_name.empty())
		myfile_name = file_name;

	    if (new_line_nr != 0)
		myline_nr = new_line_nr;

	    // DBX issues either locations or functions
	    myfunc = "";
	}
	else if (info_output.contains ("in ", 0))
	{
	    string line = info_output.after("in ");
	    if (line.contains('\n'))
		line = line.before('\n');

	    if (line.contains("\":"))
	    {
		// Ladebug output:
		// `PC==x in TYPE FUNC(ARGS...) "FILE":LINE { COMMANDS }

		myfile_name = line.after("\"");
		myfile_name = myfile_name.before("\"");
		myline_nr   = get_positive_nr(line.after("\":"));
		myfunc      = line.before("\"");
		strip_space(myfunc);

		// Be sure to remove TYPE
		while (myfunc.contains(" "))
		    myfunc = myfunc.after(" ");
	    }
	    else
	    {
		// DBX output:
		// `stop in FUNC'
		myfunc = line.before(rxblanks_or_tabs);
		strip_space(myfunc);

		myfile_name = "";
		myline_nr = 0;

		// Attempt to get exact position of FUNC
		const string pos = dbx_lookup(myfunc);
		if (!pos.empty())
		{
		    const string file_name = pos.before(":");
		    const string line_s    = pos.after(":");
		    int new_line_nr  = get_positive_nr(line_s);

		    myfile_name = file_name;
		    if (new_line_nr != 0)
			myline_nr = new_line_nr;
		}
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
	    myignore_count = atoi(count.chars());
	}

	if (options.contains(" if ") || options.contains(" -if "))
	{
	    string cond = options.after("if ");
	    if (!myinfos.empty())
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

	myignore_count = atoi(count.chars());
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

    const string pos = dbx_lookup(myfunc);
    if (!pos.empty())
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
	if (line_no >= 0 && !class_name.empty())
	{
	    // Strip JDB 1.2 info like `breakpoint', etc.
	    strip_space(class_name);
	    int last_space = class_name.index(" ", -1);
	    if (last_space > 0)
		class_name = class_name.after(last_space);

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
    // Format: [FILE:]
    //          LINE_NO: LINE
    //           INFO 1
    //           INFO 2 ...

    if (!info_output.contains(' ', 0))
    {
	const string first_line = info_output.before('\n');
	if (first_line.contains(':', -1))
	{
	    // Get leading file name
	    myfile_name = first_line.before(':');
	    info_output = info_output.after('\n');
	}
    }

    static const StringArray empty;
    mycommands = empty;
    myline_nr = atoi(info_output.chars());
    info_output = info_output.after('\n');
    bool break_seen = false;
    while (info_output.contains("  ", 0))
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
	    break_seen = true;
	}
	else if (info.contains("action: ", 0))
	{
	    string commands = info.after(':');
	    strip_space(commands);

	    if (commands.contains("d " + itostring(line_nr())))
		mydispo = BPDEL; // Temporary breakpoint

	    string command = "";
	    while (!commands.empty())
	    {
		const string token = read_token(commands);
		if (token != ";")
		    command += token;

		if (token == ";" || commands.empty())
		{
		    strip_space(command);
		    if (!command.empty())
		    {
			mycommands += command;
			command = "";
		    }
		}
	    }
	}
	else
	{
	    myinfos += info + '\n';
	}
    }

    if (!break_seen)
	mytype = ACTIONPOINT;
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
			std::ostream& undo_commands,
			bool& need_total_undo)
{
    string file = file_name();
    BreakPoint new_bp(info_output, arg(), number(), file);

    bool changed       = false;
    myenabled_changed  = false;
    myposition_changed = false;
    myfile_changed     = false;
    myaddress_changed  = false;
    need_total_undo    = false;

    const string num = "@" + itostring(number()) + "@";

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
	myenabled = new_bp.myenabled;

	if (myenabled)
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

    if (new_bp.mycondition != mycondition)
    {
	if (gdb->has_condition_command())
	    undo_commands << gdb->condition_command(num, condition()) << "\n";
	else
	    need_total_undo = true;

	changed = myenabled_changed = true;
	mycondition = new_bp.mycondition;
    }

    if (!equal(new_bp.commands(), commands()))
    {
        if (gdb->type() == GDB || gdb->type() == PYDB || gdb->type() == BASH)
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
    else if (file_name().empty())
	return itostring(line_nr());
    else
	return file_name() + ":" + itostring(line_nr());
}

string BreakPoint::symbol() const
{
    char c;
    if (!enabled())
	c = '_';
    else if (!condition().empty() || ignore_count() != 0)
	c = '?';
    else
	c = '#';

    return c + itostring(number()) + c;
}

string BreakPoint::condition() const
{
  return
    (is_false(real_condition())) ?
    real_condition().after(and_op()) :
    real_condition();
}

bool BreakPoint::enabled() const
{
  return
    (is_false(real_condition())) ?
    false :
    myenabled;
}



//-----------------------------------------------------------------------------
// Condition stuff
//-----------------------------------------------------------------------------

// Return "0" (or appropriate)
const string& BreakPoint::false_value()
{
#define FALSE_TABLE        \
X(C_FALSE,"0"),            \
X(PERL_FALSE,"\"0\""),     \
X(FORTRAN_FALSE,".FALSE."),\
X(JAVA_FALSE,"false"),     \
X(ADA_FALSE,"FALSE")

    static
    string const Falses[] =
      {
#define X(a,b) b
        FALSE_TABLE
#undef X
      };

    enum {
#define X(a,b) a
        FALSE_TABLE
#undef X
    };

    switch (gdb->program_language())
    {
    case LANGUAGE_BASH:
    case LANGUAGE_C:
    case LANGUAGE_PHP:
    case LANGUAGE_MAKE:
    case LANGUAGE_PYTHON:
    case LANGUAGE_OTHER:
	return Falses[C_FALSE];

    case LANGUAGE_PERL:
	// In Perl, giving a breakpoint a condition of `0' is not
	// accepted by the debugger.  So, we use the string "0"
	// instead, which Perl also evaluates to False.
	return Falses[PERL_FALSE];

    case LANGUAGE_FORTRAN:
	return Falses[FORTRAN_FALSE];

    case LANGUAGE_JAVA:
	return Falses[JAVA_FALSE];

    case LANGUAGE_CHILL:	// ?
    case LANGUAGE_PASCAL:
    case LANGUAGE_ADA:
	return Falses[ADA_FALSE];
    }

    return Falses[C_FALSE];
}

// Return " && " (or appropriate)
const string& BreakPoint::and_op()
{
#define AND_TABLE          \
X(C_AND," && "),           \
X(FORTRAN_AND," .AND. "),  \
X(ADA_AND," AND "),        \
X(PYTHON_AND," and ")

    static
    string const Ands[] =
      {
#define X(a,b) b
        AND_TABLE
#undef X
      };

    enum {
#define X(a,b) a
        AND_TABLE
#undef X
    };

    switch (gdb->program_language())
    {
    case LANGUAGE_C:
    case LANGUAGE_PERL:
    case LANGUAGE_BASH:
    case LANGUAGE_MAKE:
    case LANGUAGE_JAVA:
    case LANGUAGE_PHP:
    case LANGUAGE_OTHER:
	return Ands[C_AND];

    case LANGUAGE_FORTRAN:
	return Ands[FORTRAN_AND];

    case LANGUAGE_CHILL:	// ??
    case LANGUAGE_PASCAL:
    case LANGUAGE_ADA:
	return Ands[ADA_AND];

    case LANGUAGE_PYTHON:
	return Ands[PYTHON_AND];
    }

    return Ands[C_AND];
}

const string& BreakPoint::title() const
{
    static
    string const BreakPoints[] =
      {
#define X(a,b) b
        BREAKPOINT_TABLE
#undef X
      };

    switch (type())
    {
#define X(a,b) \
    case a:    \
        return BreakPoints[a];
    BREAKPOINT_TABLE_NOC
#undef X
    }
    // Never reached
    /*NOTREACHED*/
    ::abort();
}

// True if COND is `false' or starts with `false and'
bool BreakPoint::is_false(const string& cond)
{
    if (cond == false_value())
	return true;

    const string c = downcase(cond);
    const string prefix = downcase(false_value() + and_op());

    return c.contains(prefix, 0);
}

// Make COND `false' or `false and COND'
string BreakPoint::make_false(const string& cond)
{
    if (is_false(cond))
	return cond;
    else if (cond.empty())
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
bool BreakPoint::get_state(std::ostream& os, int nr, bool as_dummy,
			   string pos, string cond)
{
    if (pos.empty())
    { 
	if (line_nr() > 0)
	    pos = file_name() + ":" + itostring(line_nr());
	else
	    pos = string('*') + address();
    }

    if (cond == char(-1))
	cond = real_condition();

    const string num = "@" + itostring(nr) + "@";

    switch (gdb->type())
    {
    case BASH:
    case GDB:
    case MAKE:
    case PYDB:
    case DBG:
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

	case TRACEPOINT:
	case ACTIONPOINT:
	{
	    // Not handled - FIXME
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
	    if (!cond.empty() && gdb->has_condition_command())
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
	if (!cond.empty())
	{
	    if (gdb->has_handler_command())
		cond_suffix = " -if " + cond;
	    else
		cond_suffix = " if " + cond;
	}

	switch (type())
	{
	case BREAKPOINT:
	    if (!func().empty())
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

	case TRACEPOINT:
	case ACTIONPOINT:
	{
	    // Not handled - FIXME
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
	if (!cond.empty() && !gdb->has_condition_command())
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
	if (!cond.empty())
	    cond_suffix = " " + cond;

	os << "f " << pos.before(':') << "\n";

	if (type() == BREAKPOINT)
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
