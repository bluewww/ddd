// $Id$
// Deklaration Klasse HFixBox, VFixBox

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

#ifndef _Nora_FixBox_h
#define _Nora_FixBox_h

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
