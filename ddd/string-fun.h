// $Id$
// Miscellaneous string functions

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
// Verschiedene Funktionen auf string
//-----------------------------------------------------------------------------

#ifndef _string_fun_h
#define _string_fun_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Nora-Zeugs
#include "strclass.h"

//-----------------------------------------------------------------------------

extern regex RXblanks;
extern regex RXblanks_or_tabs;


// ***************************************************************************
string itostring (int nr);

// ***************************************************************************
// gibt den Integer zurueck, der am Anfang des String steht
// 
int get_nr (/*const*/ string& s);

// ***************************************************************************
int get_positive_nr (const char* s);

// ***************************************************************************
int get_positive_nr (/*const*/ string& s);

// ***************************************************************************
void read_leading_blanks (string& value);

// ***************************************************************************
// gibt den Nummer-String zurueck, der am Anfang des String steht und
// loescht diesen aus dem uebergebenen String.
// 
string read_nr_str (string& s);

// ***************************************************************************
int read_positive_nr (string& s);

// Strip final characters
void strip_final_blanks(string& text);
void strip_final_newlines(string& text);

#endif
