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

#define LOG_CREATE_VALUES 0
#define LOG_UPDATE_VALUES 0

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
class StructOrClassDispValue {
public:
    DispValueArray members;
    int            member_count;

    StructOrClassDispValue()
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
// Flags
//-----------------------------------------------------------------------------

bool DispValue::expand_repeated_values = false;

//-----------------------------------------------------------------------------
// Function defs
//-----------------------------------------------------------------------------

// Constructor
DispValue::DispValue (DispValue* p, 
		      int d,
		      string& value,
		      const string& f_n, 
		      const string& p_n,
		      DispValueType given_type)
    : mytype(UnknownType), myparent(p), mydepth (d), myexpanded(true), 
      myfull_name(f_n), print_name(p_n), changed(false), myrepeats(1)
{
    v.simple = 0;
    init(value, given_type);

    // A new display is not changed, but initialized
    changed = false;
}

// Duplicator
DispValue::DispValue (const DispValue& dv)
    : mytype(dv.mytype), myparent(dv.myparent), mydepth(dv.mydepth),
      myexpanded(dv.myexpanded), myfull_name(dv.myfull_name),
      print_name(dv.print_name), changed(false), myrepeats(dv.myrepeats)
{
    switch (mytype)
    {
    case UnknownType:
    {
	v.simple = 0;
	break;
    }

    case Simple:
    case Text:
    {
	v.simple = new SimpleDispValue;
	v.simple->value = dv.v.simple->value;
	break;
    }

    case Pointer:
    {
	v.pointer = new PointerDispValue;
	v.pointer->value = dv.v.pointer->value;
	v.pointer->dereferenced = false;
	break;
    }

    case Array:
    {
	v.array = new ArrayDispValue;
	v.array->member_count = dv.v.array->member_count;
	for (int i = 0; i < v.array->member_count; i++)
	    v.array->members[i] = dv.v.array->members[i]->dup();
	v.array->align = dv.v.array->align;
	break;
    }

    case StructOrClass:
    case BaseClass:
    case Reference:
    case Sequence:
    case List:
    {
	v.str_or_cl = new StructOrClassDispValue;
	v.str_or_cl->member_count = dv.v.str_or_cl->member_count;
	for (int i = 0; i < v.str_or_cl->member_count; i++)
	    v.str_or_cl->members[i] = dv.v.str_or_cl->members[i]->dup();
	break;
    }
    }
}

DispValue *DispValue::dup() { return new DispValue(*this); }

// True if more sequence members are coming
bool DispValue::sequence_pending(const string& value) const
{
    string v = value;
    strip_leading_space(v);

    if (v != "" && depth() == 0)
	return true;		// Still more to read

    if (!is_delimited(value))
	return true;		// Not at delimiter - more stuff follows

    if (parent() != 0 && parent()->type() == StructOrClass)
    {
	// Check for invalid member name.  If we're not at a delimiter,
	// but cannot read the next member name, return true.
	if (is_ending(v) || v.contains(',', 0) || v.contains(';', 0))
	    return false;

	if (!read_str_or_cl_next(v))
	    return false;

	if (read_member_name(v) == "")
	    return true;
    }

    // Sequence is done
    return false;
}


// Initialization
void DispValue::init(string& value, DispValueType given_type)
{
#if LOG_CREATE_VALUES
    clog << "Building value from " << quote(value) << "\n";
#endif

    string initial_value = value;

    if (background(value.length()))
    {
	clear();

	mytype          = Simple;
	v.simple        = new SimpleDispValue;
	v.simple->value = "(Aborted)";
	return;
    }

    mytype = given_type;
    if (mytype == UnknownType && print_name == "")
	mytype = Text;
    if (mytype == UnknownType && is_user_command (print_name))
	mytype = List;
    if (mytype == UnknownType && is_BaseClass_name (print_name))
	mytype = BaseClass;
    if (mytype == UnknownType)
	mytype = determine_type(value);

    bool ignore_repeats = (myparent != 0 && myparent->type() == Array);

    switch (mytype) 
    {

    case Simple:
    {
	v.simple = new SimpleDispValue;
	v.simple->value = read_simple_value(value, depth(), ignore_repeats);
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(v.simple->value) << "\n";
#endif
	break;
    }

    case Text:
    {
	v.simple = new SimpleDispValue;
	v.simple->value = value;
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(v.simple->value) << "\n";
#endif
	break;
    }

    case Pointer:
    {
	v.pointer = new PointerDispValue;
	v.pointer->value = read_pointer_value (value, ignore_repeats);
	v.pointer->dereferenced = false;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(v.pointer->value) << "\n";
#endif
	// Hide vtable pointers.
	if (v.pointer->value.contains("virtual table")
	    || v.pointer->value.contains("vtable"))
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

	v.array = new ArrayDispValue;
	v.array->align = Vertical;
	v.array->member_count = 0;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << "\n";
#endif

	read_array_begin (value, myaddr);

	// Check for `vtable entries' prefix.
	string vtable_entries = read_vtable_entries(value);
	if (vtable_entries != "")
	{
	    v.array->members[v.array->member_count++] = 
		new DispValue (this, depth() + 1,
			       vtable_entries, 
			       myfull_name, myfull_name);
	}

	// Read the array elements.  Assume that the type is the
	// same across all elements.
	DispValueType member_type = UnknownType;
	if (!v.array->have_index_base)
	{
	    v.array->index_base = index_base(base, depth());
	    v.array->have_index_base = true;
	}
	int array_index = v.array->index_base;

	// The array has at least one element.  Otherwise, GDB
	// would treat it as a pointer.
	do {
	    string repeated_value = value;
	    string member_name = 
		gdb->index_expr("", itostring(array_index++));
	    DispValue *dv = 
		new DispValue(this, depth() + 1, value,
			      add_member_name(base, member_name), 
			      member_name, member_type);
	    member_type = dv->type();
	    v.array->members[v.array->member_count++] = dv;

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
			new DispValue(this, depth() + 1, val,
				      add_member_name(base, member_name),
				      member_name, member_type);
		    v.array->members[v.array->member_count++] = 
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
		init(value);
		return;
	    }
	} while (read_array_next (value) != 0);
	read_array_end (value);

	// Expand only if at top-level.
	myexpanded = (depth() == 0 || v.array->member_count <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " has " << v.array->member_count << " members\n";
#endif
	break;
    }

    case List:
	// Some DBXes issue the local variables via a frame line, just
	// like `set_date(d = 0x10003060, day_of_week = Sat, day = 24,
	// month = 12, year = 1994)'.  Make this more readable.
	munch_dump_line(value);

	// FALL THROUGH
    case StructOrClass:
    case BaseClass:
    {
	v.str_or_cl = new StructOrClassDispValue;
	v.str_or_cl->member_count = 0;
	
#if LOG_CREATE_VALUES
	clog << mytype << " " << quote(myfull_name) << "\n";
#endif
	string member_prefix = myfull_name;
	if (mytype == List)
	{
	    member_prefix = "";
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

	    read_str_or_cl_begin (value, myaddr);
	}

	bool more_values = true;
	string member_name = read_member_name (value);
	while (more_values)
	{
	    if (member_name == "")
	    {
		// Some struct stuff that is not a member
		string old_value = value;

		DispValue *dv = 
		    new DispValue(this, depth() + 1, value, myfull_name, "");

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

		if (!consume)
		{
		    // Discard the value just read
		    delete dv;
		}
		else
		{
		    v.str_or_cl->members[v.str_or_cl->member_count++] = dv;
		}

		more_values = read_str_or_cl_next (value);
	    }
	    else if (is_BaseClass_name (member_name) || member_name == " ")
	    {
		// Anonymous union
		v.str_or_cl->members[v.str_or_cl->member_count++] = 
		    new DispValue (this, depth() + 1, value, myfull_name,
				   member_name);
		more_values = read_str_or_cl_next (value);
		read_members_of_xy (value);
	    }
	    else
	    {
		string full_name;

		// If a member name contains `.', quote it.  This
		// happens with vtable pointers on Linux (`_vptr.').
		if (member_name.contains('.') && gdb->has_quotes())
		    full_name = member_prefix + quote(member_name, '\'');
		else
		    full_name = member_prefix + member_name;

		v.str_or_cl->members[v.str_or_cl->member_count++] = 
		    new DispValue (this, depth() + 1, value, 
				   full_name, member_name);
		more_values = read_str_or_cl_next (value);
	    }
	    if (more_values)
		member_name = read_member_name (value);

	    if (background(value.length()))
	    {
		init(value);
		return;
	    }
	}

	if (mytype == List && value != "")
	{
	    // Add remaining value as text
	    v.str_or_cl->members[v.str_or_cl->member_count++] = 
		new DispValue(this, depth() + 1, value, "", "");
	}
	else
	{
	    // Skip the remainder
	    read_str_or_cl_end(value);
	}

	// Expand only if at top-level.
	myexpanded = (depth() == 0 || v.array->member_count <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " "
	     << quote(myfull_name)
	     << " has " << v.str_or_cl->member_count << " members\n";
#endif

	break;
    }

    case Reference:
    {
	v.str_or_cl = new StructOrClassDispValue;
	myexpanded = true;
	v.str_or_cl->member_count = 2;

	int sep = value.index('@');
	sep = value.index(':', sep);

	string ref = value.before(sep);
	value = value.after(sep);

	v.str_or_cl->members[0] = 
	    new DispValue(this, depth() + 1, ref, 
			  gdb->address_expr(myfull_name), 
			  myfull_name, Pointer);

	v.str_or_cl->members[1] = 
	    new DispValue(this, depth() + 1, value,
			  myfull_name, myfull_name);

	if (background(value.length()))
	{
	    init(value);
	    return;
	}
	break;
    }

    case Sequence:
    case UnknownType:
	assert(0);
	abort();
    }

    if (parent() == 0 || parent()->type() != Sequence)
    {
	bool need_clear = true;
	while (sequence_pending(value))
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

		v.str_or_cl = new StructOrClassDispValue;
		v.str_or_cl->member_count = 0;
		need_clear = false;
	    }
	    
	    string old_value = value;

	    DispValue *dv = new DispValue(this, depth() + 1, value, 
					  myfull_name, myfull_name);

	    if (value == old_value)
	    {
		// Nothing consumed - stop here
		delete dv;
		break;
	    }
	    else if (dv->type() == Simple && dv->value() == "")
	    {
		// Empty value - ignore
		delete dv;
	    }
	    else
	    {
		v.str_or_cl->members[v.str_or_cl->member_count++] = dv;
	    }
	}

#if LOG_CREATE_VALUES
	if (!need_clear)
	{
	    clog << mytype << " "
		 << quote(myfull_name)
		 << " has " << v.str_or_cl->member_count << " members\n";
	}
#endif
    }

