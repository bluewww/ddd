// $Id$
// Klasse ListBox (Deklaration)

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
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:46  zeller
// Initial revision
//
// Revision 10.22  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.21  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.20  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.19  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.18  1994/07/19  18:42:54  zeller
// More changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.17  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.16  1994/02/19  20:20:30  zeller
// Neu: nchild() statt nson() (PC schlaegt zu)
//
// Revision 10.15  1993/11/11  13:37:53  zeller
// Fix: abort() meint globales abort()
//
// Revision 10.14  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.13  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.12  1993/05/22  20:15:04  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.11  1993/04/22  10:51:58  zeller
// Lizenz verbessert
// 
// Revision 10.10  1993/04/20  22:57:51  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.9  1993/04/16  11:31:46  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.8  1993/03/24  11:40:59  zeller
// Fix: abort() deklariert
// 
// Revision 10.7  1993/02/23  16:17:44  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.6  1992/07/22  10:52:34  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/02  02:55:57  zeller
// Neu: #pragma interface
// 
// Revision 10.4  1992/06/01  12:53:52  zeller
// #pragma once entfernt
// 
// Revision 10.3  1992/05/20  15:36:54  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:31  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:05  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:47:02  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:46  zeller
// Installierte Version
// 
// Revision 1.16  1991/07/05  13:36:39  zeller
// Neu: ListBox und alle abgeleiteten Boxen haben weder Groesse
// noch Ausdehnung
// 
// Revision 1.15  1991/06/10  14:08:22  zeller
// Neu: DataLink mit Kopierfunktionen
// 
// Revision 1.14  1991/06/10  11:54:17  zeller
// Neu: info()
// 
// Revision 1.13  1991/06/09  17:38:09  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.12  1991/06/09  17:16:59  zeller
// Neu: tag()
// 
// Revision 1.11  1991/06/09  16:01:06  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.10  1991/05/23  11:27:51  zeller
// Neu: uncons() macht cons() rueckgaengig;
// cons() gibt Parameter fuer uncons() zurueck
// 
// Revision 1.9  1991/05/20  18:25:31  zeller
// Operator += korrigiert
// 
// Revision 1.8  1991/05/09  15:08:10  zeller
// Warnungen vermieden
// 
// Revision 1.7  1991/05/05  17:23:29  zeller
// Neu: atom()
// 
// Revision 1.6  1991/04/27  16:19:41  zeller
// Neu: cons(), _head(), _tail()
// 
// Revision 1.5  1991/02/25  14:13:50  zeller
// Neu: leere Liste nicht mehr durch 0,
// sondern durch ListBox ohne Soehne dargestellt.
// 
// Revision 1.4  1991/02/24  21:17:27  zeller
//  Neu: eigenes matches() eingefuehrt
// (CompositeBox::matches() verglich nsons() => Fehler);
// Neu: OK()
// 
// Revision 1.3  1991/02/20  18:07:28  zeller
// dump() selbst definiert
// 
// Revision 1.2  1991/02/20  14:16:39  zeller
// Neu: isListBox()
// 
// Revision 1.1  1991/02/18  12:38:02  zeller
// Initial revision
// 

#ifndef _Nora_ListBox_h
#define _Nora_ListBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine ListBox ist eine Liste von Boxen.


#include <values.h>
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

    virtual void _draw(Widget w, 
		       const BoxRegion& r, 
		       const BoxRegion& exposed,
		       GC gc, 
		       bool context_selected) const
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
	for (int i = 0; !t->isEmpty(); i++)
	    t = t->tail();
	return i;
    }

    bool matches(const Box &b, const Box *callbackArg = 0) const;

    bool isListBox() const { return true; }

    void _print(ostream& os, const BoxRegion& r, const BoxPrintGC& gc) const
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
