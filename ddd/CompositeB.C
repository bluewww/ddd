// $Id$
// Klasse CompositeBox (Implementation)

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

char CompositeBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "misc.h"
#include "CompositeB.h"

DEFINE_TYPE_INFO_1(CompositeBox, Box)

// CompositeBox

void CompositeBox::grow()
// vergroessert die Boxanzahl auf das Anderthalbfache
{
    unsigned newSize = _size + _size / 2 + 1;
    Box **newBoxes = new Box* [newSize];
    for (int i = 0; i < _nchildren; i++)
	newBoxes[i] = boxes[i];

    delete[] boxes;
    boxes = newBoxes;
    _size = newSize;
}

// Font propagieren
void CompositeBox::newFont(const string& font)
{
    for (int i = 0; i < nchildren(); i++)
    {
	Box* child = (*this)[i];
	child->newFont(font);
    }
    resize();
}

Box *CompositeBox::resize()
// Groesse neu berechnen
{
    for (int i = 0; i < nchildren(); i++)
    {
	Box* child = (*this)[i];
	child->resize();
    }
    return this;
}

// string aus Box bilden
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

// TagBox zu Punkt p in CompositeBox suchen
const TagBox *CompositeBox::findTag(BoxPoint p) const
{
    if (p != BoxPoint(-1, -1))
	for (int i = 0; i < nchildren(); i++)
	{
	    const Box *child = (*this)[i];
	    const TagBox *t = child->findTag(p);
	    if (t != 0)
		return t;   // gefunden
	}
    return 0; // nicht gefunden
}

// MatchBoxes zaehlen
void CompositeBox::countMatchBoxes(int instances[]) const
{
    for (int i = 0; i < nchildren(); i++)
    {
	const Box *child = (*this)[i];
	child->countMatchBoxes(instances);
    }
}

// Auf Gleichheit pruefen
bool CompositeBox::matches (const Box &b, const Box *callbackArg) const
{
    // Groesse und BoxExtend nicht vergleichen,
    // da MatchBoxen die Groesse 0 haben
    if (strcmp(type(), b.type()))
	return false;

    CompositeBox *c = (CompositeBox *)&b;   // dirty trick
    if (nchildren() != c->nchildren())
	return false;

    for (int i = 0; i < nchildren(); i++)
	if (*((*this)[i]) != *((*c)[i]))
	    return false;

    return true;
}

// Alignment ausgeben
void CompositeBox::dumpComposite(ostream& s, 
				 char *sep, char *head, char *tail) const
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


bool CompositeBox::OK() const
// Pruefen, ob alles in Ordnung
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
