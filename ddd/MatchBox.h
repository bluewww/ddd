// $Id$ 
// Deklaration Klasse MatchBox

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
// Revision 1.1  1995/05/01 15:47:47  zeller
// Initial revision
//
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.13  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.12  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.11  1993/05/22  20:15:19  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.10  1993/04/22  10:52:04  zeller
// Lizenz verbessert
// 
// Revision 10.9  1993/04/21  00:11:07  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.8  1993/04/20  22:57:53  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.7  1993/04/16  11:31:48  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/10/13  20:49:06  zeller
// strclass.h statt String.h
// 
// Revision 10.5  1992/07/22  10:52:35  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:55:59  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:53:54  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:30  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:03  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:47:16  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:52  zeller
// Installierte Version
// 
// Revision 1.13  1991/06/09  16:01:02  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.12  1991/05/24  18:36:31  zeller
// isMatchBox() durch isDummyBox() ersetzt
// 
// Revision 1.11  1991/05/24  18:25:41  zeller
// MatchBox jetzt von DummyBox abgeleitet
// 
// Revision 1.10  1991/05/09  15:08:00  zeller
// Warnungen vermieden
// 
// Revision 1.9  1991/04/27  17:30:56  zeller
// Neu: countMatchBoxes()
// 
// Revision 1.8  1991/02/27  18:44:19  zeller
// Steuerungsflags ueberarbeitet;
// Kommentierung verbessert
// 
// Revision 1.7  1991/02/27  03:19:55  zeller
// Neu: isMatchBox()
// 
// Revision 1.6  1991/02/25  14:12:55  zeller
// MatchBox ist jetzt leere ListBox
// 
// Revision 1.5  1991/02/20  19:30:55  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.4  1991/02/20  14:36:41  zeller
// MatchBox ist jetzt von ListBox abgeleitet,
// um auch ListBox matchen zu koenne
// 
// Revision 1.3  1991/02/18  19:57:24  zeller
// _matched global gemacht
// 
// Revision 1.2  1991/02/18  12:37:47  zeller
// Neu: Bei Matching Match-Funktion aufrufen
// 
// Revision 1.1  1991/02/17  16:20:02  zeller
// Initial revision
// 

#ifndef _Nora_MatchBox_h
#define _Nora_MatchBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine MatchBox wird -- fast ausschliesslich -- zum Pattern-Matching
// verwendet. Ist 'matchesAll' gesetzt, matcht die MatchBox jede
// andere Box (sonst nur eine gleiche). Ist 'matchCallback' gesetzt,
// wird eine Funktion nach jedem geglueckten Matchen aufgerufen.


#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "DummyBox.h"

// MatchBox

typedef void (*MatchBoxFunc)(int, const Box *);


class MatchBox: public DummyBox {
public:
    DECLARE_TYPE_INFO

private:
    int _data;  // Datum

protected:
    // Kopieren
    MatchBox(const MatchBox& box):
	DummyBox(box),
	_data(box._data)
    {}

    void dump(ostream& s) const;

    // Bei Vergleichen: Match-Funktion aufrufen
    bool matches (const Box &b, const Box *callbackArg = 0) const;

public:
    // Konstruktor
    MatchBox(int d, char *t = "MatchBox"):
	DummyBox(t),
	_data(d)
    {}

    Box *dup() const { return new MatchBox(*this); }

    static MatchBoxFunc matchCallback;  // Funktion bei Matching

    // MatchBoxen zaehlen
    void countMatchBoxes(int instances[]) const
    {
	instances[_data]++;
    }
};

#endif
