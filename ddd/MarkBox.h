// $Id$  -*- C++ -*-
// The Mark Box saves its region

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_MarkBox_h
#define _DDD_MarkBox_h

#include "strclass.h"
#include "bool.h"
#include "mutable.h"
#include "Box.h"
#include "Widget.h"
#include "THatBox.h"


class MarkBox: public TransparentHatBox {
public:
    DECLARE_TYPE_INFO

protected:
    mutable BoxRegion _region;          // Region of last draw

    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc,
		       bool context_selected) const;

    // Copy Constructor
    MarkBox(const MarkBox& box):
	TransparentHatBox(box),
	_region(box._region)
    {}

public:
    // Constructor
    MarkBox(Box *box):
	TransparentHatBox(box), _region(BoxPoint(0,0), BoxSize(-1,-1))
    {}

    Box *dup() const { return new MarkBox(*this); }

    // Resources
    const BoxRegion& __region() const { return _region; }
};

#endif // _DDD_MarkBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
