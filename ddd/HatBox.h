// $Id$
// Deklaration Klasse HatBox

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

#ifndef _DDD_HatBox_h
#define _DDD_HatBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine HatBox stuelpt sich wie ein Hut ueber eine andere Box,
// um bestimmte Eigenschaften zu veraendern 
// (insbesondere Groesse, Ausdehnung usw.).

#include "Box.h"
#include "Widget.h"


// HatBox

class HatBox: public Box {
public:
    DECLARE_TYPE_INFO

protected:
    Box *_box;

    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, GC gc,
		       bool context_selected) const;

    HatBox(const HatBox& box):
	Box(box), _box(box._box->link())
    {}

    bool matches (const Box &b, const Box * = 0) const
    {
	return Box::matches(b) && *_box == *(((HatBox *)&b)->_box);
    }

    const TagBox *findTag(const BoxPoint& p) const
    {
	return _box->findTag(p);
    }

public:
    HatBox(Box *box, char *t = "HatBox"): 
	Box(box->size(), box->extend(), t), _box(box->link())
    {}
    ~HatBox() { _box->unlink(); }
    
    Box *box() { return _box; }

    Box *resize() { 
	_box->resize();
	thesize() = _box->size();
	theextend() = _box->extend();
	return this; 
    }
    void newFont(const string& font) { _box->newFont(font); resize(); }

    void countMatchBoxes(int instances[]) const
    {
	_box->countMatchBoxes(instances);
    }

    void _print(ostream& os, 
		const BoxRegion& region, 
		const BoxPrintGC& gc) const;

    BoxSize corner() const       { return _box->corner(); }
    string str() const           { return _box->str(); }
};

#endif
