// $Id$ -*- C++ -*-
// Edge annotation

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

#ifndef _DDD_EdgeAnnotation_h
#define _DDD_EdgeAnnotation_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Box.h"
#include "GraphGC.h"

class EdgeAnnotation {
protected:
    virtual void _draw(Widget w, const BoxPoint& p,
		       const BoxRegion& exposed, const GraphGC& gc) const = 0;

public:
    DECLARE_TYPE_INFO

    virtual ~EdgeAnnotation() {};

    // Draw annotation centered around P
    virtual void draw(Widget w, const BoxPoint& p,
		      const BoxRegion& exposed,
		      const GraphGC& gc) const;

    // Custom function
    void draw(Widget w, const BoxPoint& p,
	      const BoxRegion& exposed = 
	      BoxRegion(BoxPoint(0,0), BoxSize(INT_MAX, INT_MAX)))
    {
	draw(w, p, exposed, GraphGC());
    }

    // Print
    virtual void _print(ostream& os, const BoxPoint& p, 
			const GraphGC& gc) const = 0;

    // Size used by annotation
    virtual BoxSize size(const GraphGC& gc) const = 0;

    // Region used by annotation
    virtual BoxRegion region(const BoxPoint& p, const GraphGC& gc) const = 0;

    // Duplication
    virtual EdgeAnnotation *dup() const = 0;
};

#endif // _DDD_EdgeAnnotation_h
// DON'T ADD ANYTHING BEHIND THIS #endif
