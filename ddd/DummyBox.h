// $Id$ 
// Deklaration Klasse DummyBox

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

#ifndef _Nora_DummyBox_h
#define _Nora_DummyBox_h

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
