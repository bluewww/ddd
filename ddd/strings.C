// $Id$ -*- C++ -*-
// Some DDD strings

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

// $Log$
// Revision 1.1  1995/05/01 15:47:14  zeller
// Initial revision
//
// Revision 1.1  1995/04/18  16:18:06  zeller
// Initial revision
//

static const char rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "strings.h"

String ddd_man_page =
""
#include "ddd.man.C"
"";

String ddd_fallback_resources[] = {
#include "Ddd.ad.h"
0
};
