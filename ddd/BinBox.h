// $Id$
// Deklaration Klasse BinBox

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

#ifndef _Nora_BinBox_h
#define _Nora_BinBox_h

#ifdef __GNUG__
#pragma interface
#endif

// Eine BinBox setzt die Ausdehung ihres Sohnes auf 1, wenn sie > 1 ist.


#include "HatBox.h"


// BinBox

class BinBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    BinBox(const BinBox& box): HatBox(box)
    {}

    void dump(ostream& s) const;

public:
    BinBox(Box *box, char *t = "BinBox"): HatBox(box, t)
    { 
	theextend()[X] = extend(X) > 0;
	theextend()[Y] = extend(Y) > 0;
    }

    Box *resize()
    {
	HatBox::resize();
	theextend()[X] = extend(X) > 0;
	theextend()[Y] = extend(Y) > 0;

	return this;
    }

    Box *dup() const { return new BinBox(*this); }
};

#endif
