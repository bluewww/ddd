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

//-----------------------------------------------------------------------------
// DispValue liest und speichert Typ und Wert eines Display-Ausdrucks
//-----------------------------------------------------------------------------

#ifndef _DispValue_h
#define _DispValue_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "value-read.h"

//-----------------------------------------------------------------------------
class DispValue {
    DispValueType mytype;
    DispValue*    myparent;
    int           mydepth;      // = Verschachtelungstiefe
    bool          myexpanded;
    string        myfull_name;
    string        print_name;   // name relativ zum parent
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
    // Der Typ wird aus dem value selbst ermittelt.
    DispValue (DispValue* parent, 
	       int           depth,
	       string&       value,
	       string        full_name, 
	       string        print_name);

    ~DispValue();

    DispValueType type()       const { return mytype; }
    DispValue*    parent()     const { return myparent; }
    int           depth()      const { return mydepth; }
    string        full_name()  const { return myfull_name; }
    string        name()       const { return print_name; }
    bool          is_changed() const { return changed; }
    bool          expanded()   const { return myexpanded; }
    bool          collapsed()  const { return !expanded(); }

    // nur fuer type() == Simple oder Pointer sinnvoll
    string  value()      const;
    
    // nur fuer type() == Pointer sinnvoll
    bool dereferenced()      const;
    string  dereferenced_name() const;

    int number_of_childs() const;
    DispValue* get_child (int i) const;
    
    // nur fuer type() == Array sinnvoll 
    bool vertical_aligned()   const;
    bool horizontal_aligned() const;

    // nur fuer type() == Pointer sinnvoll
    void dereference();

    // nur fuer type() == Array, StructOrClass, BaseClass sinnvoll
    void collapse ()  { myexpanded = false; }
    void expand ()    { myexpanded = true;  }

    // nur fuer type() == Array sinnvoll 
    void align_vertical ();
    void align_horizontal ();

    // true, wenn value wirklich neu war (sich also etwas veraendert)
    void update (string& value, bool& changed, bool& inited);
    bool new_BaseClass_name (string name);
};


#endif
