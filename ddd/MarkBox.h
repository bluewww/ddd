// $Id$  -*- C++ -*-
// The Mark Box saves its region

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
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_MarkBox_h
#define _Nora_MarkBox_h

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

#endif // _Nora_MarkBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
