// $Id$ -*- C++ -*-
// Printing context

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_PrintGC_h
#define _DDD_PrintGC_h

#ifdef __GNUG__
#pragma interface
#endif

#include "bool.h"
#include "TypeInfo.h"

// Printing environment
struct PrintGC {
    DECLARE_TYPE_INFO

    virtual bool isFig() const        { return false; }
    virtual bool isPostScript() const { return false; }
};

struct FigPrintGC: public PrintGC {
    DECLARE_TYPE_INFO

    virtual bool isFig() const        { return true; }

    FigPrintGC() {}
};

struct PostScriptPrintGC: public PrintGC {
    DECLARE_TYPE_INFO
    enum Orientation { PORTRAIT, LANDSCAPE };

    virtual bool isPostScript() const { return true; }

    int hsize;			// Maximum size of graph in 1/72" (Portrait)
    int vsize;
    int hoffset;		// Lower left corner of graph
    int voffset;		// on page in 1/72" (Portrait)

    Orientation orientation;	// Whether graph should be rotated
    bool color;			// Whether to print in color

    // Default: let params fit to an A4-sized page
    PostScriptPrintGC()
        : hsize(510), vsize(794), 
	  hoffset(42), voffset(35), 
	  orientation(PORTRAIT),
	  color(false)
    {}
};

#endif // _DDD_PrintGC_h
// DON'T ADD ANYTHING BEHIND THIS #endif
