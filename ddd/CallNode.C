// $Id$
// Implementation Klasse CallNode

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char CallNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "VSEFlags.h"
#include "ConstNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(CallNode, VSLNode)

// CallNode

bool CallNode::matchesAll = false;


// Funktion ausgeben
void CallNode::dump(ostream& s) const
{
    s << func_name();

    if (VSEFlags::include_list_info)
	s << *_arg;
    else
	if (_arg->isArgNode())
	    s << "(" << *_arg << "...)";
	else
	    s << *_arg;
}

// ...in Baum-Notation
void CallNode::_dumpTree(ostream& s) const
{
    s << "\"" << func_name() << "\", ";
    _arg->dumpTree(s);
}

// Funktion auswerten
const Box *CallNode::_eval(ListBox *arglist) const
{
    // Argument auswerten
    const Box *myarg = _arg->eval(arglist);
    if (myarg == 0)
	return 0;

    // Funktion aufrufen
    const Box *box = call((Box *)myarg);

    ((Box *)myarg)->unlink();

    return box;
}



// Optimierung

// im Allgemeinen: Funktion auf Argumenten ausfuehren
int CallNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    return arg()->resolveDefs(cdef, complain_recursive);
}

int CallNode::resolveSynonyms(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->resolveSynonyms(cdef, &arg());
}

int CallNode::foldOps(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->foldOps(cdef, &arg());
}

int CallNode::inlineFuncs(VSLDef *cdef, VSLNode ** /* node */)
{
    // assert (this == *node);
    return arg()->inlineFuncs(cdef, &arg());
}

int CallNode::instantiateArgs(VSLDef *cdef, VSLNode ** /* node */,
			      VSLNode *values[], unsigned base, unsigned n)
{
    // assert(this == *node); 
    return arg()->instantiateArgs(cdef, &arg(), values, base, n);
}

void CallNode::countArgNodes(VSLDef *cdef, int instances[],
    unsigned base, unsigned n)
{
    arg()->countArgNodes(cdef, instances, base, n);
}

void CallNode::compilePatterns(VSLDef *cdef) const
{
    arg()->compilePatterns(cdef);
}

void CallNode::uncompilePatterns(VSLDef *cdef) const
{
    arg()->uncompilePatterns(cdef);
}

int CallNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    return arg()->countSelfReferences(cdef, deflist);
}

int CallNode::resolveName(VSLDef *cdef, VSLNode ** /* node */, string& name,
    unsigned id)
{
    // assert(this == *node); 
    return arg()->resolveName(cdef, &arg(), name, id);
}

int CallNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    return arg()->resolveNames(cdef, base);
}

int CallNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    return arg()->reBase(cdef, newBase);
}

string CallNode::firstName() const
{
    return arg()->firstName();
}



// Konstanten zusammenziehen
int CallNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Auf allen Argumenten ausfuehren
    changes += arg()->foldConsts(cdef, &arg());

    // Pruefen, ob Ausdruck jetzt konstant;
    // wenn nicht, Abbruch
    if (!isConst())
	return changes;

    // Sonst: Funktion auswerten (hierbei Seiteneffekte vermeiden)

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *result = eval(0);

    if (result)
    {
	assert(!sideEffectsOccured);

	// CallNode durch Konstante ersetzen
	if (VSEFlags::show_optimize)
	{
	    cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		<< *this << '\n';
	    cout.flush();
	}

	// CallNode durch Konstante ersetzen
	*node = new ConstNode((Box *)result);

	if (VSEFlags::show_optimize)
	{
	    cout << "by " << **node << "\n";
	    cout.flush();
	}
	
	changes++;

	// CallNode und daranhaengenden Teilbaum loeschen
	delete this;
    }

    sideEffectsProhibited = false;

    return changes;
}



// Debugging

// Pruefen, ob alles in Ordnung
bool CallNode::OK() const
{
      assert (VSLNode::OK());
      assert (_arg && _arg->OK());

      return true;
}
