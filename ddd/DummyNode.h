// $Id$
// Deklaration Klasse DummyNode

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
// Revision 1.1  1995/05/01 15:47:39  zeller
// Initial revision
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/09/05  06:33:19  zeller
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
// Revision 10.9  1993/05/22  20:17:04  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  11:07:25  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/21  00:27:57  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/20  23:06:28  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.5  1993/04/16  11:44:28  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/11/03  19:37:16  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/06/02  03:00:02  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:26  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:59  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:57:36  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:19  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:54:02  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 1.1  1991/05/24  18:54:09  zeller
// Initial revision
// 

#ifndef _Nora_DummyNode_h
#define _Nora_DummyNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein DummyNode gibt zur Laufzeit eine DummyBox zurueck.
// Ein DummyNode matcht alle anderen Knoten, wenn matchesAll == true.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "Box.h"
#include "DummyBox.h"

// DummyNode

class DummyNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

protected:
    DummyNode(const DummyNode& node):
	VSLNode(node)
    {}
    
    void dump(ostream& s) const
    {
	s << "_";
    }

    bool matches(const VSLNode& node) const
    {
	return matchesAll || VSLNode::matches(node);
    }

public:
    // DummyNode erzeugen
    DummyNode(char *type = "DummyNode"): 
	VSLNode(type)
    {}

    // DummyNode kopieren
    VSLNode *dup() const
    {
	return new DummyNode(*this);
    }

    const Box *_eval(ListBox *arglist) const    
    { 
	return new DummyBox; 
    }

    static bool matchesAll;

    bool isConst() const { return true; }
    bool isDummyNode() const { return true; }
};

#endif
