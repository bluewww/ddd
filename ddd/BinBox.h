// $Id$
// Bin boxes

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

#ifndef _DDD_BinBox_h
#define _DDD_BinBox_h

// A BinBox sets the alignment of its single child to 1, if it is > 1.

#include "HatBox.h"


// BinBox

class BinBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    BinBox(const BinBox& box): HatBox(box)
    {}

    void dump(std::ostream& s) const;

public:
    BinBox(Box *box, const char *t = "BinBox"): HatBox(box, t)
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
