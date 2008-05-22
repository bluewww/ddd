// $Id$
// Data Link

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_DataLink_h
#define _DDD_DataLink_h

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
