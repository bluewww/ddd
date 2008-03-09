// $Id$
// Composite boxes

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

#ifndef _DDD_CompositeBox_h
#define _DDD_CompositeBox_h

// A CompositeBox is a box container.


#include "assert.h"
#include "Box.h"
#include "Widget.h"


// CompositeBox

class CompositeBox: public Box {
public:
    DECLARE_TYPE_INFO

private:
    int _size;              // Maximum number of children
    int _nchildren;         // Actual number of children
    Box **boxes;            // List of children
    void grow();            // Grow the list

    CompositeBox& operator = (const CompositeBox&);

protected:
    // Add box as child
    virtual void addChild(Box *b)
    {
	if (_nchildren >= _size)
	    grow();
	boxes[_nchildren++] = b->link();
    }

    // Direct child access
    Box *&_child(int nchild)
    {
	assert(nchild >= 0 && nchild < _nchildren);
	return boxes[nchild];
    }

    // Direct child access
    const Box *_child(int nchild) const
    {
	assert(nchild >= 0 && nchild < _nchildren);
	return boxes[nchild];
    }

    // Copy
    CompositeBox(const CompositeBox& box):
	Box(box), 
	_size(box._size),
	_nchildren(box._nchildren),
	boxes(new Box* [box._size])
    {
	// Copy children
	for (int i = 0; i < _nchildren; i++)
	    boxes[i] = box.boxes[i]->dup();
    }

    // Dump
    void dumpComposite(std::ostream& s, 
	const char *sep = ", ", const char *head = "(", const char *tail = ") ") const;

    bool matches (const Box &, const Box * = 0) const;

    const TagBox *findTag(const BoxPoint&) const;


public:
    // Konstruktor
    CompositeBox(unsigned initialSize = 2, const char *t = "CompositeBox"):
	Box(BoxSize(0, 0), BoxExtend(0, 0), t), 
	_size(initialSize), 
	_nchildren(0),
	boxes(new Box* [initialSize])
    {}

    // Destruktor
    ~CompositeBox()
    {
	for (int i = _nchildren - 1; i >= 0; i--)
	    boxes[i]->unlink();

	delete[] boxes;
    }

    // return string
    virtual string str() const;

    // Recompute size
    Box *resize();

    // propagate font
    void newFont(const string& font);

    // Resources
    int nchildren() const { return _nchildren; }
    Box *operator[] (int nchild) { return _child(nchild); }
    const Box *operator[] (int nchild) const { return _child(nchild); }

    void countMatchBoxes(int instances[]) const;

    bool OK() const;
};

#endif // _DDD_CompositeBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
