// $Id$
// Klasse ListBox (Deklaration)

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

#ifndef _Nora_ListBox_h
#define _Nora_ListBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine ListBox ist eine Liste von Boxen.


#include <limits.h>
#include <stdlib.h>

#include "CompositeB.h"
#include "assert.h"


// ListBox

class ListBox: public CompositeBox {
public:
    DECLARE_TYPE_INFO

private:
    const ListBox *_last; // letzte leere Box

    Box     *&_head()   { return _child(0); }
    Box     *&_tail()   { return _child(1); }

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
    // _last neu berechnen
    void _relast();

    // _last ggf. neu berechnen
    void relast()
    {
	if (_last == 0 || !_last->isEmpty())
	    _relast();
    }

    // ListBox kopieren
    ListBox(const ListBox& box):
	CompositeBox(box), _last(0)
    {}

    virtual void _draw(Widget, 
		       const BoxRegion&, 
		       const BoxRegion&,
		       GC, 
		       bool) const
    {
	assert(0);  // ListBox kann nicht gezeichnet werden
	::abort();
    }

    void dump(ostream& s) const;

public:
    // Konstruktor: Kopf und Schwanz
    ListBox(Box *hd, ListBox *tl, char *t = "ListBox"):
	CompositeBox(2, t), _last(tl->_last)
    {
	thesize() = NoSize;
	theextend() = NoExtend;

	addChild(hd);
	addChild(tl);
    }

    // Konstruktor: Leere Liste
    ListBox(char *t = "ListBox"):
	CompositeBox(2, t), _last(this)
    {
	thesize() = NoSize;
	theextend() = NoExtend;
    }

    Box *dup() const { return new ListBox(*this); }

    Box *tag(Data *data, DataLink *datalink = 0);

    ListBox& operator += (Box *b)
    {
	relast();

	ListBox *l = new ListBox;   // Leere Liste anhaengen
	((ListBox *)_last)->addChild(b); // head
	((ListBox *)_last)->addChild(l); // tail

	l->unlink();    // geht, da addChild neuen Link macht
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

    void _print(ostream&, const BoxRegion&, const BoxPrintGC&) const
    {
	assert(0);  // ListBox kann nicht gedruckt werden
	::abort();
    }

    // Erstes Atom zurueckgeben
    Box *atom();

    bool OK() const;

    // Liste anhaengen (nur, wenn *this nicht leer ist!)
    ListBox* cons(ListBox *b);

    // Liste abhaengen, die mit cons() angehaengt wurde
    void uncons(ListBox *attach);
};

#endif
