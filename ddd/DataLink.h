// $Id$
// Data Link

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _Nora_DataLink_h
#define _Nora_DataLink_h

#include "strclass.h"
#include "bool.h"

struct Data;			 // Opaque type

struct DataLink {
    Data *(*link)(Data *);	 // Duplicate data
    void (*unlink)(Data *);	 // Unlink data
    bool (*selected)(Data *); // Flag: is data selected?
    string (*name)(Data *);	 // Name of data
    string (*info)(Data *);	 // Debugging info of data
};

#endif
