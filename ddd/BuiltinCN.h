// $Id$
// Deklaration Klasse BuiltinCallNode

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
// Revision 1.1  1995/05/01 15:47:34  zeller
// Initial revision
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/09/05  06:34:42  zeller
// Fix: copy constructors made const correct
//
// Revision 10.12  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.11  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:16:38  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  11:07:48  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/21  00:27:50  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/20  23:06:24  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.5  1993/04/16  11:44:21  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:09  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/02  02:59:59  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:21  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:41  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:56:25  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:01:56  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:47  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.10  1991/05/09  16:05:31  zeller
// Warnungen vermieden
// 
// Revision 2.9  1991/03/01  16:55:04  zeller
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.8  1991/02/27  19:07:38  zeller
// Neu: CallNode::matchesAll
// 
// Revision 2.7  1991/02/27  06:25:53  zeller
// Neu: BuiltinCallNode(BuiltinCallNode &node, VSLNode *arg)
// (fuer resolveSynonyms())
// 
// Revision 2.6  1991/02/24  19:03:56  zeller
// Neue Pruefungen 'NoBox' eingefuehrtarg muss jetzt angegeben werden
// 
// Revision 2.5  1991/02/20  19:50:56  zeller
// Neues Vergleichsverfahren mit Trace-Moeglichkeit
// 
// Revision 2.4  1991/02/20  18:51:34  zeller
// Neues Typenkonzept eingefuehrt;
// Pattern und Argumente nicht mehr auf Listen beschraenkt
// 
// Revision 2.3  1991/02/20  13:15:12  zeller
// Argument CallNode ist jetzt VSLNode
// 
// Revision 2.2  1991/02/20  12:10:18  zeller
// Beschreibung eingefuehrt
// 
// Revision 2.1  1991/02/20  11:59:55  zeller
// Initial revision
// 

#ifndef _Nora_BuiltinCallNode_h
#define _Nora_BuiltinCallNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein BuiltinCallNode wertet zur Laufzeit eine der eingebauten
// Funktionen aus.


#include "assert.h"
#include <iostream.h>

#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "DefCallN.h"

// BuiltinCallNode

class BuiltinCallNode: public CallNode {
public:
    DECLARE_TYPE_INFO

    friend class DefCallNode;

private:
    int _index;

protected:
    BuiltinCallNode(const BuiltinCallNode& node):
	CallNode(node), _index(node._index)
    {}

    const Box *call(Box *arg) const;

    char *func_name() const { return VSLBuiltin::ext_name(_index); }
    bool isInfix() const { return VSLBuiltin::isInfix(_index); }

    bool matches (const VSLNode& node) const
    {
	return matchesAll || (CallNode::matches(node) &&
	    _index == ((BuiltinCallNode *)&node)->_index);  // dirty trick
    }

public:
    // Neuen Knoten erzeugen
    BuiltinCallNode(int i, VSLNode *a,
	char *type = "BuiltinCallNode"): 
	CallNode(a, type), _index(i)
    {}

    BuiltinCallNode(BuiltinCallNode &node, VSLNode *a,
	char *type = "BuiltinCallNode"):
	CallNode(a, type), _index(node._index)
    {}

    // BuiltinCallNode kopieren
    VSLNode *dup() const
    {
	return new BuiltinCallNode(*this);
    }

    int foldOps(VSLDef *cdef, VSLNode** node);
    int foldConsts(VSLDef *cdef, VSLNode** node);

    bool isBuiltinCallNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
