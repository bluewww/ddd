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
	
// $Log$
// Revision 1.1  1995/05/01 15:47:35  zeller
// Initial revision
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.12  1994/02/19  20:20:02  zeller
// Neu: nchild() statt nson() (PC schlaegt zu)
//
// Revision 10.11  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/07/28  12:48:45  zeller
// Fix: delete[] statt delete
//
// Revision 10.9  1993/05/22  20:13:56  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/21  00:10:56  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/16  11:31:28  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/06/02  03:04:06  zeller
// #pragma besser formatiert
// 
// Revision 10.5  1992/06/01  16:33:44  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.4  1992/06/01  14:13:43  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.3  1992/05/20  15:36:54  zeller
// Dateinamen verkuerzt
// 
// Revision 10.2  1992/02/18  10:57:16  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:51  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:43:24  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:45  zeller
// Installierte Version
// 
// Revision 1.56  1991/06/09  17:15:55  zeller
// tag() in findTag() umbenannt
// 
// Revision 1.55  1991/06/09  16:00:54  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.54  1991/05/09  15:06:43  zeller
// Warnungen vermieden
// 
// Revision 1.53  1991/04/30  11:38:00  zeller
// Bei CompositeBox Groesse und Extend nicht vergleichen,
// da MatchBoxen die Groesse 0 haben
// 
// Revision 1.52  1991/04/27  17:30:37  zeller
// Neu: countMatchBoxes()
// 
// Revision 1.51  1991/02/26  20:29:16  zeller
// PEQ entfernt (Null-Pointer nicht mehr zugelassen)
// 
// Revision 1.50  1991/02/24  21:12:52  zeller
// matches: Abfrage this == &b entfernt (ueberfluessig)
// 
// Revision 1.49  1991/02/20  19:30:19  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.48  1991/02/18  12:37:08  zeller
// Neu: dumpComposite mit Argumenten fuer Listenausgabe
// 
// Revision 1.47  1991/02/08  16:30:43  zeller
// dump() neu definiert
// 
// Revision 1.46  1991/02/01  14:51:32  zeller
// AlignBoxen nach AlignBox.C ausgelagert;
// Align-Funktionalitaeten aus CompositeBox entfernt.
// 
// Revision 1.45  1991/02/01  13:29:51  zeller
// Umstellung: Extend sind jetzt Integer-Zahlen;
// Damit entfaellt 'nextensions';
// Umstellung: Default-Extend aller Composite-Boxes = 0
// 
// Revision 1.44  1991/01/28  21:11:16  zeller
// Neu: newFont() hat String als Parameter
// 
// Revision 1.43  1991/01/27  15:29:17  zeller
// Neu: string()
// 
// Revision 1.42  1991/01/27  15:14:39  zeller
// Unterstuetzung wechselnde Fonts eingefuehrt;
// Berechnung Groesse neu gegliedert (addSize, resize etc.)
// 
// Revision 1.41  1991/01/27  12:49:43  zeller
// resize() eingefuehrt
// 
// Revision 1.40  1990/09/16  15:10:06  zeller
// CompositeBox: Konstruktor, Destruktor, += inline gemacht
// 
// Revision 1.39  1990/09/16  14:43:35  zeller
// fastMode entfernt (null Effizienzgewinn)
// 
// Revision 1.38  1990/09/16  13:00:53  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.37  1990/09/14  18:08:52  zeller
// fastMode eingefuehrt (zum schnellen Berechnen)
// 
// Revision 1.36  1990/09/11  11:18:44  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.35  1990/09/10  20:18:08  zeller
// TAlignBox: Berechnung corner() korrigiert
// 
// Revision 1.34  1990/09/10  19:53:50  zeller
// Berechnung corner() bei VAlignBox::operator |= angepasst
// 
// Revision 1.33  1990/09/10  18:27:14  zeller
// TAlignBox::display(): Anpassung Position naechster Sohn vereinfacht
// 
// Revision 1.32  1990/09/09  22:58:07  zeller
// corner() auch fuer HAlignBox und UAlignBox eingefuehrt
// 
// Revision 1.31  1990/09/09  22:40:48  zeller
// VAlignBox::|= : corner() wird nur veraendert, wenn
// nicht-leere Box hinzugefuegt wird
// 
// Revision 1.30  1990/09/09  21:25:34  zeller
// verbesserte Alignment-Abfrage in TAlignBox
// 
// Revision 1.29  1990/09/09  20:53:09  zeller
// Neu: TAlignBox fuer Text-Alignment
// 
// Revision 1.28  1990/09/07  16:15:11  zeller
// Shortcut bei == eingefuehrt
// 
// Revision 1.27  1990/09/07  14:32:10  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.26  1990/09/07  11:29:44  zeller
// Neu: tag() ohne Parameter gibt oberste TagBox zurueck (hier: 0)
// 
// Revision 1.25  1990/09/06  10:38:53  zeller
// tag() gibt jetzt TagBox() zurueck
// (um auch auf Region zugreifen zu koennen)
// 
// Revision 1.24  1990/09/05  10:54:44  zeller
// Neu: tag()
// 
// Revision 1.23  1990/09/01  13:30:38  zeller
// 'CompBox' durch 'CompositeBox' ersetzt
// 
// Revision 1.22  1990/08/30  15:02:43  zeller
// CompBox::~CompBox() muss auch 'boxes' loeschen
// 
// Revision 1.21  1990/08/29  20:30:13  zeller
// BoxBag in CompositeBox eingefuegt (mit entsprechenden Konsequenzen)
// 
// Revision 1.20  1990/08/29  18:02:54  zeller
// Iterator entfernt; stattdessen Zugriff auf Soehne via [];
// dispAlign, dumpAlign zu Methoden der Klasse CompositeBox gemacht
// 
// Revision 1.19  1990/08/29  13:31:57  zeller
// size_debug eingefuehrt
// 
// Revision 1.18  1990/08/28  16:54:05  zeller
// Methode OK() eingefuehrt
// 
// Revision 1.17  1990/08/28  14:23:39  zeller
// Umstellung auf display() mit Region's
// 
// Revision 1.16  1990/08/25  17:49:25  zeller
// Pruefung auf nicht-Erweiterbarkeit eingefuehrt
// 
// Revision 1.15  1990/08/25  17:08:09  zeller
// unlink() eingefuehrt
// 
// Revision 1.14  1990/08/25  14:45:09  zeller
// Neues Link-Interface unterstuetzt
// 
// Revision 1.13  1990/08/25  14:20:10  zeller
// dup() entfernt
// 
// Revision 1.12  1990/08/25  13:27:30  zeller
// Link-Erhoehung deutlicher geklammert
// 
// Revision 1.11  1990/08/21  11:49:31  zeller
// Neu: Wichtige Funktionen aus CompositeBox in
// neue Klasse "BoxBag" ausgelagert;
// dup() eingefuehrt (noch eher ineffizient)
// 
// Revision 1.10  1990/08/20  10:00:30  zeller
// Ausgabe Box-Groesse bei dump() unterdrueckt
// 
// Revision 1.9  1990/08/19  12:18:02  zeller
// Neu: Soehne einer CompositeBox werden nicht mehr ueber
// next-Zeiger verwaltet, sondern ueber Zeiger-Array in der
// CompositeBox.
// Neu: Iterator "()" eingefuehrt.
// 
// Revision 1.8  1990/08/13  10:14:09  zeller
// Typennamen ueberarbeitet
// 
// Revision 1.7  1990/08/10  13:51:46  zeller
// Fehler Berechnung nextensions VAlignBox korrigiert
// 
// Revision 1.6  1990/08/10  12:20:25  zeller
// Operator += gibt jetzt korrekten Wert zurueck
// 
// Revision 1.5  1990/08/01  21:15:56  zeller
// Neufassung
// 
// Revision 1.4  1990/08/01  14:45:12  zeller
// Boxes werden jetzt ueber verkettete Zeiger verwaltet
// 
// Revision 1.3  1990/07/25  18:54:35  zeller
// _sccsid durch _sccsid[] ersetzt
// 
// Revision 1.2  1990/07/25  18:45:07  zeller
// _sccsid[] eingefuehrt
// 
// Revision 1.1  1990/07/25  18:32:25  zeller
// Initial revision
// 

char CompositeBox_rcsid[] = "$Id$";

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
