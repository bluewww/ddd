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

//-----------------------------------------------------------------------------
// Funktionen zum Lesen von Variablenwerten in string-Darstellung
//-----------------------------------------------------------------------------

#ifndef _DDD_value_read_h
#define _DDD_value_read_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include <iostream.h>

// ***************************************************************************
enum DispValueType {
    Simple,			// Ordinary or other value
    Pointer,			// Pointer value
    Array,			// Array
    StructOrClass,		// Struct, Class, Record, Object
    BaseClass,			// C++ base class member
    Reference,			// C++ Reference.  2 children.
    List,			// List of values.  Last member may be text,
    Text			// Multi-line text.  Last member of a list.
};

ostream& operator<<(ostream& os, DispValueType type);

DispValueType determine_type (string value);

string read_token (string& value);
string read_simple_value (string& value, int depth);
string read_pointer_value (string& value);

// Bei Misserfolg false
bool read_array_begin (string& value);
bool read_array_next (string& value);
bool read_array_end (string& value);

// Bei Misserfolg false
bool read_str_or_cl_begin (string& value);
bool read_str_or_cl_next (string& value);
bool read_str_or_cl_end (string& value);
bool read_members_of_xy (string& value);

// Convert a DBX initial `dump' line into a readable format
void munch_dump_line (string& value);

// Bei Misserfolg ""
string read_vtable_entries (string& value);
string read_member_name (string& value);

// ***************************************************************************
// true, wenn name in spitzen Klammern (z.B.: "<>" oder "<Basis>") 
// 
bool is_BaseClass_name (const string& name);

// ***************************************************************************
// schneidet vom full_name die letzte Komponente ab
// d.h.: "blabla.<Basis>" wird zu "blabla"
// 
void cut_BaseClass_name (string& full_name);

#endif // _DDD_value_read_h
// DON'T ADD ANYTHING BEHIND THIS #endif
