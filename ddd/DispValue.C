// $Id$
// Read and store type and value of a displayed expression

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

char DispValue_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#ifndef LOG_CREATE_VALUES
#define LOG_CREATE_VALUES 0
#endif

//-----------------------------------------------------------------------------
// A `DispValue' maintains type and value of a displayed expression
//-----------------------------------------------------------------------------

#include "DispValue.h"
#include "DispValueA.h"
#include "string-fun.h"
#include "DynArray.h"
#include "assert.h"
#include "cook.h"
#include "GDBAgent.h"
#include "ddd.h"
#include "question.h"
#include "misc.h"
#include "DispNode.h"
#include "regexps.h"
#include "value-read.h"

#include <ctype.h>

//-----------------------------------------------------------------------------
// Type decls
//-----------------------------------------------------------------------------

enum Alignment {Vertical, Horizontal};

// Simple value
class SimpleDispValue {
public:
    string value;

    SimpleDispValue()
	: value()
    {}
};

// Pointer
class PointerDispValue {
public:
    string  value;
    bool dereferenced;		// True iff dereferenced

    PointerDispValue()
	: value(), dereferenced(false)
    {}
};

// Array
class ArrayDispValue {
public:
    DispValueArray members;
    int            member_count;
    int            index_base;
    bool           have_index_base;
    Alignment      align;

    ArrayDispValue()
	: members(), member_count(0),
	  index_base(-1), have_index_base(false),
	  align(Horizontal)
    {}
};

// Struct or class
class StructDispValue {
public:
    DispValueArray members;
    int            member_count;

    StructDispValue()
	: members(), member_count(0)
    {}
};


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

StringStringAssoc DispValue::type_cache;

// Get index base of expr EXPR in dimension DIM
int DispValue::index_base(const string& expr, int dim)
{
    if (gdb->program_language() != LANGUAGE_FORTRAN)
	return gdb->default_index_base();

    string base = expr;
    if (base.contains('('))
	base = base.before('(');
    if (!type_cache.has(base))
	type_cache[base] = gdb_question(gdb->whatis_command(base));
    string type = type_cache[base];

    // GDB issues array information as `type = real*8 (0:9,2:12)'.
    // However, the first dimension in the type output comes last in
    // the printed array.
    int colon = type.length();
    while (colon >= 0 && dim-- >= 0)
	colon = type.index(':', colon - type.length() - 1);
    if (colon < 0)
	return  gdb->default_index_base(); // Not found

    while (colon >= 0 && isdigit(type[colon - 1]))
	colon--;

    return atoi((char *)type + colon);
}

// In FORTRAN mode, GDB issues last dimensions first.  Insert new
// dimension before first dimension and convert to FORTRAN
// multi-dimension syntax.
string DispValue::add_member_name(const string& base, 
				  const string& member_name)
{
    if (gdb->program_language() == LANGUAGE_FORTRAN && 
	member_name.contains('(', 0) &&	base.contains('('))
    {
	return base.before('(') + member_name.before(')') + ", " + 
	    base.after('(');
    }
    else
    {
	return base + member_name;
    }
}

void DispValue::clear_type_cache()
{
    static StringStringAssoc empty;
    type_cache = empty;
}

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

bool DispValue::expand_repeated_values = false;
DispValue *(*DispValue::value_hook)(string& value) = 0;

//-----------------------------------------------------------------------------
// Function defs
//-----------------------------------------------------------------------------

// Constructor
DispValue::DispValue (DispValue* parent, 
		      int depth,
		      string& value,
		      const string& f_n, 
		      const string& p_n,
		      DispValueType given_type)
    : mytype(UnknownType), myexpanded(true), 
      myfull_name(f_n), print_name(p_n), changed(false), myrepeats(1),
      _links(1)
{
    simple = 0;
    init(parent, depth, value, given_type);

    // A new display is not changed, but initialized
    changed = false;
}

