// $Id$
// Read and store type and value of a displayed expression

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

char DispValue_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "DynArray.h"
#endif

#define LOG_CREATE_VALUES 0
#define LOG_UPDATE_VALUES 0

//-----------------------------------------------------------------------------
// DispValue liest und speichert Typ und Wert eines Display-Ausdrucks
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#include "DispValue.h"
#include "string-fun.h" // fuer itostring
#include "DynArray.h"
#include "assert.h"
#include "cook.h"

//-----------------------------------------------------------------------------
// Typ-Deklarationen
//-----------------------------------------------------------------------------

// typedef DispValue* DispValuePtr;
typedef DynArray<DispValue*> DispValueArray;

enum Alignment {Vertical, Horizontal};

//-----------------------------------------------------------------------------
class SimpleDispValue {
public:
    string value;
};

//-----------------------------------------------------------------------------
class PointerDispValue {
public:
    string  value;
    bool dereferenced;  // true, wenn uber diesen Pointer deref. wird
};

//-----------------------------------------------------------------------------
class ArrayDispValue {
public:
    DispValueArray members;
    int            member_count;
    Alignment      align;
};

//-----------------------------------------------------------------------------
class StructOrClassDispValue {
public:
    DispValueArray members;
    int            member_count;
};

//-----------------------------------------------------------------------------
// Funktions-Definitionen
//-----------------------------------------------------------------------------

// ***************************************************************************
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

