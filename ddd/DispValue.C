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

#include "AppData.h"
#include "DispNode.h"
#include "DispValueA.h"
#include "DynArray.h"
#include "GDBAgent.h"
#include "PlotAgent.h"
#include "assert.h"
#include "cook.h"
#include "ddd.h"
#include "fonts.h"
#include "misc.h"
#include "plotter.h"
#include "question.h"
#include "regexps.h"
#include "string-fun.h"
#include "value-read.h"

#include <ctype.h>


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
    : mytype(UnknownType), myexpanded(true), myenabled(true),
      myfull_name(f_n), print_name(p_n), changed(false), myrepeats(1),
      _value(""), _dereferenced(false), _children(0),
      _index_base(0), _have_index_base(false), _alignment(Horizontal),
      _plotter(0), _cached_box(0), _links(1)
{
    init(parent, depth, value, given_type);

    // A new display is not changed, but initialized
    changed = false;
}

// Duplicator
DispValue::DispValue (const DispValue& dv)
    : mytype(dv.mytype), myexpanded(dv.myexpanded), 
      myenabled(dv.myenabled), myfull_name(dv.myfull_name),
      print_name(dv.print_name), myaddr(dv.myaddr),
      changed(false), myrepeats(dv.myrepeats),
      _value(dv.value()), _dereferenced(false), _children(dv.nchildren()), 
       _index_base(dv._index_base), 
      _have_index_base(dv._have_index_base), _alignment(dv._alignment),
      _plotter(0), _cached_box(0), _links(1)
{
    for (int i = 0; i < dv.nchildren(); i++)
    {
	_children += dv.child(i)->dup();
    }

    if (dv.cached_box() != 0)
	_cached_box = dv.cached_box()->link();
}


// True if more sequence members are coming
bool DispValue::sequence_pending(const string& value, 
				 const DispValue *parent)
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

    // Be sure the value is not changed in memory
    value.consuming(true);

    char *initial_value = value;

    static DispValueArray empty(0);
    _children = empty;

    if (background(value.length()))
    {
	clear();

	mytype = Simple;
	_value = "(Aborted)";
	value  = "Aborted\n";
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
	_value = read_simple_value(value, depth, ignore_repeats);
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(_value) << "\n";
#endif
	break;
    }

    case Text:
    {
	// Read in entire text
	_value = value;
	value = value.from(int(value.length())); // assigns ""
#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(_value) << "\n";
#endif
	break;
    }

    case Pointer:
    {
	_value = read_pointer_value (value, ignore_repeats);
	_dereferenced = false;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << quote(_value) << "\n";
#endif
	// Hide vtable pointers.
	if (_value.contains("virtual table") || _value.contains("vtable"))
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

	_alignment = Vertical;

#if LOG_CREATE_VALUES
	clog << mytype << ": " << "\n";
#endif

	read_array_begin(value, myaddr);

	// Check for `vtable entries' prefix.
	string vtable_entries = read_vtable_entries(value);
	if (vtable_entries != "")
	{
	    _children += parse_child(depth, vtable_entries, myfull_name);
	}

	// Read the array elements.  Assume that the type is the
	// same across all elements.
	DispValueType member_type = UnknownType;
	if (!_have_index_base)
	{
	    _index_base = index_base(base, depth);
	    _have_index_base = true;
	}
	int array_index = _index_base;

	// The array has at least one element.  Otherwise, GDB
	// would treat it as a pointer.
	do {
	    char *repeated_value = value;
	    string member_name = 
		gdb->index_expr("", itostring(array_index++));
	    DispValue *dv = parse_child(depth, value,
					add_member_name(base, member_name), 
					member_name, member_type);
	    member_type = dv->type();
	    _children += dv;

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
		    _children += repeated_dv;
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
	} while (read_array_next(value));
	read_array_end(value);

	// Expand only if at top-level.
	myexpanded = (depth == 0 || nchildren() <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " has " << nchildren() << " members\n";
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
		char *old_value = value;

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
			DispValue *dv2 = dv->child(i)->link();
			_children += dv2;
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
		    _children += dv;
		}

		more_values = read_multiple_values && read_struct_next(value);
	    }
	    else if (is_BaseClass_name(member_name))
	    {
		// Base class member
		DispValue *dv = 
		    parse_child(depth, value, myfull_name, member_name);
		_children += dv;

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

		_children += parse_child(depth, value, full_name, member_name);
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
	    _children += parse_child(depth, value, "");
	}

	if (found_struct_begin)
	{
	    // Skip the remainder
	    read_struct_end(value);
	}

	// Expand only if at top-level.
	myexpanded = (depth == 0 || nchildren() <= 1);

