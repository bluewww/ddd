// $Id$
// Read variable values in string representation

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
enum DispValueType {Simple, Pointer, Array, StructOrClass, BaseClass};

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
