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

    void init(string& value);
    void clear();

public:
    // The DispValue type is determined from VALUE
    DispValue (DispValue* parent, 
	       int        depth,
	       string&    value,
	       string     full_name, 
	       string     print_name);

    ~DispValue();

    DispValueType type()       const { return mytype; }
    DispValue*    parent()     const { return myparent; }
    int           depth()      const { return mydepth; }
    const string& full_name()  const { return myfull_name; }
    const string& name()       const { return print_name; }
    bool          is_changed() const { return changed; }
    bool          expanded()   const { return myexpanded; }
    bool          collapsed()  const { return !expanded(); }

    // Makes sense only for type() == Simple or Pointer
    string  value()      const;
    
    // Makes sense only for type() == Pointer
    bool dereferenced()      const;
    string  dereferenced_name() const;

    int number_of_childs() const;
    DispValue* get_child (int i) const;
    
    // Makes sense only for type() == Array
    bool vertical_aligned()   const;
    bool horizontal_aligned() const;

    // Makes sense only for type() == Pointer
    void dereference();

    // Makes sense only for type() == Array, StructOrClass, BaseClass
    // Expand/collapse single value
    void expand()    { myexpanded = true;  }
    void collapse()  { myexpanded = false; }

    // Expand/collapse entire tree
    void collapseAll();
    void expandAll();

    // Count expanded or selected nodes in tree
    int expandedAll()  const;
    int collapsedAll() const;

    // Makes sense only for type() == Array
    void align_vertical();
    void align_horizontal();

    // True if VALUE was really new (i.e. something changed)
    void update (string& value, bool& changed, bool& inited);
    bool new_BaseClass_name (string name);
};

#endif // _DDD_DispValue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
