// $Id$ -*- C++ -*-
// Auto-configure X

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
// Revision 1.1  1995/05/01 15:48:08  zeller
// Initial revision
//
// Revision 1.4  1995/04/30  11:36:10  zeller
// New: verbose mode
//
// Revision 1.3  1995/04/29  13:43:52  zeller
// New: verbose explicit checks
//
// Revision 1.2  1995/04/25  19:27:28  zeller
// New: auto_configure_X() takes the top-level widget as parameter
//
// Revision 1.1  1995/04/13  21:45:17  zeller
// Initial revision
//

#ifndef _DDD_xconfig_h
#define _DDD_xconfig_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "bool.h"

extern int check_x_configuration(Widget toplevel, bool verbose);

#endif // _DDD_xconfig_h
// DON'T ADD ANYTHING BEHIND THIS #endif
