// $Id$ 
// `Dummy' boxes (for pattern matching)

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

#ifndef _DDD_DummyBox_h
#define _DDD_DummyBox_h

// A DummyBox is used in pattern matching.

// If `DummyBox::matchesAll' is set, a DummyBox matches any other box
// (otherwise, only another DummyBox).


#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"

#include "ListBox.h"

// DummyBox

// A DummyBox is derived from ListBox, such that we can match Lists.

class DummyBox: public ListBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Copy
    DummyBox(const DummyBox& box):
	ListBox(box)
    {}

    void dump(std::ostream& s) const;

    bool matches (const Box &b, const Box *callbackArg = 0) const;

public:
    // Constructor
    DummyBox(const char *t = "DummyBox"):
	ListBox(t)
    {}

    Box *dup() const { return new DummyBox(*this); }

    static bool matchesAll;          // Flag: match all?

    bool isDummyBox() const { return true; }
};

#endif
