// $Id$ -*- C++ -*-
// A typedef for stream positions

// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2001 Universitaet des Saarlandes, Germany.
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

#ifndef _DDD_streampos_h
#define _DDD_streampos_h

// Older C++ releases defined `streampos' as a type for stream
// positions; in some compilers, this is implementation-defined.  

#include "config.h"
#include <iostream>
#include <fstream>

#if HAVE_STREAMPOS
#define STREAMPOS streampos
#elif HAVE_STD_STREAMPOS
// `streampos' is not visible, but `std::streampos' is.
#define STREAMPOS std::streampos
#else
// Use a simple `long' as replacement.
#define STREAMPOS long
#endif

#endif // _DDD_streampos_h
// DON'T ADD ANYTHING BEHIND THIS #endif
