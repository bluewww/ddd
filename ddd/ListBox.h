// $Id$
// Box lists

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_ListBox_h
#define _DDD_ListBox_h

#ifdef __GNUG__
#pragma interface
#endif


// A ListBox is a list of boxes.


#include <limits.h>
#include <stdlib.h>

#include "CompositeB.h"
#include "assert.h"


// ListBox

class ListBox: public CompositeBox {
public:
    DECLARE_TYPE_INFO

private:
    const ListBox *_last; // last empty box

    Box     *&_head()   { return _child(0); }
    Box     *&_tail()   { return _child(1); }

    ListBox& operator = (const ListBox&) { assert(0); return *this; }

public:
    const Box *head() const 
    { 
	return CompositeBox::operator[](0); 
    }
    const ListBox *tail() const 
    { 
	return (const ListBox *)CompositeBox::operator[](1);
    }
    Box *head()
    { 
	return CompositeBox::operator[](0); 
    }
    ListBox *tail()
    { 
	return (ListBox *)CompositeBox::operator[](1);
    }

    bool isEmpty() const { return nchildren() == 0; }

protected:
    // Recompute _last
    void _relast();

    // Recompute _last if needed
    void relast()
    {
	if (_last == 0 || !_last->isEmpty())
	    _relast();
    }

    // Copy
    ListBox(const ListBox& box):
	CompositeBox(box), _last(0)
    {}

    virtual void _draw(Widget, 
		       const BoxRegion&, 
		       const BoxRegion&,
		       GC, 
		       bool) const
    {
	assert(0);  // A ListBox cannot be drawn
	::abort();
    }

    void dump(ostream& s) const;

public:
    // Constructor: head and tail
    ListBox(Box *hd, ListBox *tl, char *t = "ListBox"):
	CompositeBox(2, t), _last(tl->_last)
    {
	thesize()   = BoxSize();
	theextend() = BoxExtend();

	addChild(hd);
	addChild(tl);
    }

    // Constructor: empty list
    ListBox(char *t = "ListBox"):
	CompositeBox(2, t), _last(this)
    {
	thesize()   = BoxSize();
	theextend() = BoxExtend();
    }

    Box *dup() const { return new ListBox(*this); }

    Box *tag(Data *data, DataLink *datalink = 0);

    ListBox& operator += (Box *b)
    {
	relast();

	ListBox *l = new ListBox;   // Append empty list
	((ListBox *)_last)->addChild(b); // head
	((ListBox *)_last)->addChild(l); // tail

	l->unlink();    // possible, since addChild makes a new link
	_last = l;

	return *this;
    }
    
    const ListBox *sublist(int start = 1)
    {
	const ListBox *t = this;
	for (int i = 0; !t->isEmpty() && i < start; i++)
	    t = t->tail();

	return t;
    }

    const Box *operator [] (int nchild) 
    {
	const ListBox *t = sublist(nchild);
	return t->isEmpty() ? 0 : t->head();
    }

    int length()
    {
	const ListBox *t = this;
	int i;
	for (i = 0; !t->isEmpty(); i++)
	    t = t->tail();
	return i;
    }

    bool matches(const Box &b, const Box *callbackArg = 0) const;

    bool isListBox() const { return true; }

    void _print(ostream&, const BoxRegion&, const PrintGC&) const
    {
	assert(0);  // Cannot print lists
	::abort();
    }

    // Return first atom in list
    Box *atom();

    bool OK() const;

    // Append list (only if *this is non-empty)
    ListBox* cons(ListBox *b);

    // Detach a list that was attached with cons()
    void uncons(ListBox *attach);
};

#endif
