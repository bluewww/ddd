// $Id$
// Klasse CompositeBox (Deklaration)

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
	
// $Log$
// Revision 1.1  1995/05/01 15:47:35  zeller
// Initial revision
//
// Revision 10.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.17  1994/11/14  21:52:13  zeller
// Fix: copy constructor doesn't need a name space
//
// Revision 10.16  1994/11/14  19:12:22  zeller
// Fixes for gcc 2.6.1
//
// Revision 10.15  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.14  1994/02/19  20:25:38  zeller
// Neu: nchild() statt nson() (PC schlaegt zu)
//
// Revision 10.13  1993/12/07  16:56:21  zeller
// Fix: Reihenfolge Deklaration angepasst
//
// Revision 10.12  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.11  1993/07/28  12:48:26  zeller
// Fix: delete[] statt delete
//
// Revision 10.10  1993/05/22  20:14:00  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/21  00:10:57  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.8  1993/04/20  22:57:42  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.7  1993/04/16  11:31:29  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/07/22  10:52:24  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/01  16:33:46  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.4  1992/06/01  13:08:26  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.3  1992/06/01  12:53:33  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:18  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:52  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:43:43  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:50  zeller
// Installierte Version
// 
// Revision 1.42  1991/06/09  17:16:04  zeller
// tag() in findTag() umbenannt
// 
// Revision 1.41  1991/06/09  16:00:57  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.40  1991/05/20  18:09:52  zeller
// Reihenfolge Freigabe Boxen umgedreht (wg. ID-Debugging)
// 
// Revision 1.39  1991/05/09  15:06:51  zeller
// Warnungen vermieden
// 
// Revision 1.38  1991/04/27  17:30:47  zeller
// Neu: countMatchBoxes()
// 
// Revision 1.37  1991/04/27  16:18:57  zeller
// Neu: _son() gibt Referenz auf Sohn-Zeiger zurueck
// (fuer fiese kleine Manipulationen)
// 
// Revision 1.36  1991/02/20  19:30:27  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.35  1991/02/18  12:37:16  zeller
// Neu: dumpComposite mit Argumenten fuer Listenausgabe
// 
// Revision 1.34  1991/02/08  16:30:50  zeller
// dump() neu definiert
// 
// Revision 1.33  1991/02/01  14:52:01  zeller
// AlignBoxen nach AlignBox.h ausgelagert;
// Align-Funktionalitaeten aus CompositeBox entfernt.
// 
// Revision 1.32  1991/02/01  13:30:14  zeller
// CornerBox in AlignBox umbenannt;
// Umstellung: Extend sind jetzt Integer-Zahlen;
// Damit entfaellt 'nextensions';
// Umstellung: Default-Extend aller Composite-Boxes = 0
// 
// Revision 1.31  1991/01/28  21:11:23  zeller
// Neu: newFont() hat String als Parameter
// 
// Revision 1.30  1991/01/28  19:09:41  zeller
// dup() eingefuehrt
// 
// Revision 1.29  1991/01/27  15:29:24  zeller
// Neu: string()
// 
// Revision 1.28  1991/01/27  15:14:49  zeller
// Unterstuetzung wechselnde Fonts eingefuehrt;
// Berechnung Groesse neu gegliedert (addSize, resize etc.)
// 
// Revision 1.27  1991/01/27  12:49:49  zeller
// resize() eingefuehrt
// 
// Revision 1.26  1990/09/16  15:11:50  zeller
// Neue (einfachere) Konstruktoren fuer AlignBoxes
// (nur noch initialSize angeben);
// einfache Funktionen CompositeBox inline gemacht.
// 
// Revision 1.25  1990/09/16  13:00:58  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.24  1990/09/11  11:10:01  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.23  1990/09/09  22:58:20  zeller
// corner() auch fuer HAlignBox und UAlignBox eingefuehrt
// 
// Revision 1.22  1990/09/09  20:53:31  zeller
// Neu: TAlignBox fuer Text-Alignment
// 
// Revision 1.21  1990/09/07  14:32:26  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.20  1990/09/07  11:29:54  zeller
// Neu: tag() ohne Parameter gibt oberste TagBox zurueck
// 
// Revision 1.19  1990/09/06  10:39:01  zeller
// tag() gibt jetzt TagBox() zurueck
// (um auch auf Region zugreifen zu koennen)
// 
// Revision 1.18  1990/09/05  10:54:53  zeller
//  Neu: tag()
// 
// Revision 1.17  1990/09/01  13:30:42  zeller
// 'CompBox' durch 'CompositeBox' ersetzt
// 
// Revision 1.16  1990/08/29  20:30:40  zeller
// BoxBag in CompositeBox eingefuegt (mit entsprechenden Konsequenzen)
// 
// Revision 1.15  1990/08/29  18:03:09  zeller
// Iterator entfernt; stattdessen Zugriff auf Soehne via [];
// dispAlign, dumpAlign zu Methoden der Klasse CompositeBox gemacht
// 
// Revision 1.14  1990/08/28  16:54:14  zeller
// Methode OK() eingefuehrt
// 
// Revision 1.13  1990/08/28  14:22:41  zeller
// Umstellung auf display() mit Regions
// 
// Revision 1.12  1990/08/25  18:12:30  zeller
// Default-Extend von HAlignBox, VAlignBox so gewaehlt,
// dass beide gesetzt bzw. geloescht werden koennen
// 
// Revision 1.11  1990/08/25  14:20:19  zeller
// dup() entfernt
// 
// Revision 1.10  1990/08/21  11:47:01  zeller
// Wichtige Teile aus CompositeBox in neue Klasse
// "BoxBag" ausgelagert;
// dup() eingefuehrt
// 
// Revision 1.9  1990/08/19  12:18:12  zeller
// Neu: Soehne einer CompositeBox werden nicht mehr ueber
// next-Zeiger verwaltet, sondern ueber Zeiger-Array in der
// CompositeBox.
// Neu: Iterator "()" eingefuehrt.
// 
// Revision 1.8  1990/08/13  11:21:58  zeller
// #pragma once eingefuehrt
// 
// Revision 1.7  1990/08/13  10:14:12  zeller
// Typennamen ueberarbeitet
// 
// Revision 1.6  1990/08/10  12:37:30  zeller
//  Widget.h statt Intrinsic.h (schneller)
// 
// Revision 1.5  1990/08/10  12:20:28  zeller
// Operator += gibt jetzt korrekten Wert zurueck
// 
// Revision 1.4  1990/08/01  21:16:08  zeller
// Neufassung
// 
// Revision 1.3  1990/08/01  14:45:31  zeller
// Boxes werden jetzt ueber verkettete Zeiger verwaltet
// 
// Revision 1.2  1990/07/25  18:39:44  zeller
// ID korrigiert
// 
// Revision 1.1  1990/07/25  18:32:26  zeller
// Initial revision
// 

#ifndef _Nora_CompositeBox_h
#define _Nora_CompositeBox_h

#ifdef __GNUG__
#pragma interface
#endif

// Eine CompositeBox ist ein Behaelter fuer eine Anordnung von Boxen.


#include "assert.h"
#include <values.h>
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

    bool matches (const Box &b, const Box *callbackArg = 0) const;

    const TagBox *findTag(BoxPoint p = BoxPoint(-1, -1)) const;


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
