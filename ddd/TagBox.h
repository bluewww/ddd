// $Id$
// Deklaration Klasse TagBox

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
// Revision 1.1  1995/05/01 15:47:30  zeller
// Initial revision
//
// Revision 10.25  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.24  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.23  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.22  1994/07/19  18:42:15  zeller
// More changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.21  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.20  1994/02/23  15:07:43  zeller
// Neu: link = dup wird jetzt hier erledigt
//
// Revision 10.19  1994/02/19  20:53:25  zeller
// Neu: link()-Problem wird jetzt von MarkBox erledigt
//
// Revision 10.18  1994/02/19  20:51:24  zeller
// Neu: Kommentare auf englisch
//
// Revision 10.17  1994/02/19  20:48:15  zeller
// Fix: Initialisierung in Reihenfolge der Deklaration, nicht der
// Initialisierer
//
// Revision 10.16  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.15  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.14  1993/05/22  20:16:01  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.13  1993/04/22  10:52:00  zeller
// Lizenz verbessert
// 
// Revision 10.12  1993/04/21  00:11:10  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.11  1993/04/20  22:58:00  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.10  1993/04/16  11:32:01  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.9  1992/11/05  16:37:57  zeller
// Region-Kram jetzt in neuer Oberklasse MarkBox
// 
// Revision 10.8  1992/11/05  15:41:44  zeller
// Bei TagBox kann auch auf Datum verzichtet werden
// 
// Revision 10.7  1992/10/13  20:49:09  zeller
// strclass.h statt String.h
// 
// Revision 10.6  1992/07/22  10:52:41  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/02  02:56:09  zeller
// Neu: #pragma interface
// 
// Revision 10.4  1992/06/01  14:13:52  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.3  1992/06/01  12:54:11  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:06  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:41  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:49:38  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:30  zeller
// Installierte Version
// 
// Revision 1.23  1991/06/17  12:28:59  zeller
// TagBox jetzt von TransparentHatBox abgeleitet,
// um transparent matchen zu koennen (d.h. die TagBox ist unsichtbar)
// 
// Revision 1.22  1991/06/10  14:08:06  zeller
// Neu: DataLink mit Kopierfunktionen
// 
// Revision 1.21  1991/06/10  11:54:03  zeller
// Neu: info()
// 
// Revision 1.20  1991/06/09  17:37:40  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.19  1991/06/09  17:15:42  zeller
// Attribute Box jetzt in Box.C; tag() in findTag() umbenannt.
// 
// Revision 1.18  1991/05/09  15:05:30  zeller
// Warnungen vermieden
// 
// Revision 1.17  1991/02/08  16:29:21  zeller
// dump() neu definiert
// 
// Revision 1.16  1991/01/28  19:14:55  zeller
// free() jetzt nicht mehr inline
// 
// Revision 1.15  1991/01/28  19:08:47  zeller
// dup() eingefuehrt
// 
// Revision 1.14  1990/09/21  13:56:21  zeller
// Neu: Shortcut's region(), data(), name()
// 
// Revision 1.13  1990/09/18  11:39:33  zeller
// link() auf TagBox(const TagBox& tagbox) zurueckgefuehrt
// 
// Revision 1.12  1990/09/16  17:18:33  zeller
// Referenzzaehler _data eingefuehrt, um doppeltes Loeschen zu vermeiden
// 
// Revision 1.11  1990/09/16  13:01:29  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.10  1990/09/15  14:19:27  zeller
// Callback bei Destruktor eingefuehrt
// 
// Revision 1.9  1990/09/14  11:51:57  zeller
// Neu: selected, name wird jetzt ueber Funktionen bestimmt
// 
// Revision 1.8  1990/09/11  11:48:55  zeller
// selected-Flag eingefuehrt, damit display() ausgewaehlte
// Knoten invers anzeigt
// 
// Revision 1.7  1990/09/11  10:55:10  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.6  1990/09/07  14:30:55  zeller
// *** empty log message ***
// 
// Revision 1.5  1990/09/07  11:32:27  zeller
// name() ist jetzt vom Typ String
// 
// Revision 1.4  1990/09/07  11:29:28  zeller
// Neu: tag() ohne Parameter gibt oberste TagBox zurueck;
// altes tag() durch data() ersetzt;
// Komponente name() eingefuehrt
// 
// Revision 1.3  1990/09/06  10:38:46  zeller
// tag() gibt jetzt TagBox() zurueck
// (um auch auf Region zugreifen zu koennen)
// 
// Revision 1.2  1990/09/05  11:06:15  zeller
// Compilierfaehig gemacht
// 
// Revision 1.1  1990/09/05  10:55:13  zeller
// Initial revision
// 

#ifndef _Nora_TagBox_h
#define _Nora_TagBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "MarkBox.h"

// TagBox

class TagBox: public MarkBox {
public:
    DECLARE_TYPE_INFO

private:
    DataLink *datalink;         // Access functions
    Data *_data;                // Tag Data 

    // Default access functions
    static DataLink default_datalink;

protected:
    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc,
		       bool context_selected) const;

    // Copy
    TagBox(const TagBox& tagbox):
	MarkBox(tagbox._box),
	datalink(tagbox.datalink),
	_data((*(tagbox.datalink->link))(tagbox._data))
    {}

    void dump(ostream& s) const;

    const TagBox *findTag(BoxPoint p = BoxPoint(-1, -1)) const;

public:
    // Constructor
    TagBox(Box *box, Data *dta = 0, DataLink *dl = 0):
	MarkBox(box),
	datalink(dl == 0 ? &default_datalink : dl),
	_data(dta)
    {}

    Box *dup() const { return new TagBox(*this); }

    Box *link()
    {
	// When a tag box is drawn, it may be drawn in several regions.
	// To make sure all these are properly recorded, a tag box is
        // always duplicated.

	return dup();
    }

    // Destructor
    ~TagBox() { (*(datalink->unlink))(_data); }

    // Resources
    Data *__data() const        { return _data; }
    bool __selected() const  { return (*(datalink->selected))(_data); }
    string __name() const       { return (*(datalink->name))(_data); }
    string __info() const       { return (*(datalink->info))(_data); }
};

#endif
