// $Id$
// Deklaration Klasse HFixBox, VFixBox

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

#ifndef _DDD_FixBox_h
#define _DDD_FixBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine HFixBox oder VFixBox setzt die Ausdehung ihres Sohnes auf 0.


#include "HatBox.h"



// FixBox

class FixBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    FixBox(const FixBox& box): HatBox(box)
    {}

public:
    FixBox(Box *box, char *t = "FixBox"): HatBox(box, t)
    {}
    
    // Must be overloaded by subclasses
    virtual Box *resize() = 0;
};



// HFixBox

class HFixBox: public FixBox {
public:
    DECLARE_TYPE_INFO

protected:
    HFixBox(const HFixBox& box): FixBox(box)
    {}
    void dump(ostream& s) const;

public:
    HFixBox(Box *box, char *t = "HFixBox"): FixBox(box, t)
    { theextend().fix(X); }

    Box *resize()
    {
	HatBox::resize();
	theextend().fix(X);

	return this;
    }

    Box *dup() const { return new HFixBox(*this); }
};


// VFixBox

class VFixBox: public FixBox {
public:
    DECLARE_TYPE_INFO

protected:
    VFixBox(const VFixBox& box): FixBox(box)
    {}
    void dump(ostream& s) const;

public:
    VFixBox(Box *box, char *t = "VFixBox"): FixBox(box, t)
    { theextend().fix(Y); }

    Box *resize()
    {
	HatBox::resize();
	theextend().fix(Y);

	return this;
    }

    Box *dup() const { return new VFixBox(*this); }
};

#endif
