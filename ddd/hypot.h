// $Id$ -*- C++ -*-
// hypot() declaration

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_hypot_h
#define _DDD_hypot_h

#include "config.h"

#if HAVE_HYPOT
#if HAVE_HYPOT_DECL
#include <math.h>
#else  // !HAVE_HYPOT_DECL
extern double hypot(double x, double y);
#endif // !HAVE_HYPOT_DECL
#else  // !HAVE_HYPOT
inline double hypot(double x, double y) { return sqrt (x * x + y * y); }
#endif // !HAVE_HYPOT

#endif // _DDD_hypot_h
// DON'T ADD ANYTHING BEHIND THIS #endif
