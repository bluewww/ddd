// $Id$
// Read variable values in string representation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de)
//        and Andreas Zeller (zeller@ips.cs.tu-bs.de)
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

static const char rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Funktionen zum Lesen von Variablenwerten in string-Darstellung
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "value-read.h"
#include <ctype.h>

#include "string-fun.h"
#include "assert.h"
#include "comm-manag.h"
#include "cook.h"


// ***************************************************************************
DispValueType determine_type (string value)
{
    read_leading_blanks (value);

    switch(gdb->type())
    {
    case GDB:
	break;

    case DBX:
	// DBX uses this representation for out-of-range Pascal/Modula-2
	// enumerations.
	if (value.contains("(scalar = ", 0))
	    return Simple;
    }

    // We use [^\001]* for matching everything, including '\n'
    static regex RXstr_or_cl_begin("\\({\n\\|record\n\\)[^\001]*", true);

    if (value.matches(RXstr_or_cl_begin))
	return StructOrClass;

    static regex 
	RXpointer_value("\\((.*) \\)?\\(0x[0-9a-f]+\\|(nil)\\)[^\001]*", true);
    if (value.matches(RXpointer_value))
	return Pointer;

    switch(gdb->type())
    {
    case GDB:
	if (value.contains('{', 0))
	    return Array;
	break;

    case DBX:
	if (value.contains('(', 0))
	    return Array;
	break;
    }

    return Simple;
}

// ***************************************************************************

static void read_token(const char *value, int& pos);

static void read_up_to(const char *value, int& pos, char delim)
{
    if (value[pos] == '\0')
	return;

    // Add opening delimiter
    pos++;

    // Read up to closing delimiter
    while (value[pos] && value[pos] != delim)
	read_token(value, pos);

    if (value[pos] == '\0')
	return;

    // Add closing delimiter
    pos++;

    return;
}

static void read_string(const char *value, int& pos, char delim)
{
    if (value[pos] == '\0')
	return;

    // Add opening delimiter
    pos++;

    // Read up to closing delimiter
    while (value[pos] != '\0')
    {
	if (value[pos] == '\\' && value[pos + 1] != '\0')
	    pos += 2;
	else if (value[pos] == delim)
	    break;
	else
	    pos++;
    }

    if (value[pos] == '\0')
	return;

    // Add closing delimiter
    pos++;

    return;
}


static void read_token(const char *value, int& pos)
{
    if (value[pos] == '\0')
	return;

    string token;
    
    switch (value[pos])
    {
    case '\\':
	if (value[pos + 1] != '\0')
	    pos += 2;
	else
	    pos++;
	break;

    case '(':
	read_up_to(value, pos, ')');
	break;

    case '[':
	read_up_to(value, pos, ']');
	break;

    case '{':
	read_up_to(value, pos, '}');
	break;

#if 0
    case '<':
	read_up_to(value, pos, '>');
	break;
#endif

    case '\"':
    case '\'':
	read_string(value, pos, value[pos]);
	break;

    default:
	if (isalnum(value[pos]))
	{
	    // Name or number
	    pos++;
	    while (isalnum(value[pos]))
		pos++;
	}
	else if (isspace(value[0]))
	{
	    // Whitespace
	    pos++;
	    while (isspace(value[pos]))
		pos++;
	}
	else
	{
	    // Single-character token
	    pos++;
	}
	break;
    }
}

string read_token(string& value)
{
    if (value == "")
	return "";

    int pos = 0;
    read_token(value, pos);
    string token = string(value, pos);
    value = value.from(pos);

    // clog << "read_token() = " << quote(token) << "\n";
    return token;
}

string read_simple_value(string& value)
{
    // Read values up to [)}],\n]

    string old_value = value;
    read_leading_blanks (value);

    string ret;
    while (value != "" 
	   && value[0] != '\n' 
	   && value[0] != ')'
	   && value[0] != ']'
	   && value[0] != '}'
	   && value[0] != ',')
    {
	ret += read_token(value);
    }

    // clog << "read_simple_value() = " << quote(ret) << "\n";
    return ret;
}

