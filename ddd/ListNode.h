// $Id$
// Deklaration Klasse ListNode

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
// Revision 10.17  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.16  1994/09/05  06:32:38  zeller
// Fix: copy constructors made const correct
//
// Revision 10.15  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.14  1994/07/21  07:48:09  zeller
// Fix: compilePatterns() must be const
//
// Revision 10.13  1994/07/21  06:22:37  zeller
// Fix: Destructor cannot be const
//
// Revision 10.12  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:17:16  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:34  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:28:01  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:29  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/20  22:06:21  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.5  1993/04/16  11:44:32  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:19  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/02  03:00:03  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:28  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:51  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:58:03  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:37  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:38:14  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:53:16  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.26  1991/05/24  19:35:19  zeller
// Quantoren eingefuehrt
// 
// Revision 2.25  1991/05/24  12:03:36  zeller
// reBase -> _reBase, resolveNames -> _resolveNames umbenannt
// 
// Revision 2.24  1991/05/24  11:04:03  zeller
// reBase() eingefuehrt (fuer inlining von LET-Konstrukten)
// 
// Revision 2.23  1991/05/24  10:09:47  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.22  1991/05/23  18:54:38  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.21  1991/05/23  11:01:50  zeller
// Neu: Vor dem Uebersetzen der Pattern zunaechst alte Pattern loeschen
// 
// Revision 2.20  1991/05/22  11:28:46  zeller
// Neues Namenskonzept: erst alle Namen einlesen
// und erst spaeter durch ArgNode's ersetzen
// 
// Revision 2.19  1991/05/21  21:42:12  zeller
// Anpassungen fuer LetNode; nargs() neu definiert
// 
// Revision 2.18  1991/05/09  16:06:10  zeller
// Warnungen vermieden
// 
// Revision 2.17  1991/04/27  19:17:25  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.16  1991/04/27  17:29:45  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.15  1991/03/01  19:09:35  zeller
// Neu: countInstances()
// 
// Revision 2.14  1991/03/01  18:12:29  zeller
// Wieder da: instantiateArgs(), inlineFuncs()
// 
// Revision 2.13  1991/03/01  16:55:21  zeller
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.12  1991/03/01  12:54:17  zeller
// Neu: append();
// ~ListNode() const: Faelle '_head == 0 || _tail == 0' beruecksichtigt
// 
// Revision 2.11  1991/02/26  18:01:39  zeller
// isStraight() eingefuehrt
// 
// Revision 2.10  1991/02/26  14:12:24  zeller
// Neu: nargs()
// 
// Revision 2.9  1991/02/25  16:24:37  zeller
// Neues Konzept leere Listen eingefuehrt
// 
// Revision 2.8  1991/02/24  19:29:22  zeller
// ~ListNode: bessere Pruefungen eingebaut
// 
// Revision 2.7  1991/02/24  19:05:46  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.6  1991/02/24  16:16:06  zeller
// ListNode(ListNode &node) dupliziert jetzt auch Soehne
// 
// Revision 2.5  1991/02/20  19:51:58  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit;
// Vergleichsbedingung erheblich kuerzer gefasst
// 
// Revision 2.4  1991/02/20  18:57:14  zeller
// dumpList() eintfernt;
// Neue Fehlerbehandlung: Bei Fehler NOT_A_BOX zurueckgeben;
// _tail() kann jetzt auch etwas anderes als ein ListNode sein
// 
// Revision 2.3  1991/02/20  15:03:01  zeller
// 'tail' kann jetzt beliebiger Knoten sein
// 
// Revision 2.2  1991/02/20  12:10:45  zeller
// Beschreibung eingefuehrt
// 
// Revision 2.1  1991/02/20  12:00:05  zeller
// Initial revision
// 

#ifndef _Nora_ListNode_h
#define _Nora_ListNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein ListNode fasst andere Knoten k_i zu Listen zusammen. Zur Laufzeit
// erzeugt ein ListNode eine ListBox mit den Ergebnissen der k_i.


#include "assert.h"
#include <iostream.h>
#include "misc.h"       // max()

#include "VSLNode.h"


// ListNode

class ListNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_head;
    VSLNode *_tail;

protected:
    ListNode(const ListNode& node):
	VSLNode(node),
	_head(node._head->dup()),
	_tail(node._tail->dup())
    {}

    bool matches(const VSLNode& node) const
    {
	return VSLNode::matches(node) &&
	    *_head == *(((ListNode *)&node)->_head) && 
	    *_tail == *(((ListNode *)&node)->_tail);    // dirty trick
    }

public:
    ListNode(VSLNode *hd, VSLNode *tl, char *type = "ListNode"):
	VSLNode(type), _head(hd), _tail(tl)
    {}

    VSLNode *dup() const { return new ListNode(*this); }

    // ListNode zerstoeren: inklusive Argumenten!
    ~ListNode()
    {
	if (_head) delete _head;
	if (_tail) delete _tail;
    }

    // Ressourcen
    VSLNode*& head() { return _head; }
    VSLNode*& tail() { return _tail; }
    VSLNode* head() const { return _head; }
    VSLNode* tail() const { return _tail; }

    const Box *_eval(ListBox *arglist) const;

    // Optimierung
    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int foldOps(VSLDef *cdef, VSLNode **node);
    int foldConsts(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);
    int _reBase(VSLDef *cdef, unsigned newBase);

    // Sonstige Baum-Funktionen
    void compilePatterns(VSLDef *cdef) const;
    void uncompilePatterns(VSLDef *cdef) const;
    int resolveName(VSLDef *cdef, VSLNode **node, string& name, unsigned id);
    int _resolveNames(VSLDef *cdef, unsigned base);
    string firstName() const;

    // Quantoren
    bool exists(VSLNodeFunc func);
    bool forAll(VSLNodeFunc func);

    // Liste anhaengen
    int append(VSLNode *node);

    // Namen ausgeben
    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

    bool isConst() const;
    bool isListNode() const { return true; }

    bool isStraight() const;

    unsigned nargs() const;

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
