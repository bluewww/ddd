// $Id$
// Deklaration Klasse DefCallNode

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
// Revision 1.1  1995/05/01 15:47:37  zeller
// Initial revision
//
// Revision 10.16  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.15  1994/11/14  21:08:28  zeller
// Fixes for gcc 2.6.1
//
// Revision 10.14  1994/09/05  06:34:25  zeller
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
// Revision 10.10  1993/05/22  20:16:58  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:24  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:27:56  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:27  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/20  22:06:19  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.5  1993/04/16  11:44:26  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:15  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/02  03:00:01  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:25  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:49  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:57:22  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:12  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:53:12  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.12  1991/05/09  16:06:05  zeller
// Warnungen vermieden
// 
// Revision 2.11  1991/04/27  19:17:07  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.10  1991/03/01  18:12:18  zeller
// Wieder da: inlineFuncs()
// 
// Revision 2.9  1991/03/01  16:55:15  zeller
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.8  1991/02/27  19:08:02  zeller
// Neu: CallNode::matchesAll
// 
// Revision 2.7  1991/02/26  21:41:08  zeller
// isDefCallNode() const korrekt geschrieben
// 
// Revision 2.6  1991/02/24  19:05:15  zeller
// arg muss jetzt angegeben werden
// 
// Revision 2.5  1991/02/20  19:51:32  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit
// 
// Revision 2.4  1991/02/20  18:55:29  zeller
// Neues Typenkonzept eingefuehrt;
// Pattern und Argumente nicht mehr auf Listen beschraenkt
// 
// Revision 2.3  1991/02/20  13:15:45  zeller
// Argument CallNode ist jetzt VSLNode
// 
// Revision 2.2  1991/02/20  12:10:39  zeller
// Beschreibung eingefuehrt
// 
// Revision 2.1  1991/02/20  12:00:03  zeller
// Initial revision
// 

#ifndef _Nora_DefCallNode_h
#define _Nora_DefCallNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein DefCallNode wertet zur Laufzeit eine selbstdefinierte Funktion aus.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "CallNode.h"

#include "VSLDefList.h"
#include "VSLDef.h"

// DefCallNode

class DefCallNode: public CallNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLDef *_def;           // Wenn eindeutig: Zeiger auf Definition, sonst 0
    VSLDefList *_deflist;   // Zeiger auf Definitionsliste

protected:
    DefCallNode(const DefCallNode& node);

    const Box *call(Box *arg) const;

    char *func_name() const;

    bool matches(const VSLNode& node) const
    {
	return matchesAll ||
	    (CallNode::matches(node) &&
		_deflist == ((DefCallNode *)&node)->_deflist); // dirty trick
    }

public:
    // DefCallNode kopieren
    VSLNode *dup() const
    {
	return new DefCallNode(*this);
    }

    // DefCallNode aus Definition erzeugen
    DefCallNode(VSLDef *def, VSLNode *arg, char *type = "DefCallNode");

    // DefCallNode aus Definitionsliste erzeugen
    DefCallNode(VSLDefList *deflist, VSLNode *arg, char *type = "DefCallNode");

    // DefCallNode zerstoeren
    ~DefCallNode();

    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);

    bool isDefCallNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
