// $Id$
// Deklaration Klasse NameNode

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
// Revision 1.1  1995/05/01 15:47:24  zeller
// Initial revision
//
// Revision 10.15  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.14  1994/09/05  06:36:33  zeller
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
// Revision 10.10  1993/05/22  20:17:28  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:28  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:28:03  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:31  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:44:35  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/03  19:37:22  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.4  1992/10/13  20:57:49  zeller
// strclass.h statt String.h
// 
// Revision 10.3  1992/06/02  03:00:05  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:30  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:20:00  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:58:15  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:47  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:38:49  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:54:07  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.2  1991/05/23  18:56:38  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.1  1991/05/22  11:26:08  zeller
// Initial revision
// 

#ifndef _Nora_NameNode_h
#define _Nora_NameNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein NameNode ist ein Platzhalter fuer einen Variablennamen.


#include "assert.h"
#include <iostream.h>
#include "strclass.h"

#include "VSLNode.h"


// NameNode

class NameNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    string _name;           // Variablenname

protected:
    NameNode(const NameNode& node):
	VSLNode(node), 
	_name(node._name)
    {}

    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

public:
    // NameNode erzeugen
    NameNode(string nm, char *type = "NameNode"):
	VSLNode(type), _name(nm)
    {}

    // NameNode kopieren
    VSLNode *dup() const { return new NameNode(*this); }

    // Auswerten
    const Box *_eval(ListBox *arglist) const        { assert(0); return 0; }

    // Ressourcen
    bool isConst() const   { return false; }
    bool isNameNode() const { return true; }

    unsigned nargs() const    { return 1; }

    // Namen durch Variable ersetzen
    int resolveName(VSLDef *cdef, VSLNode **node, string& name, unsigned id);
    string firstName() const { return _name; }

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
