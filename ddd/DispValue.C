// $Id$
// Read and store type and value of a displayed expression

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

char DispValue_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "DynArray.h"
#pragma implementation "VarArray.h"
#endif

#define LOG_CREATE_VALUES 0
#define LOG_UPDATE_VALUES 0

//-----------------------------------------------------------------------------
// A `DispValue' maintains type and value of a displayed expression
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#include "DispValue.h"
#include "string-fun.h" // itostring
#include "DynArray.h"
#include "assert.h"
#include "cook.h"
#include "GDBAgent.h"
#include "ddd.h"
#include "DispNode.h"

//-----------------------------------------------------------------------------
// Type decls
//-----------------------------------------------------------------------------

// An array of disp values
typedef DynArray<DispValue*> DispValueArray;

enum Alignment {Vertical, Horizontal};

// Simple value
class SimpleDispValue {
public:
    string value;
};

// Pointer
class PointerDispValue {
public:
    string  value;
    bool dereferenced;  // True iff dereferenced
};

// Array
class ArrayDispValue {
public:
    DispValueArray members;
    int            member_count;
    Alignment      align;
};

// Struct or class
class StructOrClassDispValue {
public:
    DispValueArray members;
    int            member_count;
};


//-----------------------------------------------------------------------------
// Function defs
//-----------------------------------------------------------------------------

// Constructor
DispValue::DispValue (DispValue* p, 
		      int           d,
		      string&       value,
		      string        f_n, 
		      string        p_n)
    : myparent(p), mydepth (d), myexpanded(true), 
      myfull_name(f_n), print_name(p_n)
{
    init(value);
}

// Initialization
void DispValue::init(string& value)
{
#if LOG_CREATE_VALUES
    clog << "Building value from " << quote(value) << "\n";
#endif

    if (print_name == "")
	mytype = Text;
    else if (is_user_command (print_name))
	mytype = List;
    else if (is_BaseClass_name (print_name))
	mytype = BaseClass;
    else
	mytype = determine_type(value);

    switch (mytype) {
    case Simple:
	{
	    v.simple = new SimpleDispValue;
	    v.simple->value = read_simple_value (value);
#if LOG_CREATE_VALUES
	    clog << mytype << ": " << v.simple->value << "\n";
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
	    v.pointer->value = read_pointer_value (value);
	    v.pointer->dereferenced = false;

#if LOG_CREATE_VALUES
	    clog << mytype << ": " << v.pointer->value << "\n";
#endif
	    // Hide vtable pointers.
	    if (v.pointer->value.contains("virtual table")
		|| v.pointer->value.contains("vtable"))
		myexpanded = false;
	    break;
	}

    case Array:
	{
	    static regex RXsimple("([][a-zA-Z0-9_().]|->)*");

	    string base = myfull_name;
	    if (!base.matches(RXsimple))
		base = "(" + base + ")";

	    v.array = new ArrayDispValue;
	    myexpanded = (mydepth > 0) ? false : true;
	    v.array->align = Vertical;
	    v.array->member_count = 0;

#if LOG_CREATE_VALUES
	    clog << mytype << ": " << "\n";
#endif

	    read_array_begin (value);
	    // The array has at least one element.  Otherwise, GDB would
	    // treat it as a pointer.

	    string vtable_entries = read_vtable_entries(value);
	    if (vtable_entries != "")
	    {
		v.array->members[v.array->member_count++] = 
		    new DispValue (this, mydepth + 1,
				   vtable_entries, 
				   myfull_name, myfull_name);
	    }

	    string member_name;
	    int i = 0;
	    do {
		member_name = "[" + itostring (i++) + "]";
		v.array->members[v.array->member_count++] = 
		    new DispValue (this, mydepth + 1,
				   value, 
				   base + member_name, member_name);
	    } while (read_array_next (value) != 0);
	    read_array_end (value);

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
	    myexpanded = (mydepth > 0) ? false : true;
	    v.str_or_cl->member_count = 0;
	
#if LOG_CREATE_VALUES
	    clog << mytype << " " << quote(myfull_name) << "\n";
#endif
	    string member_prefix = myfull_name;
	    if (mytype == List)
	    {
		member_prefix = "";
	    }
	    else if (member_prefix.contains('*', 0))
	    {
		member_prefix.del("*");
		member_prefix.prepend ("(");
		member_prefix += ")->";
	    }
	    else
	    {
		member_prefix += ".";
	    }

	    read_str_or_cl_begin (value);
	
	    bool more_values = true;
	    string member_name = read_member_name (value);
	    int i = 0;
	    while (more_values && member_name != "") {
		v.str_or_cl->member_count++;
		if (is_BaseClass_name (member_name) || member_name == " ")
		{
		    // Anonymous union
		    v.str_or_cl->members[i] = 
			new DispValue (this, mydepth + 1, value, myfull_name,
				       member_name);
		    more_values = read_str_or_cl_next (value);
		    read_members_of_xy (value);
		}
		else
		{
		    v.str_or_cl->members[i] = 
			new DispValue (this, mydepth + 1, value, 
				       member_prefix + member_name, 
				       member_name);
		    more_values = read_str_or_cl_next (value);
		}
		i++;
		if (more_values)
		    member_name = read_member_name (value);
	    }
	    read_str_or_cl_end (value);

	    if (mytype == List && value != "")
	    {
		// Add remaining value as text
		v.str_or_cl->members[v.str_or_cl->member_count++] = 
		    new DispValue(this, mydepth + 1, value, "", "");
	    }

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

	    string ref = value.before(':');
	    value = value.after(':');

	    v.str_or_cl->members[0] = 
		new DispValue(this, mydepth + 1, ref, 
			      "&" + myfull_name, myfull_name);
	    v.str_or_cl->members[1] = 
		new DispValue(this, mydepth + 1, value,
			      myfull_name, myfull_name);
	    break;
	}
    }
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

    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	for (i = 0; i < v.str_or_cl->member_count; i++) {
	    delete v.str_or_cl->members[i];
	}
	delete v.str_or_cl;
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
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return false;
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
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return "";
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
    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return "";
    }

    return "";
}




