// $Id$
// Read variable values in string representation

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char value_read_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Read variable values in string representation
//-----------------------------------------------------------------------------

#include "value-read.h"
#include <ctype.h>

#include "string-fun.h"
#include "assert.h"
#include "comm-manag.h"
#include "cook.h"
#include "ddd.h"
#include "regexps.h"
#include "GDBAgent.h"
#include "PosBuffer.h"

#if !WITH_FAST_RX
static regex rxindex("[[]-?[0-9][0-9]*].*");
static regex rxvtable("[^\n]*<[^\n>]* v(irtual )?t(a)?bl(e)?>[^{},]*[{].*");
#endif

// Determine the type of VALUE.
DispValueType determine_type (string value)
{
    read_leading_blanks (value);
    strip_final_blanks (value);

    // DBX on DEC prepends `[N]' before array member N
    if (value.matches(rxindex))
	value = value.after(']');

    // References.
#if !WITH_FAST_RX
    static regex rxreference(
	"([(][^)]+[)] )? *@ *(0(0|x)[0-9a-f]+|[(]nil[)]) *:.*");
#endif
    if (value.matches(rxreference))
	return Reference;

    // Vtables.
    if (value.matches(rxvtable))
	return Array;

    // Scalars.
    switch(gdb->type())
    {
    case DBX:
	// DBX uses this representation for out-of-range Pascal/Modula-2
	// enumerations.
	if (value.contains("(scalar = ", 0))
	    return Simple;

    default:
	break;
    }

    // Structs.
    // XDB issues the struct address before each struct.
#if !WITH_FAST_RX
    static regex rxstr_or_cl_begin(
	"(0(0|x)[0-9a-f]+|[(]nil[)])? *"
	"([(]|[{]|record\n|RECORD\n|RECORD |OBJECT "
	"|struct|class|union).*");
    static regex rxstr_or_cl_end("([)]|[}]|end\n|END\n|end;|END;)");
#endif

    if (value.matches(rxstr_or_cl_begin))
    {
	// Check for empty struct.
	string v = value;
	read_str_or_cl_begin(v);
	if (v.contains(rxstr_or_cl_end, 0))
	    return Simple;

	// Check for leading keywords.
#if !WITH_FAST_RX
	static regex rxstruct_keyword_begin(
	    "(0(0|x)[0-9a-f]+|[(]nil[)])? *"
	    "(record\n|RECORD\n|RECORD |OBJECT "
	    "|struct|class|union).*");
#endif
	if (value.matches(rxstruct_keyword_begin))
	    return StructOrClass;

	// Check for leading braces.  DEC DBX uses `{' for arrays as
	// well as for structs; likewise, AIX DBX uses `(' for arrays
	// and structs.  To disambiguate between arrays and structs,
	// we check for some struct member -- that is, a ` = ' before
	// any other sub-struct or struct end.

	// Go behind leading `{' or '('
	int i = 0;
	while (i < int(value.length()) && value[i] != '(' && value[i] != '{')
	    i++;
	i++;

	// Check for further `{' or '('
	while (i < int(value.length()))
	{
	    switch(value[i++])
	    {
	    case '=':
		return StructOrClass;

	    case '(':
	    case '{':
	    case ')':
	    case '}':
		goto next;		// Something else, possibly an array
	    }
	}
    next: ;
    }

    // Pointers.

    // GDB prepends the exact pointer type enclosed in `(...)'.  If
    // the pointer type contains `(...)' itself (such as in pointers
    // to functions), GDB uses '{...}' instead (as in `{int ()} 0x2908
    // <main>').
#if !WITH_FAST_RX
    static regex rxpointer1_value("([(][^)]+[)] )?" RXADDRESS ".*");
#endif
    if (value.matches(rxpointer1_value))
	return Pointer;
#if !WITH_FAST_RX
    static regex rxpointer2_value("[{][^{}]+[}] " RXADDRESS_START ".*");
#endif
    if (value.matches(rxpointer2_value))
	return Pointer;

    // Arrays.
    if (value.contains('{', 0) /* && value.contains('}', -1) */)
	return Array;
    if (value.contains('[', 0) /* && value.contains(']', -1) */)
	return Array;
    if (value.contains('(', 0) /* && value.contains(')', -1) */)
	return Array;

    // Simple values.
    // Everything else failed - assume simple value.
    return Simple;
}


static void read_token(const char *value, int& pos);
static void read_leading_junk(string& value);
static void read_leading_comment(string& value);

// Read tokens up to character DELIM
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

// Read tokens up to word DELIM
static void read_up_to(const char *value, int& pos, char* delim)
{
    if (value[pos] == '\0')
	return;

    int len = strlen(delim);

    // Read up to closing delimiter
    while (value[pos] && strncmp(value + pos, delim, len))
	read_token(value, pos);

    if (value[pos] == '\0')
	return;

    // Add closing delimiter
    pos += len;

    return;
}

// Read a string enclosed in DELIM.
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


