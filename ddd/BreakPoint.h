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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

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

// Breakpoint type
enum BPType {
    BREAKPOINT,			// Breakpoint
    WATCHPOINT			// Watchpoint
};

// What to do when breakpoint is reached
enum BPDispo {
    BPKEEP,			// Keep (default)
    BPDEL,			// Delete (temporary breakpoint)
    BPDIS			// Disable (???)
};

class BreakPoint {
    string  mynumber_str;
    int     mynumber;
    BPType  mytype;
    BPDispo mydispo;
    bool    myenabled;
    string  myfile_name;
    int     myline_nr;
    string  myaddress;
    string  myexpr;
    string  myinfos;
    int     myignore_count;
    string  mycondition;
    string  myarg;
    WatchMode mywatch_mode;
    bool    myenabled_changed;
    bool    myfile_changed;
    bool    myposition_changed;
    bool    myaddress_changed;
    bool    myselected;
    Widget  mysource_glyph;
    Widget  mycode_glyph;

private:
    BreakPoint(const BreakPoint& b)
	: mynumber_str(b.mynumber_str),
	  mynumber(b.mynumber),
	  mytype(b.mytype),
	  mydispo(b.mydispo),
	  myenabled(b.myenabled),
	  myfile_name(b.myfile_name),
	  myline_nr(b.myline_nr),
	  myaddress(b.myaddress),
	  myexpr(b.myexpr),
	  myinfos(b.myinfos),
	  myignore_count(b.myignore_count),
	  mycondition(b.mycondition),
	  myarg(b.myarg),
	  mywatch_mode(b.mywatch_mode),
	  myenabled_changed(b.myenabled_changed),
	  myfile_changed(b.myfile_changed),
	  myposition_changed(b.myposition_changed),
	  myaddress_changed(b.myaddress_changed),
	  myselected(b.myselected),
	  mysource_glyph(b.mysource_glyph),
	  mycode_glyph(b.mycode_glyph)
    {
	assert(0);
    }
    BreakPoint& operator = (const BreakPoint&)
    {
	assert(0); return *this;
    }

public:
    // Create new breakpoint from INFO_OUTPUT.  ARG denotes the
    // argument of a breakpoint setting command.  If NUMBER is given,
    // assign this number.  Delete own info from INFO_OUTPUT.
    BreakPoint (string& info_output, string arg = "", int number = 0);

    // Breakpoint number.
    const string& number_str()   const { return mynumber_str; }
    int           number()       const { return mynumber; }

    // Breakpoint symbol (`#N#' or `_N_')
    string        symbol()       const;

    // Breakpoint type.
    BPType        type()         const { return mytype; }

    // What to do when breakpoint is reached.
    BPDispo       dispo()        const { return mydispo; }

    // Whether breakpoint is enabled
    bool          enabled()      const { return myenabled; }

    // Breakpoint position
    const string& file_name()    const { return myfile_name; }
    int           line_nr()      const { return myline_nr; }
    const string& address()      const { return myaddress; }
    string pos() const;

    // Watchpoint info
    const string& expr()         const { return myexpr; }
    WatchMode watch_mode()       const { return mywatch_mode; }

    // Additional infos
    const string& infos()        const { return myinfos; }
    int ignore_count()           const { return myignore_count; }
    const string& condition()    const { return mycondition; }

    // Argument of breakpoint-setting command, as passed to constructor
    const string& arg()          const { return myarg; }

    // Selection state
    bool&         selected()        { return myselected; }

    // Associated glyphs in source and machine code
    Widget&       source_glyph()    { return mysource_glyph; }
    Widget&       code_glyph()      { return mycode_glyph; }

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
    bool update (string& info_output);

    // Return commands to restore this breakpoint.  If AS_DUMMY is
    // set, delete the breakpoint immediately in order to increase the
    // breakpoint number.  If ADDR is set, use ADDR as (fake) address.
    // If COND is set, use COND as (fake) condition.
    bool get_state(ostream& os, int num, bool as_dummy = false, 
		   string addr = "", string cond = char(-1));
};

#endif // _DDD_BreakPoint_h
// DON'T ADD ANYTHING BEHIND THIS #endif
