// $Id$
// Read and store type and value of a displayed expression

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

#ifndef _DDD_DispValue_h
#define _DDD_DispValue_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// A DispValue reads and stores type and value of a displayed expression
//-----------------------------------------------------------------------------

#include "strclass.h"
#include "bool.h"
#include "DispValueA.h"
#include "DispValueT.h"
#include "StringSA.h"
#include "Box.h"

class Agent;
class PlotAgent;

enum DispValueAlignment {Vertical, Horizontal};

class DispValue {
    // General members
    DispValueType mytype;
    bool myexpanded;
    bool myenabled;
    string myfull_name;		// Full name
    string print_name;		// Name relative to parent
    string myaddr;		// Address as found
    bool changed;
    int myrepeats;		// Number of repetitions

    // Type-dependent members
    string _value;		// Value of basic types
    bool _dereferenced;		// True iff pointer is dereferenced
    DispValueArray _children;	// Array or Struct members 
    int _index_base;		// First index
    bool _have_index_base;	// True if INDEX_BASE is valid
    DispValueAlignment _alignment; // Array alignment

    // Plotting stuff
    PlotAgent *_plotter;	// Plotting agent

    // Caching stuff
    Box *_cached_box;		// Last box

    // Initialize from VALUE.  If TYPE is given, use TYPE as type
    // instead of inferring it.
    void init(DispValue *parent, int depth, 
	      string& value, DispValueType type = UnknownType);

    // Delete helper
    void clear();

    // Assignment
    void assign(DispValue& dv);

    DispValue& operator = (const DispValue&)
    {
	assert(0); return *this;
    }

    // Helpers
    static StringStringAssoc type_cache;
    static int index_base(const string& expr, int dim);
    static string add_member_name(const string& base, 
				  const string& member_name);

    // Plotting stuff
    void _plot(PlotAgent *plotter, int ndim) const;
    void plot1d(PlotAgent *plotter, int ndim) const;
    void plot2d(PlotAgent *plotter, int ndim) const;
    void plot3d(PlotAgent *plotter, int ndim) const;
    bool can_plot1d() const;
    bool can_plot2d() const;
    bool can_plot3d() const;

    static void PlotterDiedHP(Agent *, void *, void *);

    // Update helper
    DispValue *_update(DispValue *source, 
		       bool& was_changed, bool& was_initialized);

    // Clear cached box
    void clear_cached_box()
    {
	if (_cached_box != 0)
	{
	    _cached_box->unlink();
	    _cached_box = 0;
	}
    }

protected:
    int _links;			// #references (>= 1)

    // Array, Struct
    // Expand/collapse single value
    void _expand()
    {
	if (myexpanded)
	    return;

	myexpanded = true;
	clear_cached_box();
    }
    void _collapse()
    {
	if (myexpanded)
	    return;

	myexpanded = false;
	clear_cached_box();
    }

    // Plot Agent
    PlotAgent *plotter() const { return _plotter; }

    // True if more sequence members are coming
    static bool sequence_pending(const string& value, 
				 const DispValue *parent);

    // Numeric value
    string num_value() const;

    // The DispValue type and address are determined from VALUE
    DispValue (DispValue *parent, 
	       int        depth,
	       string&    value,
	       const string& full_name, 
	       const string& print_name,
	       DispValueType type = UnknownType);

    // Parsing function
    static DispValue *parse(DispValue *parent, 
			    int depth,
			    string& value,
			    const string& full_name, 
			    const string& print_name,
			    DispValueType type = UnknownType);

    DispValue *parse_child(int depth,
			   string& value,
			   const string& full_name, 
			   const string& print_name,
			   DispValueType type = UnknownType)
    {
	return parse(this, depth + 1, value, full_name, print_name, type);
    }

    DispValue *parse_child(int depth,
			   string& value,
			   const string& name, 
			   DispValueType type = UnknownType)
    {
	return parse_child(depth, value, name, name, type);
    }


    // Copy constructor
    DispValue (const DispValue& dv);

public:
    // Global settings
    static bool expand_repeated_values;

    // Parse VALUE into a DispValue tree
    static DispValue *parse(string& value, const string& name)
    {
	return parse(0, 0, value, name, name);
    }

    // Duplicator
    DispValue *dup() const
    {
	return new DispValue(*this);
    }

    // Destructor
    virtual ~DispValue()
    {
	assert (_links == 0);
	clear();
    }

