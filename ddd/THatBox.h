// $Id$
// A transparent hat box

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

#ifndef _DDD_TransparentHatBox_h
#define _DDD_TransparentHatBox_h

// A TransparentHatBox is a HatBox that is not considered during matching.

#include "HatBox.h"
#include "Widget.h"


// TransparentHatBox

class TransparentHatBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual const Box& matchMe() const { return _box->matchMe(); }

    TransparentHatBox(const TransparentHatBox& box):
	HatBox(box)
    {}

    virtual void dump(std::ostream& s) const { _box->dump(s); }

public:
    TransparentHatBox(Box *box):
	HatBox(box, "TransparentHatBox")
    {}
};

#endif
