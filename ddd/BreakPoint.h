// $Id$
// Breakpoint handler

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_BreakPoint_h
#define _DDD_BreakPoint_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// A `BreakPoint' stores information about an existing debugger breakpoint.
//-----------------------------------------------------------------------------

#include <X11/Intrinsic.h>

// Misc includes
#include "strclass.h"
#include "bool.h"
#include "GDBAgent.h"
#include "StringA.h"

// Breakpoint type, respectively:
//  Breakpoint
//  Watchpoint
//  Tracepoint (as in GDB)
//  Actionpoint (`when' in DBX, action in Perl)
#define BREAKPOINT_TABLE     \
X(BREAKPOINT,"Breakpoint"),  \
X(WATCHPOINT,"Watchpoint"),  \
X(TRACEPOINT,"Tracepoint"),  \
X(ACTIONPOINT,"Actionpoint")

#define BREAKPOINT_TABLE_NOC \
X(BREAKPOINT,"Breakpoint")   \
X(WATCHPOINT,"Watchpoint")   \
X(TRACEPOINT,"Tracepoint")   \
X(ACTIONPOINT,"Actionpoint")

enum BPType {
#define X(a,b) a
    BREAKPOINT_TABLE
#undef X
};

// What to do when breakpoint is reached
enum BPDispo {
    BPKEEP,			// Keep (default)
    BPDEL,			// Delete (temporary breakpoint)
    BPDIS			// Disable (`enable once' in GDB)
};

class BreakPoint {
    int     mynumber;		// Breakpoint number
    BPType  mytype;		// Type, as above
    BPDispo mydispo;		// Disposition, as above
    bool    myenabled;		// Is breakpoint enabled?
    string  myfile_name;	// File name
    int     myline_nr;		// Line number
    string  myaddress;		// Address in memory
    string  myfunc;		// Function name
    string  myexpr;		// Expression to watch (for watchpoints)
    string  myinfos;		// Additional information (human-readable)
    int     myignore_count;	// Ignore count
    string  mycondition;	// Breakpoint condition
    StringArray mycommands;	// Commands to be issued when reached
    string  myarg;		// Argument as given to breakpoint command
    WatchMode mywatch_mode;	// Watchpoint detail

    bool    myenabled_changed;	// True if `enabled' state changed
    bool    myfile_changed;	// True if source file changed
    bool    myposition_changed;	// True if position changed
    bool    myaddress_changed;	// True if address changed
    bool    myselected;		// True if selected

    Widget  mysource_glyph;	// Associated glyph in source
    Widget  mycode_glyph;	// Associated glyph in code

private:
    BreakPoint(const BreakPoint&);
    BreakPoint& operator = (const BreakPoint&);

protected:
    // Helpers
    static const string& false_value();
    static const string& and_op();
    static bool is_false(const string& cond);

    void process_gdb(string& info_output);
    void process_dbx(string& info_output);
    void process_xdb(string& info_output);
    void process_jdb(string& info_output);
    void process_pydb(string& info_output);
    void process_perl(string& info_output);
    void process_bash(string& info_output);

public:
    // Create new breakpoint from INFO_OUTPUT.  ARG denotes the
    // argument of a breakpoint setting command.  If NUMBER is != 0,
    // assign this number.  If FILE is != "", assume this file.
    // Delete own info from INFO_OUTPUT.
    BreakPoint(string& info_output, 
	       const string& arg, int number, string& file);

    // This should avoid link warnings on SGI
    // as reported by Mike Tosti <tosti@cinesite.com>
    ~BreakPoint() {}

    // Breakpoint number.
    int number() const               { return mynumber; }

    // Breakpoint symbol (`#N#', `?N?', or `_N_')
    string symbol() const;

    // Breakpoint type.
    BPType type() const { return mytype; }
    const string& title() const;

    // What to do when breakpoint is reached.
    BPDispo dispo() const { return mydispo; }

    // Whether breakpoint is enabled
    bool enabled() const;

    // Breakpoint position
    const string& file_name() const { return myfile_name; }
    int line_nr() const             { return myline_nr; }
    const string& address() const   { return myaddress; }
    string pos() const;
    const string& func() const      { return myfunc; }

    // Watchpoint info
    const string& expr() const   { return myexpr; }
    WatchMode watch_mode() const { return mywatch_mode; }

    // Additional infos
    const string& infos() const          { return myinfos; }
    int ignore_count() const             { return myignore_count; }
    const string& real_condition() const { return mycondition; }
    const StringArray& commands() const  { return mycommands; }
    string condition() const;

    // Argument of breakpoint-setting command, as passed to constructor
    const string& arg() const { return myarg; }

    // Selection state
    bool& selected() { return myselected; }

    // Associated glyphs in source and machine code
    Widget& source_glyph() { return mysource_glyph; }
    Widget& code_glyph()   { return mycode_glyph; }

    // True iff `enabled' status changed
    bool enabled_changed () const { return myenabled_changed; }

    // True iff file name changed
    bool file_changed () const { return myfile_changed; }

    // True iff position changed
    bool position_changed () const { return myposition_changed; }

    // True iff address changed
    bool address_changed () const { return myaddress_changed; }

    // Update breakpoint from breakpoint info INFO_OUTPUT.  Return
    // true iff changes occurred.  Delete own info from INFO_OUTPUT.
    // UNDO_COMMANDS contains commands to undo the change.
    // NEED_TOTAL_UNDO is set iff the change requires deleting and
    // setting a new breakpoint.
    bool update(string& info_output, std::ostream& undo_commands, 
		bool& need_total_undo);

    // Return commands to restore this breakpoint, using the dummy
    // number NR.  If AS_DUMMY is set, delete the breakpoint
    // immediately in order to increase the breakpoint number.  If
    // ADDR is set, use ADDR as (fake) address.  If COND is set, use
    // COND as (fake) condition.  Return true iff successful.
    bool get_state(std::ostream& os, int nr, bool as_dummy = false, 
		   string addr = "", string cond = char(-1));

    // Stuff for constructing `false' breakpoint conditions
    static string make_false(const string& cond);
};

#endif // _DDD_BreakPoint_h
// DON'T ADD ANYTHING BEHIND THIS #endif
