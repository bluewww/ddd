// $Id$
// Implementation Klasse ConstNode

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
// Revision 1.1  1995/05/01 15:47:35  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.9  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.8  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.7  1993/05/22  20:16:49  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.6  1993/04/22  11:07:40  zeller
// Lizenz verbessert
// 
// Revision 10.5  1993/04/21  00:27:54  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.4  1993/04/16  11:44:24  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.3  1992/11/03  19:37:12  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:15  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:19:46  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:56:55  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:05  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:37:44  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:53:00  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.7  1991/05/23  18:54:04  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.6  1991/05/09  16:05:48  zeller
// Warnungen vermieden
// 
// Revision 2.5  1991/02/26  18:01:20  zeller
// isStraight() eingefuehrt
// 
// Revision 2.4  1991/02/25  16:23:59  zeller
// Neues Konzept leere Listen eingefuehrt
// 
// Revision 2.3  1991/02/24  19:04:28  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.2  1991/02/20  18:54:21  zeller
// Neues Typenkonzept eingefuehrt;
// _box == 0 steht jetzt fuer 'leere Liste' ([]).
// 
// Revision 2.1  1991/02/20  11:59:59  zeller
// Initial revision
// 

char ConstNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <iostream.h>
#include "assert.h"
#include "VSLLib.h"
#include "VSEFlags.h"
#include "VSLNode.h"
#include "ConstNode.h"
#include "ListBox.h"
#include "StringBox.h"

DEFINE_TYPE_INFO_1(ConstNode, VSLNode)

// ConstNode

// Konstante auswerten
const Box *ConstNode::_eval(ListBox *arglist) const
{
    return _box->link();
}

// Konstante ausgeben
void ConstNode::dump(ostream& s) const
{
    if (VSEFlags::include_const_info)
	s << *_box;
    else
	s << "const_" << _box->id();
}

// ...in Baum-Notation
void ConstNode::_dumpTree(ostream& s) const
{
    bool old = StringBox::quoted;
    StringBox::quoted = true;
    s << "\"" << *_box << "\"";
    StringBox::quoted = old;
}

// Pruefen, ob Argumentliste direkt uebernommen werden kann
bool ConstNode::isStraight() const
{
    // Auf leere Liste pruefen
    return (_box->isListBox() && ((ListBox *)_box)->isEmpty());
}


// Debugging

// Pruefen, ob alles in Ordnung
bool ConstNode::OK() const
{
    assert (VSLNode::OK());
    assert (_box && _box->OK());

    return true;
}
