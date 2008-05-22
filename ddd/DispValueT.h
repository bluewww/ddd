// $Id$ -*- C++ -*-
// DispValue Types

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_DispValueType_h
#define _DDD_DispValueType_h

#include <iostream>

// Various types of elements that can be read
enum DispValueType {
    UnknownType = 0,		// Unknown type
    Simple = 1,			// Ordinary or other value
    Pointer,			// Pointer value
    Array,			// Array
    Struct,		        // Struct, Class, Record, Object
    Reference,			// C++ Reference.  2 children.
    Sequence,			// Sequence of values.
    List,			// List of values.  Last member may be text.
    Text			// Multi-line text.  Last member of a list.
};

std::ostream& operator<<(std::ostream& os, DispValueType type);

#endif // _DDD_DispValueType_h
// DON'T ADD ANYTHING BEHIND THIS #endif
