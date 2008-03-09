// $Id$ 
// Matching boxes (for pattern matching)

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

#ifndef _DDD_MatchBox_h
#define _DDD_MatchBox_h


// A MatchBox is used in pattern matching.

// If `DummyBox::matchesAll' is set, a MatchBox matches any other box
// (otherwise, only the same MatchBox).  'MatchBox::matchCallback' is
// called after each successful match.


#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "DummyBox.h"

// MatchBox

typedef void (*MatchBoxFunc)(int, const Box *);


class MatchBox: public DummyBox {
public:
    DECLARE_TYPE_INFO

private:
    int _data;  // Associated data

protected:
    // Copy
    MatchBox(const MatchBox& box):
	DummyBox(box),
	_data(box._data)
    {}

    void dump(std::ostream& s) const;

    // Call matchCallback if matches
    bool matches (const Box &b, const Box *callbackArg = 0) const;

public:
    // Constructor
    MatchBox(int d, const char *t = "MatchBox"):
	DummyBox(t),
	_data(d)
    {}

    Box *dup() const { return new MatchBox(*this); }

    static MatchBoxFunc matchCallback;  // Match callback

    // Count match boxes
    void countMatchBoxes(int instances[]) const
    {
	instances[_data]++;
    }
};

#endif
