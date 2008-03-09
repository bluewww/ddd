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

char CompositeBox_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "misc.h"
#include "CompositeB.h"

DEFINE_TYPE_INFO_1(CompositeBox, Box)

// CompositeBox

// Grow by 1.5
void CompositeBox::grow()
{
    unsigned newSize = _size + _size / 2 + 1;
    Box **newBoxes = new Box* [newSize];
    for (int i = 0; i < _nchildren; i++)
	newBoxes[i] = boxes[i];

    delete[] boxes;
    boxes = newBoxes;
    _size = newSize;
}

// Propagate font
void CompositeBox::newFont(const string& font)
{
    for (int i = 0; i < nchildren(); i++)
    {
	Box* child = (*this)[i];
	child->newFont(font);
    }
    resize();
}

// Recompute size
Box *CompositeBox::resize()
{
    for (int i = 0; i < nchildren(); i++)
    {
	Box* child = (*this)[i];
	child->resize();
    }
    return this;
}

// Create string from Box
string CompositeBox::str() const
{
    string s("");
    for (int i = 0; i < nchildren(); i++)
    {
	const Box* child = (*this)[i];
	s += child->str();
    }
    return s;
}

// Find TagBox for point P
const TagBox *CompositeBox::findTag(const BoxPoint& p) const
{
    if (p != BoxPoint(-1, -1))
	for (int i = 0; i < nchildren(); i++)
	{
	    const Box *child = (*this)[i];
	    const TagBox *t = child->findTag(p);
	    if (t != 0)
		return t;   // found
	}
    return 0; // not found
}

// Count MatchBoxes
void CompositeBox::countMatchBoxes(int instances[]) const
{
    for (int i = 0; i < nchildren(); i++)
    {
	const Box *child = (*this)[i];
	child->countMatchBoxes(instances);
    }
}

// Check for equality
bool CompositeBox::matches (const Box &b, const Box *) const
{
    // Don't compare size and extend, as MatchBoxen have zero size
    if (strcmp(type(), b.type()))
	return false;

    const CompositeBox *c = (const CompositeBox *)&b;   // dirty trick
    if (nchildren() != c->nchildren())
	return false;

    for (int i = 0; i < nchildren(); i++)
	if (*((*this)[i]) != *((*c)[i]))
	    return false;

    return true;
}

// Dump
void CompositeBox::dumpComposite(std::ostream& s, 
				 const char *sep, const char *head, const char *tail) const
{
    s << head;
    for (int i = 0; i < nchildren(); i++)
    {
	const Box* child = (*this)[i];
	if (i > 0)
	    s << sep;
	s << *child;
    }
    s << tail;
}


// Representation invariant
bool CompositeBox::OK() const
{
    assert (boxes != 0);
    assert (_nchildren >= 0);
    assert (_size >= 0);
    assert (_nchildren <= _size);

    for (int i = 0; i < _nchildren; i++)
	assert (boxes[i]->OK());

    assert (Box::OK());

    return true;
}