// Duplicator
DispValue::DispValue (const DispValue& dv)
    : mytype(dv.mytype), 
      myexpanded(dv.myexpanded), myfull_name(dv.myfull_name),
      print_name(dv.print_name), myaddr(dv.myaddr),
      changed(false), myrepeats(dv.myrepeats), _links(1)
{
    switch (mytype)
    {
    case UnknownType:
    {
	simple = 0;
	break;
    }

    case Simple:
    case Text:
    {
	simple = new SimpleDispValue;
	simple->value = dv.simple->value;
	break;
    }

    case Pointer:
    {
	pointer = new PointerDispValue;
	pointer->value = dv.pointer->value;
	pointer->dereferenced = false;
	break;
    }

    case Array:
    {
	array = new ArrayDispValue;
	array->member_count = dv.array->member_count;
	for (int i = 0; i < array->member_count; i++)
	{
	    array->members[i] = dv.array->members[i]->dup();
	}
	array->index_base = dv.array->index_base;
	array->have_index_base = dv.array->have_index_base;
	array->align = dv.array->align;
	break;
    }

    case Struct:
    case Reference:
    case Sequence:
    case List:
    {
	str = new StructDispValue;
	str->member_count = dv.str->member_count;
	for (int i = 0; i < str->member_count; i++)
	{
	    str->members[i] = dv.str->members[i]->dup();
	}
	break;
    }
    }
}

// True if more sequence members are coming
bool DispValue::sequence_pending(const string& value, 
				 const DispValue *parent) const
{
    if (parent != 0)
    {
	switch (parent->type())
	{
	case Sequence:
	case List:
	case Struct:
	case Reference:
	case Array:
	    // In a composite, we always read everything up to the
	    // final delimiter.
	    return false;

	case Simple:
	case Pointer:
	case Text:
	case UnknownType:
	    break;
	}
    }

    string v = value;
    strip_leading_space(v);

    if (v != "" && parent == 0)
	return true;		// Still more to read

    if (!is_delimited(value))
	return true;		// Not at delimiter - more stuff follows

    // Sequence is done
    return false;
}

// Parsing
DispValue *DispValue::parse(DispValue *parent, 
			    int        depth,
			    string&    value,
			    const string& full_name, 
			    const string& print_name,
			    DispValueType type)
{
    if (value_hook != 0)
    {
	DispValue *dv = (*value_hook)(value);
	if (dv != 0)
	{
	    // Just take values from given element
#if LOG_CREATE_VALUES
	    clog << "External value " << quote(dv->full_name()) << "\n";
#endif
	    return dv;
	}
    }

    return new DispValue(parent, depth, value, full_name, print_name, type);
}

