// $Id$
// Deklaration Klasse HatBox

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
// Revision 1.1  1995/05/01 15:47:25  zeller
// Initial revision
//
// Revision 10.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.17  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.16  1994/07/19  20:53:30  zeller
// New: printing functions const-corrected
//
// Revision 10.15  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.14  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 10.13  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.12  1993/05/22  20:14:52  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.11  1993/04/22  10:52:05  zeller
// Lizenz verbessert
// 
// Revision 10.10  1993/04/20  22:57:50  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.9  1993/04/16  11:31:43  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.8  1993/02/23  16:17:44  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.7  1993/01/18  13:30:30  zeller
// Neu: box()
// 
// Revision 10.6  1992/07/22  10:52:33  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.5  1992/06/02  02:55:54  zeller
// Neu: #pragma interface
// 
// Revision 10.4  1992/06/01  14:13:45  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.3  1992/06/01  12:53:49  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:13  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:47  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:26  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:35  zeller
// Installierte Version
// 
// Revision 1.27  1991/06/23  09:49:47  zeller
// Korrektur findTag(): muss auch Wert zurueckgeben!
// 
// Revision 1.26  1991/06/20  18:23:01  zeller
// Neu: findTag() (war wohl irgendwann verlorengegangen...)
// 
// Revision 1.25  1991/06/17  12:30:03  zeller
// matches() prueft jetzt auf Identitaet (wie bei allen anderen
// Boxen auch), Transparenz gibt es in TransparentHatBox;
// 
// Revision 1.24  1991/06/09  17:37:57  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.23  1991/06/09  16:00:52  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.22  1991/05/07  15:58:25  zeller
// HatBox matcht jetzt auch HatBox
// 
// Revision 1.21  1991/04/30  16:03:49  zeller
// corner() wieder eingefuehrt
// 
// Revision 1.20  1991/04/27  17:30:28  zeller
// Neu: countMatchBoxes()
// 
// Revision 1.19  1991/02/24  15:10:08  zeller
// corner() entfernt
// 
// Revision 1.18  1991/02/20  19:30:06  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.17  1991/02/01  14:50:46  zeller
// FixBoxen nach FixBox.C ausgelagert
// 
// Revision 1.16  1991/02/01  13:28:55  zeller
// resize() uebernimmt jetzt auch den Extend des Sohnes
// 
// Revision 1.15  1991/01/28  21:11:10  zeller
// Neu: newFont() hat String als Parameter
// 
// Revision 1.14  1991/01/28  19:09:11  zeller
// dup() eingefuehrt
// 
// Revision 1.13  1991/01/28  13:58:42  zeller
// Bei resize() muss auch die HatBox ihre Groesse anpassen
// 
// Revision 1.12  1991/01/27  15:58:56  zeller
// _draw() protected gemacht (fuer FontBox)
// 
// Revision 1.11  1991/01/27  15:29:10  zeller
// Neu: string()
// 
// Revision 1.10  1991/01/27  15:14:10  zeller
// Unterstuetzung wechselnde Fonts eingefuehrt
// 
// Revision 1.9  1991/01/27  12:49:35  zeller
// resize() eingefuehrt
// 
// Revision 1.8  1990/09/16  13:01:07  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.7  1990/09/11  10:54:53  zeller
// display(): Parameter gc fuer Grafik-Kontext eingefuehrt
// 
// Revision 1.6  1990/09/09  21:54:21  zeller
// corner() eingefuegt
// 
// Revision 1.5  1990/09/07  16:14:58  zeller
// Shortcut bei == eingefuehrt
// 
// Revision 1.4  1990/09/07  14:31:59  zeller
// Neu: Unterstuetzung Operatoren ==, !=
// 
// Revision 1.3  1990/08/31  16:06:08  zeller
// Kleine Funktionen inline gemacht
// 
// Revision 1.2  1990/08/28  14:22:43  zeller
// Umstellung auf display() mit Regions
// 
// Revision 1.1  1990/08/25  17:09:06  zeller
// Initial revision
// 

#ifndef _Nora_HatBox_h
#define _Nora_HatBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine HatBox stuelpt sich wie ein Hut ueber eine andere Box,
// um bestimmte Eigenschaften zu veraendern 
// (insbesondere Groesse, Ausdehnung usw.).

#include "Box.h"
#include "Widget.h"


// HatBox

class HatBox: public Box {
public:
    DECLARE_TYPE_INFO

protected:
    Box *_box;

    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, GC gc,
		       bool context_selected) const;

    HatBox(const HatBox& box):
	Box(box), _box(box._box->link())
    {}

    bool matches (const Box &b, const Box *callbackArg = 0) const
    {
	return Box::matches(b) && *_box == *(((HatBox *)&b)->_box);
    }

    const TagBox *findTag(BoxPoint p = BoxPoint(-1, -1)) const
    {
	return _box->findTag(p);
    }

public:
    HatBox(Box *box, char *t = "HatBox"): 
	Box(box->size(), box->extend(), t), _box(box->link())
    {}
    ~HatBox() { _box->unlink(); }
    
    Box *box() { return _box; }

    Box *resize() { 
	_box->resize();
	thesize() = _box->size();
	theextend() = _box->extend();
	return this; 
    }
    void newFont(const string& font) { _box->newFont(font); resize(); }

    void countMatchBoxes(int instances[]) const
    {
	_box->countMatchBoxes(instances);
    }

    void _print(ostream& os, 
		const BoxRegion& region, 
		const BoxPrintGC& gc) const;

    BoxSize corner() const { return _box->corner(); }
    string str() const { return _box->str(); }
};

#endif
