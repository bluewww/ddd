// $Id$
// Breakpoint handler

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

char BreakPoint_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementation von BreakPoint.h
//-----------------------------------------------------------------------------

#include "BreakPoint.h"
#include <ctype.h>

// includes von Nora-Zeugs
#include "assert.h"
#include "cook.h"

// includes eigener Sachen
#include "string-fun.h"
#include "comm-manag.h"
#include "ddd.h"
#include "dbx-lookup.h"
#include "GDBAgent.h"

regex RXnl_int ("\n[1-9]", true);
regex RXname_colon_int_nl ("[^ ]+:[0-9]+\n", true);

BreakPoint::BreakPoint (string& info_output)
    : mytype(BREAKPOINT),
      mydispo(BPKEEP),
      myenabled(true),
      myfile_name(""),
      myline_nr(0),
      myaddress(""),
      myinfos(""),
      myfile_changed(true),
      myposition_changed(true),
      myaddress_changed(true),
      myselected(false),
      mysource_glyph(0),
      mycode_glyph(0)
{
    mynumber_str = read_nr_str (info_output);
    mynumber = get_positive_nr (mynumber_str);

    switch(gdb->type())
    {
    case GDB:
    {
	// "Type" lesen
	read_leading_blanks (info_output);
	if (info_output.contains ("watchpoint", 0))
	    mytype = WATCHPOINT;
	else if (info_output.contains ("breakpoint", 0))
	    mytype = BREAKPOINT;

	info_output = info_output.after(RXblanks_or_tabs);

	// "Disp" lesen
	if (info_output.contains("dis", 0))
	    mydispo = BPDIS;
	else if (info_output.contains("del", 0))
	    mydispo = BPDEL;
	else if (info_output.contains("keep", 0))
	    mydispo = BPKEEP;
	info_output = info_output.after(RXblanks_or_tabs);

	// "Enb" lesen
	if (info_output.contains("n", 0))
	    myenabled = false;
	else {
	    myenabled = true;
	}
	info_output = info_output.after(RXblanks_or_tabs);

	if (mytype == BREAKPOINT)
	{
	    myaddress   = info_output.through(rxalphanum);

	    info_output = info_output.from (RXname_colon_int_nl);
	    myfile_name = info_output.before(":");
	    info_output = info_output.after (":");
	    if (info_output != "" && isdigit(info_output[0]))
		myline_nr = read_positive_nr (info_output);
	}
	else if (mytype == WATCHPOINT) {
	    // "Address" ueberlesen
	    info_output = info_output.after(RXblanks_or_tabs);

	    myinfos += info_output.through ("\n");
	}
	info_output = info_output.after ("\n");

	if (info_output != "" && isdigit(info_output[0]))
	{
	    // Extra info is following
	    int next_index = info_output.index(RXnl_int);
	    if (next_index == -1) {
		// dies war die letzte Breakpoint-Ausgabe
		myinfos += info_output;
		info_output = "";
	    }
	    else {
		myinfos += info_output.through (next_index);
		info_output = info_output.after (next_index);
	    }
	}
    }
    break;

    case DBX:
    {
	read_leading_blanks (info_output);
	if (info_output.contains ("stop ", 0)
	    || info_output.contains ("stopped ", 0))
	{
	    info_output = info_output.after(RXblanks_or_tabs);
	    read_leading_blanks (info_output);
	    if (info_output.contains ("at ", 0))
	    {
		info_output = info_output.after(RXblanks_or_tabs);
		if (info_output.contains('"', 0))
		{
		    // ``stop at "FILE":LINE''
		    myfile_name = unquote(info_output.before(":"));
		    info_output = info_output.after (":");
		}
		else if (info_output.contains('[', 0))
		{
		    // ``stop at [file:line ...]''
		    myfile_name = info_output.before(":");
		    myfile_name = myfile_name.after('[');
		    info_output = info_output.after (":");
		}
		else
		{
		    // ``stop at LINE''
		    myfile_name = "";
		}

		if (info_output != "" && isdigit(info_output[0]))
		    myline_nr = get_positive_nr(info_output);
	    }
	    else if (info_output.contains ("in ", 0))
	    {
		// ``stop in FUNC''
		string func = info_output.after(RXblanks_or_tabs);
		func = func.before('\n');
		string pos = dbx_lookup(func);
		if (pos != "")
		{
		    myfile_name = pos.before(":");
		    string line_s = pos.after(":");
		    myline_nr   = get_positive_nr(line_s);
		}
	    }
	}
	info_output = info_output.after('\n');
    }
    }
}


bool BreakPoint::update (string& info_output)
{
    bool changed       = false;
    myenabled_changed  = false;
    myposition_changed = false;
    myfile_changed     = false;
    myaddress_changed  = false;

    int ret = read_positive_nr (info_output);
    if (ret < 0)
	return false;

    switch(gdb->type())
    {
    case GDB:
    {
	// "Type" pruefen (kann sich nicht aendern)
	read_leading_blanks (info_output);

	if (!info_output.contains("breakpoint", 0) &&
	    !info_output.contains("watchpoint", 0))
	    return false;
	info_output = info_output.after(RXblanks_or_tabs);

	// "Disp" lesen
	if (info_output.index ("dis") == 0) {
	    if (mydispo != BPDIS) {
		changed = true;
		mydispo = BPDIS;
	    }
	}
	else if (info_output.index ("del") == 0) {
	    if (mydispo != BPDEL) {
		changed = true;
		mydispo = BPDEL;
	    }
	}
	else if (info_output.index ("keep") == 0) {
	    if (mydispo != BPKEEP) {
		changed = true;
		mydispo = BPKEEP;
	    }
	}
	info_output = info_output.after(RXblanks_or_tabs);

	// "Enb" lesen
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
	info_output = info_output.after(RXblanks_or_tabs);

	string new_info = "";
	if (mytype == BREAKPOINT) {
	    string new_address = info_output.through(rxalphanum);

	    if (myaddress != new_address)
	    {
		changed = myaddress_changed = true;
		myaddress = new_address;
	    }

	    info_output = info_output.from (RXname_colon_int_nl);
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
	else if (mytype == WATCHPOINT) {

	    // "Adress" ueberlesen
	    info_output = info_output.after(RXblanks_or_tabs);

	    new_info = info_output.through ("\n");
	}

	if (info_output.length() > 0 && !isdigit(info_output[0]))
	{
	    // es folgen noch Informationen
	    int next_nl = info_output.index('\n');
	    if (next_nl == -1) {
		// dies war die letzte Breakpoint-Ausgabe
		new_info += info_output;
		info_output = "";
	    }
	    else {
		new_info += info_output.through(next_nl);
		info_output = info_output.after(next_nl);
	    }
	}

	if (new_info != myinfos) {
	    changed = true;
	    myinfos = new_info;
	}
    }
    break;

    case DBX:
    {
	// One may ask why a DBX breakpoint should change... :-)
	read_leading_blanks (info_output);
	if (info_output.contains ("stop ", 0)
	    || info_output.contains ("stopped ", 0))
	{
	    info_output = info_output.after(RXblanks_or_tabs);
	    read_leading_blanks (info_output);
	    if (info_output.contains ("at ", 0))
	    {
		info_output = info_output.after(RXblanks_or_tabs);
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
		string func = info_output.after(RXblanks_or_tabs);
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
			changed = myposition_changed = myfile_changed = true;
		    }

		    if (new_line_nr && new_line_nr != myline_nr)
		    {
			myline_nr = new_line_nr;
			changed = myposition_changed = true;
		    }
		}
	    }
	}
	info_output = info_output.after('\n');
    }
    }

    return changed;
}
