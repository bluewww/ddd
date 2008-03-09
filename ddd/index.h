// $Id$ -*- C++ -*-
// Fast index function

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_index_h
#define _DDD_index_h

#include "strclass.h"

// Return index of R in S, starting with POS; PREFIX is the constant
// prefix of R.
extern int index(const string& s, const regex& r, 
		 const char *prefix, int pos = 0);

inline int index(const string& s, const regex& r, 
		 const string& prefix, int pos = 0){
  return index(s, r, prefix.chars(), pos);
}

#endif // _DDD_index_h
// DON'T ADD ANYTHING BEHIND THIS #endif
