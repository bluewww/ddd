// $Id$
// Read variable values in string representation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// Funktionen zum Lesen von Variablenwerten in string-Darstellung
//-----------------------------------------------------------------------------

#ifndef _value_read_h
#define _value_read_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"

// ***************************************************************************
enum DispValueType {
    Simple,			// Ordinary or other value
    Pointer,			// Pointer value
    Array,			// Array
    StructOrClass,		// `struct' or `class'
    BaseClass,			// C++ Base class member
    Reference			// C++ Reference
};

DispValueType determine_type (string value);

string read_token (string& value);
string read_simple_value (string& value);

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


#endif
