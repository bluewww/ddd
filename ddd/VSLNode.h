// $Id$
// Deklaration Klasse VSLNode

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
// Revision 1.1  1995/05/01 15:47:56  zeller
// Initial revision
//
// Revision 10.17  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.16  1994/09/05  06:33:00  zeller
// Fix: copy constructors made const correct
//
// Revision 10.15  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.14  1994/07/21  07:48:17  zeller
// Fix: compilePatterns() must be const
//
// Revision 10.13  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.12  1993/11/11  15:44:19  zeller
// Fix: Syntax Funktionsaufruf korrigiert
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:18:14  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:33  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:28:17  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:36  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/20  22:06:26  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.5  1993/04/16  11:44:51  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:36  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/02  03:00:10  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:39  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:28  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  06:00:15  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:40  zeller
// Installierte Version
// 
// Revision 4.3  1991/07/02  14:35:45  zeller
// _dumpTree() jetzt mit Default "nichts tun"
// 
// Revision 4.2  1991/07/02  12:38:55  zeller
// Neu: dumpTree() -- als Baum ausgeben
// 
// Revision 4.1  1991/05/25  12:51:58  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.25  1991/05/25  12:09:02  zeller
// Quantoren eingefuehrt;
// neu: isDummyNode()
// 
// Revision 2.24  1991/05/24  12:02:23  zeller
// Neu: Feld _base gibt #Argumente im Kontext an;
// wird von resolveNames() initialisiert und von reBase() veraendert
// 
// Revision 2.23  1991/05/24  11:03:19  zeller
// reBase() eingefuehrt (fuer inlining von LET-Konstrukten)
// 
// Revision 2.22  1991/05/24  10:08:44  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.21  1991/05/23  18:52:23  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.20  1991/05/23  11:00:53  zeller
// Neu: Vor dem Uebersetzen der Pattern zunaechst alte Pattern loeschen
// 
// Revision 2.19  1991/05/22  11:27:36  zeller
// Neues Namenskonzept: erst alle Namen einlesen
// und erst spaeter durch ArgNode's ersetzen
// 
// Revision 2.18  1991/05/21  21:40:16  zeller
// Anpassungen fuer LetNode; nargs() neu definiert
// 
// Revision 2.17  1991/05/09  16:04:53  zeller
// Warnungen vermieden
// 
// Revision 2.16  1991/04/27  19:16:28  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.15  1991/04/27  17:29:29  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.14  1991/03/01  19:08:23  zeller
// Neu: countInstances()
// 
// Revision 2.13  1991/03/01  18:12:42  zeller
// Wieder da: instantiateArgs(), inlineFuncs()
// 
// Revision 2.12  1991/03/01  16:55:36  zeller
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.11  1991/02/27  17:46:15  zeller
// Doppelseitiges Matchen eingefuehrt
// 
// Revision 2.10  1991/02/27  02:46:52  zeller
// isListNode() eingefuehrt (hatten wir wohl vergessen)
// 
// Revision 2.9  1991/02/26  18:00:27  zeller
// isStraight() eingefuehrt
// 
// Revision 2.8  1991/02/26  14:11:10  zeller
// Neu: nargs()
// 
// Revision 2.7  1991/02/24  19:07:49  zeller
// NOT_A_BOX entfernt
// 
// Revision 2.6  1991/02/24  15:39:37  zeller
// isMatchNode() entfernt
// 
// Revision 2.5  1991/02/20  19:50:21  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit
// 
// Revision 2.4  1991/02/20  18:42:12  zeller
// Neues Typenkonzept eingefuehrt
// 
// Revision 2.3  1991/02/20  12:10:05  zeller
// Beschreibung eingefuehrt
// 
// Revision 2.2  1991/02/20  11:56:30  zeller
// Jeder Klasse eigene Datei zugewiesen
// 
// Revision 2.1  1991/02/19  22:46:41  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.36  1991/02/17  17:27:44  zeller
// STD_MAXDEPTH durch VSEFlags::max_eval_nesting ersetzt
// 
// Revision 1.35  1991/02/14  14:46:32  zeller
// Neu: TestNode::foldConsts
// 
// Revision 1.34  1991/02/14  14:01:31  zeller
// TestNode jetzt Erbe von CallNode
// (fuer Argument-Verwaltung, Konstantenbildung usw.)
// 
// Revision 1.33  1991/02/14  13:13:08  zeller
// CallNode::CallNode(CallNode& node): Fall _args == 0 beruecksichtigt
// 
// Revision 1.32  1991/02/14  11:11:12  zeller
// Neue Namen Optimierungen eingefuehrt;
// Bei Loeschen VSLNode type auf UNKNOWN gesetzt, um Dangling Pointers
// auf die Spur zu kommen
// 
// Revision 1.31  1991/02/11  20:13:21  zeller
// DefCallNode::resolveSynonyms zum 'friend' gemacht,
// um Optimierungsbedingungen zu pruefen
// 
// Revision 1.30  1991/02/11  19:25:44  zeller
// Neu: resolveDefs();
// DefListCallNode entfernt
// (Aufgaben werden jetzt von DefCallNode wahrgenommen)
// 
// Revision 1.29  1991/02/11  16:53:50  zeller
// callee_name entfernt;
// dump() erweitert;
// BuiltinCallNode vereinfacht.
// 
// Revision 1.28  1991/02/11  15:24:29  zeller
// Neu: removeSelfReferences
// 
// Revision 1.27  1991/02/10  16:20:04  zeller
// Kopierfunktionen nach 'VSLNode.C' ausgelagert,
// um 'references' mitzuzaehlen
// 
// Revision 1.26  1991/02/10  15:49:58  zeller
// Konstruktoren, Destruktoren nach VSLNode.C ausgelagert,
// um 'references' einzufuehren
// 
// Revision 1.25  1991/02/08  18:30:07  zeller
// dump() durch << ersetzt
// 
// Revision 1.24  1991/02/08  17:09:34  zeller
// Alle Optimierungsfunktionen geben die #Aenderungen zurueck
// 
// Revision 1.23  1991/02/08  13:24:17  zeller
// Neu: instance_count zaehlt Instanziierungen in inlineFuncs
// 
// Revision 1.22  1991/02/06  20:55:49  zeller
// instantiateArgs() bekommt jetzt #Argumente uebergeben
// 
// Revision 1.21  1991/02/06  19:24:34  zeller
// Optimierungsfunktionen fuer TestNode eingefuehrt
// (auch TestNode muss seine Argumente optimieren)
// 
// Revision 1.20  1991/02/06  18:16:43  zeller
// Initialisierungen ueberarbeitet;
// inlineFunc-Debugging eingefuehrt
// 
// Revision 1.19  1991/02/06  12:13:15  zeller
// dup() eingefuehrt;
// inlineFuncs() vorbereitet
// 
// Revision 1.18  1991/02/01  17:32:28  zeller
// Neu: BuiltinCallNode::makeConsts()
// 
// Revision 1.17  1991/01/29  14:55:40  zeller
// Kontrolle ueber Seiteneffekte eingefuehrt
// 
// Revision 1.16  1991/01/24  13:03:18  zeller
// Pruefung unendliche Rekursion eingefuegt ("maxDepth")
// 
// Revision 1.15  1991/01/23  18:16:25  zeller
// Neu: isConst()
// 
// Revision 1.14  1990/09/18  18:25:19  zeller
// ArgNode: Name Parameter wird nicht mehr mitgefuehrt
// 
// Revision 1.13  1990/09/09  17:46:29  zeller
// Neu: TestNode fuer bedingte Auswertungen
// 
// Revision 1.12  1990/08/31  16:28:51  zeller
// Kleine Funktionen inline gemacht
// 
// Revision 1.11  1990/08/28  18:31:52  zeller
// Flag 'optimizing' entfernt
// 
// Revision 1.10  1990/08/28  17:34:21  zeller
// Methoden OK() eingefuehrt
// 
// Revision 1.9  1990/08/25  14:50:03  zeller
// Neues Link-Interface unterstuetzt
// 
// Revision 1.8  1990/08/23  15:27:25  zeller
// Destruktoren eingefuehrt;
// ConstNode erhoeht Link-Zahl der Box.
// 
// Revision 1.7  1990/08/21  23:03:06  zeller
// ExprCallNode entfernt; optimierende Funktionen eingefuehrt
// 
// Revision 1.6  1990/08/19  13:19:07  zeller
// Neu: DefCallNode zeigt jetzt auf VSLDef;
// alter DefCallNode durch DefListCallNode ersetzt.
// 
// Revision 1.5  1990/08/18  18:09:38  zeller
// Funktionsaufruf in CallNode::eval() zusammengefasst
// 
// Revision 1.4  1990/08/18  13:45:03  zeller
// VSLNode(): jetzt korrekte Initialisierung von '_next';
// Berechnung nargs() verbessert
// 
// Revision 1.3  1990/08/17  17:42:10  zeller
// Einfuehrung 'Range', nargs()
// 
// Revision 1.2  1990/08/14  20:32:41  zeller
// Weitere CallNode's deklariert
// 
// Revision 1.1  1990/08/14  17:41:18  zeller
// Initial revision
// 