#if LOG_CREATE_VALUES
	clog << mytype << " "
	     << quote(myfull_name)
	     << " has " << nchildren() << " members\n";
#endif

	break;
    }

    case Reference:
    {
	myexpanded = true;

	int sep = value.index('@');
	sep = value.index(':', sep);

	string ref = value.before(sep);
	value = value.after(sep);

	string addr = gdb->address_expr(myfull_name);

	_children += parse_child(depth, ref, addr, myfull_name, Pointer);
	_children += parse_child(depth, value, myfull_name);

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

		need_clear = false;
	    }
	    
	    char *old_value = value;

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
		_children += dv;
	    }
	}

#if LOG_CREATE_VALUES
	if (!need_clear)
	{
	    clog << mytype << " "
		 << quote(myfull_name)
		 << " has " << nchildren() << " members\n";
	}
#endif
    }

    background(value.length());
    changed = true;
}

// Destructor helper
void DispValue::clear()
{
    for (int i = 0; i < nchildren(); i++)
	child(i)->unlink();

    static DispValueArray empty(0);
    _children = empty;

    if (plotter() != 0)
    {
	plotter()->removeHandler(Died, PlotterDiedHP, (void *)this);
	delete plotter();
	_plotter = 0;
    }

    clear_cached_box();
}

//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------

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


//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

// Expand.  Like expand(), but expand entire subtree
void DispValue::expandAll(int depth)
{
    if (depth == 0)
	return;

    _expand();

    for (int i = 0; i < nchildren(); i++)
    {
	child(i)->expandAll(depth - 1);
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
	child(i)->collapseAll(depth - 1);
    }
}

// Count expanded nodes in tree
int DispValue::expandedAll() const
{
    int count = 0;
    if (expanded())
	count++;
    for (int i = 0; i < nchildren(); i++)
	count += child(i)->expandedAll();

    return count;
}

// Count collapsed nodes in tree
int DispValue::collapsedAll() const
{
    int count = 0;
    if (collapsed())
	count++;
    for (int i = 0; i < nchildren(); i++)
	count += child(i)->collapsedAll();

    return count;
}


// Return height of entire tree
int DispValue::height() const
{
    int d = 0;

    for (int i = 0; i < nchildren(); i++)
	d = max(d, child(i)->height());

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
	if (child(i)->collapsed())
	    return 1;

	d = max(d, child(i)->heightExpanded());
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

    if (background(value.length()))
    {
	// Aborted while parsing - use SOURCE instead of original
	DispValue *ret = source->link();
	ret->changed = was_changed = was_initialized = true;
	unlink();
	return ret;
    }

    DispValue *dv = update(source, was_changed, was_initialized);

    source->unlink();

    if (was_changed || was_initialized)
	clear_cached_box();

    return dv;
}


// Update values from SOURCE.  Set WAS_CHANGED iff value changed; Set
// WAS_INITIALIZED iff type changed.  Note: Neither THIS nor SOURCE
// can be referenced after calling this function; use the returned
// value instead.
DispValue *DispValue::update(DispValue *source, 
			     bool& was_changed, bool& was_initialized)
{
    bool was_plotted = (plotter() != 0);

    DispValue *dv = _update(source, was_changed, was_initialized);
    if (was_plotted && was_changed)
	dv->plot();

    return dv;
}

