// $Id$  -*- C++ -*-
// BoxDimension

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

#ifndef _Nora_BoxDimension_h
#define _Nora_BoxDimension_h

// Used to be an enum, but ANSI C++ forbids incrementing an enum
typedef int BoxDimension;

const BoxDimension X           = 0;
const BoxDimension Y           = 1;
const BoxDimension NDimensions = 2;

#endif // _Nora_BoxDimension_h
// DON'T ADD ANYTHING BEHIND THIS #endif