    // Create new reference
    DispValue *link()
    {
	assert(_links > 0);
	_links++;
	return this;
    }

    // Kill reference
    void unlink()
    {
	assert(_links > 0);
	if (--_links == 0)
	    delete this;
    }

    // General resources
    DispValueType type()       const { return mytype; }
    bool enabled()             const { return myenabled; }
    const string& full_name()  const { return myfull_name; }
    const string& name()       const { return print_name; }
    const string& addr()       const { return myaddr; }
    int repeats()              const { return myrepeats; }

    int& repeats()       { clear_cached_box(); return myrepeats; }
    string& full_name()  { clear_cached_box(); return myfull_name; }
    string& name()       { clear_cached_box(); return print_name; }
    bool& enabled()      { clear_cached_box(); return myenabled; }

    bool is_changed() const { return changed; }
    bool expanded()   const { return myexpanded; }
    bool collapsed()  const { return !expanded(); }

    // Return height of entire tree
    int height() const;

    // Return height of expanded tree
    int heightExpanded() const;


    // Type-specific resources

    // Simple or Pointer
    const string& value() const { return _value; }
    
    // Pointer
    bool dereferenced() const { return _dereferenced; }
    string dereferenced_name() const;

    // Array, Struct, List, Sequence ...
    int nchildren() const { return _children.size(); }
    DispValue *child(int i) const { return _children[i]; }
    int nchildren_with_repeats() const;

    // General modifiers

    // Expand/collapse entire tree.  If DEPTH is non-negative, expand
    // DEPTH levels only.  If DEPTH is negative, expand all.
    void collapseAll(int depth = -1);
    void expandAll(int depth = -1);

    // Custom calls
    void collapse() { collapseAll(1); }
    void expand()   { expandAll(1); }

    // Count expanded or selected nodes in tree
    int expandedAll()  const;
    int collapsedAll() const;

    // Type-specific modifiers

    // Array
    bool vertical_aligned()   const { return _alignment == Vertical; }
    bool horizontal_aligned() const { return _alignment == Horizontal; }
    void align_vertical()
    {
	if (_alignment == Vertical)
	    return;

	_alignment = Vertical;
	clear_cached_box();
    }
    void align_horizontal()
    {
	if (_alignment == Horizontal)
	    return;

	_alignment = Horizontal;
	clear_cached_box();
    }

    // Pointer
    void dereference(bool set = true)
    {
	if (_dereferenced)
	    return;

	_dereferenced = set;
	clear_cached_box();
    }

    // Updating

    // Update values from VALUE.  Set WAS_CHANGED iff value changed;
    // Set WAS_INITIALIZED iff type changed.  If TYPE is given, use
    // TYPE as type instead of inferring it.  Note: THIS can no more
    // be referenced after calling this function; use the returned
    // value instead.
    DispValue *update(string& value, bool& was_changed, bool& was_initialized,
		      DispValueType type = UnknownType);

    // Update values from SOURCE.  Set WAS_CHANGED iff value changed;
    // Set WAS_INITIALIZED iff type changed.  Note: Neither THIS nor
    // SOURCE can be referenced after calling this function; use the
    // returned value instead.
    DispValue *update(DispValue *source, 
		      bool& was_changed, bool& was_initialized);

    // Return true iff SOURCE and this are structurally equal.
    // If SOURCE_DESCENDANT (a descendant of SOURCE) is not 0,
    // return its equivalent descendant of this in DESCENDANT.
    bool structurally_equal(const DispValue *source,
			    const DispValue *source_descendant,
			    const DispValue *&descendant) const;

    // Short version
    bool structurally_equal(const DispValue *source) const
    {
	const DispValue *dummy = 0;
	return structurally_equal(source, 0, dummy);
    }

    // Plotting

    // Return 0 if we cannot plot; return number of required
    // dimensions, otherwise (1, 2, or 3)
    int can_plot() const;

    // Plot value
    void plot() const;

    // Background processing.  PROCESSED is the number of characters
    // processed so far.  If this returns true, abort operation.
    static bool (*background)(int processed);

    // Clear cache of all types read so far
    static void clear_type_cache();

    // Hook for inserting previously computed DispValues
    static DispValue *(*value_hook)(string& value);

    // Box cache
    Box *cached_box() const
    {
	return _cached_box;
    }
    void set_cached_box(Box *value)
    {
	clear_cached_box();
	_cached_box = value->link();
    }
};

#endif // _DDD_DispValue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