// Return #children.  Only if type() == Array, StructOrClass, or BaseClass.
int DispValue::number_of_childs() const
{
    switch (mytype) {
    case Array:
	return v.array->member_count;

    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
	return v.str_or_cl->member_count;

    case Simple:
    case Text:
    case Pointer:
	return 0;
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
    }

    return 0;
}



// Check if vertically aligned.  Only if type() == Array.
bool DispValue::vertical_aligned()   const
{
    switch (mytype) {
    case Array:
	return v.array->align == Vertical;

    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
    case Pointer:
    case Simple:
    case Text:
	return false;
    }

    return false;
}

// Check if horizontally aligned.  Only if type() == Array.
bool DispValue::horizontal_aligned() const
{
    switch (mytype) {
    case Array:
	return v.array->align == Horizontal;

    case List:
    case StructOrClass:
    case BaseClass:
    case Reference:
    case Pointer:
    case Simple:
    case Text:
	return false;
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
void DispValue::expandAll()
{
    expand();
    for (int i = 0; i < number_of_childs(); i++)
    {
	DispValue *child = get_child(i);
	child->expandAll();
    }
}

// Collapse.  Like collapse(), but collapse entire subtree
void DispValue::collapseAll()
{
    collapse();
    for (int i = 0; i < number_of_childs(); i++)
    {
	DispValue *child = get_child(i);
	child->collapseAll();
    }
}

// Count expanded nodes in tree
int DispValue::expandedAll() const
{
    int count = 0;
    if (expanded())
	count++;
    for (int i = 0; i < number_of_childs(); i++)
	count += get_child(i)->expandedAll();

    return count;
}

// Count collapsed nodes in tree
int DispValue::collapsedAll() const
{
    int count = 0;
    if (collapsed())
	count++;
    for (int i = 0; i < number_of_childs(); i++)
	count += get_child(i)->collapsedAll();

    return count;
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


// Update values from VALUE.  Make WAS_CHANGED true iff value changed;
// Make WAS_INITIALIZED true iff type changed.
void DispValue::update (string& value, 
			bool& was_changed, bool& was_initialized)
{
    string init_value = value;

    int i;
    string new_value;
    string member_name;
    bool more_values = true;

#if LOG_UPDATE_VALUES
    clog << "Updating " << mytype 
	 << " " << full_name() << " with " << quote(value) << "\n";
#endif

    DispValueType new_type;

    if (print_name == "")
	new_type = Text;
    else if (is_user_command (print_name))
	new_type = List;
    else if (is_BaseClass_name (print_name))
	new_type = BaseClass;
    else
	new_type = determine_type(value);

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
	new_value = read_simple_value (value);
	if (v.simple->value != new_value) {
	    v.simple->value = new_value;
	    was_changed = true;
	}
	break;

    case Text:
	if (v.simple->value != value) {
	    v.simple->value = value;
	    was_changed = true;
	}
	break;

    case Pointer:
	new_value = read_pointer_value (value);
	if (v.pointer->value != new_value) {
	    v.pointer->value = new_value;
	    was_changed = true;
	}
	break;

    case Array:
    {
	read_array_begin (value);

	i = 0;
	string vtable_entries = read_vtable_entries(value);
	if (vtable_entries != "")
	{
	    v.array->members[i++]->update(vtable_entries, 
					  was_changed, was_initialized);
	    if (was_initialized)
		break;
	}

	if (!was_initialized)
	{
	    for (; more_values && i < v.array->member_count; i++) {
		v.array->members[i]->update(value, 
					    was_changed, was_initialized);
		if (was_initialized)
		    break;
		more_values = read_array_next (value);
	    }
	}

	if (was_initialized || i != v.array->member_count)
	{
#if LOG_UPDATE_VALUES
	    clog << mytype << " changed\n";
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

	    read_str_or_cl_begin (value);
	    for (i = 0; more_values && i < v.str_or_cl->member_count; i++)
	    {
		member_name = read_member_name (value);
		
		if (is_BaseClass_name (member_name))
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
		    more_values = read_str_or_cl_next (value);
		}
	    }

	    if (was_initialized 
		|| i != v.str_or_cl->member_count 
		|| more_values)
	    {
#if LOG_UPDATE_VALUES
		clog << mytype << " changed\n";
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

    case Reference: {
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
    }

#if LOG_UPDATE_VALUES
    clog << "Update done\n";
#endif
    return;
}
