// $Id$
// Implementation Klasse ConstNode

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char ConstNode_rcsid[] = 
    "$Id$";

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
const Box *ConstNode::_eval(ListBox *) const
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
