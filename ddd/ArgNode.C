// $Id$ 
// ArgNode class

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
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.12  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/12/14  12:51:17  zeller
// Neu: Funktions-Aufrufe (*p)() durch vereinfachte Syntax p() ersetzt
//
// Revision 10.10  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:16:30  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  11:07:53  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/21  15:17:56  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// 
// Revision 10.6  1993/04/21  00:27:48  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:44:19  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1993/01/30  16:26:24  zeller
// All messages after german oversitted
// 
// Revision 10.3  1992/11/03  19:37:07  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:11  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:19:40  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:55:57  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:01:47  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:37:11  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:52:43  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.19  1991/05/24  18:54:46  zeller
// Neu: von DummyNode abgeleitet (deshalb matchesAll entfernt);
// OK() entfernt (in VSLNode)
// 
// Revision 2.18  1991/05/24  11:03:39  zeller
// Debugging-Ausgabe instantiateArgs ausgeklammert
// 
// Revision 2.17  1991/05/24  10:08:58  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.16  1991/05/22  12:32:20  zeller
// Fehler 'Argument kann nicht isoliert werden' besser behandelt
// 
// Revision 2.15  1991/05/09  16:05:26  zeller
// Warnungen vermieden
// 
// Revision 2.14  1991/04/27  18:50:44  zeller
// MatchBoxes werden jetzt nicht mehr von ArgNode's erzeugt
// (erstens verwirrend, zweitens Probleme bei verschachtelten Funktionen),
// sondern MatchBoxes werden jetzt in VSLDef::compile_pattern() angelegt
// und hier wie sonstige Argumente auch durchgeschleust.
// 
// Revision 2.13  1991/04/27  17:29:32  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.12  1991/03/01  19:10:30  zeller
// Debugging instantiateArgs() entfernt (nicht hilfreich)
// 
// Revision 2.11  1991/03/01  19:08:36  zeller
// Neu: countInstances()
// 
// Revision 2.10  1991/03/01  18:13:04  zeller
// Neu: instantiateArgs()
// 
// Revision 2.9  1991/03/01  16:56:05  zeller
// Optimierungsfunktionen von 'VSLOptim' hierher eingefuegt;
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.8  1991/02/27  19:07:17  zeller
// Neues Matching-Interface eingefuehrt
// 
// Revision 2.7  1991/02/26  20:50:37  zeller
// ArgNode::eval muss wie alle eval-Funktionen einen link() zurueckegeben
// 
// Revision 2.6  1991/02/25  16:22:33  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.5  1991/02/24  19:03:26  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.4  1991/02/24  15:41:50  zeller
// Neu: MatchNode-Funktionen mit uebernommen
// (werden durch Setzen von ArgNode_matches aktiviert)
// 
// Revision 2.3  1991/02/20  18:50:17  zeller
// Neue Fehlerbehandlung: Bei Fehler NOT_A_BOX zurueckgeben.
// 0 steht jetzt fuer 'leere Liste' ([]).
// 
// Revision 2.2  1991/02/20  14:42:18  zeller
// Negative ID's abgeschaltet
// 
// Revision 2.1  1991/02/20  11:59:52  zeller
// Initial revision
// 

char ArgNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <strstream.h>
#include "assert.h"
#include "VSLLib.h"
#include "ArgNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(ArgNode, DummyNode)

// ArgNode

ArgNodeFunc ArgNode::matchCallback = 0;


// Argument auswerten
const Box *ArgNode::_eval(ListBox *arglist) const
{
    const Box *box = 0;

    if (arglist)
    {
	box = (*arglist)[_id];
	if (box)
	    box = ((Box *)box)->link();
    }

    if (box == 0)
    {
	ostrstream os;
	os << "arg" << _id << " cannot be isolated";
	VSLLib::eval_error(os);
    }

    return box;
}

// ArgNode ausgeben
void ArgNode::dump(ostream& s) const
{
    s << "arg" << _id;
}

// ...in Baum-Notation
void ArgNode::_dumpTree(ostream& s) const
{
    s << _id;
}

bool ArgNode::matches(const VSLNode &node) const
{
    bool ret = matchesAll ||
	VSLNode::matches(node) &&
	    _id == ((ArgNode *)&node)->_id; // dirty trick

    if (ret && matchCallback)
	matchCallback(_id, &node);

    return ret;
}


// Optimierung

// instantiateArgs: In Ausdruecken der Form f(arg1, arg2, ...) 
// arg1 .. argn durch konkrete Argumente ersetzen

int ArgNode::instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
    unsigned base, unsigned n)
// Knoten durch entsprechenden Ausdruck aus 'values' ersetzen
{
    assert(this == *node);

    // Wenn nicht im Bereich, abbrechen
    if (_id < base || _id >= base + n)
	return 0;

    VSLNode *source = values[_id];
    assert (source != 0);

    VSLNode *newNode = source->dup();
    *node = newNode;

#if 0
    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": instantiateArgs: replacing\n" 
	    << *this << "\nby " << *newNode << '\n';
	cout.flush();
    }
#endif

    // ArgNode loeschen
    delete this;

    return 1;
}

void ArgNode::countArgNodes(VSLDef *cdef, int instances[],
    unsigned base, unsigned n)
// Instanzen-Zaehler hinaufsetzen
{
    // Wenn nicht im Bereich, abbrechen
    if (_id < base || _id >= base + n)
	return;

    instances[_id]++;
}