// Initialization
void DispValue::init(DispValue *parent, int depth, string& value,
		     DispValueType given_type)
{
#if LOG_CREATE_VALUES
    clog << "Building value from " << quote(value) << "\n";
#endif

    string initial_value = value;

    if (background(value.length()))
    {
	clear();

	mytype        = Simple;
	simple        = new SimpleDispValue;
	simple->value = "(Aborted)";
	return;
    }

    mytype = given_type;
    if (mytype == UnknownType && 
	(parent == 0 || parent->type() == List) && print_name == "")
	mytype = Text;
    if (mytype == UnknownType && parent == 0 && is_user_command(print_name))
	mytype = List;
    if (mytype == UnknownType)
	mytype = determine_type(value);

    bool ignore_repeats = (parent != 0 && parent->type() == Array);

    switch (mytype) 
    {

    case Simple:
    {
	simple = new SimpleDispValue;
	simple->value = read_simple_value(value, depth, ignore_repeats);
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(simple->value) << "\n";
#endif
	break;
    }

    case Text:
    {
	// Read in entire text
	simple = new SimpleDispValue;
	simple->value = value;
	value = "";
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(simple->value) << "\n";
#endif
	break;
    }

    case Pointer:
    {
	pointer = new PointerDispValue;
	pointer->value = read_pointer_value (value, ignore_repeats);
	pointer->dereferenced = false;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(pointer->value) << "\n";
#endif
	// Hide vtable pointers.
	if (pointer->value.contains("virtual table")
	    || pointer->value.contains("vtable"))
	    myexpanded = false;
	break;
    }

    case Array:
    {
#if RUNTIME_REGEX
	static regex rxsimple("([][a-zA-Z0-9_$().]|->)*");
#endif

	string base = myfull_name;
	if (!base.matches(rxsimple))
	    base = "(" + base + ")";

	array = new ArrayDispValue;
	array->align = Vertical;
	array->member_count = 0;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << "\n";
#endif

	read_array_begin(value, myaddr);

	// Check for `vtable entries' prefix.
	string vtable_entries = read_vtable_entries(value);
	if (vtable_entries != "")
	{
	    array->members[array->member_count++] = 
		parse_child(depth, vtable_entries, myfull_name);
	}

	// Read the array elements.  Assume that the type is the
	// same across all elements.
	DispValueType member_type = UnknownType;
	if (!array->have_index_base)
	{
	    array->index_base = index_base(base, depth);
	    array->have_index_base = true;
	}
	int array_index = array->index_base;

	// The array has at least one element.  Otherwise, GDB
	// would treat it as a pointer.
	do {
	    string repeated_value = value;
	    string member_name = 
		gdb->index_expr("", itostring(array_index++));
	    DispValue *dv = parse_child(depth, value,
					add_member_name(base, member_name), 
					member_name, member_type);
	    member_type = dv->type();
	    array->members[array->member_count++] = dv;

	    int repeats = read_repeats(value);

	    if (expand_repeated_values)
	    {
		// Create one value per repeat
		while (--repeats > 0)
		{
		    member_name = 
			gdb->index_expr("", itostring(array_index++));
		    string val = repeated_value;
		    DispValue *repeated_dv = 
			parse_child(depth, val, 
				    add_member_name(base, member_name),
				    member_name, member_type);
		    array->members[array->member_count++] = 
			repeated_dv;
		}
	    }
	    else
	    {
		// Show repetition in member
		if (repeats > 1)
		{
		    array_index--;

#if 0
		    // We use the GDB `artificial array' notation here,
		    // since repeat recognition is supported in GDB only.
		    member_name += "@" + itostring(repeats);

		    dv->full_name() = add_member_name(base, member_name);
		    dv->name()      = member_name;
#endif
		    dv->repeats()   = repeats;

		    array_index += repeats;
		}
	    }

	    if (background(value.length()))
	    {
		init(parent, depth, value);
		return;
	    }
	} while (read_array_next (value) != 0);
	read_array_end (value);

	// Expand only if at top-level.
	myexpanded = (depth == 0 || array->member_count <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " has " << array->member_count << " members\n";
#endif
	break;
    }

    case List:
	// Some DBXes issue the local variables via a frame line, just
	// like `set_date(d = 0x10003060, day_of_week = Sat, day = 24,
	// month = 12, year = 1994)'.  Make this more readable.
	munch_dump_line(value);

	// FALL THROUGH
    case Struct:
    {
	str = new StructDispValue;
	str->member_count = 0;
	bool found_struct_begin   = false;
	bool read_multiple_values = false;
	
#if LOG_CREATE_VALUES
	clog << mytype << " " << quote(myfull_name) << "\n";
#endif
	string member_prefix = myfull_name;
	if (mytype == List)
	{
	    member_prefix = "";
	    read_multiple_values = true;
	}
	else
	{
	    // In C and Java, `*' binds tighter than `.'
	    if (member_prefix.contains('*', 0))
	    {
		if (gdb->program_language() == LANGUAGE_C)
		{
		    // Use the C `->' operator instead
		    member_prefix.del("*");
#if RUNTIME_REGEX
		    static regex rxchain("[-a-zA-Z0-9::_>.]+");
#endif
		    if (member_prefix.matches(rxchain))
		    {
			// Simple chain of identifiers - prepend `->'
			member_prefix += "->";
		    }
		    else
		    {
			member_prefix.prepend("(");
			member_prefix += ")->";
		    }
		}
		else
		{
		    member_prefix.prepend("(");
		    member_prefix += ").";
		}
	    }
	    else
	    {
		member_prefix += ".";
	    }

	    // In case we do not find a struct beginning, read only one value
	    found_struct_begin = read_struct_begin(value, myaddr);
	    read_multiple_values = found_struct_begin;
	}

	bool more_values = true;
	while (more_values)
	{
	    string member_name = read_member_name(value);

	    if (member_name == "")
	    {
		// Some struct stuff that is not a member
		string old_value = value;

		DispValue *dv = parse_child(depth, value, myfull_name, "");

		bool consume = true;
		if (value == old_value)
		{
		    // Nothing consumed - stop here
		    consume = false;
		}
		else if (dv->type() == Simple && dv->value() == "")
		{
		    // Empty value - stop here
		    consume = false;
		}
		else if (dv->type() == Struct)
		{
		    // What's this - a struct within a struct?  Just
		    // adopt the members.
		    // (This happens when we finally found the struct
		    // after having read all the AIX DBX base classes.)
		    for (int i = 0; i < dv->nchildren(); i++)
		    {
			DispValue *dv2 = dv->get_child(i)->link();
			str->members[str->member_count++] = dv2;
		    }
		    consume = false;
		}

		if (!consume)
		{
		    // Discard the value just read
		    dv->unlink();
		}
		else
		{
		    str->members[str->member_count++] = dv;
		}

		more_values = read_multiple_values && read_struct_next(value);
	    }
	    else if (is_BaseClass_name(member_name))
	    {
		// Base class member
		DispValue *dv = 
		    parse_child(depth, value, myfull_name, member_name);
		str->members[str->member_count++] = dv;

		more_values = read_multiple_values && read_struct_next(value);

		// Skip a possible `members of CLASS:' prefix
		read_members_prefix(value);

		// AIX DBX does not place a separator between base
		// classes and the other members, so we always
		// continue reading after having found a base
		// class.  After all, the own class members are
		// still missing.
		if (mytype == Struct && !found_struct_begin)
		    more_values = true;
	    }
	    else
	    {
		// Ordinary member
		string full_name;

		if (member_name == " ")
		{
		    // Anonymous union
		    full_name = myfull_name;
		}
		else if (member_name.contains('.') && gdb->has_quotes())
		{
		    // The member name contains `.' => quote it.  This
		    // happens with vtable pointers on Linux (`_vptr.').
		    full_name = member_prefix + quote(member_name, '\'');
		}
		else
		{
		    // Ordinary member
		    full_name = member_prefix + member_name;
		}

		str->members[str->member_count++] = 
		    parse_child(depth, value, full_name, member_name);
		more_values = read_multiple_values && read_struct_next(value);
	    }

	    if (background(value.length()))
	    {
		init(parent, depth, value);
		return;
	    }
	}

	if (mytype == List && value != "")
	{
	    // Add remaining value as text
	    str->members[str->member_count++] = parse_child(depth, value, "");
	}

	if (found_struct_begin)
	{
	    // Skip the remainder
	    read_struct_end(value);
	}

	// Expand only if at top-level.
	myexpanded = (depth == 0 || str->member_count <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " "
	     << quote(myfull_name)
	     << " has " << str->member_count << " members\n";
#endif

	break;
    }

    case Reference:
    {
	str = new StructDispValue;
	myexpanded = true;
	str->member_count = 2;

	int sep = value.index('@');
	sep = value.index(':', sep);

	string ref = value.before(sep);
	value = value.after(sep);

	string addr = gdb->address_expr(myfull_name);

	str->members[0] = parse_child(depth, ref, addr, myfull_name, Pointer);
	str->members[1] = parse_child(depth, value, myfull_name);

	if (background(value.length()))
	{
	    init(parent, depth, value);
	    return;
	}
	break;
    }

    case Sequence:
    case UnknownType:
	assert(0);
	abort();
    }

    // Handle trailing stuff (`sequences')
    if (parent == 0 || parent->type() != Sequence)
    {
	bool need_clear = true;
	while (sequence_pending(value, parent))
	{
	    if (need_clear)
	    {
#if LOG_CREATE_VALUES
		clog << "Sequence detected at " << quote(value) << "\n";
#endif

		clear();
		value = initial_value;

		mytype = Sequence;

#if LOG_CREATE_VALUES
		clog << mytype << " " << quote(myfull_name) << "\n";
#endif

		str = new StructDispValue;
		str->member_count = 0;
		need_clear = false;
	    }
	    
	    string old_value = value;

	    DispValue *dv = parse_child(depth, value, myfull_name);

	    if (value == old_value)
	    {
		// Nothing consumed - stop here
		dv->unlink();
		break;
	    }
	    else if (dv->type() == Simple && dv->value() == "")
	    {
		// Empty value - ignore
		dv->unlink();
	    }
	    else
	    {
		str->members[str->member_count++] = dv;
	    }
	}

#if LOG_CREATE_VALUES
	if (!need_clear)
	{
	    clog << mytype << " "
		 << quote(myfull_name)
		 << " has " << str->member_count << " members\n";
	}
#endif
    }

    background(value.length());
    changed = true;
}

// Destructor helper
void DispValue::clear()
{
    int i;
    switch (mytype) {
    case Simple:
    case Text:
	delete simple;
	break;
    case Pointer:
	delete pointer;
	break;
    case Array:
	for (i = 0; i < array->member_count; i++) {
	    array->members[i]->unlink();
	}
	delete array;
	break;

    case Sequence:
    case List:
    case Struct:
    case Reference:
	for (i = 0; i < str->member_count; i++) {
	    str->members[i]->unlink();
	}
	delete str;
	break;

    case UnknownType:
	break;
    }
}

//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------

// Return True iff dereferenced.  Only if type() == Pointer.
bool DispValue::dereferenced() const
{
    switch (mytype)
    {
    case Pointer:
	return pointer->dereferenced;

    case Simple:
    case Text:
    case Array:
    case Sequence:
    case List:
    case Struct:
    case Reference:
	return false;

    case UnknownType:
	assert(0);
	abort();
    }

    return false;
}


// Return dereferenced name.  Only if type() == Pointer.
string DispValue::dereferenced_name() const
{
    switch (mytype)
    {
    case Pointer:
	{
	    string f = full_name();
	    if (f.contains('/', 0))
		f = f.from(2);	// Skip /FMT expressions

	    return gdb->dereferenced_expr(f);
	}

    case Simple:
    case Text:
    case Array:
    case Sequence:
    case List:
    case Struct:
    case Reference:
	return "";

    case UnknownType:
	assert(0);
	abort();
    }

    return "";
}



// Return value.  Only if type() == Simple, Text, or Pointer.
string DispValue::value() const
{
    switch (mytype) {
    case Simple:
    case Text:
	return simple->value;

    case Pointer:
	return pointer->value;

    case Array:
    case Sequence:
    case List:
    case Struct:
    case Reference:
	return "";

    case UnknownType:
	assert(0);
	abort();
    }

    return "";
}




// Return #children.  Only if type() == Array or Struct
int DispValue::nchildren() const
{
    switch (mytype) {
    case Array:
	return array->member_count;

    case Sequence:
    case List:
    case Struct:
    case Reference:
	return str->member_count;

    case Simple:
    case Text:
    case Pointer:
	return 0;

    case UnknownType:
	assert(0);
	abort();
    }

    return 0;
}


// Get child #i (0: first child).
// Only if type() == Array or Struct
DispValue* DispValue::get_child (int i) const
{
    switch (mytype) {
    case Array:
	assert (i >= 0);
	assert (i < array->member_count);
	return array->members[i];

    case Sequence:
    case List:
    case Struct:
    case Reference:
	assert (i >= 0);
	assert (i < str->member_count);
	return str->members[i];

    case Pointer:
    case Simple:
    case Text:
	return 0;

    case UnknownType:
	assert(0);
	abort();
    }

    return 0;
}



// Check if vertically aligned.  Only if type() == Array.
bool DispValue::vertical_aligned()   const
{
    switch (mytype) {
    case Array:
	return array->align == Vertical;

    case Sequence:
    case List:
    case Struct:
    case Reference:
    case Pointer:
    case Simple:
    case Text:
	return false;

    case UnknownType:
	assert(0);
	abort();
    }

    return false;
}

// Check if horizontally aligned.  Only if type() == Array.
bool DispValue::horizontal_aligned() const
{
    switch (mytype) {
    case Array:
	return array->align == Horizontal;

    case Sequence:
    case List:
    case Struct:
    case Reference:
    case Pointer:
    case Simple:
    case Text:
	return false;

    case UnknownType:
	assert(0);
	abort();
    }

    return false;
}



//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

// Mark as dereferenced.  Only if type() == Pointer.
void DispValue::dereference()
{
    if (mytype == Pointer)
	pointer->dereferenced = true;
}

// Align vertically.  Only if type() == Array.
void DispValue::align_vertical ()
{
    if (mytype == Array)
	array->align = Vertical;
}

// Align horizontally.  Only if type() == Array.
void DispValue::align_horizontal ()
{
    if (mytype == Array)
	array->align = Horizontal;
}


// Expand.  Like expand(), but expand entire subtree
void DispValue::expandAll(int depth)
{
    if (depth == 0)
	return;

    _expand();

    for (int i = 0; i < nchildren(); i++)
    {
	DispValue *child = get_child(i);
	child->expandAll(depth - 1);
    }
}

// Collapse.  Like collapse(), but collapse entire subtree
void DispValue::collapseAll(int depth)
{
    if (depth == 0)
	return;

    _collapse();

    for (int i = 0; i < nchildren(); i++)
    {
	DispValue *child = get_child(i);
	child->collapseAll(depth - 1);
    }
}

// Count expanded nodes in tree
int DispValue::expandedAll() const
{
    int count = 0;
    if (expanded())
	count++;
    for (int i = 0; i < nchildren(); i++)
	count += get_child(i)->expandedAll();

    return count;
}

// Count collapsed nodes in tree
int DispValue::collapsedAll() const
{
    int count = 0;
    if (collapsed())
	count++;
    for (int i = 0; i < nchildren(); i++)
	count += get_child(i)->collapsedAll();

    return count;
}


// Return height of entire tree
int DispValue::height() const
{
    int d = 0;

    for (int i = 0; i < nchildren(); i++)
	d = max(d, get_child(i)->height());

    return d + 1;
}

// Return height of expanded tree
int DispValue::heightExpanded() const
{
    if (collapsed())
	return 0;

    int d = 0;

    for (int i = 0; i < nchildren(); i++)
    {
	if (get_child(i)->collapsed())
	    return 1;

	d = max(d, get_child(i)->heightExpanded());
    }

    return d + 1;
}


//-----------------------------------------------------------------------------
// Update values
//-----------------------------------------------------------------------------

// Update values from VALUE.  Set WAS_CHANGED iff value changed; Set
// WAS_INITIALIZED iff type changed.  If TYPE is given, use TYPE as
// type instead of inferring it.  Note: THIS can no more be referenced
// after calling this function; use the returned value instead.
DispValue *DispValue::update(string& value, 
			     bool& was_changed, bool& was_initialized,
			     DispValueType given_type)
{
    DispValue *source = parse(0, 0, value, 
			      full_name(), name(), given_type);
    return update(source, was_changed, was_initialized);
}


// Update values from SOURCE.  Set WAS_CHANGED iff value changed; Set
// WAS_INITIALIZED iff type changed.  Note: Neither THIS nor SOURCE
// can be referenced after calling this function; use the returned
// value instead.
DispValue *DispValue::update(DispValue *source, 
			     bool& was_changed, bool& was_initialized)
{
    if (changed)
    {
	// Clear `changed' flag
	changed = false;
	was_changed = true;
    }

    if (source->type() == type())
    {
	switch (type())
	{
	case Simple:
	case Text:
	    if (simple->value != source->simple->value)
	    {
		simple->value = source->simple->value;
		changed = was_changed = true;
	    }
	    delete source;
	    return this;

	case Pointer:
	    if (pointer->value != source->pointer->value)
	    {
		pointer->value = source->pointer->value;
		changed = was_changed = true;
	    }
	    delete source;
	    return this;
		
	case Array:
	    if (array->member_count == source->array->member_count &&
		array->have_index_base == source->array->have_index_base &&
		(!array->have_index_base || 
		 array->index_base == source->array->index_base))
	    {
		for (int i = 0; i < array->member_count; i++)
		{
		    array->members[i] = 
			array->members[i]->update(source->array->members[i],
						  was_changed,
						  was_initialized);
		}
		source->array->member_count = 0;
		delete source;
		return this;
	    }
	    break;

	case List:
	case Struct:
	case Sequence:
	case Reference:
	    if (str->member_count == source->str->member_count)
	    {
		for (int i = 0; i < str->member_count; i++)
		{
		    str->members[i] = 
			str->members[i]->update(source->str->members[i],
						was_changed,
						was_initialized);
		}
		source->str->member_count = 0;
		delete source;
		return this;
	    }
	    break;

	case UnknownType:
	    assert(0);
	    abort();
	}
    }

    // Type or structure have changed -- use SOURCE instead of original
    unlink();

    source->changed = was_changed = was_initialized = true;
    return source;
}


//-----------------------------------------------------------------------------
// Find descendant
//-----------------------------------------------------------------------------

// Return true iff SOURCE and this are structurally equal.
// If SOURCE_DESCENDANT (a descendant of SOURCE) is set, 
// return its equivalent descendant of this in DESCENDANT.
bool DispValue::structurally_equal(const DispValue *source,
				   const DispValue *source_descendant,
				   const DispValue *&descendant) const
{
    if (source == source_descendant)
	descendant = this;

    if (type() != source->type())
	return false;		// Differing type

    switch (type())
    {
	case Simple:
	case Text:
	case Pointer:
	    return true;	// Structurally equal
		
	case Array:
	{
	    if (array->member_count != source->array->member_count)
		return false;	// Differing size

	    if (array->have_index_base != source->array->have_index_base)
		return false;	// Differing base

	    if (array->have_index_base && 
		array->index_base != source->array->index_base)
		return false;	// Differing base

	    for (int i = 0; i < array->member_count; i++)
	    {
		DispValue *child = array->members[i];
		DispValue *source_child = source->array->members[i];
		bool eq = child->structurally_equal(source_child, 
						    source_descendant,
						    descendant);

		if (!eq)
		    return false;
	    }
	    return true;	// All children structurally equal
	}

	case List:
	case Struct:
	case Sequence:
	case Reference:
	{
	    if (str->member_count != source->str->member_count)
		return false;

	    for (int i = 0; i < array->member_count; i++)
	    {
		DispValue *child = str->members[i];
		DispValue *source_child = source->str->members[i];
		bool eq = child->structurally_equal(source_child, 
						    source_descendant,
						    descendant);

		if (!eq)
		    return false;
	    }
	    return true;	// All children structurally equal
	}

	case UnknownType:
	    assert(0);
	    abort();
    }

    return false;		// Not found
}

//-----------------------------------------------------------------------------
// Background processing
//-----------------------------------------------------------------------------

static bool nop(int) { return false; }

bool (*DispValue::background)(int processed) = nop;