#ifndef _Nora_VSLNode_h
#define _Nora_VSLNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein VSLNode ist ein Knoten eines Auswertungsbaumes, der die Ausdruecke
// der VSLLib repraesentiert.


#include "assert.h"
#include <iostream.h>
#include <string.h>
#include "TypeInfo.h"

#include "VSLBuiltin.h"

class Box;
class ListBox;
class VSLDefList;
class VSLDef;
class VSLNode;

typedef bool (VSLNode::*VSLNodeFunc)();

// VSLNode

class VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    char *_type;    // Typ

protected:
    unsigned _base; // Anzahl Argumente im Kontext

    // Flag: Seiteneffekte zugelassen?
    static bool sideEffectsProhibited;

    // Flag: Seiteneffekte aufgetreten?
    static bool sideEffectsOccured;

    // Knoten kopieren
    VSLNode(const VSLNode& node):
	_type(node._type),
	_base(node._base)
    {}

    // Knoten ausgeben
    virtual void dump(ostream& s) const = 0;      // als VSL-Ausdruck
    virtual void _dumpTree(ostream& s) const {}   // als Baum (default: nichts)

    virtual bool matches(const VSLNode& node) const
    {
	return strcmp(_type, node._type) == 0;
    }

public:
    // Knoten erzeugen
    VSLNode(char *type = "VSLNode"):
	_type(type),
	_base(0)
    {}

    // Default-Destruktor
    virtual ~VSLNode()
    {
	_type = 0;  // Schutz gegen weitere Referenzen
    }

    // Knoten kopieren
    virtual VSLNode *dup() const = 0;

    // Knoten auswerten
    virtual const Box *eval(ListBox *arglist) const;
    virtual const Box *_eval(ListBox *arglist) const = 0;

    // Optimierungs-Funktionen (Default: nichts tun)
    virtual int resolveDefs(VSLDef *cdef, 
	bool complain_recursive = true)                      { return 0; }
    virtual int resolveSynonyms(VSLDef *cdef, VSLNode **node)   { return 0; }
    virtual int foldOps(VSLDef *cdef, VSLNode **node)           { return 0; }
    virtual int foldConsts(VSLDef *cdef, VSLNode **node)        { return 0; }
    virtual int countSelfReferences(VSLDef *cdef, 
	VSLDefList *deflist)                                    { return 0; }
    virtual int inlineFuncs(VSLDef *cdef, VSLNode **node)       { return 0; }
    virtual int instantiateArgs(VSLDef *cdef, VSLNode **node,
	VSLNode *values[], unsigned base, unsigned n)           { return 0; }
    virtual void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n)                              { return; }
    virtual int _reBase(VSLDef *cdef, unsigned newBase)         { return 0; }
    int reBase(VSLDef *cdef, unsigned newBase) 
    { 
	int changes = _reBase(cdef, newBase);
	_base = newBase;
	return changes;
    }

    // Sonstige Baum-Operationen (Default: nichts tun)
    virtual void compilePatterns(VSLDef *cdef) const            { return; }
    virtual void uncompilePatterns(VSLDef *cdef) const          { return; }
    virtual int resolveName(VSLDef *cdef, VSLNode **node, 
	string& name, unsigned id)                              { return 0; }
    virtual int _resolveNames(VSLDef *cdef, unsigned base)      { return 0; }
    int resolveNames(VSLDef *cdef, unsigned base)
    {
	_base = base;
	return _resolveNames(cdef, base);
    }

    virtual string firstName() const          { return ""; }

    // Pruef-Funktionen
    virtual bool isConst() const = 0;

    virtual bool isArgNode() const         { return false; }
    virtual bool isBuiltinCallNode() const { return false; }
    virtual bool isCallNode() const        { return false; }
    virtual bool isConstNode() const       { return false; }
    virtual bool isDefCallNode() const     { return false; }
    virtual bool isDummyNode() const       { return false; }
    virtual bool isLetNode() const         { return false; }
    virtual bool isListNode() const        { return false; }
    virtual bool isNameNode() const        { return false; }
    virtual bool isTestNode() const        { return false; }

    virtual bool isStraight() const        { return false; }

    // Quantoren ueber Praedikate
    virtual bool exists(VSLNodeFunc func)    { return func(); }
    virtual bool forAll(VSLNodeFunc func)    { return func(); }

    // #NameNode's zurueckgeben
    virtual unsigned nargs() const            { return 0; }

    // Match-Funktionen
    static bool bothSidesCanMatch;
    bool operator == (const VSLNode& node) const;
    bool operator != (const VSLNode& node) const
    { 
	return !(operator == (node)); 
    }

    // Repraesentations-Invariante
    virtual bool OK() const;

    // Baum ausgeben
    friend ostream& operator << (ostream& s, const VSLNode& node); // als VSL
    void dumpTree(ostream& s) const;                               // als Baum
};

#endif
