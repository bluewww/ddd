// $Id$
// Box alignments

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

#ifndef _DDD_AlignBox_h
#define _DDD_AlignBox_h


// An AlignBox is a container for an alignment of boxes.

// A HAlignBox is a horizontal alignment (in VSL: &)
// A VAlignbox is a vertical alignment (in VSL: |),
// A UAlignBox places its members at the same place (in VSL: ^) 
// A TAlignBox aligns its members like text blocks, from left to right
// (in VSL: ~)


#include "assert.h"
#include "CompositeB.h"
#include "Widget.h"


// AlignBox

class AlignBox: public CompositeBox {
public:
    DECLARE_TYPE_INFO

protected:
    BoxSize _corner; // Space in lower right corner

    AlignBox(const AlignBox& box):
	CompositeBox(box), _corner(box._corner)
    {}

    // Take size from B
    void setSize(Box *b)
    {
	thesize()   = b->size();
	theextend() = b->extend();
	_corner     = b->corner();
    }

    // Add a new size
    virtual void addSize(Box *b) = 0;

    // Recompute size
    Box *resize();

    // Add B as child
    void addChild(Box *b)
    {
	// Add child
	CompositeBox::addChild(b);

	// Set or add size
	if (nchildren() == 1)
	    setSize(b);     // 1st child: set size
	else
	    addSize(b);     // later child: add size
    }

    // Draw it
    void drawAlign(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed,
		   GC gc, 
		   bool context_selected, 
		   BoxDimension dimen) const;

public:
    AlignBox(unsigned initialSize = 2, 
	const char *t = "AlignBox"):
	CompositeBox(initialSize, t), _corner(0,0)
    {}

    BoxSize corner() const { return _corner; }

    AlignBox& operator += (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// HAlignBox

class HAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

protected:
    HAlignBox(const HAlignBox& box):
	AlignBox(box)
    {}

    void dump(std::ostream& s) const { dumpComposite(s, " & "); }

    virtual void _draw(Widget w, 
		       const BoxRegion& r, 
		       const BoxRegion& exposed,
		       GC gc, bool context_selected) const
    { 
	drawAlign(w, r, exposed, gc, context_selected, X); 
    }

public:
    HAlignBox(unsigned initialSize = 2, const char *t = "HAlignBox"):
	AlignBox(initialSize, t) 
    {}

    Box *dup() const  { return new HAlignBox(*this); }
    Box *dup0() const { return new HAlignBox; }

    void addSize(Box *b);

    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const;

    HAlignBox& operator &= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// VAlignBox

class VAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& r, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const
    { 
	drawAlign(w, r, exposed, gc, context_selected, Y); 
    }

protected:
    VAlignBox(const VAlignBox& box):
	AlignBox(box)
    {}

    void dump(std::ostream& s) const { dumpComposite(s, " | "); }

public:
    VAlignBox(unsigned initialSize = 2, const char *t = "VAlignBox"):
	    AlignBox(initialSize, t)
    {}

    Box *dup() const  { return new VAlignBox(*this); }
    Box *dup0() const { return new VAlignBox; }

    void addSize(Box *b);

    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const;

    VAlignBox& operator |= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// UAlignBox

class UAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& region, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const;

protected:
    UAlignBox(const UAlignBox& box):
	AlignBox(box)
    {}

    void dump(std::ostream& s) const { dumpComposite(s, " ^ "); }

public:
    UAlignBox(unsigned initialSize = 2, const char *t = "UAlignBox"):
	AlignBox(initialSize, t)
    {}

    Box *dup() const  { return new UAlignBox(*this); }
    Box *dup0() const { return new UAlignBox; }

    void addSize(Box *b);

    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const;

    UAlignBox& operator ^= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


// TAlignBox

class TAlignBox: public AlignBox {
public:
    DECLARE_TYPE_INFO

private:
    void _draw(Widget w, 
	       const BoxRegion& region, 
	       const BoxRegion& exposed,
	       GC gc, 
	       bool context_selected) const;

protected:
    TAlignBox(const TAlignBox& box):
	AlignBox(box)
    {}

    void dump(std::ostream& s) const { dumpComposite(s, " ~ "); }

public:
    TAlignBox(unsigned initialSize = 2, const char *t = "TAlignBox"):
	AlignBox(initialSize, t)
    {}

    Box *dup() const  { return new TAlignBox(*this); }
    Box *dup0() const { return new TAlignBox; }

    void addSize(Box *b);

    virtual void _print(std::ostream& os, 
			const BoxRegion& region, 
			const PrintGC& gc) const;

    TAlignBox& operator &= (Box *b)
    {
	addChild(b);
	return *this;
    }
};


#endif
