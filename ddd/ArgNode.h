// $Id$
// Deklaration Klasse ArgNode

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
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/09/05  06:33:42  zeller
// Fix: copy constructors made const correct
//
// Revision 10.13  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.12  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:16:33  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:52  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:27:49  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:24  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:44:20  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/03  19:37:08  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.4  1992/10/13  20:57:48  zeller
// strclass.h statt String.h
// 
// Revision 10.3  1992/06/02  02:59:58  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:19  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:40  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:56:04  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:01:49  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:37:19  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:52:45  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.19  1991/05/24  18:55:06  zeller
// Neu: von DummyNode abgeleitet (deshalb matchesAll entfernt);
// OK() entfernt (in VSLNode)
// 
// Revision 2.18  1991/05/24  10:09:04  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.17  1991/05/23  18:53:34  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.16  1991/05/21  21:40:27  zeller
// Anpassungen fuer LetNode; nargs() neu definiert
// 
// Revision 2.15  1991/05/09  16:05:28  zeller
// Warnungen vermieden
// 
// Revision 2.14  1991/04/27  18:51:06  zeller
// MatchBoxes werden jetzt nicht mehr von ArgNode's erzeugt
// (erstens verwirrend, zweitens Probleme bei verschachtelten Funktionen),
// sondern MatchBoxes werden jetzt in VSLDef::compile_pattern() angelegt
// und hier wie sonstige Argumente auch durchgeschleust.
// 
// Revision 2.13  1991/04/27  17:29:34  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.12  1991/03/01  19:08:42  zeller
// Neu: countInstances()
// 
// Revision 2.11  1991/03/01  18:11:53  zeller
// Wieder da: instantiateArgs()
// 
// Revision 2.10  1991/03/01  16:54:56  zeller
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.9  1991/02/27  19:07:25  zeller
// Neues Matching-Interface eingefuehrt
// 
// Revision 2.8  1991/02/26  18:01:14  zeller
// id() eingefuehrt (fuer 'isStraight')
// 
// Revision 2.7  1991/02/26  14:12:06  zeller
// Neu: nargs()
// 
// Revision 2.6  1991/02/24  15:41:57  zeller
// Neu: MatchNode-Funktionen mit uebernommen
// (werden durch Setzen von ArgNode_matches aktiviert)
// 
// Revision 2.5  1991/02/20  19:50:47  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit
// 
// Revision 2.4  1991/02/20  18:50:45  zeller
// Neues Typenkonzept eingefuehrt
// 
// Revision 2.3  1991/02/20  14:42:27  zeller
// Negative ID's abgeschaltet
// 
// Revision 2.2  1991/02/20  12:10:12  zeller
// Beschreibung eingefuehrt
// 
// Revision 2.1  1991/02/20  11:59:53  zeller
// Initial revision
// 

#ifndef _Nora_ArgNode_h
#define _Nora_ArgNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein ArgNode wertet zur Laufzeit ein Argument aus der Argumentliste aus.

// ArgNode's koennen fuer Pattern-Matching verwendet werden. 
// Ist 'matchAll' gesetzt, so matcht ein ArgNode alle anderen Knoten.
// Ist 'matchCallback' gesetzt, wird beim Matchen eine Funktion aufgerufen,
// die den gematch'ten Knoten erhaelt.


#include "assert.h"
#include <iostream.h>
#include "strclass.h"

#include "VSLNode.h"
#include "DummyNode.h"


// ArgNode

typedef void (*ArgNodeFunc)(int, const VSLNode *);

class ArgNode: public DummyNode {
public:
    DECLARE_TYPE_INFO

private:
    unsigned _id;           // #Argument in arglist: erstes 0, zweites 1...

protected:
    ArgNode(const ArgNode& node):
	DummyNode(node), 
	_id(node._id)
    {}

    bool matches(const VSLNode &node) const;

    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

public:
    // ArgNode erzeugen
    ArgNode(unsigned i, char *type = "ArgNode"):
	DummyNode(type), _id(i)
    {}

    // ArgNode kopieren
    VSLNode *dup() const { return new ArgNode(*this); }

    // Auswerten
    const Box *_eval(ListBox *arglist) const;

    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);

    bool isConst() const   { return false; }
    bool isArgNode() const { return true; }

    unsigned id()       { return _id; }

    unsigned nargs() const    { return 1; }

    static ArgNodeFunc matchCallback; // Flag: bei Matching Funktion aufrufen?
};

#endif
