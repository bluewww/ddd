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
#include "Box.h"
#include "value-read.h"
#include "StringSA.h"

//-----------------------------------------------------------------------------
class DispValue {
    DispValueType mytype;
    DispValue*    myparent;
    int           mydepth;      // Nesting depth
    bool          myexpanded;
    string        myfull_name;
    string        print_name;   // Name relative to MYPARENT
    bool          changed;

    union {
	class SimpleDispValue*        simple;    // mytype == Simple
	class PointerDispValue*       pointer;   // mytype == Pointer
	class ArrayDispValue*         array;     // mytype == Array
	class StructOrClassDispValue* str_or_cl; // mytype == StructOrClass
				                 // or mytype == BaseClass
				                 // or mytype == Reference
    } v;

    // Initialize from VALUE.  If TYPE is given, use TYPE as type
    // instead of inferring it.
    void init(string& value, DispValueType type = UnknownType);
    void clear();

    DispValue& operator = (const DispValue&)
    {
	assert(0); return *this;
    }

    // Helpers
    static StringStringAssoc type_cache;
    static int index_base(const string& expr, int dim);
    static string add_member_name(const string& base, 
				  const string& member_name);

protected:
    // Makes sense only for type() == Array, StructOrClass, BaseClass
    // Expand/collapse single value
    void _expand()    { myexpanded = true;  }
    void _collapse()  { myexpanded = false; }

public:
    // The DispValue type is determined from VALUE
    DispValue (DispValue* parent, 
	       int        depth,
	       string&    value,
	       const string& full_name, 
	       const string& print_name,
	       DispValueType type = UnknownType);

    // Copy constructor
    DispValue (const DispValue& dv);

    // Destructor
    ~DispValue();

    // Duplicator
    DispValue *dup();

    DispValueType type()       const { return mytype; }
    DispValue*    parent()     const { return myparent; }
    int           depth()      const { return mydepth; }
    const string& full_name()  const { return myfull_name; }
    const string& name()       const { return print_name; }

    DispValue*&   parent()     { return myparent; }
    int&          depth()      { return mydepth; }
    string&       full_name()  { return myfull_name; }
    string&       name()       { return print_name; }

    bool          is_changed() const { return changed; }
    bool          expanded()   const { return myexpanded; }
    bool          collapsed()  const { return !expanded(); }

    // Makes sense only for type() == Simple or Pointer
    string value() const;
    
    // Makes sense only for type() == Pointer
    bool dereferenced() const;
    string dereferenced_name() const;

    int nchildren() const;
    DispValue* get_child (int i) const;
    
    // Makes sense only for type() == Array
    bool vertical_aligned()   const;
    bool horizontal_aligned() const;

    // Makes sense only for type() == Pointer
    void dereference();

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

    // Return height of entire tree
    int height() const;

    // Return height of expanded tree
    int heightExpanded() const;

    // Makes sense only for type() == Array
    void align_vertical();
    void align_horizontal();

    // Update values from VALUE.  Set WAS_CHANGED iff value changed;
    // Set WAS_INITIALIZED iff type changed.  If TYPE is given, use
    // TYPE as type instead of inferring it.
    void update (string& value, bool& changed, bool& inited,
		 DispValueType type = UnknownType);
    bool new_BaseClass_name (string name);

    // Background processing.  PROCESSED is the number of characters
    // processed so far.  If this returns true, abort operation.
    static bool (*background)(int processed);

    // Clear cache of all types read so far
    static void clear_type_cache();
};

#endif // _DDD_DispValue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
