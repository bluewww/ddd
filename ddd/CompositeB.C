// $Id$
// Klasse CompositeBox (Implementation)

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
const TagBox *CompositeBox::findTag(const BoxPoint& p) const
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
bool CompositeBox::matches (const Box &b, const Box *) const
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
