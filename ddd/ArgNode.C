// $Id$ 
// ArgNode class

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

char ArgNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include <strstream.h>
#include "assert.h"
#include "VSLLib.h"
#include "ArgNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(ArgNode, DummyNode)

// ArgNode

ArgNodeFunc ArgNode::matchCallback = 0;


// Argument auswerten
const Box *ArgNode::_eval(ListBox *arglist) const
{
    const Box *box = 0;

    if (arglist)
    {
	box = (*arglist)[_id];
	if (box)
	    box = ((Box *)box)->link();
    }

    if (box == 0)
    {
	ostrstream os;
	os << "arg" << _id << " cannot be isolated";
	VSLLib::eval_error(os);
    }

    return box;
}

// ArgNode ausgeben
void ArgNode::dump(ostream& s) const
{
    s << "arg" << _id;
}

// ...in Baum-Notation
void ArgNode::_dumpTree(ostream& s) const
{
    s << _id;
}

bool ArgNode::matches(const VSLNode &node) const
{
    bool ret = matchesAll ||
	VSLNode::matches(node) &&
	    _id == ((ArgNode *)&node)->_id; // dirty trick

    if (ret && matchCallback)
	matchCallback(_id, &node);

    return ret;
}


// Optimierung

// instantiateArgs: In Ausdruecken der Form f(arg1, arg2, ...) 
// arg1 .. argn durch konkrete Argumente ersetzen

int ArgNode::instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
    unsigned base, unsigned n)
// Knoten durch entsprechenden Ausdruck aus 'values' ersetzen
{
    assert(this == *node);

    // Wenn nicht im Bereich, abbrechen
    if (_id < base || _id >= base + n)
	return 0;

    VSLNode *source = values[_id];
    assert (source != 0);

    VSLNode *newNode = source->dup();
    *node = newNode;

#if 0
    if (VSEFlags::show_optimize)
    {
	cout << "\n" << cdef->longname() << ": instantiateArgs: replacing\n" 
	    << *this << "\nby " << *newNode << '\n';
	cout.flush();
    }
#endif

    // ArgNode loeschen
    delete this;

    return 1;
}

void ArgNode::countArgNodes(VSLDef *cdef, int instances[],
    unsigned base, unsigned n)
// Instanzen-Zaehler hinaufsetzen
{
    // Wenn nicht im Bereich, abbrechen
    if (_id < base || _id >= base + n)
	return;

    instances[_id]++;
}
