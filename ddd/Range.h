// $Id$
// Klasse "Range" (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_Range_h
#define _DDD_Range_h

#include <iostream.h>

struct Range {
    int start;          // Beginn Bereich
    int end;            // Ende Bereich
    Range(int x = 0) {
	start = end = x;
    }
    Range(int s, int e) {
	start = s; end = e;
    }
    int operator == (Range r) {
	return start == r.start && end == r.end;
    }
    int operator != (Range r) {
	return start != r.start || end != r.end;
    }
    friend ostream& operator << (ostream& s, Range& range);
};

#endif