DispValue *DispValue::_update(DispValue *source, 
			      bool& was_changed, bool& was_initialized)
{
    if (source == this)
    {
	// We're updated from ourselves -- ignore it all.  
	// This happens when a cluster is updated from the values of
	// the clustered dislays.
	return this;
    }

    if (changed)
    {
	// Clear `changed' flag
	changed = false;
	was_changed = true;
    }

    if (source->enabled() != enabled())
    {
	myenabled = source->enabled();
	was_changed = true;

	// We don't set CHANGED to true since enabled/disabled changes
	// are merely a change in the view, not a change in the data.
    }

    if (source->full_name() == full_name() && source->type() == type())
    {
	switch (type())
	{
	case Simple:
	case Text:
	case Pointer:
	    // Atomic values
	    if (_value != source->value())
	    {
		_value = source->value();
		changed = was_changed = true;
	    }
	    return this;

	case Array:
	    // Array.  Check for 1st element, too.
	    if (_have_index_base != source->_have_index_base &&
		(_have_index_base && _index_base != source->_index_base))
		break;

	    // FALL THROUGH
	case Reference:
	case Sequence:
	    // Numbered children.  If size changed, we assume
	    // the whole has been changed.
	    if (nchildren() == source->nchildren())
	    {
		for (int i = 0; i < nchildren(); i++)
		{
		    // Update each child
		    _children[i] = child(i)->update(source->child(i),
						    was_changed,
						    was_initialized);
		}
		return this;
	    }
	    break;

	case List:
	case Struct:
	{
	    // Named children.  Check whether names are the same.
	    bool same_members = (nchildren() == source->nchildren());

	    for (int i = 0; same_members && i < nchildren(); i++)
	    {
		if (child(i)->full_name() != source->child(i)->full_name())
		    same_members = false;
	    }

	    if (same_members)
	    {
		// Update each child
		for (int i = 0; i < nchildren(); i++)
		{
		    _children[i] = child(i)->update(source->child(i),
						    was_changed,
						    was_initialized);
		}
		return this;
	    }

	    // Members have changed.
	    // Be sure to mark only those members that actually have changed
	    // (i.e. don't mark the entire struct and don't mark new members)
	    // We do so by creating a new list of children.  `Old' children
	    // that still are reported get updated; `new' children are added.
	    DispValueArray new_children;
	    DispValueArray processed_children;
	    for (int j = 0; j < source->nchildren(); j++)
	    {
		DispValue *c = 0;
		for (int i = 0; c == 0 && i < nchildren(); i++)
		{
		    bool processed = false;
		    for (int k = 0; k < processed_children.size(); k++)
		    {
			if (child(i) == processed_children[k])
			    processed = true;
		    }
		    if (processed)
			continue;

		    if (child(i)->full_name() == source->child(j)->full_name())
		    {
			c = child(i)->update(source->child(j),
					     was_changed,
					     was_initialized);
			processed_children += child(i);
		    }
		}

		if (c == 0)
		{
		    // Child not found -- use source child
		    c = source->child(j)->link();
		}

		new_children += c;
	    }
	    _children = new_children;
	    was_changed = was_initialized = true;
	    return this;
	}

	case UnknownType:
	    assert(0);
	    abort();
	}
    }

    // Type, name or structure have changed -- use SOURCE instead of original
    DispValue *ret = source->link();
    ret->changed = was_changed = was_initialized = true;

    // Copy the basic settings
    ret->myexpanded = expanded();
    ret->dereference(dereferenced());
    if (vertical_aligned())
	ret->align_vertical();
    if (horizontal_aligned())
	ret->align_horizontal();

    unlink();
    return ret;
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
	    if (nchildren() != source->nchildren())
		return false;	// Differing size

	    if (_have_index_base != source->_have_index_base)
		return false;	// Differing base

	    if (_have_index_base && _index_base != source->_index_base)
		return false;	// Differing base

	    for (int i = 0; i < nchildren(); i++)
	    {
		DispValue *child = _children[i];
		DispValue *source_child = source->child(i);
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
	    if (nchildren() != source->nchildren())
		return false;

	    for (int i = 0; i < nchildren(); i++)
	    {
		DispValue *child = _children[i];
		DispValue *source_child = source->child(i);
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
// Plotting
//-----------------------------------------------------------------------------

int DispValue::can_plot() const
{
    if (can_plot3d())
	return 3;

    if (can_plot2d())
	return 2;

    if (can_plot1d())
	return 1;

    // Search for plottable array children
    int ndim = 0;
    for (int i = 0; i < nchildren(); i++)
	ndim = max(ndim, child(i)->can_plot());

    return ndim;
}

inline bool starts_number(char c)
{
    return c == '.' || c == '+' || c == '-' || isdigit(c);
}

bool DispValue::can_plot1d() const
{
    if (type() != Simple)
	return false;

    const string& v = value();
    if (v.length() == 0)
	return false;	// Empty value
    if (!starts_number(v[0]))
	return false;	// Not a numeric value

    return true;
}

bool DispValue::can_plot2d() const
{
    if (type() != Array)
	return false;

    for (int i = 0; i < nchildren(); i++)
    {
	if (!child(i)->can_plot1d())
	    return false;
    }

    return true;
}

bool DispValue::can_plot3d() const
{
    if (type() != Array)
	return false;

    int grandchildren;
    for (int i = 0; i < nchildren(); i++)
    {
	if (!child(i)->can_plot2d())
	    return false;

	if (i == 0)
	    grandchildren = child(i)->nchildren_with_repeats();
	else if (child(i)->nchildren_with_repeats() != grandchildren)
	    return false;	// Differing number of grandchildren
    }

    return true;
}

int DispValue::nchildren_with_repeats() const
{
    int sum = 0;
    for (int i = 0; i < nchildren(); i++)
	sum += child(i)->repeats();
    return sum;
}

void DispValue::plot() const
{
    int ndim = can_plot();
    if (ndim == 0)
	return;

    if (plotter() == 0)
    {
	string title = full_name();
	if (is_user_command(title))
	{
	    title = user_command(title);
	    if (title.contains("info ", 0))
		title = title.after("info ");
	    else if (title.contains(" "))
		title = title.before(" ");
	    if (title.length() > 0)
		title = toupper(title[0]) + title.after(0);
	}

	((DispValue *)this)->_plotter = new_plotter(title);
	plotter()->addHandler(Died, PlotterDiedHP, (void *)this);
    }

    plotter()->plot_2d_settings = app_data.plot_2d_settings;
    plotter()->plot_3d_settings = app_data.plot_3d_settings;

    _plot(plotter(), ndim);

    plotter()->flush();
}

void DispValue::_plot(PlotAgent *plotter, int ndim) const
{
    if (can_plot3d())
    {
	plot3d(plotter, ndim);
	return;
    }

    if (can_plot2d())
    {
	plot2d(plotter, ndim);
	return;
    }

    if (can_plot1d())
    {
	plot1d(plotter, ndim);
	return;
    }

    // Plot all array children into one window
    for (int i = 0; i < nchildren(); i++)
	child(i)->_plot(plotter, ndim);
}

string DispValue::num_value() const
{
    const string& v = value();
    if (v.contains(rxdouble, 0))
	return v.through(rxdouble);

    if (v.contains(rxint, 0))
	return v.through(rxint);

    return "0";			// Illegal value
}

void DispValue::plot1d(PlotAgent *plotter, int ndim) const
{
    plotter->start_plot(full_name(), ndim);
    plotter->add_point(num_value(), horizontal_aligned() ? 0 : 1);
    plotter->end_plot();
}

void DispValue::plot2d(PlotAgent *plotter, int ndim) const
{
    plotter->start_plot(full_name(), ndim);

    int index;
    if (_have_index_base)
	index = _index_base;
    else
	index = gdb->default_index_base();

    for (int i = 0; i < nchildren(); i++)
    {
	DispValue *c = child(i);
	for (int ii = 0; ii < c->repeats(); ii++)
	{
	    plotter->add_point(index++, c->num_value());
	}
    }

    plotter->end_plot();
}

void DispValue::plot3d(PlotAgent *plotter, int ndim) const
{
    plotter->start_plot(full_name(), ndim);

    int index;
    if (_have_index_base)
	index = _index_base;
    else
	index = gdb->default_index_base();

    for (int i = 0; i < nchildren(); i++)
    {
	DispValue *c = child(i);
	int c_index;
	if (c->_have_index_base)
	    c_index = c->_index_base;
	else
	    c_index = gdb->default_index_base();

	for (int ii = 0; ii < c->repeats(); ii++)
	{
	    for (int j = 0; j < c->nchildren(); j++)
	    {
		DispValue *cc = c->child(j);
		for (int jj = 0; jj < cc->repeats(); jj++)
		    plotter->add_point(index, c_index++, cc->num_value());
	    }

	    index++;
	}

	plotter->add_break();
    }
    
    plotter->end_plot();
}

void DispValue::PlotterDiedHP(Agent *, void *client_data, void *)
{
    DispValue *dv = (DispValue *)client_data;
    dv->_plotter = 0;
}


//-----------------------------------------------------------------------------
// Background processing
//-----------------------------------------------------------------------------

static bool nop(int) { return false; }

bool (*DispValue::background)(int processed) = nop;