// ***************************************************************************
string read_pointer_value (string& value)
{
    return read_simple_value (value);
}

// ***************************************************************************
// Bei Misserfolg false
bool read_array_begin (string& value)
{
    read_leading_blanks (value);

    if (value.contains('{', 0))
	value = value.after(0);
    else if (value.contains('(', 0))
	value = value.after(0);
    else if (value.contains('[', 0))
	value = value.after(0);
    else if (value.contains("record", 0))
	value = value.after("record");
    else
	return false;

    return true;
}

// ***************************************************************************
bool read_array_next (string& value)
{
    bool following = false;

    if (value.contains('\n', 0))
	following = true;
    
    read_leading_blanks (value);

    if (value.contains(',', 0))
    {
	value = value.after(0);
	return value != "";	// More stuff follows
    }

    if (value.contains('{', 0)
	|| value.contains('(', 0)
	|| value.contains('[', 0)
	|| value.contains("record\n", 0))
    {
	// DBX on Solaris issues arrays of structs without special delimiter
	return true;
    }

    if (value.contains('}', 0)
	|| value.contains(')', 0)
        || value.contains(']', 0)
	|| value.contains("end\n", 0))
    {
	return false;		// Array is done.
    }

    return following;		// Anything else except `\n': array is done.
}

// ***************************************************************************
bool read_array_end (string& value)
{
    read_leading_blanks (value);

    if (value.contains("end\n", 0))
    {
	value = value.after("end");
	return false;		// Array is done.
    }

    if (value.contains('}', 0)
	|| value.contains(')', 0)
	|| value.contains(']', 0))
    {
	value = value.after(0);
	return false;		// Array is done.
    }

    return value != "";		// More stuff may follow.
}


// ***************************************************************************
// Bei Misserfolg false
bool read_str_or_cl_begin (string& value)
{
    return read_array_begin(value);
}

// ***************************************************************************
bool read_str_or_cl_next (string& value)
{
    return read_array_next(value);
}

// ***************************************************************************
bool read_str_or_cl_end (string& value)
{
    return read_array_end(value);
}

// ***************************************************************************
bool read_members_of_xy (string& value)
{
    static regex RXmembers_of_nl   ("members of .+: ?\n", true);

    read_leading_blanks (value);
    if (!(value.index (RXmembers_of_nl) == 0))
	return false;
    value = value.after (RXmembers_of_nl);
    return true;
}


// ***************************************************************************
// Bei Misserfolg ""
string read_member_name (string& value)
{
    // We use [^\001]* for matching everything, including '\n'
    static regex RXmember_name(".* = [^\001]*", true);

    read_leading_blanks (value);
    if (!value.matches(RXmember_name))
	return "";

    string member_name = value.before (" = ");
    value = value.after (" = ");

    read_leading_blanks (member_name);
    // union hat u.U. keinen Namen (Spezialfall)
    if (member_name == "")
	member_name = " ";
    return member_name;
}


// ***************************************************************************
// true, wenn name in spitzen Klammern (z.B.: "<>" oder "<Basis>") 
// 
bool is_BaseClass_name (const string& name)
{
    static regex RXbase_class_name ("<>\\|<[A-Za-z_][A-Za-z0-9_< >,]*>", true);
    return name.matches (RXbase_class_name);
}

// ***************************************************************************
// schneidet vom full_name die letzte Komponente ab
// d.h.: "blabla.<Basis>" wird zu "blabla"; "blabla-><Basis>" wird zu "*blabla"
// 
void cut_BaseClass_name (string& full_name)
{
    int index = full_name.index (".<", -1);
    if (index > 0) {
	full_name = full_name.before(index);
    }
    else if ((index = full_name.index ("-><", -1)) > 0) {
	full_name = full_name.before(index);
	full_name.prepend ("*");
    }
}
