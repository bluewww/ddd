// $Id$ -*- C++ -*-
// Integer Array

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration environment.
// For details, see the ICE World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ice/',
// or send a mail to the ICE developers at `ice@ips.cs.tu-bs.de'.

#ifndef _ICE_IntArray_h
#define _ICE_IntArray_h

#ifdef __GNUG__
#pragma interface
#endif

#include "VarArray.h"
#include "DynArray.h"

typedef VarArray<int> VarIntArray;
typedef DynArray<int> DynIntArray;

typedef VarIntArray IntArray;

#endif // _ICE_IntArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
