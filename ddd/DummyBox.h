// $Id$ 
// Deklaration Klasse DummyBox

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

#ifndef _DDD_DummyBox_h
#define _DDD_DummyBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine DummyBox wird -- fast ausschliesslich -- zum Pattern-Matching
// verwendet. Ist 'matchesAll' gesetzt, matcht die DummyBox jede
// andere Box (sonst nur eine andere DummyBox).


#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"

#include "ListBox.h"

// DummyBox

// Wir leiten eine DummyBox von einer ListBox ab, um mit ihr
// auch Listen matchen zu koennen.

class DummyBox: public ListBox {
public:
    DECLARE_TYPE_INFO

protected:
    // Kopieren
    DummyBox(const DummyBox& box):
	ListBox(box)
    {}

    void dump(ostream& s) const;

    bool matches (const Box &b, const Box *callbackArg = 0) const;

public:
    // Konstruktor
    DummyBox(char *t = "DummyBox"):
	ListBox(t)
    {}

    Box *dup() const { return new DummyBox(*this); }

    static bool matchesAll;          // Flag: alles Matchen?

    bool isDummyBox() const { return true; }
};

#endif
