// $Id$
// Deklaration Klasse TransparentHatBox

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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_TransparentHatBox_h
#define _DDD_TransparentHatBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine TransparentHatBox ist eine HatBox, die beim Matchen
// unsichtbar ist.

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

    virtual void dump(ostream& s) const { _box->dump(s); }

public:
    TransparentHatBox(Box *box):
	HatBox(box, "TransparentHatBox")
    {}
};

#endif
