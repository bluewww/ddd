// $Id$  -*- C++ -*-
// The Mark Box saves its region

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

#ifndef _DDD_MarkBox_h
#define _DDD_MarkBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "THatBox.h"


class MarkBox: public TransparentHatBox {
public:
    DECLARE_TYPE_INFO

protected:
    BoxRegion _region;          // Region of last draw

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