    background(value.length());
    changed = true;
}


// Destructor
DispValue::~DispValue ()
{
    clear();
}

// Destructor helper
void DispValue::clear()
{
    int i;
    switch (mytype) {
    case Simple:
    case Text:
	delete v.simple;
	break;
    case Pointer:
	delete v.pointer;
	break;
    case Array:
	for (i = 0; i < v.array->member_count; i++) {
	    delete v.array->members[i];
	}
	delete v.array;
	break;

    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	for (i = 0; i < v.str_or_cl->member_count; i++) {
	    delete v.str_or_cl->members[i];
	}
	delete v.str_or_cl;
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
	return v.pointer->dereferenced;

    case Simple:
    case Text:
    case Array:
    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
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
    case StructOrClass:
    case BaseClass:
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
	return v.simple->value;

    case Pointer:
	return v.pointer->value;

    case Array:
    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return "";

    case UnknownType:
	assert(0);
	abort();
    }

    return "";
}




// Return #children.  Only if type() == Array, StructOrClass, or BaseClass.
int DispValue::nchildren() const
{
    switch (mytype) {
    case Array:
	return v.array->member_count;

    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return v.str_or_cl->member_count;

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
// Only if type() == Array, StructOrClass, or BaseClass.
DispValue* DispValue::get_child (int i) const
{
    switch (mytype) {
    case Array:
	assert (i >= 0);
	assert (i < v.array->member_count);
	return v.array->members[i];

    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	assert (i >= 0);
	assert (i < v.str_or_cl->member_count);
	return v.str_or_cl->members[i];

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
	return v.array->align == Vertical;

    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
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
	return v.array->align == Horizontal;

    case Sequence:
    case List:
    case StructOrClass:
    case BaseClass:
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
	v.pointer->dereferenced = true;
}

// Align vertically.  Only if type() == Array.
void DispValue::align_vertical ()
{
    if (mytype == Array)
	v.array->align = Vertical;
}

// Align horizontally.  Only if type() == Array.
void DispValue::align_horizontal ()
{
    if (mytype == Array)
	v.array->align = Horizontal;
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

// Return baseclass name.  Only if type() == BaseClass.
bool DispValue::new_BaseClass_name (string name)
{
    if (mytype != BaseClass)
	return false;

    if (print_name != name)
    {
	print_name = name;
	changed = true;
	return true;
    }
    return false;
}

// Update values from VALUE.  Set WAS_CHANGED iff value changed; Set
// WAS_INITIALIZED iff type changed.  If GIVEN_TYPE is given, use
// GIVEN_TYPE as type instead of inferring it.
void DispValue::update(string& value, bool& was_changed, bool& was_initialized,
		       DispValueType given_type)
{
    if (changed)
    {
	was_changed = true;	// Changed from `changed' to `unchanged'
	changed     = false;
    }

    string init_value = value;

#if LOG_UPDATE_VALUES
    clog << "Updating " << mytype 
	 << " " << full_name() << " with " << quote(value) << "\n";
#endif

    DispValueType new_type = given_type;
    if (mytype == Sequence)
	new_type = Sequence;
    if (new_type == UnknownType && print_name == "")
	new_type = Text;
    if (new_type == UnknownType && is_user_command (print_name))
	new_type = List;
    if (new_type == UnknownType && is_BaseClass_name (print_name))
	new_type = BaseClass;
    if (new_type == UnknownType)
	new_type = determine_type(value);

    bool ignore_repeats = (myparent != 0 && myparent->type() == Array);

    if (mytype != new_type)
    {
	// Type changed -- re-initialize.  The most common cause for
	// this effect is an incomplete display output - due to
	// illegal references, for example.
#if LOG_UPDATE_VALUES
	clog << "Type changed from " << mytype << " to " << new_type << "\n";
#endif
	value = init_value;
	clear();
	init(value);
	was_initialized = was_changed = true;
	return;
    }

    switch (mytype) {
    case Simple:
    {
	string new_value = 
	    read_simple_value(value, depth(), ignore_repeats);
	if (v.simple->value != new_value) {
	    v.simple->value = new_value;
	    changed = was_changed = true;
	}
	break;
    }

    case Text:
    {
	if (v.simple->value != value) {
	    v.simple->value = value;
	    changed = was_changed = true;
	}
	break;
    }

    case Pointer:
    {
	string new_value = read_pointer_value (value, ignore_repeats);
	if (v.pointer->value != new_value) {
	    v.pointer->value = new_value;
	    changed = was_changed = true;
	}
	break;
    }

    case Array:
    {
	read_array_begin (value, myaddr);

	string vtable_entries = read_vtable_entries(value);
	int member_index = 0;
	bool size_changed = false;
	if (vtable_entries != "")
	{
	    v.array->members[member_index++]->update(vtable_entries, 
						     was_changed, 
						     was_initialized);
	    if (was_initialized)
		break;
	    if (background(value.length()))
		break;
	}

	if (!was_initialized)
	{
	    DispValueType member_type = UnknownType;
	    bool more_values = true;
	    while (more_values && member_index < v.array->member_count)
	    {
		string repeated_value = value;
		DispValue *member = v.array->members[member_index++];
		member->update(value, was_changed, was_initialized, 
			       member_type);

		if (was_initialized)
		    break;
		if (background(value.length()))
		    break;

		member_type = member->type();

		int repeats = read_repeats(value);

		if (expand_repeated_values)
		{
		    // Update each repeated value
		    while (--repeats > 0)
		    {
			string val = repeated_value;
			DispValue *member = v.array->members[member_index++];

			if (member->repeats() > 1)
			{
			    size_changed = true;
			    break;
			}

			member->update(val, was_changed, 
				       was_initialized, member_type);
			if (was_initialized)
			    break;
			if (background(value.length()))
			    break;
		    }
		}
		else
		{
		    // Check whether the repeat count has changed
		    if (repeats != member->repeats())
		    {
			size_changed = true;
			break;	// No way to update this
		    }
		}

		more_values = read_array_next (value);
	    }
	}

	if (was_initialized || size_changed || 
	    member_index != v.array->member_count)
	{
#if LOG_UPDATE_VALUES
	    clog << mytype << " changed";
	    if (member_index != v.array->member_count)
	    {
		clog << " (old size " << v.array->member_count 
		     << "!= new size " << member_index << ")";
	    }
	    clog << "\n";
#endif
	    // Array size changed -- re-initialize.  This may happen
	    // if the user sets a length on the number of array
	    // elements to be displayed.
	    value = init_value;
	    clear();
	    init(value);
	    was_initialized = was_changed = true;
	    return;
	}
	read_array_end (value);
	break;
    }

    case Sequence:
    {
	int i;
	for (i = 0; i < v.str_or_cl->member_count; i++)
	{
	    v.str_or_cl->members[i]->update(value, was_changed, 
					    was_initialized);
	    if (was_initialized)
		break;
	    if (background(value.length()))
		break;
	}

	break;
    }

    case List:
    case StructOrClass:
    case BaseClass:
    {
	if (mytype == List 
	    && v.str_or_cl->member_count == 1
	    && v.str_or_cl->members[0]->type() == Text
	    && v.str_or_cl->members[0]->value() != value)
	{
	    // Re-initialize single text.
	    init(value);
	    was_initialized = was_changed = true;
	    return;
	}

	if (mytype == List)
	    munch_dump_line (value);

	read_str_or_cl_begin (value, myaddr);
	int i;
	bool more_values = true;
	for (i = 0; more_values && i < v.str_or_cl->member_count; i++)
	{
	    string member_name = read_member_name (value);

	    if (member_name == "")
	    {
		if (v.str_or_cl->members[i]->full_name() == myfull_name)
		{
		    v.str_or_cl->members[i]->update(value, was_changed,
						    was_initialized);
		    if (was_initialized)
			break;
		    if (background(value.length()))
			break;
		    more_values = read_str_or_cl_next (value);
		}
		else
		{
		    // Ignore previously empty value
		}
	    }
	    else if (is_BaseClass_name (member_name))
	    {
		if (v.str_or_cl->members[i]->
		    new_BaseClass_name(member_name))
		    was_changed = true;
		v.str_or_cl->members[i]->update(value, was_changed,
						was_initialized);
		if (was_initialized)
		    break;
		if (!read_str_or_cl_next (value))
		    break;
		if (background(value.length()))
		    break;
		read_members_of_xy (value);
	    }
	    else
	    {
		if (member_name != v.str_or_cl->members[i]->name())
		    break;

		v.str_or_cl->members[i]->update(value, was_changed,
						was_initialized);
		if (was_initialized)
		    break;
		if (background(value.length()))
		    break;
		more_values = read_str_or_cl_next (value);
	    }
	}

	if (was_initialized 
	    || i != v.str_or_cl->member_count 
	    || more_values)
	{
#if LOG_UPDATE_VALUES
	    clog << mytype << " changed";
	    if (i != v.str_or_cl->member_count)
	    {
		clog << " (old size " << v.str_or_cl->member_count
		     << "!= new size " << i << ")";
	    }
	    if (more_values)
	    {
		clog << " (more values)";
	    }
	    clog << "\n";
#endif
	    // Member count or member name changed -- re-initialize.
	    // Really weird stuff.  Can this ever happen?
	    value = init_value;
	    clear();
	    init(value);
	    was_initialized = was_changed = true;
	    return;
	}
	read_str_or_cl_end (value);
    }
    break;

    case Reference:
    {
	string ref = value.before(':');
	value = value.after(':');

	v.str_or_cl->members[0]->update(ref, was_changed, was_initialized);
	if (!was_initialized)
	    v.str_or_cl->members[1]->update(value, 
					    was_changed, was_initialized);
	if (was_initialized)
	{
	    value = init_value;
	    clear();
	    init(value);
	    was_initialized = was_changed = true;
	    return;
	}
	break;
    }

    case UnknownType:
	assert(0);
	abort();
    }

#if LOG_UPDATE_VALUES
    clog << "Update done\n";
#endif
    background(value.length());
    return;
}


//-----------------------------------------------------------------------------
// Background processing
//-----------------------------------------------------------------------------

static bool nop(int) { return false; }

bool (*DispValue::background)(int processed) = nop;
