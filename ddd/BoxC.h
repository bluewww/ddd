// $Id$  -*- C++ -*-
// BoxCoordinate

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

// $Log$
// Revision 1.1  1995/05/01 15:47:31  zeller
// Initial revision
//
// Revision 1.2  1993/05/22  19:58:59  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.1  1993/04/21  00:16:40  zeller
// Initial revision
// 

#ifndef _Nora_BoxCoordinate_h
#define _Nora_BoxCoordinate_h

#ifdef __GNUG__
#pragma interface
#endif

#include <values.h>

typedef int BoxCoordinate;

const BoxCoordinate NoCoordinate = -MAXINT;

#endif // _Nora_BoxCoordinate_h
// DON'T ADD ANYTHING BEHIND THIS #endif
