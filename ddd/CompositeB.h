// $Id$
// Klasse CompositeBox (Deklaration)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _Nora_CompositeBox_h
#define _Nora_CompositeBox_h

#ifdef __GNUG__
#pragma interface
#endif

// Eine CompositeBox ist ein Behaelter fuer eine Anordnung von Boxen.


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

protected:
    // Box als Kind hinzufuegen
    virtual void addChild(Box *b)
    {
	// Kind in Array eintragen
	if (_nchildren >= _size)
	    grow();
	boxes[_nchildren++] = b->link();
    }

    // Direkt-Zugriff auf Kind
    Box *&_child(int nchild) {
	assert(nchild >= 0 && nchild < _nchildren);
	return boxes[nchild];
    }
    // Direkt-Zugriff auf Kind
    const Box *_child(int nchild) const {
	assert(nchild >= 0 && nchild < _nchildren);
	return boxes[nchild];
    }

    // CompositeBox kopieren
    CompositeBox(const CompositeBox& box):
	Box(box), 
	_size(box._size),
	_nchildren(box._nchildren),
	boxes(new Box* [box._size])
    {
	// Soehne kopieren und eintragen
	for (int i = 0; i < _nchildren; i++)
	    boxes[i] = box.boxes[i]->dup();
    }

    // CompositeBox ausgeben
    void dumpComposite(ostream& s, 
	char *sep = ", ", char *head = "(", char *tail = ") ") const;

    bool matches (const Box &, const Box * = 0) const;

    const TagBox *findTag(const BoxPoint&) const;


public:
    // Konstruktor
    CompositeBox(unsigned initialSize = 2, char *t = "CompositeBox"):
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

    // string zurueckgeben
    virtual string str() const;

    // Groesse neu berechnen
    Box *resize();

    // Font propagieren
    void newFont(const string& font);

    // Ressourcen
    int nchildren() const { return _nchildren; }
    Box *operator[] (int nchild) { return _child(nchild); }
    const Box *operator[] (int nchild) const { return _child(nchild); }

    void countMatchBoxes(int instances[]) const;

    bool OK() const;

    // Obsolete calls
    int nsons() const { return nchildren(); }
};

#endif // _Nora_CompositeBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