// Read a C-like token.
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

    case '\"':
    case '\'':
	read_string(value, pos, value[pos]);
	break;

    case '<':
	if (gdb->has_print_r_option())
	{
	    // David Kirwan <David-Kirwan@vertel.com> reports that SUN
	    // DBX 3.0 has trouble issuing templates - the closing `>'
	    // is missing, as in `TSL_map<unsigned = { ...'.  Hence,
	    // don't check for the closing `>'.
	    pos++;
	}
	else
	{
	    read_up_to(value, pos, '>');
	}
	break;

    default:
	if (isalnum(value[pos]))
	{
	    // Name or number
	    int start = pos;
	    pos++;
	    while (isalnum(value[pos]))
		pos++;

	    string name(value + start, pos - start);
	    if (name == "record")
		read_up_to(value, pos, "end");
	    else if (name == "RECORD")
		read_up_to(value, pos, "END");
	    else if (name == "OBJECT")
		read_up_to(value, pos, "END");
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

// Read next token from VALUE.
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

// Read a simple value from VALUE.
string read_simple_value(string& value, int depth)
{
    // Read values up to [)}],\n]

    string old_value = value;
    read_leading_junk (value);

    string ret;
    while (value != "" 
	   && value[0] != '\n'
	   && (depth == 0 || (value[0] != ')'
			      && value[0] != ']'
			      && value[0] != '}'
			      && value[0] != ','
			      && value[0] != ';')))
    {
	ret += read_token(value);
    }

    // clog << "read_simple_value() = " << quote(ret) << "\n";
    return ret;
}

// Read a pointer value.
string read_pointer_value (string& value)
{
    return read_simple_value(value, 1);
}

// Read the beginning of an array from VALUE.  Return false iff failure.
bool read_array_begin (string& value)
{
    read_leading_blanks (value);

    // GDB has a special format for vtables
    if (value.matches(rxvtable))
	value = value.from("{");

    // XDB prepends the address before each struct
    if (value.contains("0x", 0) || value.contains("00", 0))
	value = value.after(rxblanks_or_tabs);

    // DBX on DEC prepends `struct' or `class' before each struct;
    // XDB also appends the struct type name.
    if (value.contains("struct", 0) 
	|| value.contains("class", 0) 
	|| value.contains("union", 0))
	value = value.from('{');

    if (value.contains('{', 0))
	value = value.after(0);
    else if (value.contains('(', 0))
	value = value.after(0);
    else if (value.contains('[', 0))
	value = value.after(0);
    else if (value.contains("record", 0))
	value = value.after("record");
    else if (value.contains("RECORD", 0))
	value = value.after("RECORD");
    else if (value.contains("OBJECT", 0))
	value = value.after("OBJECT");
    else
	return false;

    read_leading_blanks (value);

    // DBX on DEC prepends `[N]' before array member N
    if (value.matches(rxindex))
	value = value.after(']');

    return true;
}

// Read next array element from VALUE.  Return false iff done.
bool read_array_next (string& value)
{
    bool following = false;

    if (value.contains('\n', 0))
	following = true;
    
    read_leading_junk (value);

    // DBX on DEC prepends `[N]' before array member N
    if (value.matches(rxindex))
	value = value.after(']');

    // XDB and M3GDB append `;' after each struct element; others use `,'
    if (value.contains(',', 0) || value.contains(';', 0))
    {
	value = value.after(0);
	read_leading_junk (value);
	return value != "" 
	    && !value.contains("END", 0)
	    && !value.contains('}', 0)
	    && !value.contains(')', 0)
	    && !value.contains(']', 0); // More stuff follows
    }

    if (value.contains('{', 0)
	|| value.contains('(', 0)
	|| value.contains('[', 0)
	|| value.contains("record\n", 0)
	|| value.contains("RECORD\n", 0)
	|| value.contains("OBJECT\n", 0))
    {
	// DBX on Solaris issues arrays of structs without special delimiter
	return true;
    }

    if (value.contains('}', 0)
	|| value.contains(')', 0)
        || value.contains(']', 0)
	|| value.contains("end\n", 0)
	|| value.contains("END\n", 0))
    {
	return false;		// Array is done.
    }

    return following;		// Anything else except `\n': array is done.
}

// Read end of array from VALUE.  Return false iff done.
bool read_array_end (string& value)
{
    read_leading_junk (value);

    if (value.contains("end\n", 0))
    {
	value = value.after("end");
	return false;		// Array is done.
    }

    if (value.contains("END\n", 0))
    {
	value = value.after("END");
	return false;		// Array is done.
    }

    if (value.contains("END;", 0))
    {
	value = value.after("END");
	return true;		// M3GDB: More stuff may follow.
    }

    if (value.contains('}', 0)
	|| value.contains(')', 0)
	|| value.contains(']', 0))
    {
	value = value.after(0);

	// XDB appends `;' after each struct
	if (value.contains(';', 0))
	    value = value.after(0);

	return false;		// Array is done.
    }

    return value != "";		// More stuff may follow.
}


// Read the beginning of a struct from VALUE.  Return false iff done.
bool read_str_or_cl_begin (string& value)
{
    return read_array_begin(value);
}

// Read next struct element from VALUE.  Return false iff done.
bool read_str_or_cl_next (string& value)
{
    return read_array_next(value);
}

// Read end of struct from VALUE.  Return false iff done.
bool read_str_or_cl_end (string& value)
{
    return read_array_end(value);
}

// Some DBXes issue the local variables via a frame line, just like
// `set_date(d = 0x10003060, day_of_week = Sat, day = 24, month = 12,
// year = 1994) LOCATION', where LOCATION is either `[FRAME]' (DEC
// DBX) or `, line N in FILE' (AIX DBX).  Make this more readable.
void munch_dump_line (string& value)
{
    if (gdb->type() == DBX)
    {
	string initial_line = value.before('\n');
	strip_final_blanks(initial_line);

#if !WITH_FAST_RX
	static regex rxdbxframe("[a-zA-Z_$][a-zA-Z_$0-9]*[(].*[)].*"
				"([[].*[]]|, line .*)");
#endif
	if (initial_line.matches(rxdbxframe))
	{
	    // Strip enclosing parentheses
	    initial_line = initial_line.after('(');
	    int index = initial_line.index(')', -1);
	    initial_line = initial_line.before(index);

	    // Place one arg per line
	    initial_line.gsub(", ", "\n");

	    value = initial_line + value.from('\n');
	}
    }
}

// Skip `members of SUBCLASS:' in VALUE.  Return false iff failure.
bool read_members_of_xy (string& value)
{
#if !WITH_FAST_RX
    static regex rxmembers_of_nl("members of [^\n]+: ?\n");
#endif

    read_leading_junk (value);
    if (value.index (rxmembers_of_nl) == 0)
    {
	value = value.after('\n');
	return true;
    }

    return false;
}

// Read member name; return "" upon error
string read_member_name (string& value)
{
    read_leading_junk (value);

    if (value.contains(')', 0)
	|| value.contains('}', 0)
	|| value.contains(']', 0)
	|| value.contains(',', 0))
    {
	// No value
	return "";
    }

    if (value.contains('=', 0))
    {
	// Missing member name: anonymous union or likewise
	value = value.after(0);
	read_leading_junk(value);
	return " ";
    }

    if (!value.contains(" = "))
    {
	// No value
	return "";
    }

    string member_name = value.before (" = ");
    value = value.after (" = ");

    if (!is_BaseClass_name(member_name))
    {
	// Work around buggy member names as generated by Centerline CC.
	// Reported by eggert@igd.fhg.de (Lars Eggert)
	if (member_name.contains("::"))
	    member_name = member_name.after("::");
	if (member_name.contains("("))
	    member_name = member_name.before("(");
    }

    read_leading_junk (member_name);

    if (member_name == "")
    {
	// Anonymous union
	return " ";
    }

    return member_name;
}

// Read vtable entries.  Return "" upon error.
string read_vtable_entries (string& value)
{
#if !WITH_FAST_RX
    static regex rxvtable_entries("[{][0-9][0-9]* vtable entries,.*");
#endif

    read_leading_blanks (value);
    if (!value.matches(rxvtable_entries))
	return "";

    string vtable_entries = value.through("entries");
    value = value.after(',');

    return vtable_entries;
}


// Return true iff NAME is a baseclass name (enclosed in <>).
bool is_BaseClass_name (const string& name)
{
    return name.length() >= 2
	&& name[0] == '<' && name[name.length() - 1] == '>';
}

// Remove baseclass names. 
// "foo.<Base>" becomes "foo"; "blabla-><Base>" becomes "*foo"
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

// Skip blanks, M3 comments, and GDB warnings
static void read_leading_junk (string& value)
{
#if !WITH_FAST_RX
    static regex rxm3comment("\\(\\*.*\\*\\).*");
#endif
  
    read_leading_blanks(value);
  
    while (value.matches(rxm3comment))
    {
	read_leading_comment(value);
	read_leading_blanks(value);
    }

    while (value.contains("warning: ", 0))
    {
	value = value.after('\n');
	read_leading_blanks(value);
    }
}

// Read leading M2/M3 comment.
static void read_leading_comment (string& value)
{
    int i = 2;
    bool sf = false;

    while (i < int(value.length()))
    {
	switch (value[i])
	{
	case '*': 
	    sf = true;
	    break;

	case ')': 
	    if (sf)
	    {
		value = value.from(i + 1);
		return;
	    }

	default: 
	    sf = false; 
	    break;
	}
	i++;
    }

    value = value.from(i);
}

//-----------------------------------------------------------------------------
// Debugging stuff
//-----------------------------------------------------------------------------

ostream& operator<<(ostream& os, DispValueType type)
{
    switch (type)
    {
    case Simple:        os << "Simple";        break;
    case Pointer:       os << "Pointer";       break;
    case Array:         os << "Array";         break;
    case StructOrClass:	os << "StructOrClass"; break;
    case BaseClass:     os << "BaseClass";     break;
    case Reference:     os << "Reference";     break;
    case List:   	os << "List";	       break;
    case Text:          os << "Text";	       break;
    }
    
    return os;
}
