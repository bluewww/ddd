// $Id$
// Implementation Klasse NameNode

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
// Revision 10.10  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.9  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.8  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.7  1993/05/22  20:17:25  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.6  1993/04/22  11:07:27  zeller
// Lizenz verbessert
// 
// Revision 10.5  1993/04/21  00:28:03  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.4  1993/04/16  11:44:34  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.3  1992/11/03  19:37:21  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:21  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:20:00  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:58:10  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:02:42  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:38:44  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:54:05  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.4  1991/05/23  18:56:32  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.3  1991/05/22  13:42:21  zeller
// Debugging verschoenert
// 
// Revision 2.2  1991/05/22  12:28:38  zeller
// Debugging eingefuehrt
// 
// Revision 2.1  1991/05/22  11:26:07  zeller
// Initial revision
// 

char NameNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"
#include "NameNode.h"
#include "ArgNode.h"
#include "VSEFlags.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(NameNode, VSLNode)


// NameNode

// NameNode ausgeben
void NameNode::dump(ostream& s) const
{
    s << _name;
}

// ...in Baum-Notation
void NameNode::_dumpTree(ostream& s) const
{
    s << "\"" << _name << "\"";
}


// Ersetzen

// Namen durch Variable ersetzen
int NameNode::resolveName(VSLDef *cdef, VSLNode **node, string& name, 
    unsigned id)
{
    assert (this == *node);

    if (_name == name)
    {
	// NameNode durch ArgNode(id) ersetzen
	*node = new ArgNode(id);

	if (VSEFlags::show_vars)
	{
	    cout << "\n" << cdef->longname() << ":resolveNames: replacing " 
		<< *this << " by " << **node;
	    cout.flush();
	}

	delete this; 
	return 1;
    }

    return 0;
}

// Debugging

bool NameNode::OK() const
// Pruefen, ob alles in Ordnung
{
    assert (VSLNode::OK());
    return true;
}
