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

// $Log$
// Revision 1.1  1995/05/01 15:47:32  zeller
// Initial revision
//
// Revision 1.4  1993/07/02  11:27:48  zeller
// Fix: #pragma interface entfernt
//
// Revision 1.3  1993/06/29  10:41:24  zeller
// Neudefinition: ANSI C++ forbids incrementing an enum
//
// Revision 1.2  1993/05/22  20:13:21  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.1  1993/04/20  23:38:13  zeller
// Initial revision
// 

#ifndef _Nora_BoxDimension_h
#define _Nora_BoxDimension_h

// Used to be an enum, but ANSI C++ forbids incrementing an enum
typedef int BoxDimension;

const BoxDimension X           = 0;
const BoxDimension Y           = 1;
const BoxDimension NDimensions = 2;

#endif // _Nora_BoxDimension_h
// DON'T ADD ANYTHING BEHIND THIS #endif
