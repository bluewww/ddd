// $Id$
// Klasse DiagBox (Deklaration)

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef _Nora_DiagBox_h
#define _Nora_DiagBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "PrimitiveB.h"

// DiagBox
class DiagBox: public PrimitiveBox {
public:
    DECLARE_TYPE_INFO

protected:
    DiagBox(const DiagBox& box):
	PrimitiveBox(box)
    {}

    void dump(ostream& s) const;

    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed,
		       GC gc, bool context_selected) const;

public:
    DiagBox(char *t = "DiagBox"):
	PrimitiveBox(BoxSize(0,0), BoxExtend(true, true), t) 
    {}

    Box *dup() const { return new DiagBox(*this); }

    virtual void _print(ostream& os, 
			const BoxRegion& region, 
			const BoxPrintGC& gc) const
    {
	// do nothing
    }
};

#endif