void DispValue::init(string& value)
{
#if LOG_CREATE_VALUES
    clog << "Building value from " << quote(value) << "\n";
#endif

    if (is_BaseClass_name (print_name))
    {
	mytype = BaseClass;
    }
    else
    {
	mytype = determine_type(value);
    }

    switch (mytype) {
    case Simple: {
	v.simple = new SimpleDispValue;
	v.simple->value = read_simple_value (value);
#if LOG_CREATE_VALUES
	clog << "Simple value: " << v.simple->value << "\n";
#endif
	break;
    }
    case Pointer: {
	v.pointer = new PointerDispValue;
	v.pointer->value = read_pointer_value (value);
	v.pointer->dereferenced = false;
#if LOG_CREATE_VALUES
	clog << "Pointer value: " << v.pointer->value << "\n";
#endif
	break;
    }
    case Array: {
	v.array = new ArrayDispValue;
	myexpanded = (mydepth > 0) ? false : true;
	v.array->align = Vertical;
	v.array->member_count = 0;

#if LOG_CREATE_VALUES
	clog << "Array: " << "\n";
#endif

	read_array_begin (value);
	// array hat mind. ein element (wird sonst vom gdb nicht als solches,
	// sondern als pointer angezeigt)

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
			       myfull_name + member_name, member_name);
	} while (read_array_next (value) != 0);
	read_array_end (value);

#if LOG_CREATE_VALUES
	clog << "Array has " << v.array->member_count << " members\n";
#endif
	break;
    }
    case StructOrClass:
    case BaseClass: {
	v.str_or_cl = new StructOrClassDispValue;
	myexpanded = (mydepth > 0) ? false : true;
	v.str_or_cl->member_count = 0;
	
#if LOG_CREATE_VALUES
	clog << "Struct or class " 
	     << quote(myfull_name) << "\n";
#endif

	string member_prefix = myfull_name;
	if ( member_prefix.contains ('*', 0) ) {
	    member_prefix.del("*");
	    member_prefix.prepend ("(");
	    member_prefix += ")->";
	}
	else {
	    member_prefix += ".";
	}
	
	read_str_or_cl_begin (value);
	
	bool more_values = true;
	string member_name = read_member_name (value);
	int i = 0;
	while (more_values && member_name != "") {
	    v.str_or_cl->member_count++;
	    if ( is_BaseClass_name (member_name) ||
		 member_name == " " // Spezialfall namenlose union
		) {
		v.str_or_cl->members[i] = new DispValue (this,
							 mydepth + 1,
							 value,
							 myfull_name,
							 member_name);
		more_values = read_str_or_cl_next (value);
		read_members_of_xy (value);
	    }
	    else {
		v.str_or_cl->members[i] = 
		    new DispValue (this,
				   mydepth + 1,
				   value,
				   member_prefix + member_name,
				   member_name);
		more_values = read_str_or_cl_next (value);
	    }
	    i++;
	    if (more_values)
		member_name = read_member_name (value);
	}
	read_str_or_cl_end (value);

#if LOG_CREATE_VALUES
	clog << "struct or class " 
	     << quote(myfull_name)
	     << " has " << v.str_or_cl->member_count << " members\n";
#endif

	break;
    }
    case Reference: {
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


// ***************************************************************************
DispValue::~DispValue ()
{
    clear();
}

void DispValue::clear()
{
    int i;
    switch (mytype) {
    case Simple:
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
    case StructOrClass:
    case BaseClass:
    case Reference:
	for (i = 0; i < v.str_or_cl->member_count; i++) {
	    delete v.str_or_cl->members[i];
	}
	delete v.str_or_cl;
	break;
    default:
	break;
    }
}

//-----------------------------------------------------------------------------
// Abragen
//-----------------------------------------------------------------------------

// ***************************************************************************
// nur fuer type() == Pointer sinnvoll
//
bool DispValue::dereferenced() const
{
    if (mytype == Pointer)
	return v.pointer->dereferenced;
    else
	return false;
}


// ***************************************************************************
// nur fuer type() == Pointer sinnvoll
//
string DispValue::dereferenced_name() const
{
    if (mytype == Pointer)
	return "*(" + full_name() + ")";
    else
	return "";
}



// ***************************************************************************
// nur fuer type() == Simple oder Pointer sinnvoll
//
string DispValue::value() const
{
    switch (mytype) {
    case Simple:
	return v.simple->value;
    case Pointer:
	return v.pointer->value;
    default:
	break;
    }
    return "";
}




// ***************************************************************************
// nur fuer type() == Array, StructOrClass, BaseClass sinnvoll
//
int DispValue::number_of_childs() const
{
    switch (mytype) {
    case Array:
	return v.array->member_count;
    case StructOrClass:
    case BaseClass:
    case Reference:
	return v.str_or_cl->member_count;
    default:
	break;
    }
    return 0;
}

// ***************************************************************************
// nur fuer type() == Array, StructOrClass, BaseClass sinnvoll
//
DispValue* DispValue::get_child (int i) const
{
    switch (mytype) {
    case Array:
	assert (i >= 0);
	assert (i < v.array->member_count);
	return v.array->members[i];
    case StructOrClass:
    case BaseClass:
    case Reference:
	assert (i >= 0);
	assert (i < v.str_or_cl->member_count);
	return v.str_or_cl->members[i];
    default:
	break;
    }
    return 0;
}



// ***************************************************************************
// nur fuer type() == Array sinnvoll
//
bool DispValue::vertical_aligned()   const
{
    if (mytype != Array)
	return false;
    return (v.array->align == Vertical);
}

// ***************************************************************************
// nur fuer type() == Array sinnvoll
//
bool DispValue::horizontal_aligned() const
{
    if (mytype != Array)
	return false;
    return (v.array->align == Horizontal);
}

//-----------------------------------------------------------------------------
// Modifizierungen
//-----------------------------------------------------------------------------

// ***************************************************************************
// nur fuer type() == Pointer sinnvoll
//
void DispValue::dereference()
{
    if (mytype == Pointer)
	v.pointer->dereferenced = true;
}

// ***************************************************************************
// nur fuer type() == Array sinnvoll
//
void DispValue::align_vertical ()
{
    if (mytype == Array)
	v.array->align = Vertical;
}

// ***************************************************************************
// nur fuer type() == Array sinnvoll
//
void DispValue::align_horizontal ()
{
    if (mytype == Array)
	v.array->align = Horizontal;
}

//-----------------------------------------------------------------------------
// Aktualisierungen
//-----------------------------------------------------------------------------

// ***************************************************************************
// nur fuer type() == BaseClass sinnvoll
//
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


// ***************************************************************************
void DispValue::update (string& value, 
			bool& was_changed, bool& was_initialized)
{
    string init_value = value;

    int i;
    string new_value;
    string member_name;
    bool more_values = true;

#if LOG_UPDATE_VALUES
    clog << "Updating " << full_name() << " with " << quote(value) << "\n";
#endif

    DispValueType new_type = determine_type(value);
    if (mytype != new_type)
    {
	// Type changed -- re-initialize.  The most common cause for
	// this effect is an incomplete display output - due to
	// illegal references, for example.
#if LOG_UPDATE_VALUES
	clog << "Type changed\n";
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
	    clog << "Array changed\n";
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

    case StructOrClass:
    case BaseClass:
	read_str_or_cl_begin (value);
	for (i = 0; more_values && i < v.str_or_cl->member_count; i++) {
	    member_name = read_member_name (value);
	    
	    if (is_BaseClass_name (member_name)) {
		if ( v.str_or_cl->members[i]->new_BaseClass_name (member_name))
		    was_changed = true;
		v.str_or_cl->members[i]->update(value, 
						was_changed, was_initialized);
		if (was_initialized)
		    break;
		if (!read_str_or_cl_next (value))
		    break;
		read_members_of_xy (value);
	    }
	    else {
		if (member_name != v.str_or_cl->members[i]->name())
		    break;

		v.str_or_cl->members[i]->update(value, 
						was_changed, was_initialized);
		if (was_initialized)
		    break;
		more_values = read_str_or_cl_next (value);
	    }
	}
	if (was_initialized || i != v.str_or_cl->member_count)
	{
#if LOG_UPDATE_VALUES
	    clog << "Struct/Class changed\n";
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
