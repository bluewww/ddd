// $Id$
// Deklaration Klasse BinBox

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

#ifndef _DDD_BinBox_h
#define _DDD_BinBox_h

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
