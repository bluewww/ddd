// $Id$
// Implementation Klasse VSLNode

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
// Revision 1.1  1995/05/01 15:47:55  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:18:11  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  11:07:36  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:28:16  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:44:49  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:34  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/01  16:51:35  zeller
// Neu: #pragma implementation
// 
// Revision 10.2  1992/06/01  14:58:48  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.1  1992/02/18  11:19:27  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  06:00:07  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:35  zeller
// Installierte Version
// 
// Revision 4.3  1991/07/02  16:03:43  zeller
// Default-Tiefe bei dumpTree jetzt 1
// 
// Revision 4.2  1991/07/02  14:35:10  zeller
// Neu: dumpTree()
// 
// Revision 4.1  1991/05/25  12:51:55  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.13  1991/05/23  11:56:26  zeller
// -Dshow_tiny_eval: Vor Einstieg in Ausdruck Ausdruck angeben;
// Aktuelle Argumentliste ausgeben
// 
// Revision 2.12  1991/05/09  16:04:48  zeller
// Warnungen vermieden
// 
// Revision 2.11  1991/02/27  17:46:53  zeller
// Doppelseitiges Matchen eingefuehrt
// 
// Revision 2.10  1991/02/25  16:20:31  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.9  1991/02/24  19:19:35  zeller
// show-eval: Ausgabe 'Tiny eval' weggelassen
// 
// Revision 2.8  1991/02/24  19:07:34  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.7  1991/02/21  15:20:30  zeller
// Neue VSE-Flags eingefuehrt
// 
// Revision 2.6  1991/02/21  14:20:49  zeller
// Debugging: \n vorneangestellt
// 
// Revision 2.5  1991/02/21  11:47:43  zeller
// Matching: Debugging-Ausgabe verbessert
// 
// Revision 2.4  1991/02/20  19:50:10  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit
// 
// Revision 2.3  1991/02/20  18:41:51  zeller
// Fall 'box == 0' als 'leere Liste' behandelt;
// Neues Typenkonzept eingefuehrt
// 
// Revision 2.2  1991/02/20  11:56:04  zeller
// Jeder Klasse eigene Datei zugewiesen
// 
// Revision 2.1  1991/02/19  22:46:14  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.44  1991/02/17  17:29:22  zeller
// Neue Auswertungsfunktionen VSLLib eingefuehrt
// 
// Revision 1.43  1991/02/14  14:01:19  zeller
// TestNode jetzt Erbe von CallNode
// (fuer Argument-Verwaltung, Konstantenbildung usw.)
// 
// Revision 1.42  1991/02/11  19:25:19  zeller
// DefListCallNode entfernt
// (Aufgaben werden jetzt von DefCallNode wahrgenommen)
// 
// Revision 1.41  1991/02/11  17:17:29  zeller
// arg-range nur noch auf Wunsch ausgeben;
// inFix-Funktionen nur dann, wenn mindestens 2 Argumente vorhanden;
// Namen ohne '$' zu Beginn verwenden.
// 
// Revision 1.40  1991/02/11  16:53:34  zeller
// callee_name entfernt;
// dump() erweitert;
// BuiltinCallNode vereinfacht.
// 
// Revision 1.39  1991/02/10  16:19:35  zeller
// Auch Kopier-Funktionen muessen 'references' mitzaehlen
// 
// Revision 1.38  1991/02/10  15:49:21  zeller
// 'references' eingefuehrt
// 
// Revision 1.37  1991/02/10  14:20:03  zeller
// Optimierungsfunktionen nach VSLOptim.C ausgelagert
// 
// Revision 1.36  1991/02/10  13:35:09  zeller
// VSEFlags eingefuehrt
// 
// Revision 1.35  1991/02/08  18:29:30  zeller
// dump() durch << ersetzt;
// const_debug eingefuehrt
// 
// Revision 1.34  1991/02/08  17:09:15  zeller
// Alle Optimierungsfunktionen geben die #Aenderungen zurueck
// 
// Revision 1.33  1991/02/08  13:52:40  zeller
// inlineFunc-Optimierung auf solche Funktionen eingeschraenkt,
// die ihre Argumente maximal 1x auswerten (sofern das Argument nicht
// konstant oder Parameter ist)
// 
// Revision 1.32  1991/02/08  12:52:02  zeller
// instantiateArgs: Debugging eingefuehrt;
// instantiateArgs: Wenn keine Argumente vorhanden, nichts ersetzen
// 
// Revision 1.31  1991/02/07  18:34:44  zeller
// resolveSynonyms: Debugging-Ausgabe eingefuegt
// 
// Revision 1.30  1991/02/06  20:55:36  zeller
// instantiateArgs() bekommt jetzt #Argumente uebergeben;
// Regeln zum Herausholen der passenden Argumente verbessert
// (aber immer noch nicht vollkommen)
// 
// Revision 1.29  1991/02/06  19:46:45  zeller
// TestNode: Optimierungen an Argumente weitergeben;
// Assertion's instantiateArgs korrigiert;
// Testausgaben jeweils geflush'ed.
// 
// Revision 1.28  1991/02/06  18:16:17  zeller
// Optimierung-Debugging eingefuehrt;
// inlineFunc-Optimierung eingefuehrt
// 
// Revision 1.27  1991/02/02  12:45:15  zeller
// Neu: Teiloptimierung assoziativer Funktionen
// 
// Revision 1.26  1991/01/29  14:55:25  zeller
// Kontrolle ueber Seiteneffekte eingefuehrt
// 
// Revision 1.25  1991/01/24  12:15:51  zeller
// Pruefung gegen unendliche Rekursion eingefuehrt
// 
// Revision 1.24  1991/01/23  18:16:14  zeller
// Neu: isConst()
// 
// Revision 1.23  1990/09/19  12:18:57  zeller
// Ausgabe ArgNode's ueberarbeitet
// 
// Revision 1.22  1990/09/18  18:24:48  zeller
// Neu: ArgNode::dump() gibt symbolische Argument-Namen aus;
// CallNode::dump() laesst fuehrendes '$' weg
// 
// Revision 1.21  1990/09/09  23:03:03  zeller
// TestNode::makeConsts optimiert jetzt auch die beiden Alternativen
// 
// Revision 1.20  1990/09/09  17:46:18  zeller
// Neu: TestNode fuer bedingte Auswertungen
// 
// Revision 1.19  1990/09/01  15:34:03  zeller
// Privaten Teil VSLLib eingebunden (wg. VSLDef, VSLDefList)
// 
// Revision 1.18  1990/08/31  16:28:48  zeller
// Kleine Funktionen inline gemacht
// 
// Revision 1.17  1990/08/29  12:26:47  zeller
// Fall "leere Argumentliste" korrekt behandelt
// 
// Revision 1.16  1990/08/28  18:31:36  zeller
// resolveSynonyms() macht jetzt keine Querschlaeger mehr
// quer durch die gesamte Bibliothek, da Optimierungsfunktionen
// ohnehin in Reihenfolge ihrer Deklaration aufgerufen werden.
// 
// Revision 1.15  1990/08/28  17:34:17  zeller
// Methoden OK() eingefuehrt
// 
// Revision 1.14  1990/08/26  18:45:21  zeller
// Neugefasste Bedingung fuer combineArgs
// 
// Revision 1.13  1990/08/25  17:10:39  zeller
// Einfuehrung unlink()
// 
// Revision 1.12  1990/08/25  14:59:29  zeller
// Neues Link-Interface unterstuetzt
// 
// Revision 1.11  1990/08/23  15:26:51  zeller
// Destruktoren eingefuehrt;
// Warnung vor optimierenden Funktionen eingesetzt
// 
// Revision 1.10  1990/08/21  23:45:55  zeller
// Fall "Keine Argumente" in resolveSynonyms() beruecksichtigt
// 
// Revision 1.9  1990/08/21  23:02:53  zeller
// ExprCallNode entfernt; optimierende Funktionen eingefuehrt
// 
// Revision 1.8  1990/08/20  10:48:50  zeller
// Eval-Debugging wieder abgeschaltet (default)
// 
// Revision 1.7  1990/08/19  19:17:20  zeller
// ArgNode: Wenn nargs == 0, nichts auf Stack ablegen
// 
// Revision 1.6  1990/08/19  15:08:30  zeller
// Zuruecksetzen auf alte Basis korrigiert;
// Debugging-Tests eingeklammert
// 
// Revision 1.5  1990/08/19  13:18:47  zeller
// Neu: DefCallNode zeigt jetzt auf VSLDef;
// alter DefCallNode durch DefListCallNode ersetzt.
// 
// Revision 1.4  1990/08/18  18:09:25  zeller
// Debug-Moeglichkeiten eingefuehrt;
// Funktionsaufruf in CallNode::eval() zusammengefasst
// 
// Revision 1.3  1990/08/17  17:41:55  zeller
// Einfuehrung 'Range'
// 
// Revision 1.2  1990/08/14  20:32:25  zeller
// Weitere CallNode's implementiert
// 
// Revision 1.1  1990/08/14  17:41:17  zeller
// Initial revision
// 

char VSLNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <iostream.h>

#include "VSLLib.h"
#include "VSEFlags.h"

#include "VSLNode.h"

DEFINE_TYPE_INFO_0(VSLNode)

// VSLNode

// Flag: Seiteneffekte zugelassen?
bool VSLNode::sideEffectsProhibited = false;

// Flag: Seiteneffekte aufgetreten?
bool VSLNode::sideEffectsOccured    = false;

// Baum ausgeben
ostream& operator << (ostream& s, const VSLNode& node)
{
    node.dump(s);
    return s;
}

// ...in Baum-Notation
void VSLNode::dumpTree(ostream& s) const
{
    static unsigned depth = 1;

    s << "\n";
    for (unsigned i = 0; i < depth; i++)
	s << "  ";

    depth++;
    s << _type << "(";
    _dumpTree(s);
    s << ")";
    depth--;
}


// Knoten auswerten
const Box *VSLNode::eval(ListBox *arglist) const
{
    if (VSEFlags::show_tiny_eval)
    {
	cout << "\n" << *this;
	if (arglist)
	    cout << " " << *arglist;

	cout << ": ";
	cout.flush();
    }

    const Box *box = _eval(arglist);

    if (VSEFlags::show_tiny_eval)
    {
	cout << "\n" << *this;
	if (arglist)
	    cout << " " << *arglist;

	if (box == 0)
	    cout << " FAILS";
	else
	    cout << " = " << *box;

	cout.flush();
    }

    return box;
}


// Boxen vergleichen

bool VSLNode::bothSidesCanMatch = false;

bool VSLNode::operator == (const VSLNode &node) const
{
    if (VSEFlags::show_match_nodes)
    {
	cout << "\nNode match: " << *this << " ? " << node;
	cout.flush();
    }

    bool flag = (this == &node) || matches(node);

    if (bothSidesCanMatch)
	flag = flag || node.matches(*this);

    if (VSEFlags::show_match_nodes)
    {
	if (flag)
	    cout << "\nNode match: " << *this << " == " << node;
	else
	    cout << "\nNode match: " << *this << " != " << node;
	
	cout.flush();
    }

    return flag;
}


// Debugging

bool VSLNode::OK() const
// Pruefen, ob alles in Ordnung
{
    assert (_type != 0);
    return true;
}
