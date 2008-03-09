// $Id$
// A box with a single child

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

#ifndef _DDD_HatBox_h
#define _DDD_HatBox_h

// A HatBox envelops a child (like a hat) to change specific
// properties such as size or extent.

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
	return Box::matches(b) && *_box == *(((const HatBox *)&b)->_box);
    }

    const TagBox *findTag(const BoxPoint& p) const
    {
	return _box->findTag(p);
    }

private:
    HatBox& operator = (const HatBox&);

public:
    HatBox(Box *box, const char *t = "HatBox"): 
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

    void _print(std::ostream& os, 
		const BoxRegion& region, 
		const PrintGC& gc) const;

    BoxSize corner() const       { return _box->corner(); }
    string str() const           { return _box->str(); }
};

#endif
